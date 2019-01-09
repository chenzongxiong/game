#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>

class [[eosio::contract]] challenge : public eosio::contract {
public:
    using contract::contract;
    challenge (eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds) : eosio::contract(receiver, code, ds),
        registrations(receiver, code.value),
        games(receiver, code.value)
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
        uint64_t awards_eos;
        uint64_t awards_token;

        uint64_t primary_key() const {
            return id;
        }
        uint64_t by_game_id() const {
            return game_id;
        }
        uint64_t by_awards_eos() const {
            return awards_eos;
        }
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

public:
    [[eosio::action]] void enroll(eosio::name org, eosio::name game, uint64_t min_duration, uint64_t max_duration);
    [[eosio::action]] void startgame(eosio::name user,eosio::name org, eosio::name game,
                                     uint64_t target, uint64_t bet, eosio::name referuser, uint64_t ts);
    [[eosio::action]] void endgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target,
                                   uint64_t achievement, uint64_t duration, eosio::name referuser, uint64_t ts);
    [[eosio::action]] void sendtokens(eosio::name user);

private:
    auto get_registration(eosio::name org, eosio::name game);
};
