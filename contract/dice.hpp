#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/transaction.hpp>

#include <boost/algorithm/string.hpp>

/* DOING:
 * TODO: auto-play
 * DONE: call random function
 *    - DONE: add PCG random function
 *    - DONE: toss a dice
 *    - DONE: random initalize a game
 *       - DONE: random initalize all goals
 *       - DONE: random initalize inital postion in a game
 * TODO: advoid overlapping of steps in map, verify in backend
 *    - features in next version
 * DONE: show waiting timestamp
 * DONE: validate route steps, total steps should be equal to dice number
 * DONE: security checking
 * https://www.bcskill.com/index.php/archives/516.html
 *    DONE: Delay transfer -> rollback
 *    DONE: fake EOS tokens
 *    DONE: random number generator, use tapos block number and tapos block prefix
 * https://www.bcskill.com/index.php/archives/516.html
 * DONE: seperate awards transaction and move transaction
 * DONE: show hero board
 * TODO: generate goals randomly
 * TODO: help user buy cpu/ram/net
 * TODO: random function review
 * TODO: test multiple actions per transaction
 * TODO: generate random number
 * DONE: efficiently schedule
 */


#define DEBUG 1

class [[eosio::contract]] dice : public eosio::contract {

public:
    using contract::contract;
    dice( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds),
        _games(receiver, code.value), _waitingpool(receiver, code.value),
        _scheduled_users(receiver, code.value), _rngtbl(receiver, code.value),
        _heroes(receiver, code.value), _winners(receiver, code.value)
    {}
private:
    struct game;

private:
    std::string _VERSION = "0.1.2";

    static constexpr uint8_t MAXGOALS = 10;
    static constexpr uint32_t MAXSIZE = 50;

    static constexpr float WINNER_PERCENT = 0.7;
    static constexpr float PARTICIPANTS_PERCENT = 0.1;
    static constexpr float PLATFORM_PERCENT  = 0.05;
    static constexpr float NEXT_GOAL_PERCENT = 0.05;
    static constexpr float LAST_GOAL_PERCENT = 0.05;
    static constexpr float DIVIDEND_POOL_PERCENT = 0.05;

    static constexpr eosio::name dividend_account = "arestest4321"_n;
    static constexpr eosio::name platform = "arestest1234"_n;
    static constexpr eosio::name lastgoal = "arestest1234"_n;

    static constexpr uint32_t GAME_CLOSE = 1;
    static constexpr uint32_t GAME_START = 2;
    static constexpr uint32_t GAME_OVER = 4;
    static constexpr uint32_t DELETED_GOAL = 0xffffffff;

    // static constexpr uint32_t TIMEOUT_USERS = 120;
    static constexpr uint32_t TIMEOUT_USERS = 172800;

    struct point {
        uint32_t row;
        uint32_t col;

        point(uint32_t _row, uint32_t _col) {
            row = _row;
            col = _col;
        }
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

    std::vector<point> centroids = {point(0, 0),
                                    point(9, 0),
                                    point(21, 0),
                                    point(13, 7),
                                    point(0, 9),
                                    point(21, 10),
                                    point(7, 13),
                                    point(15, 15),
                                    point(0, 21),
                                    point(10, 21),
                                    point(21, 21)};

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

    void inner_transfer(eosio::name from, eosio::name to, int64_t amount, uint32_t seed);

    TABLE game {

        uint64_t uuid;
        uint32_t board_width;
        uint32_t board_height;
        int64_t fee;

        uint32_t status;
        uint32_t pos;
        int64_t awards;         // the number of tokens we will distribute
        int64_t shadow_awards;  // the number of tokens we collect in this game
        uint64_t total_number;  // total number of users enter this game
        eosio::name gamename;
        std::vector<uint32_t> goals;

        uint64_t primary_key() const {
            return uuid;
        }
        uint64_t by_awards() const {
            return awards;
        }
        uint64_t by_shadow_awards() const {
            return shadow_awards;
        }
        void debug() const {
            eosio::print(">| ");
            eosio::print("uuid:", uuid, ", ");
            eosio::print("board_width:", board_width, ", ");
            eosio::print("board_height:", board_height, ", ");
            eosio::print("awards:", awards, ", ");
            eosio::print("shadow_awards:", shadow_awards, ", ");
            eosio::print("pos:", pos, ", ");
            eosio::print("status: ", status, ", ");
            point pt = point(pos);
            eosio::print("pos: (", pt.row, ", ", pt.col, "), ");
            for (uint32_t i = 0; i < goals.size(); i ++) {
                point pt = point(goals[i]);
                eosio::print("goal ", i, ": ", "(", pt.row, ", ", pt.col, "), ");
            }
            eosio::print("total_number: ", total_number, ", ");
            // eosio::print("gamename: ", gamename.c_str(), ", ");
            eosio::print(" |");
        }
    };
    TABLE users {
        uint64_t uuid;
        uint64_t gameuuid;
        uint32_t steps;
        uint128_t no;           // avoid overflow
        eosio::name user;
        time_t ts;
        time_t update_ts;
        uint64_t primary_key() const {
            return uuid;
        }

        uint64_t get_secondary_1() const { // gameuuid
            return gameuuid;
        }
        uint64_t get_secondary_2() const { // update_ts
            return (uint64_t)update_ts;
        }

        uint64_t get_secondary_3() const {
            return user.value;
        }
        void debug() const {
            eosio::print(">| ");
            eosio::print("uuid: ", uuid, ", ");
            eosio::print("game_uuid: ", gameuuid, ", ");
            eosio::print("no: ", no, ", ");
            eosio::print("user: ", user, ", ");
            eosio::print("steps: ", steps, ", ");
            eosio::print("timestamp: ", ts, ", ");
            eosio::print("update timestamp: ", update_ts, ", ");
            eosio::print(" |");
        }
        // EOSLIB_SERIALIZE(users, (uuid)(gameuuid)(update_ts));
    };

    TABLE hero {
        uint64_t uuid;
        uint64_t gameuuid;
        eosio::name user;
        int64_t awards;
        int64_t acc_awards;
        uint32_t row;
        uint32_t col;
        time_t ts;
        time_t update_ts;

        uint64_t primary_key() const {
            return uuid;
        }
        uint64_t get_secondary_1() const { // gameuuid
            return gameuuid;
        }
        uint64_t get_secondary_2() const { // awards
            return (uint64_t)awards;
        }
        uint64_t get_secondary_3() const { // acc_awards
            return (uint64_t)acc_awards;
        }
        uint64_t get_secondary_4() const { // update_ts
            return update_ts;
        }
        uint64_t get_secondary_5() const {
            return user.value;
        }
        void debug() const {
            eosio::print(">| ");
            eosio::print("ts: ", ts, ", ");
            eosio::print("update_ts: ", update_ts, ", ");
            eosio::print("user: ", user, ", ");
            eosio::print("gameuuid: ", gameuuid, ", ");
            eosio::print("awards: ", awards, ", ");
            eosio::print("acc_awards: ", acc_awards, ", ");
            eosio::print(" |");
        }
    };

public:

#ifdef DEBUG
    [[eosio::action]] void version();
    [[eosio::action]] void debug();
#endif

private:
    typedef eosio::multi_index<"gametbl"_n, game> gametable;
    gametable _games;

    typedef eosio::multi_index<"waittbl"_n, users,
                               eosio::indexed_by<"gameuuid"_n, eosio::const_mem_fun<users, uint64_t, &users::get_secondary_1>>,
                               eosio::indexed_by<"updatets"_n, eosio::const_mem_fun<users, uint64_t, &users::get_secondary_2>>,
                               eosio::indexed_by<"username"_n, eosio::const_mem_fun<users, uint64_t, &users::get_secondary_3>>
                               > usertable1;
    typedef eosio::multi_index<"schedtbl"_n, users,
                               eosio::indexed_by<"gameuuid"_n, eosio::const_mem_fun<users, uint64_t, &users::get_secondary_1>>,
                               eosio::indexed_by<"updatets"_n, eosio::const_mem_fun<users, uint64_t, &users::get_secondary_2>>,
                               eosio::indexed_by<"username"_n, eosio::const_mem_fun<users, uint64_t, &users::get_secondary_3>>
                               > usertable2;
    usertable1 _waitingpool;
    usertable2 _scheduled_users; // the users in this vector already get a line number, but not toss a dice
    typedef eosio::multi_index<"herotbl"_n, hero,
                               eosio::indexed_by<"gameuuid"_n, eosio::const_mem_fun<hero, uint64_t, &hero::get_secondary_1>>,
                               eosio::indexed_by<"awards"_n, eosio::const_mem_fun<hero, uint64_t, &hero::get_secondary_2>>,
                               eosio::indexed_by<"accawards"_n, eosio::const_mem_fun<hero, uint64_t, &hero::get_secondary_3>>,
                               eosio::indexed_by<"updatets"_n, eosio::const_mem_fun<hero, uint64_t, &hero::get_secondary_4>>,
                               eosio::indexed_by<"username"_n, eosio::const_mem_fun<hero, uint64_t, &hero::get_secondary_5>>
                               > herotbl;
    typedef eosio::multi_index<"winnertbl"_n, hero> winnertbl;
    herotbl _heroes;
    winnertbl _winners;
    // void update_heroes(struct users _user, int64_t awards) {
    void update_heroes(eosio::name user, uint64_t awards) {
        eosio::print("update heroes");
        auto h_it = _heroes.find(user.value);
        if (h_it == _heroes.cend()) {
            _heroes.emplace(get_self(), [&](auto &h) {
                                            h.uuid = user.value;
                                            h.user = user;
                                            h.awards = awards;
                                            h.acc_awards = awards;
                                            h.ts = now();
                                            h.update_ts = now();
                                        });
        } else {
            _heroes.modify(h_it, get_self(), [&](auto &h) {
                                                 h.awards = awards;
                                                 h.acc_awards += awards;
                                                 h.update_ts = now();
                                             });
        }
    }

public:
    // usertable latest_scheduled_users; // the users in this vector are scheduled just now
    [[eosio::action]] void addgame(eosio::name gamename, uint32_t width,
                                   uint32_t height, uint32_t status, int64_t fee);
#ifdef DEBUG
    [[eosio::action]] void clear();
#endif
    [[eosio::action]] void startgame(uint64_t gameuuid);
    [[eosio::action]] void closegame(uint64_t gameuuid);
    [[eosio::action]] void stopgame(uint64_t gameuuid);

    // [[eosio::action]] void enter(eosio::name user, uint64_t gameuuid); // enter a game, specified by game name
    [[eosio::action]] void enter(eosio::name user); // be sure that user is eosio.token
    // call it from offchain every 100 ms/1 s
    [[eosio::action]] void schedusers(uint64_t gameuuid, uint32_t total); // schedule users
    [[eosio::action]] void sched(eosio::name user, uint64_t gameuuid, time_t ts);

    [[eosio::action]] void toss(eosio::name user, uint64_t gameuuid, uint32_t seed);
    [[eosio::action]] void move(eosio::name user, uint64_t gameuuid, uint64_t steps);
    [[eosio::action]] void sendtokens(eosio::name user, uint64_t gameuuid);

    // [[eosio::action]] void delayid(uint128_t id);
    [[eosio::action]] void delayid(eosio::name user);
    // [[eosio::action]] void delayid();

    // [[eosio::action]] void getrnd(users _user);

private:
    void moveright(eosio::name user, uint64_t gameuuid, uint32_t steps);
    void moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps);
    void moveup(eosio::name user, uint64_t gameuuid, uint32_t steps);
    void movedown(eosio::name user, uint64_t gameuuid, uint32_t steps);

    auto get_game_by_uuid(uint64_t uuid) {
        auto _game = _games.find(uuid);
        auto end = _games.cend();

        return _game;
    }
    void update_game_pos(const game &_game, const point &pt) {
        _games.modify(_game, get_self(), [&](auto &g) {
                                             g.pos = pt.to_pos();
                                         });
    }

    auto is_sched_user_in_game(const eosio::name &user, const uint64_t gameuuid) {

        auto end = _scheduled_users.cend();
        for (auto _user = _scheduled_users.cbegin(); _user != end; _user ++) {
            if (_user->user == user &&
                _user->gameuuid == gameuuid) {
                return _user;
            }
        }
        return end;
    }
    auto is_won_user_in_game(const eosio::name &user, const uint64_t gameuuid) {
        auto cend = _winners.cend();
        for (auto _winner = _winners.cbegin(); _winner != cend; _winner ++) {
            if (_winner->user == user &&
                _winner->gameuuid == gameuuid) {
                return _winner;
            }
        }
        return cend;
    }
    // 1. check game status, over or continue, close, start,
    // 2. distribute tokens
    bool reach_goal(const game &_game);

    void distribute(const game& _game, const eosio::name& winner, std::vector<eosio::name> participants, uint64_t awards);
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
    auto prepare_movement(eosio::name user, uint64_t gameuuid, uint32_t steps) {
        require_auth(user);
        // 1. check user in the game
        auto _user = is_sched_user_in_game(user, gameuuid);
        bool valid_user_game = (_user != _scheduled_users.cend());
        eosio_assert(valid_user_game, "user not in game");
        // 2. check steps is smaller than or equal 6
        eosio_assert(steps > 0 &&steps <= 6, "invalid steps: > 6");
        // 3. check steps is equal to given steps
        auto _game = get_game_by_uuid(gameuuid);
        eosio_assert(_game->status == GAME_START, "game does not start");
        incr_game_awards(*_game, _game->fee);
        return _game;
    }

private:
    // random number generator
    struct pcg_state_setseq_64 {
        uint64_t uuid;
        uint64_t state = 0x853c49e6748fea9bULL;           // RNG state.  All values are possible.
        uint64_t inc = 0xda3e39cb94b95bdbULL;             // Controls which RNG sequence (stream) is
                                                          // selected. Must *always* be odd.
        // std::vector<uint64_t> incs;  // use to change the RNG sequence

        uint64_t primary_key() const {
            return uuid;
        }
        void debug() const {
            eosio::print(">| ");
            eosio::print("uuid: ", uuid, ", ");
            eosio::print("state: ", state, ", ");
            eosio::print("inc: ", inc, ", ");
            eosio::print(" |");
        }
    };
    typedef eosio::multi_index<"pcgrnd"_n, pcg_state_setseq_64> pcg32_random_t;
    pcg32_random_t _rngtbl;

    void pcg32_srandom_r(uint64_t initstate, uint64_t initseq) {
        // add a random engine
        uint64_t block_num = (uint64_t)tapos_block_num();
        uint64_t block_prefix = (uint64_t)tapos_block_prefix();
        eosio::print("block number:", block_num, ", ");
        eosio::print("block_prefix: ", block_prefix, ", ");

        initstate = block_num << 32 | block_prefix;
        initseq = now();

        auto _rng_it = _rngtbl.begin();
        if (_rng_it == _rngtbl.end()) {
            _rng_it = _rngtbl.emplace(get_self(), [&](auto &r) {
                                                      r.state = 0U;
                                                      r.inc = (initseq << 1u) | 1u;
                                                  });
        } else {
            _rngtbl.modify(_rng_it, get_self(), [&](auto &r) {
                                                    r.state = 0U;
                                                    r.inc = (initseq << 1u) | 1u;
                                                });
        }
        // call pcg32_random_r
        uint64_t oldstate = _rng_it->state;
        _rngtbl.modify(_rng_it, get_self(), [&](auto &r) {
                                                r.state = r.state * 6364136223846793005ULL + r.inc;
                                                r.state += initstate;
                                                r.state = r.state * 6364136223846793005ULL + r.inc;
                                            });
    }

    uint32_t pcg32_random_r() {
        auto _rng_it = _rngtbl.begin();
        if (_rng_it == _rngtbl.end()) {
            eosio_assert(false, "no random generator engine");
        }
        uint64_t oldstate = _rng_it->state;
        uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
        uint32_t rot = oldstate >> 59u;
        uint32_t r = (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        _rngtbl.modify(_rng_it, get_self(), [&](auto &r) {
                                                r.state = oldstate * 6364136223846793005ULL + r.inc;
                                                // variant, idea is to change `inc` according to previous random list
                                                // but it's meaningful ?
                                            });
        return r;
    }
    uint32_t pcg32_boundedrand_r(uint32_t bound) {
        uint32_t threshold = -bound % bound;
        uint64_t loops = 0;
        for (;;) {
            uint32_t r = pcg32_random_r();
            loops ++;
            if (r >= threshold) {
                return r % bound;
            }
        }
    }

    static constexpr uint64_t initseq = 0x42;
    // NOTE: if we fix `initstate` and `initseq`, hacker can generate the whole random sequence if he steals `initstate` and `initseq`
    // However, if we don't fix `initstate` and use current timestamp as `initstate`. Again, there is a problem a hacker can also generate
    // a sequence of random values in advance.
    // Here we didn't discuss a hacker can toss whatever a dice number he wants.
    uint32_t get_rnd_dice_number() {
        pcg32_srandom_r(now(), initseq);
        return pcg32_boundedrand_r(6) + 1;
    }
    uint32_t get_rnd_game_pos(uint32_t board_width, uint32_t board_height) {

        pcg32_srandom_r(now(), initseq);
        uint32_t row = pcg32_boundedrand_r(board_height);
        uint32_t col = pcg32_boundedrand_r(board_width);
        point pt = point(row, col);
        return pt.to_pos();
    }
    // TODO:
    void set_game_goals(game &_game) {
        uint32_t pos = _game.pos;
        point pt = point(pos);

    }
    uint64_t extract_gameuuid_from_memo(std::string memo) {
        std::vector<std::string> results;
        boost::split(results, memo, [](char c){return c == ':';});
        uint64_t gameuuid = std::stoull(results[1]);
        return gameuuid;
    }
};
