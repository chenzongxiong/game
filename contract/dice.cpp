#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include "dice.hpp"


struct eosio_token_transfer {
    eosio::name from;
    eosio::name to;
    eosio::asset quantity;
    std::string memo; // when comment out, it works fine
};

// action
#ifdef DEBUG
void dice::version() {
    eosio::print(this->_VERSION.c_str());
}
/********************************************************************************
 * administrator functions
 ********************************************************************************/
// action
void dice::debug() {
    require_auth(admin);
    eosio::print(">>>>>>>>>>>>>>>>>>>>SEED>>>>>>>>>>>>>>>>>>>>");
    eosio::print("now: ", now(), ", ");
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

    eosio::print(">>>>>>>>>>>>>>>>>>>>rng engine>>>>>>>>>>>>>>>>>>>>");
    for (auto &_rng : _rngtbl) {
        _rng.debug();
    }

    eosio::print(">>>>>>>>>>>>>>>>>>>>winner table>>>>>>>>>>>>>>>>>>>>");
    for (auto &_winner : _winners) {
        _winner.debug();
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>heroes table>>>>>>>>>>>>>>>>>>>>");
    for (auto &_h : _heroes) {
        _h.debug();
    }
    eosio::print(">>>>>>>>>>>>>>>>>>>>>> config >>>>>>>>>>>>>>>>>>>>>>>");
    if (_config.exists()) {
        auto cfg = _config.get();
        cfg.debug();
    }
}
#endif
// action
void dice::addgame(eosio::name gamename, uint32_t width,
                   uint32_t height, uint32_t status, int64_t fee) {

    require_auth(admin);;

    eosio_assert(fee > 0, "fee < 0");
    eosio_assert(MAXSIZE > width, "width > MAXSIZE, invalid");
    eosio_assert(MAXSIZE > height, "height > MAXSIZE, invalid");
    eosio_assert(status == GAME_CLOSE || status == GAME_START,
                 "invalid status");
    eosio::print("successfully add a new game");
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
    eosio::print("successfully add a new game");
    _games.emplace(get_self(), [&](auto &g) {
                                   g.uuid = _games.available_primary_key();
                                   g.pos = pos;
                                   // g.status = GAME_CLOSE;
                                   g.status = status;
                                   g.board_width = width;
                                   g.board_height = height;
                                   g.fee = fee;
                                   g.awards = 0;
                                   g.shadow_awards = 0;
                                   g.total_number = 0;
                                   g.gamename = gamename;
                                   // g.gamename.assign(gamename);
                                   for (auto _gl : goals) {
                                       g.goals.push_back(_gl);
                                   }
                               });
#ifdef DEBUG
    eosio::print("successfully add a new game");
#endif

}
// action
void dice::startgame(uint64_t gameuuid) {
    require_auth(admin);;
    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "game not found");

    _games.modify(_game, get_self(), [&](auto &g){
                                         g.status = GAME_START;
                                     });

}
// // action
// void dice::schedusers(uint64_t gameuuid, uint32_t total) {
//     // 1. authorization, only platform could run schedulers
//     // 2. remove expired users
//     // 3. random pick <total> users in waiting pool
//     // 4. put them into scheduling users vector
//     // 5. erase them from waitingpool

//     require_auth(admin);;
//     auto _game = get_game_by_uuid(gameuuid);
//     eosio_assert(_game != _games.cend(), "bug ? game not found.");
//     eosio_assert(_game->status == GAME_START, "bug ? game does not start");

//     auto sched_user_it = _scheduled_users.begin();
//     while (sched_user_it != _scheduled_users.end()) {
//         if ((now() - sched_user_it->update_ts) > TIMEOUT_USERS) {
//             sched_user_it = _scheduled_users.erase(sched_user_it);
//         } else {
//             sched_user_it ++;
//         }
//     }

//     // multiple user in multiple games
//     // how many people in game gameuuid
//     std::vector<users> latest_scheduling_users;
//     for (auto _user : _waitingpool) {
//         if (_user.gameuuid == gameuuid) {
//             latest_scheduling_users.push_back(_user);
//         }
//     }

//     if (latest_scheduling_users.size() < total) {
//         total = latest_scheduling_users.size();
//     }

//     uint32_t i = 0;
//     std::vector<users> latest_scheduled_users;
//     std::set<uint64_t> book_keep;

//     while (i < total) {         // forever looping ?
//         // randomly pick one number
//         uint64_t idx = pcg32_boundedrand_r(total);
//         auto idx_it = book_keep.find(idx);

//         if (idx_it != book_keep.end()) {
//             // eosio::print("idx: ", idx, ", skip");
//             continue;

//         }
//         book_keep.insert(idx);

//         auto _user = latest_scheduling_users[idx];

//         eosio_assert(_user.gameuuid == gameuuid, "bug?");
//         // TODO: primary key of users in waitingpool can be overlapping
//         _scheduled_users.emplace(get_self(), [&](auto &u) {
//                                                  // u.uuid = _user.uuid; // make user every user only scheduled once
//                                                  u.uuid = _scheduled_users.available_primary_key();
//                                                  u.gameuuid = _user.gameuuid;
//                                                  u.steps = _user.steps;
//                                                  u.no = _scheduled_users.available_primary_key();
//                                                  u.user = _user.user;
//                                                  u.ts = _user.ts;
//                                                  u.update_ts = now();
//                                              });
//         latest_scheduled_users.push_back(_user);
//         // erase them from waitingpool since they are scheduled
//         auto u_it = _waitingpool.find(_user.uuid);
//         _waitingpool.erase(u_it);

//         i ++;
//     }
// // #ifdef DEBUG
// //     eosio::print(">>>>>>>>>>>>>>>>>>>>SCHEDING USERS>>>>>>>>>>>>>>>>>>>>");
// //     for (auto &_user : _scheduled_users) {
// //         _user.debug();
// //     }
// // #endif
// }

// action
#ifdef DEBUG
void dice::clear() {
    clear2("gametbl");
    clear2("waittbl");
    clear2("schedtbl");
    clear2("herotbl");
    clear2("winnertbl");

}
void dice::clear2(std::string tbl) {
    require_auth(admin);
    // clear gametable
    if (tbl == "gametbl") {
        auto it1 = _games.begin();
        while (it1 != _games.end()) {
            it1 = _games.erase(it1);
        }
    }
    // clear waitingpool
    if (tbl == "waittbl") {
        auto it2 = _waitingpool.begin();
        while (it2 != _waitingpool.end()) {
            it2 = _waitingpool.erase(it2);
        }
    }
    // clear schedued users
    if (tbl == "schedtbl") {
        auto it3 = _scheduled_users.begin();
        while (it3 != _scheduled_users.end()) {
            it3 = _scheduled_users.erase(it3);
        }
    }
    if (tbl == "herotbl") {
        auto it4 = _heroes.begin();
        while (it4 != _heroes.end()) {
            it4 = _heroes.erase(it4);
        }
    }
    if (tbl == "winnertbl") {
        auto it5 = _winners.begin();
        while (it5 != _winners.end()) {
            it5 = _winners.erase(it5);
        }
    }
}
#endif
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
     * 4. schedule the user to schedule table
     */
    eosio_assert(user == "eosio.token"_n, "fake tokens");
    auto data = eosio::unpack_action_data<eosio_token_transfer>();
    if (data.from != get_self()) {
        eosio_assert(data.to == get_self(), "didn't transfer tokens to me");

        eosio_assert(data.quantity.is_valid(), "invalid quantity");
        eosio_assert(data.quantity.amount > 0, "must transfer positive quantity");
        eosio_assert(data.quantity.symbol == eosio::symbol("EOS", 4), "symbol precision mismatch");
        eosio_assert(data.memo.size() <= 256, "memo has more than 256 bytes");

        uint64_t gameuuid = extract_gameuuid_from_memo(data.memo);
        auto _game = get_game_by_uuid(gameuuid);
        eosio_assert(_game != _games.cend(), "not found game.");
        eosio_assert(_game->status != GAME_OVER, "game is over");

        if (data.quantity.amount >= _game->fee) { // pay enough fee
            // check game
            incr_game_shadow_awards(*_game, _game->fee);

            // seed
            // uint32_t lucky_number = 0;
            // eosio::checksum256 seed1 = user_seed_hash(lucky_number);
            // eosio::checksum256 seed2 = tx_hash();
            // st_seeds seeds = {
            //     .seed1 = seed1,
            //     .seed2 = seed2,
            // };
            // eosio::checksum256 seed = eosio::sha256((char *)&seeds.seed1, sizeof(seeds.seed1)*2);
            // eosio::print(", debuing enter...., ");
            // eosio::print("seed1: ", seed1, ", ");
            // eosio::print("seed2: ", seed2, ", ");
            // eosio::print("seed: ", seed, ", ");

            _waitingpool.emplace(get_self(), [&](auto &u) {
                                                 u.uuid= _waitingpool.available_primary_key();
                                                 u.gameuuid = _game->uuid;
                                                 u.no = -1;
                                                 u.user = data.from;
                                                 u.steps = 0;
                                                 u.ts = now();
                                                 u.update_ts = now();
                                             });
            // xxxx
            eosio::transaction txn {};
            txn.actions.emplace_back(
                eosio::permission_level{_self, "active"_n},
                get_self(), "schedhelper"_n,
                std::make_tuple(data.from, gameuuid, now())
                );
            // delay 10 seconds
            uint32_t delayed = 10;
            txn.delay_sec = delayed;
            txn.send(next_sender_id(), get_self(), false);

            _games.modify(_game, get_self(), [&](auto &g) {
                                                 g.total_number ++;
                                             });
        } else {
            // come here because someone send request directly and doesn't
            // pay enough fee. Never open a door for him, but we accept his fee
            eosio::print("crackers come");
        }
    } else {
        eosio::print("transfer action is trigger by platform, ignore it");
    }
#ifdef DEBUG
    eosio::print("call enter successfully");
#endif
}

void dice::schedhelper(eosio::name user, uint64_t gameuuid, time_t ts) {
    // only this contract can run
    require_auth(get_self());

    uint32_t block_num = tapos_block_num();
    uint32_t block_prefix = tapos_block_prefix();
    // eosio::print("schedule-helper");

    // eosio::print("block number: ", block_num, ", ");
    // eosio::print("block prefix: ", block_prefix, ", ");

    eosio::transaction txn {};
    txn.actions.emplace_back(
        // eosio::permission_level{user, "active"_n},
        eosio::permission_level{get_self(), "active"_n},
        get_self(), "sched"_n,
        std::make_tuple(user, gameuuid, now())
        );
    // NOTE: delay `x` (3 - 32) seconds, to avoid one man controls too many accounts
    // this random number generator might be vunerable, but it's fine since
    // it's only affect the order of the scheduled table.
    // a hacker might be able to occupy at leading position.
    // but a common user still has chance to be put at leading position
    // and a common user can also enter this game multiple time to increase
    // the chance to be put at leading position
    pcg32_srandom_r(now(), now());
    uint32_t delayed = pcg32_boundedrand_r(29) + 3;
    eosio::print("delay: ", delayed, ", ");

    txn.delay_sec = delayed;

    txn.send(next_sender_id(), get_self(), false);
}

void dice::sched(eosio::name user, uint64_t gameuuid, time_t ts) {
    // only this contract can run
    require_auth(get_self());

    uint32_t block_num = tapos_block_num();
    uint32_t block_prefix = tapos_block_prefix();

    // eosio::print("scheduled.");
    // eosio::print("block number: ", block_num, ", ");
    // eosio::print("block prefix: ", block_prefix, ", ");

    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "bug ? game not found.");
    eosio_assert(_game->status == GAME_START, "bug ? game does not start");
    uint128_t proof = 0;
    proof |= block_num;
    proof <<= 32;
    proof |= block_prefix;
    proof <<= 64;
    time_t curr_ts = now();
    proof |= (uint64_t)curr_ts;
    uint64_t sched_no = next_sched_no();
    uint64_t num_sched_users = get_num_sched_users();
    time_t expired = curr_ts + num_sched_users * SCHED_TIMEOUT;

    _scheduled_users.emplace(get_self(), [&](auto &u) {
                                             // u.uuid = sched_no;
                                             u.uuid = _scheduled_users.available_primary_key();
                                             u.gameuuid = gameuuid;
                                             u.steps = 0;
                                             u.no = _scheduled_users.available_primary_key();
                                             // u.no = sched_no;
                                             u.user = user;
                                             u.ts = ts;
                                             u.update_ts = curr_ts;
                                             u.expired = expired;
                                             u.proof = proof;
                                         });
    incr_num_sched_users();

    // TODO: run defer transaction to erase schedule users

}

void dice::erasetimeout(users _user) {
    eosio::print("_user.update_ts: ", _user.update_ts, ", ");
    eosio::print("now: ", now(), ", ");
}

void dice::resetremove() {
    require_auth(admin);
    auto cfg = _config.get_or_default({});

    cfg.stop_remove_sched = false;
    _config.set(cfg, get_self());
}

void dice::setremove() {
    require_auth(admin);
    auto cfg = _config.get_or_default({});

    cfg.stop_remove_sched = true;
    _config.set(cfg, get_self());
}

void dice::removesched() {
    require_auth(admin);
    auto cfg = _config.get();
    if (cfg.stop_remove_sched) {
        eosio::print("run remove schedued users");
        auto sched_user_it = _scheduled_users.begin();
        while (sched_user_it != _scheduled_users.end()) {
            if ((now() - sched_user_it->update_ts) > TIMEOUT_USERS) {
                sched_user_it = _scheduled_users.erase(sched_user_it);
            } else {
                sched_user_it ++;
            }
        }
        // remove time-out users every 10 seconds
        eosio::transaction txn {};
        txn.actions.emplace_back(
            eosio::permission_level{admin, "active"_n},
            get_self(), "removesched"_n,
            std::make_tuple()
            );

        uint32_t delayed = 10;
        txn.delay_sec = delayed;
        txn.send(now(), admin, false);
    } else {
        eosio::print("stop remove schedued users");
    }
}

// action
void dice::toss(eosio::name user, uint64_t gameuuid, uint32_t seed) {
    // 1. user authentication
    // 2. randomly generate dice number
    // 3. assign that number to that user
    // NOTE: each user can only toss once

    require_auth(user);

    auto end = _scheduled_users.cend();

    for (auto _user = _scheduled_users.cbegin(); _user != end; _user ++) {
        if (_user->gameuuid == gameuuid) { // in this game
            // this user doesn't toss yet
            if (_user->user == user && _user->steps == 0) {

                uint128_t proof = _user->proof;
                uint32_t dice_number = get_rnd_dice_number(proof);

                eosio::print("{");
                eosio::print("\"dice_number\": ", dice_number, ", ");
                eosio::print("\"user\": \"", user, "\", ");
                eosio::print("\"gameuuid\": ", gameuuid, ", ");
                eosio::print("\"update_ts\": ", now(), ", ");
                eosio::print("\"toss\": ", true);
                eosio::print("}");

                _scheduled_users.modify(_user, get_self(), [&](auto &u) {
                                                               u.steps = dice_number;
                                                               u.update_ts = now();
                                                           });
            } else {

                eosio::print("{");
                eosio::print("\"dice_number\": ", _user->steps, ", ");
                eosio::print("\"user\": \"", user, "\", ");
                eosio::print("\"gameuuid\": ", gameuuid, ", ");
                eosio::print("\"update_ts\": ", now(), ", ");
                eosio::print("\"toss\": ", false);
                eosio::print("}");

            }
            break;              // this game only executes once
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
    uint32_t inner_steps = _user->steps;
    bool expired = (now() - _user->update_ts >= SCHED_TIMEOUT);
    eosio_assert(valid_user_game, "user not in game");
    eosio_assert(inner_steps <= 6 && inner_steps > 0, "inner steps error");
    eosio_assert(expired, "bug ? expired user");

    // 2. check steps is smaller than or equal 6
    // TODO: how to deal with order
    // right: steps & 0xffff 0000 0000 0000
    // left : steps & 0x0000 ffff 0000 0000
    // up   : steps & 0x0000 0000 ffff 0000
    // down : steps & 0x0000 0000 0000 ffff

    uint32_t right = (steps & 0xffff000000000000) >> 48;
    uint32_t left  = (steps & 0x0000ffff00000000) >> 32;
    uint32_t up    = (steps & 0x00000000ffff0000) >> 16;
    uint32_t down  = (steps & 0x000000000000ffff);
#ifdef DEBUG
    eosio::print("|> right: ", right, ", left: ", left, ", up: ", up, ", down: ", down, " |");
#endif
    eosio_assert(inner_steps <= 6, "inner steps error");
    eosio_assert(right <= inner_steps, "right steps error");
    eosio_assert(left  <= inner_steps, "left  steps error");
    eosio_assert(up    <= inner_steps, "up    steps error");
    eosio_assert(down  <= inner_steps, "down  steps error");

    uint32_t _steps = (right + left + up + down);
    // 3. check steps is equal to given steps
    eosio_assert(_steps == inner_steps, "total steps error");

    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "not found game");
    eosio_assert(_game->status == GAME_START, "game does not start");
    incr_game_awards(*_game, _game->fee);

    // 4. arrange the sequence
    // DONE: right <--> left
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

    // up    <--> down
    bool up_overflow = (pt.col - up > pt.col);
    if (! up_overflow) {
        pt.col -= up;
        bool down_overflow = (pt.col + down < pt.col);
        if (! down_overflow) {
            // ok
            up_first = true;
        } else {
            eosio_assert(false, "error");
        }
    } else {
        bool down_overflow  = (pt.col + down < pt.col);
        if (! down_overflow) {
            pt.col += down;
            bool up_overflow = (pt.col - up > pt.col);
            if (! up_overflow) {
                // ok
                up_first = false;
            } else {
                eosio_assert(false, "error");
            }
        } else {
            // move down is also wrong
            eosio_assert(false, "error");
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

    bool is_won = reach_goal(*_game);
    if (is_won) {
// #ifdef DEBUG
//         eosio::print("user: ", user, " won.");
// #endif
        // add it to winner_table
        point pt = point(_game->pos);
        _winners.emplace(get_self(), [&](auto &w) {
                                         w.uuid = _winners.available_primary_key();
                                         w.user = user;
                                         w.gameuuid = _game->uuid;
                                         w.ts = now();
                                         w.update_ts = now();
                                         w.awards = _game->awards;
                                         w.row = pt.row;
                                         w.col = pt.col;
                                         w.acc_awards = 0;
                                   });
        desc_game_awards(*_game, _game->awards);
        desc_game_shadow_awards(*_game, _game->awards);
        close_game(*_game);

        eosio::print("{");
        eosio::print("\"gameuuid\": ", gameuuid, ", ");
        eosio::print("\"user\": \"", user, "\", ");
        eosio::print("\"awards\": ", _game->awards, ", ");
        eosio::print("\"row\": ", pt.row, ", ");
        eosio::print("\"col\": ", pt.col, ", ");
        eosio::print("\"update_ts\": ", now(), ", ");
        eosio::print("\"win\": ", true);
        eosio::print("}");

        // TODO: how to deal with the rest people when game is closed

    } else {
        eosio::print("{");
        eosio::print("\"gameuuid\": ", gameuuid, ", ");
        eosio::print("\"user\": \"", user, "\", ");
        eosio::print("\"awards\": ", _game->awards, ", ");
        eosio::print("\"row\": ", pt.row, ", ");
        eosio::print("\"col\": ", pt.col, ", ");
        eosio::print("\"update_ts\": ", now(), ", ");
        eosio::print("\"win\": ", false);
        eosio::print("}");
    }
    // erase this user in scheduled_users table
    _scheduled_users.erase(_user);
    _games.modify(_game, get_self(), [&](auto &g) {
                                         g.total_number --;
                                     });
}

void dice::sendtokens(eosio::name user, uint64_t gameuuid) {
    require_auth(admin);;

    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "not found game");

    auto _winner = is_won_user_in_game(user, gameuuid);
    eosio_assert(_winner != _winners.cend(), "winner not in game");

    std::vector<eosio::name> participants;

    for (auto _u : _waitingpool) {
        participants.push_back(_u.user);
    }
    for (auto _u : _scheduled_users) {
        participants.push_back(_u.user);
    }

    // eosio::print("distribute tokens");
    distribute(*_game,
               user,
               participants,
               _winner->awards);
    uint64_t real_awards = _winner->awards*WINNER_PERCENT;
    update_heroes(user, real_awards);
    _winners.erase(_winner);
}

void dice::moveright(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    // DONE check valid user in given game;
    // DONE: check valid steps
    // DONE: update steps
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
// #ifdef DEBUG
//     pt.debug();
// #endif
    bool overflow = (pt.row + steps < pt.row);
    eosio_assert(! overflow, "upper overflow");
    pt.row = pt.row + steps;
    bool valid = is_valid_pos(*_game, pt);
    eosio_assert(valid, "invalid right steps");

    update_game_pos(*_game, pt);
// #ifdef DEBUG
//     eosio::print("move right");
//     _game->debug();
// #endif
}

void dice::moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
// #ifdef DEBUG
//     pt.debug();
// #endif
    bool overflow = (pt.row - steps > pt.row);
    eosio_assert(! overflow, "lower overflow");

    pt.row = pt.row - steps;
    bool valid = is_valid_pos(*_game, pt);
    eosio_assert(valid, "invalid left steps");

    update_game_pos(*_game, pt);
// #ifdef DEBUG
//     eosio::print("move left");
//     _game->debug();
// #endif

}

void dice::moveup(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
// #ifdef DEBUG
//     pt.debug();
// #endif
    bool overflow = (pt.col - steps > pt.col);
    eosio_assert(! overflow, "upper overflow");

    pt.col -= steps;
    bool valid = is_valid_pos(*_game, pt);
    eosio_assert(valid, "invalid up steps");

    update_game_pos(*_game, pt);
// #ifdef DEBUG
//     eosio::print("move up");
//     _game->debug();
// #endif

}

void dice::movedown(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
// #ifdef DEBUG
//     pt.debug();
// #endif
    bool overflow = (pt.col + steps < pt.col);
    eosio_assert(! overflow, "lower overflow");
    pt.col += steps;
    bool valid = is_valid_pos(*_game, pt);
    eosio_assert(valid, "invalid down steps");

    update_game_pos(*_game, pt);
// #ifdef DEBUG
//     eosio::print("move down");
//     _game->debug();
// #endif
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
                      const eosio::name& winner,
                      std::vector<eosio::name> participants,
                      const uint64_t awards) {
    // winner gets WINNER_PERCENTS
    // PARTICIPANTS get PARTICPANTS PERCENTS
    // DONE: reach a goal, we distribute token to all participants, just for fun.
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
    uint32_t seed = 0;
    // to winner
    inner_transfer(get_self(), winner, winner_amount, seed);
    // to dividend pool
    seed ++;
    inner_transfer(get_self(), dividend_account, dividend_pool_amount, seed);
    // to our platform
    seed ++;
    inner_transfer(get_self(), platform, platform_amount, seed);
    // to all participants, for fun
    // for (auto part : participants) {
    //     // inner_transfer(get_self(), part.user, participants_amount);
    //     ++ seed;
    //     inner_transfer(get_self(), part, participants_amount, seed);
    // }
}

void dice::inner_transfer(eosio::name from, eosio::name to, int64_t amount, uint32_t seed) {

    eosio::name permission = "active"_n;
    eosio::asset quantity = eosio::asset(amount, eosio::symbol("EOS", 4));

    // delay transaction:
    // https://blog.csdn.net/ITleaks/article/details/83069431
    // https://blog.csdn.net/ITleaks/article/details/83378319
    eosio::transaction txn {};
    txn.actions.emplace_back(
        eosio::permission_level{ from, permission },
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(from, to, quantity, std::string(""))
        );
    // delay 10 seconds
    txn.delay_sec = 10;

    txn.send(next_sender_id(), from, false);
}

void dice::delayid(eosio::name user, uint32_t amount) {

    uint64_t block_num = (uint64_t)tapos_block_num();
    uint64_t block_prefix = (uint64_t)tapos_block_prefix();
    eosio::print("delayed block number:", block_num, ", ");
    eosio::print("delayed block_prefix: ", block_prefix, ", ");
    // eosio::print("delayed Hello %", id);
    eosio::transaction txn {};
    eosio::action atx = {eosio::permission_level{_self, "active"_n},
                         get_self(), "delayid"_n,
                         std::make_tuple(get_self(), amount)};
    for (uint32_t i = 0; i < amount; i ++) {
        txn.actions.push_back(atx);
    }

    // delay 5 seconds
    txn.delay_sec = 5;
    txn.send(next_sender_id(), _self, false);

    // auto size = read_transaction(nullptr, 0);
    // char *tx = (char *)malloc(size);
    // read_transaction(tx, size);
    // eosio::transaction _txn = eosio::unpack<eosio::transaction>(tx, size);
    // eosio::checksum256 _tx_hash = eosio::sha256(tx, size);
    // eosio::print("tx.ref_block_num: ", (uint32_t)_txn.ref_block_num, ", ");
    // eosio::print("tx.ref_block_prefix: ", (uint32_t)_txn.ref_block_prefix, ", ");
    // eosio::print("tx.delay_sec: ", (uint32_t)_txn.delay_sec, ", ");
    // eosio::print("tx.hash: ", _tx_hash);
}


#define EOSIO_DISPATCH2( TYPE, MEMBERS )                                \
    extern "C" {                                                        \
        void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
            /* https://mp.weixin.qq.com/s/1sV2ps1n6pPNb-0qiTXG9Q */     \
            if (code == receiver and  eosio::name(action) != "enter"_n) { \
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
                return inst.enter(eosio::name(code));                   \
                break;                                                  \
                }                                                       \
            }                                                           \
            /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
        }                                                               \
    }                                                                   \

EOSIO_DISPATCH2(dice,
#ifdef DEBUG
                (version)
                (debug)
                (clear)
                (clear2)
                (delayid)
#endif
                (addgame)
                (startgame)
                // (schedusers)
                (move)
                (toss)
                (sendtokens)
                (sched)
                (removesched)
                (setremove)
                (resetremove)
                (schedhelper)
    )
