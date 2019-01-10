// #include <eosiolib/asset.hpp>
// #include <esoiolib/transaction.hpp>
#include "challenge.hpp"


auto challenge::get_registration(eosio::name org, eosio::name game) {
    auto org_index = registrations.get_index<"organization"_n>();
    auto org_index_lower = org_index.lower_bound(org.value);
    auto org_index_upper = org_index.upper_bound(org.value);
    while (org_index_lower != org_index_upper) {
        if (org_index_lower->game_name == game) {
            return org_index_lower;
        }
        org_index_lower ++;
    }
    eosio_assert(false, "not found registration");
}
std::map<std::string, std::string> challenge::parse_memo(std::string memo) {
    std::map<std::string, std::string> parameters;
    return parameters;
}
void challenge::enroll(eosio::name org, eosio::name game, uint64_t min_duration, uint64_t max_duration) {
    require_auth(org);
    registrations.emplace(get_self(), [&](auto &r) {
                                          r.id = registrations.available_primary_key();
                                          r.organization = org;
                                          r.game_name = game;
                                          r.min_duration = min_duration;
                                          r.max_duration = max_duration;
                                      });
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

        std::map<std::string, std::string> parameters = parse_memo(data.memo);
        eosio::name org = eosio::name(parameters["organization"]);
        eosio::name game = eosio::name(parameters["game"]);
        eosio::name referuser = eosio::name(parameters["referuser"]);
        uint64_t target = std::stoull(parameters["target"]);
        uint64_t bet = std::stoull(parameters["bet"]);
        uint64_t ts = std::stoull(parameters["ts"]);

        _startgame(data.from, org, game, target, bet, referuser, ts);
    }
}

void challenge::_startgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target,
                           uint64_t bet, eosio::name referuser, uint64_t ts) {
    require_auth(get_self());

    auto registration = get_registration(org, game);

    games.emplace(get_self(), [&](auto &g) {
                                  g.id = games.available_primary_key();
                                  g.registration_id = registration->id;
                                  g.user = user;
                                  g.refer_user = referuser;
                                  g.bet = bet;
                                  g.target = target;
                                  g.duration = -1;
                              });
    // genarate duration randomly again
}

void challenge::endgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target, uint64_t achievement,
                        uint64_t duration,
                        eosio::name referuser, uint64_t ts) {
    require_auth(get_self());
    auto registration = get_registration(org, game);

    auto game_index = games.get_index<"registration"_n>();
    auto game_index_lower = game_index.lower_bound(registration->id);
    auto game_index_upper = game_index.upper_bound(registration->id);

    while (game_index_lower != game_index_upper) {
        if (game_index_lower->user == user &&
            game_index_lower->target == target &&
            game_index_lower->duration == duration &&
            game_index_lower->finished == 0) {
            eosio_assert(game_index_lower->achievement == 0, "error happens");
            games.modify(*game_index_lower, get_self(),
                         [&](auto &g) {
                             g.achievement = achievement;
                             g.finished = 1;
                         });
            // TODO: add information to hero table
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
               (endgame)
    )
