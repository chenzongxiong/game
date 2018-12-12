#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include "./eosio.token/include/eosio.token/eosio.token.hpp"
#include "dice.hpp"


struct eosio_token_transfer {
    eosio::name from;
    eosio::name to;
    eosio::asset quantity;
    std::string memo; // when comment out, it works fine
};

// action
void dice::version() {
    eosio::print(_VERSION.c_str(), ", self: ", get_self());
}
/********************************************************************************
 * administrator functions
 ********************************************************************************/
// action
void dice::setfee(int64_t fee) {
    require_auth(get_self());
    FEE = fee;
}
void dice::setwidth(uint32_t w) {
    require_auth(get_self());
    eosio_assert(w >= 6, "error: w < 6");
    GAMEBOARD_WIDTH = w;
}
void dice::setheight(uint32_t h) {
    require_auth(get_self());
    eosio_assert(h >= 6, "error: h < 6");
    GAMEBOARD_HEIGHT = h;
}
// action
void dice::debug() {
    require_auth(get_self());
    eosio::print(">>>>>>>>>>>>>>>>>>>>SEED>>>>>>>>>>>>>>>>>>>>");
    eosio::print("now: ", now(), ", ");
    // auto info = get_info();
    uint32_t block_num = tapos_block_num();
    uint32_t block_prefix = tapos_block_prefix();

    eosio::print("block number: ", block_num, ", ");
    eosio::print("block prefix: ", block_prefix, ", ");

    eosio::print(">>>>>>>>>>>>>>>>>>>>GAME>>>>>>>>>>>>>>>>>>>>");
    for (auto &_game : _games) {
        _game.debug();
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>POINT>>>>>>>>>>>>>>>>>>>>");
    for (auto &_game : _games) {
        point pt = point(_game.pos);
        pt.debug();
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>WAITING USERS>>>>>>>>>>>>>>>>>>>>");
    for (auto &_user : _waitingpool) {
        _user.debug();
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>SCHEDING USERS>>>>>>>>>>>>>>>>>>>>");
    for (auto &_user : _scheduled_users) {
        _user.debug();
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>random>>>>>>>>>>>>>>>>>>>>");
    // pcg32_srandom_r(now(), 1);
    pcg32_srandom_r(42u, 54u);
    // uint32_t rounds = 5;
    // for (uint32_t round = 1; round <= rounds; ++ round) {
    //     eosio::print("Round ", round, ", ");
    //     for (uint32_t i = 0; i < 33; i ++) {
    //         eosio::print(pcg32_boundedrand_r(6) + 1, ", ");
    //     }
    //     eosio::print("====================");
    // }
    eosio::print(">>>>>>>>>>>>>>>>>>>>rng engine>>>>>>>>>>>>>>>>>>>>");
    for (auto &_rng : _rngtbl) {
        _rng.debug();
    }
}
// action
// void dice::addgame(uint32_t board_width, uint32_t board_height) {
void dice::addgame() {
    require_auth(get_self());

    uint32_t board_width = GAMEBOARD_WIDTH;
    uint32_t board_height = GAMEBOARD_HEIGHT;

    eosio_assert(MAXSIZE > board_width, "width > MAXSIZE");
    eosio_assert(MAXSIZE > board_height, "height > MAXSIZE");
    // uint32_t pos = get_rnd_game_pos(board_width, board_height);
    pcg32_srandom_r(now(), initseq);
    // 1. randomly pick a point as initalized points
    uint32_t idx = pcg32_boundedrand_r(centroids.size());
    point pt = centroids[idx];
    uint32_t pos = pt.to_pos();
    // 2. the rest set as goals
    std::vector<uint32_t> goals;
    for (uint32_t i = 0; i < centroids.size(); i ++) {
        if (i == idx) {
            continue;
        }
        goals.push_back(centroids[i].to_pos());
    }
    _games.emplace(get_self(), [&](auto &g) {
                                   g.uuid = _games.available_primary_key();
                                   g.pos = pos;
                                   g.status = GAME_CLOSE;
                                   g.awards = 0;
                                   g.shadow_awards = 0;
                                   for (auto _gl : goals) {
                                       g.goals.push_back(_gl);
                                   }
                               });
}
// action
void dice::startgame(uint64_t gameuuid) {
    require_auth(get_self());
    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "game not found");

    _games.modify(_game, get_self(), [&](auto &g){
                                         g.status = GAME_START;
                                     });

}

// action
void dice::schedusers(uint64_t gameuuid, uint32_t total) {
    // 1. authorization, only platform could run schedulers
    // 2. random pick <total> users in waiting pool
    // 3. put them into scheduling users vector
    // 4. erase them from waitingpool

    require_auth(get_self());
    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "bug ? game not found.");
    eosio_assert(_game->status == GAME_START, "bug ? game does not start");

    // multiple user in multiple games
    // how many people in game gameuuid
    std::vector<users> latest_scheduling_users;
    for (auto _user : _waitingpool) {
        if (_user.gameuuid == gameuuid) {
            latest_scheduling_users.push_back(_user);
        }
    }

    if (latest_scheduling_users.size() < total) {
        total = latest_scheduling_users.size();
    }

    uint32_t i = 0;
    std::vector<users> latest_scheduled_users;
    std::set<uint64_t> book_keep;

    while (i < total) {         // forever looping ?
        // randomly pick one number
        uint64_t idx = pcg32_boundedrand_r(total);
        auto idx_it = book_keep.find(idx);

        if (idx_it != book_keep.end()) {
            // eosio::print("idx: ", idx, ", skip");
            continue;

        }
        book_keep.insert(idx);

        auto _user = latest_scheduling_users[idx];

        eosio_assert(_user.gameuuid == gameuuid, "bug?");
        // TODO: primary key of users in waitingpool can be overlapping
        _scheduled_users.emplace(get_self(), [&](auto &u) {
                                                 // u.uuid = _user.uuid; // make user every user only scheduled once
                                                 u.gameuuid = _user.gameuuid;
                                                 u.steps = _user.steps;
                                                 u.no = _scheduled_users.available_primary_key();
                                                 u.user = _user.user;
                                                 u.ts = _user.ts;
                                             });
        latest_scheduled_users.push_back(_user);
        // erase them from waitingpool since they are scheduled
        // _waitingpool.erase(_user);
        auto u_it = _waitingpool.find(_user.uuid);
        _waitingpool.erase(u_it);

        i ++;
    }
#ifdef DEBUG
    eosio::print(">>>>>>>>>>>>>>>>>>>>SCHEDING USERS>>>>>>>>>>>>>>>>>>>>");
    for (auto &_user : _scheduled_users) {
        _user.debug();
    }
#endif
}


/********************************************************************************
 * Users functions
 ********************************************************************************/
// action
void dice::enter(eosio::name user) {
    /**
     * NOTE: a user can enter game multiple times
     * 1. tranfer fee
     * 2. put him/her in waiting room
     * 3. increase shadow awards pool
     */
    eosio_assert(user == "eosio.token"_n, "faks tokens");
    auto data = eosio::unpack_action_data<eosio_token_transfer>();
    if (data.to != get_self()) {
        return;
    }
    eosio_assert(data.quantity.is_valid(), "invalid quantity");
    eosio_assert(data.quantity.amount > 0, "must transfer positive quantity");
    eosio_assert(data.quantity.symbol == eosio::symbol("EOS", 4), "symbol precision mismatch");
    eosio_assert(data.memo.size() <= 256, "memo has more than 256 bytes");

    if (data.quantity.amount < FEE) {
        // eosio_assert( quantity.amount == FEE, "must transfer positive quantity" );
        return;
    }

    uint64_t gameuuid = extract_gameuuid_from_memo(data.memo);
    auto _game = get_game_by_uuid(gameuuid);
    // check game
    eosio_assert(_game != _games.cend(), "not found game.");
    eosio_assert(_game->status != GAME_OVER, "game is over");

    incr_game_shadow_awards(*_game, FEE);
    _waitingpool.emplace(get_self(), [&](auto &u) {
                                         u.uuid= _waitingpool.available_primary_key();
                                         u.gameuuid = _game->uuid;
                                         u.no = -1;
                                         u.user = data.from;
                                         u.steps = 0;
                                         u.ts = now();
                                    });

}
// action
void dice::toss(eosio::name user, uint64_t gameuuid) {
    // 1. user authentication
    // 2. randomly generate dice number
    // 3. assign that number to that user
    require_auth(user);

    auto end = _scheduled_users.cend();
    for (auto _user = _scheduled_users.cbegin(); _user != end; _user ++) {
        if (_user->user == user &&
            _user->gameuuid == gameuuid &&
            _user->steps == 0) {
            // eosio_assert(_user->steps == 0, "already assign steps to this user");
            uint32_t dice_number = get_rnd_dice_number();
#ifdef DEBUG
            eosio::print("dice number: ", dice_number);
#endif
            _scheduled_users.modify(_user, get_self(), [&](auto &u) {
                                                           u.steps = dice_number;
                                                       });
        }
    }
}

// action
void dice::move(eosio::name user, uint64_t gameuuid, uint64_t steps) {
    // 1. user authentication
    // 2. check the user is in the given game
    // 3. validate the steps from client is the same as we generate from dice.
    // 4. update position
    // 5. distribute awards
    require_auth(user);
    auto _user = is_sched_user_in_game(user, gameuuid);
    bool valid_user_game = (_user != _scheduled_users.cend());
    eosio_assert(valid_user_game, "user not in game");
    uint32_t inner_steps = _user->steps;
    // 2. check steps is smaller than or equal 6
    // TODO: how to deal with order
    // right: steps & 0xffff 0000 0000 0000
    // left : steps & 0x0000 ffff 0000 0000
    // up   : steps & 0x0000 0000 ffff 0000
    // down : steps & 0x0000 0000 0000 ffff
    uint32_t right = (steps >> 48) & 0xffff;
    uint32_t left  = (steps >> 32) & 0xffff;
    uint32_t up    = (steps >> 16) & 0xffff;
    uint32_t down  = steps         & 0xffff;
    eosio_assert(inner_steps <= 6, "inner steps error");
    eosio_assert(right <= inner_steps, "right steps error");
    eosio_assert(left  <= inner_steps, "left  steps error");
    eosio_assert(up    <= inner_steps, "up    steps error");
    eosio_assert(down  <= inner_steps, "down  steps error");
#ifdef DEBUG
    eosio::print("|> right: ", right, ", left: ", left, ", up: ", up, ", down: ", down, " |");
#endif
    uint32_t _steps = (right + left + up + down);
    // 3. check steps is equal to given steps
    eosio_assert(_steps == inner_steps, "total steps error");

    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "not found game");
    eosio_assert(_game->status == GAME_START, "game does not start");
    incr_game_awards(*_game, FEE);

    // 4. arrange the sequence
    // TODO: right <--> left
    bool right_first = true;
    bool up_first = true;
    point pt = point(_game->pos);
    bool right_overflow = (pt.row + right < pt.row);
    if (! right_overflow) {
        // 1. move right is correct
        // update virtual position
        pt.row += right;
        // move left
        bool left_overflow = (pt.row - left > pt.row);
        if (! left_overflow) {
            // ok
            right_first = true;
        } else {
            eosio_assert(false, "error");
        }
    } else {
        // 2. move right is wrong
        // try move left first
        bool left_overflow = (pt.row - left > pt.row);
        if (! left_overflow) {
            pt.row -= left;
            bool right_overflow = (pt.row + right < pt.row);
            if (! right_overflow) {
                // ok
                right_first = false;
            } else {
                eosio_assert(false, "error");
            }
        } else {
            // move left is also wrong
            eosio_assert(false, "error");
        }
    }

    // // up    <--> down
    bool up_overflow = (pt.col + up < pt.col);
    if (! up_overflow) {
        pt.col += up;
        bool down_overflow = (pt.col - down > pt.col);
        if (! down_overflow) {
            // ok
            up_first = true;
        } else {
            eosio_assert(false, "error");
        }
    } else {
        bool down_overflow  = (pt.col - down > pt.col);
        if (! down_overflow) {
            pt.col -= down;
            bool up_overflow = (pt.col + up < pt.col);
            if (! up_overflow) {
                // ok
                up_first = false;
            } else {
                eosio_assert(false, "error");
            }
        }
    }


    if (right_first) {
        if (right > 0) {
            moveright(user, gameuuid, right);
        }
        if (left > 0) {
            moveleft(user, gameuuid, left);
        }
    } else {
        if (left > 0) {
            moveleft(user, gameuuid, left);
        }
        if (right > 0) {
            moveright(user, gameuuid, right);
        }
    }
    if (up_first) {
        if (up > 0) {
            moveup(user, gameuuid, up);
        }
        if (down > 0) {
            movedown(user, gameuuid, down);
        }
    } else {
        if (down > 0) {
            movedown(user, gameuuid, down);
        }
    }

    // TODO: inner_transfer immediately or not ?
    bool is_won = reach_goal(*_game);
    if (is_won) {
        // TODO: distributed tokens
#ifdef DEBUG
        eosio::print("user: ", user, " won.");
#endif
        std::vector<users> participants;
        for (auto _u : _waitingpool) {
            participants.push_back(_u);
        }
        for (auto _u : _scheduled_users) {
            participants.push_back(_u);
        }
        distribute(*_game,
                   *_user,
                   participants);
        close_game(*_game);
    }
    // erase this user in scheduled_users table
    _scheduled_users.erase(_user);
}

void dice::moveright(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    // DONE check valid user in given game;
    // DONE: check valid steps
    // DONE: update steps
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    pt.debug();
    bool overflow = (pt.row + steps < pt.row);
    eosio_assert(! overflow, "upper overflow");
    pt.row = pt.row + steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        eosio_assert(false, "invalid right steps");
    }

    update_game_pos(*_game, pt);
#ifdef DEBUG
    eosio::print("move right");
    _game->debug();
#endif
}

void dice::moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    pt.debug();
    bool overflow = (pt.row - steps > pt.row);
    eosio_assert(! overflow, "lower overflow");

    pt.row = pt.row - steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        eosio_assert(false, "invalid left steps");
    }

    update_game_pos(*_game, pt);
#ifdef DEBUG
    eosio::print("move left");
    _game->debug();
#endif

}

void dice::moveup(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    pt.debug();
    bool overflow = (pt.col + steps < pt.col);
    eosio_assert(! overflow, "upper overflow");

    pt.col = pt.col + steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        eosio_assert(false, "invalid up steps");
    }

    update_game_pos(*_game, pt);
#ifdef DEBUG
    eosio::print("move up");
    _game->debug();
#endif

}

void dice::movedown(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    pt.debug();
    bool overflow = (pt.col - steps > pt.col);
    eosio_assert(! overflow, "lower overflow");
    pt.col = pt.col - steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        eosio_assert(false, "invalid down steps");
    }

    update_game_pos(*_game, pt);
#ifdef DEBUG
    eosio::print("move down");
    _game->debug();
#endif
}

bool dice::reach_goal(const game &_game) {
    eosio_assert(_game.status == GAME_START, "invalid game status");
    int idx = 0;
    for (auto &_goal : _game.goals) {
        if (_goal == _game.pos && _goal != DELETED_GOAL) {
            // someone reaches exactly the goal
            // 1. remove this goal
            // 2. transfer EOS tokens
            // 3. check is the last goal or not, need to close this game?
            _games.modify(_game, get_self(), [&](auto &g){
                                                 g.goals[idx] = DELETED_GOAL;
                                             });
            return true;
        }
        idx ++;
    }
    return false;
}

void dice::distribute(const game& _game,
                      const users& winner,
                      std::vector<users> participants) {
    // winner gets WINNER_PERCENTS
    // PARTICIPANTS get PARTICPANTS PERCENTS
    // DONE: reach a goal, we distribute token to all participants, just for fun.
    const int64_t awards = _game.awards;
    int64_t winner_amount = awards * WINNER_PERCENT;
    int64_t participants_amount = (awards * PARTICIPANTS_PERCENT) / participants.size();
    int64_t platform_amount = awards * PLATFORM_PERCENT;
    int64_t dividend_pool_amount = awards * DIVIDEND_POOL_PERCENT;
    int64_t next_goal_amount = awards * NEXT_GOAL_PERCENT;
    int64_t last_goal_amount = awards * LAST_GOAL_PERCENT;

    if (winner_amount <= 0 ||
        participants_amount <= 0) {
        eosio_assert(false, "bug?");
    }

    // to winner
    inner_transfer(get_self(), winner.user, winner_amount);
    // to dividend pool
    inner_transfer(get_self(), dividend_account, dividend_pool_amount);
    // to our platform
    inner_transfer(get_self(), platform, platform_amount);
    // to all participants, for fun
    for (auto part : participants) {
        inner_transfer(get_self(), part.user, participants_amount);
    }

    desc_game_awards(_game, awards);
    desc_game_shadow_awards(_game, awards);

}

void dice::inner_transfer(eosio::name from, eosio::name to, int64_t amount) {
    // TODO: verify inner_transfer action again and again
    // NOTE: cleos set account permission your_account active '{"threshold": 1,"keys": [{"key": "EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4","weight": 1}],"accounts": [{"permission":{"actor":"your_contract","permission":"eosio.code"},"weight":1}]}' owner -p your_account
    // eosio_assert(amount > 0, "amount < 0");

    eosio::name permission = "active"_n;
    eosio::asset quantity = eosio::asset(amount, eosio::symbol("EOS", 4));

    // https://blog.csdn.net/ITleaks/article/details/83069431
    // delay transaction: https://blog.csdn.net/ITleaks/article/details/83378319
    eosio::transaction txn {};
    txn.actions.emplace_back(
        eosio::permission_level{ from, permission },
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(from, to, quantity, std::string(""))
        );
    // delay 5 seconds
    txn.delay_sec = 5;
    uint128_t id = 0x0 | from.value;
    id <<= 64;
    id |= from.value;
    txn.send(id, from, false);

}


#define EOSIO_DISPATCH2( TYPE, MEMBERS )                                \
    extern "C" {                                                        \
        void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
            if (code == receiver and  eosio::name(action) != "enter"_n) { \
                switch ( action ) {                                     \
                    EOSIO_DISPATCH_HELPER( TYPE, MEMBERS);              \
                }                                                       \
            }                                                           \
            /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
            if( eosio::name(code) == "eosio.token"_n && eosio::name(action) == "transfer"_n ) { \
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
                return inst.enter(eosio::name(code));                   \
                break;                                                  \
                }                                                       \
            }                                                           \
            /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
        }                                                               \
    }                                                                   \

EOSIO_DISPATCH2(dice,
                (version)(addgame)(debug)
                (startgame)
                (schedusers)
                (move)(toss)
                (setfee)(setwidth)(setheight)
    )
