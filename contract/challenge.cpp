// #include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <boost/algorithm/string.hpp>

#include "challenge.hpp"


auto challenge::get_registration(eosio::name org, eosio::name game) {

    auto org_index = registrations.get_index<"organization"_n>();
    auto org_index_lower = org_index.lower_bound(org.value);
    auto org_index_upper = org_index.upper_bound(org.value);
    while (org_index_lower != org_index_upper) {
        if (org_index_lower->game_name == game) {
            // org_index_lower->jsonify();
            return org_index_lower;
        }
        org_index_lower ++;
    }
    return org_index.cend();
}


void challenge::wrapper(eosio::name org, eosio::name game) {
    require_auth(org);
    get_registration(org, game);
}
void challenge::clear() {
    require_auth(_self);
    auto it1 = registrations.begin();
    while (it1 != registrations.end()) {
        it1 = registrations.erase(it1);
    }
    auto it2 = games.begin();
    while (it2 != games.end()) {
        it2 = games.erase(it2);
    }
    auto it3 = heroes.begin();
    while (it3 != heroes.end()) {
        it3 = heroes.erase(it3);
    }
}

void challenge::enroll(eosio::name org, eosio::name game, uint64_t min_duration, uint64_t max_duration) {
    require_auth(org);
    eosio_assert(min_duration < max_duration, "min duration must less than max duration");

    auto registration = get_registration(org, game);

    auto org_index = registrations.get_index<"organization"_n>();
    // not existed, create a new one
    if (registration == org_index.cend()) {
        registrations.emplace(get_self(), [&](auto &r) {
                                              r.id = registrations.available_primary_key();
                                              r.organization = org;
                                              r.game_name = game;
                                              r.min_duration = min_duration;
                                              r.max_duration = max_duration;
                                          });
    } else {
        registrations.modify(*registration, get_self(), [&](auto &r) {
                                                            r.min_duration = min_duration;
                                                            r.max_duration = max_duration;
                                                        });
    }
}

void challenge::startgame(eosio::name user) {
    eosio_assert(user == "eosio.token"_n, "fake tokens");
    auto data = eosio::unpack_action_data<eosio_token_transfer>();

    if (data.from != get_self()) {
        eosio_assert(data.to == get_self(), "didn't transfer tokens to me");
        eosio_assert(data.quantity.is_valid(), "invalid quantity");
        eosio_assert(data.quantity.amount > 0, "must transfer positive quantity");
        eosio_assert(data.quantity.symbol == eosio::symbol("EOS", 4), "symbol precision mismatch");
        eosio_assert(data.memo.size() <= 256, "memo has more than 256 bytes");

        std::vector<std::string> args;
        boost::split(args, data.memo, [](char c) { return c == ':'; });
        eosio::name org = eosio::name(args[0]);
        eosio::name game = eosio::name(args[1]);
        eosio::name referuser = eosio::name(args[2]);
        uint64_t bet = (uint64_t)data.quantity.amount;
        uint64_t target = std::stoull(args[3]);
        uint64_t ts = std::stoull(args[4]);
        // eosio::print("organization: ", org, ", game: ", game, ", referuser: ", referuser, ", bet: ", bet, ", ts: ", ts, ", target: ", target);
        // eosio::name user_lucky_number = eosio::name(args[5]);
        std::string user_lucky_number = args[5];

        // eosio::print("organization: ", org,
        //              ", game: ", game,
        //              ", referuser: ", referuser,
        //              ", bet: ", bet,
        //              ", target: ", target,
        //              ", ts: ", ts
        //     );


        _startgame(data.from, org, game, target, bet, referuser, ts, user_lucky_number);
    }
}

void challenge::_startgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target,
                           uint64_t bet, eosio::name referuser, uint64_t ts,
                           std::string user_lucky_number) {

    auto registration = get_registration(org, game);

    auto org_index = registrations.get_index<"organization"_n>();
    // not existed, error
    eosio_assert(registration != org_index.cend(), "not found registered organization");

    size_t trx_size = transaction_size();
    char *trx = (char *)malloc(trx_size);
    read_transaction(trx, trx_size);
    st_seed seeds;
    seeds.seed1 = eosio::sha256(user_lucky_number.c_str(), user_lucky_number.length());
    seeds.seed2 = eosio::sha256(trx, trx_size);

    eosio::checksum256 seed_hash = eosio::sha256( (char *)&seeds.seed1, sizeof(st_seed));

    auto seed_data = seed_hash.data();
    uint64_t mixed_seed = (uint64_t)(seed_data[0] >> 64) + (uint64_t)(seed_data[0]) + (uint64_t)(seed_data[1] >> 64) + (uint64_t)(seed_data[1]);

    games.emplace(get_self(), [&](auto &g) {
                                  g.id = mixed_seed;
                                  g.registration_id = registration->id;
                                  g.user = user;
                                  g.refer_user = referuser;
                                  g.bet = bet;
                                  g.target = target;
                                  g.duration = -1;
                                  g.seed = seed_hash;
                              });
    // genarate duration randomly in another transaction
}

void challenge::resolvegame(uint64_t game_id, eosio::signature sig, eosio::public_key pubkey) {
    require_auth(admin);

    auto found_game = games.find(game_id);
    eosio_assert(found_game != games.cend(), "not found game");

    // eosio::public_key pubkey;
    eosio::assert_recover_key(found_game->seed, sig, pubkey);
    eosio::checksum256 random_number_hash = eosio::sha256( (char *)&sig, sizeof(sig));
    auto random_number_data = random_number_hash.data();
    auto registration = registrations.find(found_game->registration_id);
    eosio_assert(registration != registrations.cend(), "error, not found registration");

    uint64_t max_duration = registration->max_duration;
    uint64_t min_duration = registration->min_duration;
    const uint64_t duration = ((uint64_t)(random_number_data[0] >> 64) + (uint64_t)(random_number_data[0]) + (uint64_t)(random_number_data[1] >> 64) + (uint64_t)(random_number_data[1])) % (uint64_t)(max_duration - min_duration) + min_duration;

    games.modify(*found_game, get_self(), [&](auto &g) {
                                              g.duration = duration;
                                          });
}

void challenge::endgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target, uint64_t achievement,
                        uint64_t duration,
                        eosio::name referuser, uint64_t ts) {
    require_auth(user);

    auto registration = get_registration(org, game);

    auto org_index = registrations.get_index<"organization"_n>();
    // not existed, error
    eosio_assert(registration != org_index.cend(), "not found registered organization");

    auto game_index = games.get_index<"registration"_n>();
    auto game_index_lower = game_index.lower_bound(registration->id);
    auto game_index_upper = game_index.upper_bound(registration->id);

    while (game_index_lower != game_index_upper) {
        if (game_index_lower->user == user &&
            game_index_lower->target == target &&
            game_index_lower->duration == duration &&
            game_index_lower->finished == 0) {
            eosio_assert(game_index_lower->achievement == 0, "achievement already set");
            games.modify(*game_index_lower, get_self(),
                         [&](auto &g) {
                             g.achievement = achievement;
                             g.finished = 1;
                         });
            // TODO: add information to hero table, define a mechanism to reward winners
            // heroes.get_index()
            auto hero = heroes.find(user.value);
            if (hero == heroes.end()) {
                heroes.emplace(get_self(), [&](auto &h) {
                                               h.game_id = game_index_lower->id;
                                               h.user = game_index_lower->user;
                                               h.awards_eos = 0;
                                               h.awards_token = 0;
                                               h.acc_awards_eos = 0;
                                               h.acc_awards_token = 0;
                                               h.created_at = now();
                                               h.updated_at = now();
                                           });
            } else {
                heroes.modify(*hero, get_self(), [&](auto &h) {
                                                     h.awards_eos = 0;
                                                     h.awards_token = 0;
                                                     h.acc_awards_eos = 0;
                                                     h.acc_awards_token = 0;
                                                     h.updated_at = now();
                                                 });
            }

            // remove game entry
            break;
        }
        game_index_lower ++;
    }
}


#define EOSIO_DISPATCH2( TYPE, MEMBERS )                                \
    extern "C" {                                                        \
        void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
            /* https://mp.weixin.qq.com/s/1sV2ps1n6pPNb-0qiTXG9Q */     \
            if (code == receiver and  eosio::name(action) != "startgame"_n) { \
                switch ( action ) {                                     \
                    EOSIO_DISPATCH_HELPER( TYPE, MEMBERS);              \
                }                                                       \
            }                                                           \
            /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
            else if( eosio::name(code) == "eosio.token"_n && eosio::name(action) == "transfer"_n ) { \
                void* buffer = nullptr;                                 \
                size_t size = action_data_size();                       \
                constexpr size_t max_stack_buffer_size = 512;           \
                if( size > 0 ) {                                        \
                    buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size); \
                    read_action_data( buffer, size );                   \
                }                                                       \
                eosio::datastream<const char*> ds((char*)buffer, size); \
                TYPE inst(eosio::name(receiver), eosio::name(receiver), ds); \
                switch( eosio::name(action) ) {                         \
                case "transfer"_n:                                      \
                    return inst.startgame(eosio::name(code));           \
                break;                                                  \
                }                                                       \
            }                                                           \
            /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
        }                                                               \
    }                                                                   \


EOSIO_DISPATCH2(challenge,
                (enroll)
               // (startgame)
                (resolvegame)
                (endgame)
                (wrapper)
                (clear)
    )
