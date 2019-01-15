#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/crypto.hpp>

struct eosio_token_transfer {
    eosio::name from;
    eosio::name to;
    eosio::asset quantity;
    std::string memo;
};

class [[eosio::contract]] challenge : public eosio::contract {
public:
    using contract::contract;
    challenge (eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds) : eosio::contract(receiver, code, ds),
        registrations(receiver, code.value),
        games(receiver, code.value),
        heroes(receiver, code.value)
    {}


private:
    TABLE st_registration {
        uint64_t id;
        eosio::name organization;
        eosio::name game_name;
        uint64_t min_duration;
        uint64_t max_duration;
        uint64_t primary_key() const {
            return id;
        }
        uint64_t by_max_duration() const {
            return max_duration;
        }
        uint64_t by_min_duration() const {
            return min_duration;
        }
        uint64_t by_game_name() const {
            return game_name.value;
        }
        uint64_t by_organization() const {
            return organization.value;
        }
        void jsonify() const {
            eosio::print("{");
            eosio::print("\"organization\":\"", organization, "\", ");
            eosio::print("\"game\":\"", game_name, "\", ");
            eosio::print("\"min_duration\":", min_duration, ", ");
            eosio::print("\"max_duration\":", max_duration);
            eosio::print("}");
        }
    };
    TABLE st_game {
        uint64_t id;
        uint64_t registration_id;
        eosio::name user;
        eosio::name refer_user;

        uint64_t target;
        uint64_t achievement;

        uint64_t duration;      // generate by contract
        uint64_t bet;
        eosio::checksum256 seed;
        uint8_t finished;
        uint64_t primary_key() const {
            return id;
        }
        uint64_t by_organization() const {
            return registration_id;
        }
        uint64_t by_user() const {
            return user.value;
        }

    };
    TABLE st_hero {
        uint64_t id;
        uint64_t game_id;
        eosio::name user;
        uint64_t awards_eos;
        uint64_t awards_token;
        uint64_t acc_awards_eos;
        uint64_t acc_awards_token;
        time_t created_at;
        time_t updated_at;
        uint64_t primary_key() const {
            // return id;
            return user.value;
        }
        uint64_t by_game_id() const {
            return game_id;
        }
        uint64_t by_awards_eos() const {
            return awards_eos;
        }
        // uint64_t by_user() const {
        //     return user.value;
        // }
    };
    struct st_seed {
        eosio::checksum256 seed1;
        eosio::checksum256 seed2;
    };
public:
    typedef eosio::multi_index<"registration"_n, st_registration,
                               eosio::indexed_by<"organization"_n, eosio::const_mem_fun<st_registration, uint64_t, &st_registration::by_organization>>,
                               eosio::indexed_by<"game"_n, eosio::const_mem_fun<st_registration, uint64_t, &st_registration::by_game_name>>,
                               eosio::indexed_by<"maxduration"_n, eosio::const_mem_fun<st_registration, uint64_t, &st_registration::by_max_duration>>,
                               eosio::indexed_by<"minduration"_n, eosio::const_mem_fun<st_registration, uint64_t, &st_registration::by_min_duration>>
                               > registration_table;

    registration_table registrations;
    typedef eosio::multi_index<"game"_n, st_game,
                               eosio::indexed_by<"registration"_n, eosio::const_mem_fun<st_game, uint64_t, &st_game::by_organization>>
                               > game_table;
    game_table games;

    typedef eosio::multi_index<"hero"_n, st_hero
                               // eosio::indexed_by<"user"_n, eosio::const_mem_fun<st_hero, uint64_t, &st_hero::by_user>>
                               >  hero_table;
    hero_table heroes;

public:
    [[eosio::action]] void enroll(eosio::name org, eosio::name game, uint64_t min_duration, uint64_t max_duration);

    [[eosio::action]] void startgame(eosio::name user);

    [[eosio::action]] void resolvegame(uint64_t game_id, eosio::signature sig, eosio::public_key pubkey);

    [[eosio::action]] void endgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target,
                                   uint64_t achievement, uint64_t duration, eosio::name referuser, uint64_t ts);

    [[eosio::action]] void sendtokens(eosio::name user);

    [[eosio::action]] void wrapper(eosio::name org, eosio::name game);
    [[eosio::action]] void clear();
private:
    void _startgame(eosio::name user,eosio::name org, eosio::name game,
                    uint64_t target, uint64_t bet, eosio::name referuser, uint64_t ts,
                    std::string user_lucky_number);

    auto get_registration(eosio::name org, eosio::name game);
    static constexpr eosio::name admin = "cardgameacc"_n;
};
