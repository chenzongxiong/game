#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include "pcg-c-basic-0.9/pcg_basic.h"

/* DOING:
 * TODO: auto-play
 * DONE: entering game, transfer tokens to our platform
 * DONE: put into a waiting pools, get
 * TODO: call random function
 * TODO: adoid overlapping of steps in map
 * TODO: show waiting timestamp
 * TODO: validate route steps
 * TODO: security checking
 *    TODO: Delay transfer -> rollback
 *    TODO: fake EOS tokens
 *    TODO: random number generator ?
 */

class [[eosio::contract]] dice : public eosio::contract {

public:
    using contract::contract;
    dice( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds),
        _games(receiver, code.value), _waitingpool(receiver, code.value),
        scheduled_users(receiver, code.value), _rngtbl(receiver, code.value)
    {}
    struct game;

private:
    std::string _VERSION = "0.1.2";


    static constexpr uint8_t MAXGOALS = 10;
    static constexpr int64_t FEE = 10000; // 1 EOS
    static constexpr float WINNER_PERCENT = 0.7;
    static constexpr float PARTICIPANTS_PERCENT = 0.05;

    // static constexpr uint8_t RIGHT = 0;
    // static constexpr uint8_t LEFT = 1;
    // static constexpr uint8_t UP = 2;
    // static constexpr uint8_t DOWN = 3;

    static constexpr uint32_t GAME_CLOSE = 1;
    static constexpr uint32_t GAME_START = 2;
    // static constexpr uint32_t GAME_CONTINUE = 4;
    static constexpr uint32_t GAME_OVER = 8;
    static constexpr uint32_t DELETED_GOAL = 0xffffffff;

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

    // static void set_game_pos(game &_game, point &pt) {
    //     _game.pos = pt.to_pos();
    // }
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

    TABLE game {

        uint64_t uuid;
        uint32_t pos;
        uint32_t status;
        int64_t awards;         // the number of tokens we will distribute
        int64_t shadow_awards;  // the number of tokens we collect in this game
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
    TABLE users {
        uint64_t uuid;
        uint64_t gameuuid;
        uint32_t steps;
        uint128_t no;           // avoid overflow
        eosio::name user;
        // eosio::block_timestamp ts;
        time_t ts;
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
            eosio::print("timestamp: ", ts, ", ");
            eosio::print(" |");
        }
    };
    TABLE rndgenerator {
        uint64_t uuid;
        pcg32_random_t rng;
        uint64_t primary_key() const {
            return uuid;
        }
    };

public:

    [[eosio::action]] void version();
    [[eosio::action]] void debug();

    typedef eosio::multi_index<"game"_n, game> gametable;
    gametable _games;

    typedef eosio::multi_index<"users1"_n, users> usertable1;
    typedef eosio::multi_index<"users2"_n, users> usertable2;
    usertable1 _waitingpool;
    usertable2 scheduled_users; // the users in this vector already get a line number, but not toss a dice

    typedef eosio::multi_index<"rng"_n, rndgenerator> rngtable;
    rngtable _rngtbl;
    // TODO: random number generator urging

    // usertable latest_scheduled_users; // the users in this vector are scheduled just now
    [[eosio::action]] void addgame();
    [[eosio::action]] void startgame(uint64_t gameuuid);

    [[eosio::action]] void enter(eosio::name user, uint64_t gameuuid); // enter a game, specified by game name
    // call it from offchain every 100 ms/1 s
    [[eosio::action]] void schedusers(uint64_t gameuuid, uint64_t total); // schedule users

    [[eosio::action]] void toss(eosio::name user, uint64_t gameuuid);
    [[eosio::action]] void moveright(eosio::name user, uint64_t gameuuid, uint32_t steps);
    [[eosio::action]] void moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps);
    [[eosio::action]] void moveup(eosio::name user, uint64_t gameuuid, uint32_t steps);
    [[eosio::action]] void movedown(eosio::name user, uint64_t gameuuid, uint32_t steps);


    [[eosio::action]] void getusers() const; // extract latest scheduled users information, users to toss dices
    [[eosio::action]] void getawards();    // get current awards
    [[eosio::action]] void getshaawards(); // get shadow awards

    // TODO:
    [[eosio::action]] void setfee(int64_t fee); // set fee for current games
    [[eosio::action]] void setwidth(uint32_t w);
    [[eosio::action]] void setheight(uint32_t h);

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
    auto is_user_in_game(const eosio::name &user, const uint64_t gameuuid) {

        auto end = scheduled_users.cend();
        for (auto _user = scheduled_users.cbegin(); _user != end; _user ++) {
            if (_user->user == user &&
                _user->gameuuid == gameuuid) {
                return _user;
            }
        }
        return end;
    }
    // 1. check game status, over or continue, close, start,
    // 2. distribute tokens
    bool reach_goal(const game &_game);
    void distribute(const game& _game, const users& winner, std::vector<users> participants);
    void close_game(const game &_game) {
        for (auto _goal : _game.goals) {
            if (_goal != DELETED_GOAL) {
                // still has some goals, open it
                return;
            }
        }

        _games.modify(_game, get_self(), [&](auto &g) {
                                             g.status = GAME_OVER;
                                         });
    }
    void incr_game_shadow_awards(const game &_game, const int64_t fee) {
        _games.modify(_game, get_self(), [&](auto &g){
                                             g.shadow_awards += fee;
                                         });
    }
    void desc_game_shadow_awards(const game &_game, const int64_t fee) {
        _games.modify(_game, get_self(), [&](auto &g){
                                             g.shadow_awards -= fee;
                                         });
    }
    void incr_game_awards(const game &_game, const int64_t fee) {
        _games.modify(_game, get_self(), [&](auto &g){
                                             g.awards += fee;
                                         });
    }
    void desc_game_awards(const game &_game, const int64_t fee) {
        _games.modify(_game, get_self(), [&](auto &g){
                                             g.awards -= fee;
                                         });
    }
    auto prepare_movement(eosio::name user, uint64_t gameuuid) {
        // check authorization of user ?
        require_auth(get_self());
        auto _user = is_user_in_game(user, gameuuid);

        bool valid_user_game = (_user != scheduled_users.cend());

        eosio_assert(valid_user_game, "user not in game");
        auto _game = get_game_by_uuid(gameuuid);
        eosio_assert(_game->status == GAME_START, "game does not start");
        incr_game_awards(*_game, FEE);
        return _game;
    }
    // TODO: urging
    void set_game_goals(game &_game) {

    }
    uint32_t get_rnd_number(uint32_t bound);
};
