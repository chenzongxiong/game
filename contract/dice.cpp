#include "dice.hpp"
#include <eosiolib/asset.hpp>

// action
void dice::version() {
    eosio::print(_VERSION.c_str(), ", self: ", get_self());
}
// action
void dice::debug() {
    // for (auto &_game : _games) {
    //     _game.debug();
    // }

    // point pt = point(0xff0000ff);
    // for (auto &_game : _games) {
    //     auto it = _games.find(_game.uuid);
    //     eosio_assert(it != _games.end(), "could not find the game");

    //     _games.modify(it, get_self(), [&](auto &g){
    //                                       set_game_pos(g, pt);
    //                                   });
    // }
    require_auth(get_self());
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
    for (auto &_user : scheduled_users) {
        _user.debug();
    }
    // eosio::print(">>>>>>>>>>>>>>>>>>>>LATEST SCHEDING USERS>>>>>>>>>>>>>>>>>>>>");
    // for (auto &_user : latest_scheduled_users) {
    //     _user.debug();
    // }
}

int dice::random() {
    return 1;
}
// action
void dice::addgame() {
    // TODO: require authority, only our platform has right to add a new game
    // TODO: random initialize a game postion
    // TODO: random initialize a set of goals
    require_auth(get_self());
    uint64_t uuid = _games.available_primary_key();
    // eosio::print("uuid1: ", uuid, ", ");
    uuid = _games.available_primary_key();
    // eosio::print("uuid2: ", uuid, ", ");
    _games.emplace(get_self(), [&](auto &g) {
                                   g.uuid = uuid;
                                   g.pos = 0x00010002;
                                   g.status = GAME_CLOSE;
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
    // eosio::name from = "arestest4321"_n;
    // eosio::name to = "arestest1234"_n;
    // eosio::name from = "player1"_n;
    // eosio::name to = "cardgameacc"_n;
    // eosio::eosio_assert();
    eosio_assert(amount > 0, "amount < 0");

    eosio::name permission = "active"_n;
    // std::vector<eosio::permission_level> auths {eosio::permission_level {from, permission}, eosio::permission_level {to, permission}};
    eosio::asset quantity = eosio::asset(amount, eosio::symbol("EOS", 4));
    eosio::action(
        eosio::permission_level{ from, permission },
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(from, to, quantity, std::string(""))
        ).send();
}
// action
void dice::enter(eosio::name user, uint64_t gameuuid) {
    /**
     * DONE: tranfer fee
     * DONE: put him/her in waiting room
     */
    // TODO: check waiting for success, make sure we get EOS
    // TODO: timestamp every users
    require_auth(get_self());
    transfer(user, get_self(), FEE);

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

    // g_it->debug();
    auto _game = _games.find(gameuuid);
    if (_game == _games.end()) {
        eosio_assert(false, "invalid gameuuid");
    }
    _waitingpool.emplace(get_self(), [&](auto &u) {
                                         u.uuid= _waitingpool.available_primary_key();
                                         u.gameuuid = _game->uuid;
                                         u.no = -1;
                                         u.user = user;
                                         u.steps = 0;
                                         // u.ts = bs timestamp
                                     });

}
// action
void dice::schedusers(uint64_t gameuuid, uint64_t total) {
    // DONE: authorization, only platform could run schedulers
    // TODO: random pick <total> users in waiting pool
    // DONE: put them into scheduling users vector
    require_auth(get_self());
    eosio_assert(total > 0, "sched users < 0");
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

            scheduled_users.emplace(get_self(), [&](auto &u) {
                                                    u.uuid = _user.uuid; // make user every user only scheduled once
                                                    // u.uuid = scheduled_users.available_primary_key(),
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
    for (auto &_user : scheduled_users) {
        _user.debug();
    }
}
// action
void dice::moveright(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    // TODO check valid user in given game;
    // DONE: check valid steps
    // DONE: update steps
    require_auth(get_self());
    bool valid_user_game = is_user_in_game(user, gameuuid);
    eosio_assert(valid_user_game, "user not in game");

    auto _game = get_game_by_uuid(gameuuid);
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

    // TODO: checking win or not ?
}
// action
void dice::moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    // TODO: check valid user;
    // DONE: check valid steps
    // DONE: update steps

    auto _game = get_game_by_uuid(gameuuid);
    point pt = point(_game->pos);
    // overflow
    pt.debug();

    bool overflow = (pt.row - steps > pt.row);
    eosio_assert(! overflow, "lower overflow");
    pt.row = pt.row - steps;
    bool valid = is_valid_pos(*_game, pt) ;
    if (! valid) {
        eosio_assert(false, "invalid left steps");
    }

    update_game_pos(*_game, pt);
    eosio::print("move left");
    _game->debug();
}
// action
void dice::moveup(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = get_game_by_uuid(gameuuid);
    point pt = point(_game->pos);
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
}
// action
void dice::movedown(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = get_game_by_uuid(gameuuid);
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
}

EOSIO_DISPATCH(dice,
               (version)(addgame)(debug)
               (startgame)
               (enter)(schedusers)
               (moveright)(moveleft)(moveup)(movedown))
