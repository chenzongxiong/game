#pragma once
#include <eosiolib/eosio.hpp>

/* TODO:
 * 1. entering game, transfer tokens to our platform
 * 2. put into a waiting pools, get
 * 3. call random function
 *
 */
/*
    action(permission_level{ from, N(active) },
           N(eosio.token), N(transfer),
           std::make_tuple(from, self, price, std::string(""))
    ).send();
 */

class [[eosio::contract]] dice : public eosio::contract {

public:
    using contract::contract;
    dice( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds), _games(receiver, code.value) {}
    struct game;

private:
    std::string _VERSION = "0.0.2";
    // TODO:
    int random();

    // static const uint8_t RIGHT = 0;
    // static const uint8_t LEFT = 1;
    // static const uint8_t UP = 2;
    // static const uint8_t DOWN = 3;
    static const uint8_t MAXGOALS = 10;

    enum direction : uint8_t {
        RIGHT = 0,
        LEFT = 1,
        UP = 2,
        DOWN = 3
    };

    enum postype : uint8_t {
        GOAL = 0,
        AVAIABLE = 1,
        BARRIER = 2
    };

    // static uint32_t get_row(uint32_t pos) {
    //     return (pos & 0xffff0000) >> 16;
    // }
    // static uint32_t get_col(uint32_t pos) {
    //     return (pos & 0x0000ffff);
    // }
    struct point {
        uint32_t row;
        uint32_t col;

        point(uint32_t pos) {
            row = (pos & 0xffff0000) >> 16;
            col = (pos & 0x0000ffff);
        }
        uint32_t to_pos() {
            return (((row & 0x0000ffff) << 16) | (col & 0x0000ffff));
        }
    };

    static void set_game_pos(game &_game, point &pt) {
        _game.pos = pt.to_pos();
    }

    // struct users {
    //     eosio::name user;
    //     eosio::block_timestamp ts;
    //     uint256_t no;           // avoid overflow
    // };
    // uint256_t total_users = 0;

    bool is_valid_steps(uint16_t steps) {
        if (steps >= 1 && steps <= 6 ) {
            return true;
        }
        return false;
    }

public:

    [[eosio::action]] void version();
    [[eosio::action]] void debug();
    // TODO: mark it as private
    [[eosio::action]] void transfer();

    TABLE game {

        uint64_t uuid;
        uint32_t pos;
        std::string game_name;
        std::vector<uint32_t> goals;
        static const uint32_t board_width = 30;
        static const uint32_t board_height = board_width ;

        uint64_t primary_key() const {
            return uuid;
        }

        void debug() const {
            eosio::print("uuid:", uuid, ", ");
            eosio::print("game_name:", game_name.c_str(), ", ");
            eosio::print("board_width:", board_width, ", ");
            eosio::print("board_height:", board_height, ", ");
            eosio::print("pos:", pos, ", ");
            // eosio::print("row: ", get_row(pos), ", ");
            // eosio::print("col: : ", get_col(pos), ", ");
            eosio::print(">>>>>>>>>>>>>>>>>>>>>>>>>");
        }
    };

    typedef eosio::multi_index<"game"_n, game> gametable;
    gametable _games;

    [[eosio::action]] void addgame(std::string gamename);
    [[eosio::action]] void enter(eosio::name user, std::string room);
    [[eosio::action]] void moveright(uint16_t steps);
    [[eosio::action]] void moveleft(uint16_t steps);
    [[eosio::action]] void moveup(uint16_t steps);
    [[eosio::action]] void movedown(uint16_t steps);
};
