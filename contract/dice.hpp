#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>


#define DEBUG
/* TODO:
 * TODO: entering game, transfer tokens to our platform
 * TODO: put into a waiting pools, get
 * TODO: call random function
 * TODO:
 * TODO:
 */

class [[eosio::contract]] dice : public eosio::contract {

public:
    using contract::contract;
    dice( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds),
        _games(receiver, code.value), _waitingpool(receiver, code.value),
        scheduled_users(receiver, code.value)
    {}
    struct game;

private:
    std::string _VERSION = "0.0.2";
    // TODO:
    int random();

    static const uint8_t RIGHT = 0;
    static const uint8_t LEFT = 1;
    static const uint8_t UP = 2;
    static const uint8_t DOWN = 3;
    static const uint8_t MAXGOALS = 10;
    static const int64_t FEE = 10000; // 1 EOS
    static const uint32_t GAME_CLOSE = 1;
    static const uint32_t GAME_START = 2;
    static const uint32_t GAME_CONTINUE = 4;
    static const uint32_t GAME_OVER = 8;

    // enum direction : uint8_t {
    //     RIGHT = 0,
    //     LEFT = 1,
    //     UP = 2,
    //     DOWN = 3
    // };

    // enum postype : uint8_t {
    //     GOAL = 0,
    //     AVAIABLE = 1,
    //     BARRIER = 2
    // };

    struct point {
        uint32_t row;
        uint32_t col;

        point(uint32_t pos) {
            row = (pos & 0xffff0000) >> 16;
            col = (pos & 0x0000ffff);
        }
        uint32_t to_pos() const {
            return (((row & 0x0000ffff) << 16) | (col & 0x0000ffff));
        }

        void debug () const {
            eosio::print(">| ");
            eosio::print("row: ", row, ", col: ", col);
            eosio::print(" |");
        }
    };

    static void set_game_pos(game &_game, point &pt) {
        _game.pos = pt.to_pos();
    }
    bool is_valid_steps (uint16_t steps) {
        if (steps >= 1 && steps <= 6 ) {
            return true;
        }
        return false;
    }
    bool is_valid_pos(const game &_game, const point &pt) {
        // point pt = point(_game.pos);
        if (pt.row < 0 ||
            pt.row >= _game.board_width ||
            pt.col < 0 ||
            pt.col >= _game.board_height) {
            return false;
        }
        return true;
    }

    // TODO: mark it as private
    void transfer(eosio::name from, eosio::name to, int64_t amount);
    // std::map<uint64_t, std::string> gamename_uuid_map;


public:

    [[eosio::action]] void version();
    [[eosio::action]] void debug();

    TABLE game {

        uint64_t uuid;
        uint32_t pos;
        uint32_t status;
        // std::string gamename;
        std::vector<uint32_t> goals;
        static const uint32_t board_width = 30;
        static const uint32_t board_height = board_width ;

        uint64_t primary_key() const {
            return uuid;
        }
        // std::string get_gamename() const {
        //     return gamename;
        // }
        void debug() const {
            eosio::print(">| ");
            eosio::print("uuid:", uuid, ", ");
            // eosio::print("gamename:", gamename.c_str(), ", ");
            eosio::print("board_width:", board_width, ", ");
            eosio::print("board_height:", board_height, ", ");
            eosio::print("pos:", pos, ", ");
            eosio::print("status: ", status, ", ");
            point pt = point(pos);
            // t.debug();
            eosio::print("row: ", pt.row, ", ");
            eosio::print("col: : ", pt.col, ", ");
            eosio::print(" |");
        }
    };

    typedef eosio::multi_index<"game"_n, game> gametable;
    gametable _games;

    TABLE users {
        uint64_t uuid;
        uint64_t gameuuid;
        uint32_t steps;
        uint128_t no;           // avoid overflow
        eosio::name user;
        eosio::block_timestamp ts;
        uint64_t primary_key() const {
            return uuid;
        }
        uint64_t by_gameuuid () const {
            return gameuuid;
        }

        void debug() const {
            eosio::print(">| ");
            eosio::print("uuid: ", uuid, ", ");
            eosio::print("game_uuid: ", gameuuid, ", ");
            eosio::print("no: ", no, ", ");
            eosio::print("user: ", user, ", ");
            eosio::print("steps: ", steps, ", ");
            // eosio::print("ts: ", ts, ", ");
            eosio::print(" |");
        }
    };

    typedef eosio::multi_index<"users"_n, users, eosio::indexed_by<"gammeuuid"_n, eosio::const_mem_fun<users, uint64_t, &users::by_gameuuid>>> usertable;
    usertable _waitingpool;

    usertable scheduled_users; // the users in this vector already get a line number, but not toss a dice
    // usertable latest_scheduled_users; // the users in this vector are scheduled just now

    // [[eosio::action]] void addgame(std::string gamename);
    [[eosio::action]] void addgame();
    [[eosio::action]] void startgame(uint64_t gameuuid);
    [[eosio::action]] void enter(eosio::name user, uint64_t gameuuid); // enter a game, specified by game name
    [[eosio::action]] void schedusers(uint64_t gameuuid, uint64_t total); // schedule users
    [[eosio::action]] void getusers() const;         // extract latest scheduled users information

    [[eosio::action]] void moveright(eosio::name user, uint64_t gameuuid, uint32_t steps);
    [[eosio::action]] void moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps);
    [[eosio::action]] void moveup(eosio::name user, uint64_t gameuuid, uint32_t steps);
    [[eosio::action]] void movedown(eosio::name user, uint64_t gameuuid, uint32_t steps);

private:
    auto get_game_by_uuid(uint64_t uuid) {
        auto _game = _games.find(uuid);
        auto end = _games.cend();
        // if (_game == end) {
        //     eosio_assert(false, "not found game");
        // }
        return _game;
    }
    void update_game_pos(const game &_game, const point &pt) {
        _games.modify(_game, get_self(), [&](auto &g) {
                                             g.pos = pt.to_pos();
                                         });
    }
    // std::vector<users> _get_user_by_username(const eosio::name user) { // we only need users in different game

    // }
    bool is_user_in_game(const eosio::name &user, const uint64_t gameuuid) {
        for (auto _user : scheduled_users) {
            if (_user.user == user &&
                _user.gameuuid == gameuuid) {
                return true;
            }
        }
        return false;
    }

    // 1. check game status, over or continue, close, start,
    // 2. distribute tokens
    void update_game_status(uint64_t gameuuid);

    // auto get_game_by_username(const eosio::name user, uint64_t gameuuid) {
    //     std::vector<users> _users_vec;
    //     // std::set<uint64_t> book_keeping;
    //     for (auto _user : scheduled_users) {
    //         // auto bk = book_keeping.find(_user.game_uuid);
    //         // if (bk != book_keeping.end()) {
    //         //     continue;
    //         // }
    //         if (_user.user == user &&
    //             _user.gameuuid == gameuuid) {
    //             _users_vec.push_back(_user);
    //             // book_keeping.insert(_user.game_uuid);
    //         }
    //     }
    //     if (_users_vec.size() == 0) {
    //         eosio_assert(false, "not found user");
    //     }
    //     return _users_vec;


    //     // std::vector<users> _users_vec = _get_user_by_username(user);
    //     // for (auto _user : _users_vec) {
    //     //     _user.game_uuid
    //     // }
    // }
};
