#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include "dice.hpp"
// #include "pcg-c-basic-0.9/pcg_basic.h"
// #include "pcg-c-basic-0.9/pcg_basic.c"

// action
void dice::version() {
    eosio::print(_VERSION.c_str(), ", self: ", get_self());
}

// action
void dice::debug() {
    require_auth(get_self());
    eosio::print(">>>>>>>>>>>>>>>>>>>>TIME>>>>>>>>>>>>>>>>>>>>");
    eosio::print("now: ", now());
    eosio::print(">>>>>>>>>>>>>>>>>>>>GAME>>>>>>>>>>>>>>>>>>>>");
    for (auto &_game : _games) {
        _game.debug();
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>POINT>>>>>>>>>>>>>>>>>>>>");
    for (auto &_game : _games) {
        point pt = point(_game.pos);
        // eosio::print("row: ", pt.row, ", col: ", pt.col, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
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
    // for (uint8_t i = 0; i < 128; i ++) {
    //     // eosio::print(get_rnd_number(6), ", ");
    //     eosio::print(pcg32_random_r(), ", ");
    // }

    // for (uint8_t i = 0; i < 128; i ++) {
    //     eosio::print(pcg32_boundedrand_r(6), ", ");
    // }
    uint32_t rounds = 5;
    for (uint32_t round = 1; round <= rounds; ++ round) {
        eosio::print("Round ", round, ", ");
        for (uint32_t i = 0; i < 33; i ++) {
            eosio::print(pcg32_boundedrand_r(6) + 1, ", ");
        }
        eosio::print("====================");
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>rng engine>>>>>>>>>>>>>>>>>>>>");
    for (auto &_rng : _rngtbl) {
        _rng.debug();
    }
}
// action
void dice::addgame() {
    // TODO: require authority, only our platform has right to add a new game
    // TODO: random initialize a game postion
    // TODO: random initialize a set of goals
    require_auth(get_self());
    // uint64_t uuid = _games.available_primary_key();
    // eosio::print("uuid1: ", uuid, ", ");
    // uuid = _games.available_primary_key();
    // eosio::print("uuid2: ", uuid, ", ");
    _games.emplace(get_self(), [&](auto &g) {
                                   g.uuid = _games.available_primary_key();
                                   g.pos = 0x00010002;
                                   g.status = GAME_CLOSE;
                                   g.awards = 0;
                                   g.shadow_awards = 0;
                               });
}
// action
void dice::startgame(uint64_t gameuuid) {
    require_auth(get_self());
    auto _game = get_game_by_uuid(gameuuid);
    _games.modify(_game, get_self(), [&](auto &g){
                                         g.status = GAME_START;
                                     });

}

void dice::transfer(eosio::name from, eosio::name to, int64_t amount) {
    // NOTE: cleos set account permission your_account active '{"threshold": 1,"keys": [{"key": "EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4","weight": 1}],"accounts": [{"permission":{"actor":"your_contract","permission":"eosio.code"},"weight":1}]}' owner -p your_account
    eosio_assert(amount > 0, "amount < 0");

    eosio::name permission = "active"_n;
    // std::vector<eosio::permission_level> auths {eosio::permission_level {from, permission}, eosio::permission_level {to, permission}};
    eosio::asset quantity = eosio::asset(amount, eosio::symbol("EOS", 4));

    // https://blog.csdn.net/ITleaks/article/details/83069431
    // delay transaction: https://blog.csdn.net/ITleaks/article/details/83378319
    eosio::transaction txn {};
    txn.actions.emplace_back(
        eosio::permission_level{ from, permission },
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(from, to, quantity, std::string(""))
        );
    txn.delay_sec = 30;
    uint128_t id = 0x0 | from.value;
    id <<= 64;
    id |= from.value;
    txn.send(id, from, false);
}
// action
void dice::enter(eosio::name user, uint64_t gameuuid) {
    /**
     * DONE: tranfer fee
     * DONE: put him/her in waiting room
     */
    // TODO: check waiting for success, make sure we get EOS
    // DONE: timestamp every users

    // require_auth(get_self());
    auto _game = get_game_by_uuid(gameuuid);
    transfer(user, get_self(), FEE);
    // TODO: game close or start?
    // eosio_assert(_game->status == GAME_START, "game does not start");
    // update_game_shadow_awards(*_game);

    incr_game_shadow_awards(*_game, FEE);
    // auto g_it = _games.cbegin();
    // while (g_it != _games.cend()) {
    //     if (g_it->gamename == gamename) {
    //         break;
    //     }
    //     g_it ++;
    // }
    // if (g_it == _games.cend()) {
    //     eosio_assert(false, "invalid gamename");
    // }
    _waitingpool.emplace(get_self(), [&](auto &u) {
                                         u.uuid= _waitingpool.available_primary_key();
                                         u.gameuuid = _game->uuid;
                                         u.no = -1;
                                         u.user = user;
                                         u.steps = 0;
                                         u.ts = now();
                                     });

}

// action
void dice::schedusers(uint64_t gameuuid, uint64_t total) {
    // DONE: authorization, only platform could run schedulers
    // TODO: random pick <total> users in waiting pool
    // DONE: put them into scheduling users vector
    require_auth(get_self());
    eosio_assert(total > 0, "sched users < 0");
    auto _game = get_game_by_uuid(gameuuid);
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

    uint64_t i = 0;
    std::vector<users> latest_scheduled_users;

    while (i < total) {         // forever looping ?
        // TODO: randomly pick one number
        uint64_t idx = 0;

        auto _user = latest_scheduling_users[idx];

        if (_user.gameuuid == gameuuid) {

            _scheduled_users.emplace(get_self(), [&](auto &u) {
                                                    u.uuid = _user.uuid; // make user every user only scheduled once
                                                    // u.uuid = _scheduled_users.available_primary_key(),
                                                    // u.gameuuid = _user->gameuuid;
                                                    // u.steps = _user->steps;
                                                    // u.no = _user->no;
                                                    // u.user = _user->user;
                                                    u.gameuuid = _user.gameuuid;
                                                    u.steps = _user.steps;
                                                    u.no = _user.no;
                                                    u.user = _user.user;
                                                });
            latest_scheduled_users.push_back(_user);
            i ++;
        }
    }

    // erase them from waitingpool since they are scheduled
    for (auto _user : latest_scheduled_users) {
        _waitingpool.erase(_user);
    }


    eosio::print(">>>>>>>>>>>>>>>>>>>>SCHEDING USERS>>>>>>>>>>>>>>>>>>>>");
    for (auto &_user : _scheduled_users) {
        _user.debug();
    }
}
// action
void dice::toss(eosio::name user, uint64_t gameuuid) {
    // add auth or not ?
    // require_auth(get_self());
    uint32_t bound = 6;
    // bool is_user_in_game(user, gameuuid);
    // uint32_t dice_number = get_rnd_number(bound);
    // TODO: bind this dice_number to users

}
// action
void dice::moveright(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    // DONE check valid user in given game;
    // DONE: check valid steps
    // DONE: update steps
    auto _game = prepare_movement(user, gameuuid);

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
    eosio::print("move right");
    _game->debug();

    // // DONE: checking win or not ?

    bool is_won = reach_goal(*_game);
    if (is_won) {
        // distribute tokens
        // TODO
        // distribute(user, , _game->awards);
        close_game(*_game);
    }
}
// action
void dice::moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid);

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
    eosio::print("move left");
    _game->debug();

    // DONE: checking win or not ?
    bool is_won = reach_goal(*_game);
    if (is_won) {
        // TODO:
        // distribute(user, , _game->awards);
        close_game(*_game);
    }

}
// action
void dice::moveup(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid);

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
    eosio::print("move up");
    _game->debug();

    // DONE: checking win or not ?
    bool is_won = reach_goal(*_game);
    // if (is_won) {
    //     // TODO:
    //     // distribute(user, , _game->awards);
    //     close_game(*_game);
    // }

}
// action
void dice::movedown(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid);

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
    eosio::print("move down");
    _game->debug();
    bool is_won = reach_goal(*_game);
    if (is_won) {
        // TODO:
        // distribute(user, , _game->awards);
        close_game(*_game);
    }
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
    // TODO: reach a goal, we distribute token to all participants, just for fun.
    const int64_t awards = _game.awards;
    int64_t winner_amount = awards * WINNER_PERCENT;
    int64_t participants_amount = (awards * PARTICIPANTS_PERCENT) / participants.size();
    if (winner_amount <= 0 ||
        participants_amount <= 0) {
        eosio_assert(false, "bug?");
    }

    eosio::name platform = get_self();

    transfer(platform, winner.user, winner_amount);
    for (auto part : participants) {
        transfer(platform, part.user, participants_amount);
    }

    desc_game_shadow_awards(_game, awards);
}

EOSIO_DISPATCH(dice,
               (version)(addgame)(debug)
               (startgame)
               (enter)(schedusers)
               (moveright)(moveleft)(moveup)(movedown)
    )
