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
#if DEBUG
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
    eosio_assert(width >= 23, "width < 23, invalid");
    eosio_assert(height >= 23, "height < 23, invalid");
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
#if DEBUG
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
#if DEBUG
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
        // uint64_t count = 0;
        auto it3 = _scheduled_users.begin();
        while (it3 != _scheduled_users.end()) {
            it3 = _scheduled_users.erase(it3);
            // count ++;
        }
        // desc_num_sched_users(count);
        auto cfg = _config.get_or_default({});
        cfg.num_sched_users = 0;
        _config.set(cfg, get_self());
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

            uint64_t user_id = _waitingpool.available_primary_key();

            _waitingpool.emplace(get_self(), [&](auto &u) {
                                                 u.uuid = user_id;
                                                 u.gameuuid = _game->uuid;
                                                 u.no = -1;
                                                 u.user = data.from;
                                                 u.steps = 0;
                                                 u.ts = now();
                                                 u.update_ts = now();
                                                 u.sched_flag = 0;
                                                 u.expired_ts = -1;
                                             });
            // xxxx
            uint128_t sender_id = next_sender_id();

            eosio::transaction txn {};
            txn.actions.emplace_back(
                eosio::permission_level{_self, "active"_n},
                get_self(), "schedhelper"_n,
                std::make_tuple(user_id, gameuuid, now(), sender_id)
                );
            // delay 5 seconds
            uint32_t delayed = 5;
            txn.delay_sec = delayed;
            txn.send(sender_id, get_self(), false);
            _games.modify(_game, get_self(), [&](auto &g) {
                                                 g.total_number ++;
                                             });
        } else {
            // come here because someone send request directly and doesn't
            // pay enough fee. Never open a door for him, but we accept his fee
            eosio::print("{");
            eosio::print("\"msg\": \"crackers come\"");
            eosio::print("}");
        }
    } else {
        eosio::print("{");
        eosio::print("\"msg\": \"transfer action is trigger by platform, ignore it\"");
        eosio::print("}");
    }
}

// void dice::schedhelper(eosio::name user, uint64_t gameuuid, time_t ts) {
void dice::schedhelper(uint64_t user_id, uint64_t gameuuid, time_t ts, uint128_t sender_id) {
    // only this contract can run
    require_auth(get_self());
    // uint128_t _sender_id = next_sender_id();

    uint32_t block_num = tapos_block_num();
    uint32_t block_prefix = tapos_block_prefix();

    eosio::transaction txn {};
    txn.actions.emplace_back(
        eosio::permission_level{get_self(), "active"_n},
        get_self(), "sched"_n,
        std::make_tuple(user_id, gameuuid, now(), sender_id)
        );
    // NOTE: delay `x` (3 - 25) seconds, to avoid one man controls too many accounts
    // this random number generator might be vunerable, but it's fine since
    // it's only affect the order of the scheduled table.
    // a hacker might be able to occupy at leading position.
    // but a common user still has chance to be put at leading position
    // and a common user can also enter this game multiple time to increase
    // the chance to be put at leading position
    pcg32_srandom_r(now(), now());
    uint32_t delayed = pcg32_boundedrand_r(22) + 3;
    txn.delay_sec = delayed;
    txn.send(sender_id, get_self(), false);
}

void dice::sched(uint64_t user_id, uint64_t gameuuid, time_t ts, uint128_t sender_id) {
    // only this contract can run
    // defer action: may result a problem. some transactions are failed without notification
    // how to avoid that.
    require_auth(get_self());

    uint32_t block_num = tapos_block_num();
    uint32_t block_prefix = tapos_block_prefix();

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
    time_t expired_ts = curr_ts + (num_sched_users + 1) * SCHED_TIMEOUT;
    // time_t expired_ts = curr_ts + (num_sched_users + 1)* 5;

    auto _user = _waitingpool.find(user_id);
    eosio_assert(_user != _waitingpool.cend(), "bug? not found user");
    _scheduled_users.emplace(get_self(), [&](auto &u) {
                                             u.uuid = user_id;
                                             u.gameuuid = gameuuid;
                                             u.steps = 0;
                                             u.no = sched_no;
                                             u.user = _user->user;
                                             u.ts = ts;
                                             u.update_ts = curr_ts;
                                             u.expired_ts = expired_ts;
                                             u.proof = proof;
                                             u.sched_flag = 1;
                                         });

    incr_num_sched_users(1);
    // mapping with waiting pool
    _waitingpool.modify(*_user, get_self(),  [&](auto &u) {
                                               u.sched_flag = 1;
                                               u.expired_ts = expired_ts;
                                           });
}

void dice::rmexpired() {
    require_auth(admin);
    uint32_t count = 0;
    time_t curr_ts = now();
    // TODO: use require_recipeit
    auto it3 = _scheduled_users.begin();
    while (it3 != _scheduled_users.end()) {
        if (curr_ts >= it3->expired_ts) {
            it3 = _scheduled_users.erase(it3);
            count ++;
        } else {
            it3 ++;
        }
    }
    desc_num_sched_users(count);
    eosio::print("{");
    eosio::print("\"count\":", count, ", ");
    eosio::print("\"msg\": \"remove expired user successfully\"");
    eosio::print("}");

}

void dice::forcesched() {
    require_auth(admin);

    time_t curr_ts = now();
    uint32_t TIMEOUT = 60;
    uint32_t count = 0;

    auto it = _waitingpool.cbegin();
    auto end = _waitingpool.cend();

    while (it != end) {
        time_t update_ts = it->update_ts;
        uint32_t diff = curr_ts - update_ts;
        if (it->sched_flag == 0 && diff >= TIMEOUT) {
            count ++;
            eosio::print("foce sched waiting users, ");
            uint64_t user_id = it->uuid;
            uint64_t gameuuid = it->gameuuid;
            time_t ts = it->ts;
            uint128_t sender_id = 0;
            sched(user_id, gameuuid, ts, sender_id);
        } else {
            eosio::print("no need to foce sched waiting users");
        }
        it ++;
    }

    eosio::print("{");
    eosio::print("\"count\": ", count, ", ");
    eosio::print("\"msg\": \"forcesched successfully.\"");
    eosio::print("}");


}
// action
void dice::toss(eosio::name user, uint64_t gameuuid, uint32_t seed) {
    // 1. user authentication
    // 2. randomly generate dice number
    // 3. assign that number to that user
    // NOTE: each user can only toss once

    require_auth(user);

    auto end = _scheduled_users.cend();
    bool once = false;

    for (auto _user = _scheduled_users.cbegin(); _user != end; _user) {
        if (_user->gameuuid == gameuuid) { // in this game
            // this user doesn't toss yet
            if (_user->user == user && _user->steps == 0) {
                // _user->expired_ts >= now()) {
                if (_user->expired_ts >= now()) {

                    uint128_t proof = _user->proof;
                    uint32_t dice_number = get_rnd_dice_number(proof);

                    eosio::print("{");
                    eosio::print("\"dice_number\": ", dice_number, ", ");
                    eosio::print("\"user\": \"", user, "\", ");
                    eosio::print("\"gameuuid\": ", gameuuid, ", ");
                    eosio::print("\"update_ts\": ", now(), ", ");
                    eosio::print("\"expired_ts\": ", _user->expired_ts, ", ");
                    eosio::print("\"expired\":", false, ", ");
                    eosio::print("\"toss\": ", true);
                    eosio::print("}");

                    _scheduled_users.modify(_user, get_self(), [&](auto &u) {
                                                                   u.steps = dice_number;
                                                                   u.update_ts = now();
                                                               });
                    once = true;
                } else {
                    // expired, force remove it offline
                    eosio::print("{");
                    eosio::print("\"dice_number\": ", _user->steps, ", ");
                    eosio::print("\"user\": \"", user, "\", ");
                    eosio::print("\"gameuuid\": ", gameuuid, ", ");
                    eosio::print("\"update_ts\": ", now(), ", ");
                    eosio::print("\"expired_ts\": ", _user->expired_ts, ", ");
                    eosio::print("\"expired\":", true, ", ");
                    eosio::print("\"toss\": ", false);
                    eosio::print("}");

                }
            } else {
                eosio::print("{");
                eosio::print("\"dice_number\": ", _user->steps, ", ");
                eosio::print("\"user\": \"", user, "\", ");
                eosio::print("\"gameuuid\": ", gameuuid, ", ");
                eosio::print("\"update_ts\": ", now(), ", ");
                eosio::print("\"expired_ts\": ", _user->expired_ts, ", ");
                eosio::print("\"expired\":", false, ", ");
                // eosio::print("\"toss\": ", false);
                eosio::print("\"toss\": ", true);
                eosio::print("}");
                once = true;
            }
        }
        if (once) {
            break;              // this game only executes once
        }
    }
    if (! once) {
        eosio::print("{");
        eosio::print("\"toss\": ", false);
        eosio::print("}");
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
    bool expired = (_user->expired_ts > now());
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
// #ifdef DEBUG
//     eosio::print("|> right: ", right, ", left: ", left, ", up: ", up, ", down: ", down, " |");
// #endif
    if (inner_steps > 6) {
        eosio::print("inner steps: ", inner_steps, ", ");
        eosio_assert(false, "inner steps error");
    }

    if (right > inner_steps) {
        eosio::print("right steps: ", right, ", ");
        eosio_assert(false,  "right steps error");
    }
    if (left > inner_steps) {
        eosio::print("left steps: ", left, ", ");
        eosio_assert(false, "left  steps error");
    }
    if (up > inner_steps) {
        eosio::print("up steps: ", up, ", ");
        eosio_assert(false, "up    steps error");
    }

    if (down > inner_steps) {
        eosio::print("down steps: ", down, ", ");
        eosio_assert(false, "down  steps error");
    }

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

    point origin_pt = point(_game->pos);

    bool right_overflow = (pt.col + right < pt.col);
    if (! right_overflow) {
        // 1. move right is correct
        // update virtual position
        pt.col += right;
        // move left
        bool left_overflow = (pt.col - left > pt.col);
        if (! left_overflow) {
            // ok
            right_first = true;
        } else {
            eosio_assert(false, "error");
        }
    } else {
        // 2. move right is wrong
        // try move left first
        bool left_overflow = (pt.col - left > pt.col);
        if (! left_overflow) {
            pt.col -= left;
            bool right_overflow = (pt.col + right < pt.col);
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
    bool up_overflow = (pt.row - up > pt.row);
    if (! up_overflow) {
        pt.row -= up;
        bool down_overflow = (pt.row + down < pt.row);
        if (! down_overflow) {
            // ok
            up_first = true;
        } else {
            eosio_assert(false, "error");
        }
    } else {
        bool down_overflow  = (pt.row + down < pt.row);
        if (! down_overflow) {
            pt.row += down;
            bool up_overflow = (pt.row - up > pt.row);
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
        moveright(user, gameuuid, right);
        moveleft(user, gameuuid, left);
    } else {
        moveleft(user, gameuuid, left);
        moveright(user, gameuuid, right);
    }
    if (up_first) {
        moveup(user, gameuuid, up);
        movedown(user, gameuuid, down);
    } else {
        movedown(user, gameuuid, down);
        moveup(user, gameuuid, up);
    }

    bool is_won = reach_goal(*_game);
    if (is_won) {
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
        eosio::print("\"origin_pos\": [", origin_pt.row, ", ", origin_pt.col, "], ");
        eosio::print("\"current_pos\": [", pt.row, ", ", pt.col, "], ");

        // eosio::print("\"row\": ", pt.row, ", ");
        // eosio::print("\"col\": ", pt.col, ", ");
        eosio::print("\"update_ts\": ", now(), ", ");

        eosio::print("\"direction\": ");
        eosio::print("{");
        eosio::print("\"right_first\":", right_first, ", ");
        eosio::print("\"up_first\": ", up_first, ", ");

        eosio::print("\"right\": ", right, ", ");
        eosio::print("\"left\": ", left, ", ");
        eosio::print("\"up\": ", up, ",");
        eosio::print("\"down\": ", down);

        eosio::print("}, ");

        if (_game->status == GAME_OVER) {
            eosio::print("\"last_goal\": ", true, ", ");
        } else {
            eosio::print("\"last_goal\": ", false, ", ");
        }
        eosio::print("\"win\": ", true);

        eosio::print("}");

        // TODO: how to deal with the rest people when game is closed
        // remove them from waitting pool and the last winner gets all prize
        if (_game->status == GAME_OVER) {
            // last goal
            _winners.emplace(get_self(), [&](auto &w) {
                                             w.uuid = _winners.available_primary_key();
                                             w.user = user;
                                             w.gameuuid = _game->uuid;
                                             w.ts = now();
                                             w.update_ts = now();
                                             w.awards = _game->shadow_awards;
                                             w.row = pt.row;
                                             w.col = pt.col;
                                             w.acc_awards = 0;
                                         });
            desc_game_shadow_awards(*_game, _game->shadow_awards);
        }
    } else {
        point pt = point(_game->pos);
        eosio::print("{");
        eosio::print("\"gameuuid\": ", gameuuid, ", ");
        eosio::print("\"user\": \"", user, "\", ");
        eosio::print("\"awards\": ", _game->awards, ", ");
        // eosio::print("\"row\": ", pt.row, ", ");
        // eosio::print("\"col\": ", pt.col, ", ");
        eosio::print("\"origin_pos\": [", origin_pt.row, ", ", origin_pt.col, "], ");
        eosio::print("\"current_pos\": [", pt.row, ", ", pt.col, "], ");

        eosio::print("\"update_ts\": ", now(), ", ");

        eosio::print("\"direction\": ");
        eosio::print("{");
        eosio::print("\"right_first\":", right_first, ", ");
        eosio::print("\"up_first\": ", up_first, ", ");

        eosio::print("\"right\": ", right, ", ");
        eosio::print("\"left\": ", left, ", ");
        eosio::print("\"up\": ", up, ",");
        eosio::print("\"down\": ", down);
        eosio::print("}, ");

        eosio::print("\"last_goal\": ", false, ", ");
        eosio::print("\"win\": ", false);
        eosio::print("}");
    }
    // erase this user in scheduled_users table
    _scheduled_users.erase(_user);
    desc_num_sched_users(1);
    _games.modify(_game, get_self(), [&](auto &g) {
                                         g.total_number --;
                                     });
}
// action
void dice::sendtokens(eosio::name user, uint64_t gameuuid) {
    require_auth(admin);

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

    distribute(*_game,
               user,
               participants,
               _winner->awards);
    uint64_t real_awards = _winner->awards*WINNER_PERCENT;
    update_heroes(user, real_awards);
    _winners.erase(_winner);

    if (_game->status == GAME_OVER) {
        // clear all waiting users in this game
        auto it2 = _waitingpool.begin();
        while (it2 != _waitingpool.end()) {
            if (it2->gameuuid == _game->uuid) {
                it2 = _waitingpool.erase(it2);
            } else {
                it2 ++;
            }
        }
        // clear all sched users in this game
        auto it3 = _scheduled_users.begin();
        while (it3 != _scheduled_users.end()) {
            if (it3->gameuuid == _game->uuid) {
                it3 = _scheduled_users.erase(it3);
            } else {
                it3 ++;
            }
        }
    }
}
// action

void dice::moveright(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    // DONE check valid user in given game;
    // DONE: check valid steps
    // DONE: update steps
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    bool overflow = (pt.col + steps < pt.col);
    eosio_assert(! overflow, "upper overflow");
    pt.col = pt.col + steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        _game->debug();
        eosio::print("right steps: ", steps, ", ");
        eosio_assert(false, "invalid right steps");
    }

    update_game_pos(*_game, pt);
}

void dice::moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    bool overflow = (pt.col - steps > pt.col);
    eosio_assert(! overflow, "lower overflow");

    pt.col = pt.col - steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        _game->debug();
        eosio::print("left steps: ", steps, ", ");
        eosio_assert(false, "invalid left steps");
    }

    update_game_pos(*_game, pt);
}

void dice::moveup(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    bool overflow = (pt.row - steps > pt.row);
    eosio_assert(! overflow, "upper overflow");

    pt.row = pt.row - steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        _game->debug();
        eosio::print("up steps: ", steps, ", ");
        eosio_assert(false, "invlaid up steps");
    }

    update_game_pos(*_game, pt);
}

void dice::movedown(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    bool overflow = (pt.row + steps < pt.row);
    eosio_assert(! overflow, "lower overflow");
    pt.row = pt.row + steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        _game->debug();
        eosio::print("down steps: ", steps, ", ");
        eosio_assert(false, "invalid down steps");
    }

    update_game_pos(*_game, pt);
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

    // to winner
    if (winner_amount > 0) {
        inner_transfer(get_self(), winner, winner_amount);
    } else if (awards > 0) {
        inner_transfer(get_self(), winner, awards);
    } else {
        eosio_assert(false, "bug ? no awards in pool");
    }

    // to dividend pool
    if (dividend_pool_amount > 0) {
        inner_transfer(get_self(), dividend_account, dividend_pool_amount);
    }
    // to our platform
    if (platform_amount > 0) {
        inner_transfer(get_self(), platform, platform_amount);
    }

    // to all participants, for fun
    if (participants_amount > 0) {
        for (auto part : participants) {
            inner_transfer(get_self(), part, participants_amount);
        }
    }
}

void dice::inner_transfer(eosio::name from, eosio::name to, int64_t amount) {

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

void dice::jsonify_game(const game & _g) {
    eosio::print("{");
    eosio::print("\"uuid\": ", _g.uuid, ", ");
    eosio::print("\"board_width\": ", _g.board_width, ", ");
    eosio::print("\"board_height\": ", _g.board_height, ", ");
    eosio::print("\"awards\": ", _g.awards, ", ");
    eosio::print("\"fee\": ", _g.fee, ", ");
    eosio::print("\"shadow_awards\": ", _g.shadow_awards, ", ");
    point pt = point(_g.pos);
    eosio::print("\"pos\": (", pt.row, ", ", pt.col, "), ");
    eosio::print("\"status\": ", _g.status, ", ");
    eosio::print("\"total_waiting_users\": ", _g.total_number, ", ");
    eosio::print("\"goals\": [");
    for (uint8_t i = 0; i < _g.goals.size(); i ++) {
        point tmp = point(_g.goals[i]);
        eosio::print("(", tmp.row, ", ", tmp.col, ")");
        if (i + 1 != _g.goals.size()) {
            eosio::print(", ");
        }
    }
    eosio::print("], ");
    eosio::print("\"gamename\": ", "\"", _g.gamename, "\"");
    eosio::print("}");
}
// helper functions
void dice::getbriefmaps(eosio::name user, uint64_t gameuuid, uint32_t status) {
    require_auth(user);
    uint32_t status_game_start = status & GAME_START;
    uint32_t status_game_over  = status & GAME_OVER;
    uint32_t status_game_close = status & GAME_CLOSE;

    auto _game = _games.find(gameuuid);
    if (_game == _games.cend()) {
        // not found any game uuid, return all
        eosio::print("[");
        for (auto _g = _games.begin(); _g != _games.end(); ) {
            if (_g->status == status_game_start ||
                _g->status == status_game_over  ||
                _g->status == status_game_close) {
                jsonify_game(*_g);
            }
            ++_g;

            if (_g != _games.cend()) {
                eosio::print(", ");
            }
        }
        eosio::print("]");
    } else {
        // return specifiy gameuuid
        if (_game->status == status_game_start ||
            _game->status == status_game_over  ||
            _game->status == status_game_close) {
            eosio::print("[");
            jsonify_game(*_game);
            eosio::print("]");
        } else {
            eosio::print("[{}]");
        }
    }
}

void dice::jsonify_user(const users &_u, uint32_t lineno) {
/// doing
    eosio::print("{");
    eosio::print("\"uuid\": ", _u.uuid, ", ");
    eosio::print("\"gameuuid\": ", _u.gameuuid, ", ");
    eosio::print("\"steps\": ", _u.steps, ", ");
    eosio::print("\"no\": ", _u.no, ", ");
    eosio::print("\"user\": ", "\"", _u.user, "\", ");
    eosio::print("\"ts\": ", _u.ts, ", ");
    // point pt = point(_g.pos);
    eosio::print("\"update_ts\": ", _u.update_ts, ", ");
    eosio::print("\"expired_ts\": ", _u.expired_ts, ", ");
    eosio::print("\"proof\": ", _u.proof, ", ");
    eosio::print("\"sched_flag\": ", (uint32_t)_u.sched_flag, ", ");
    eosio::print("\"lineno\": ", lineno);
    eosio::print("}");
}
void dice::jsonify_hero(const hero &_h) {
    eosio::print("{");
    eosio::print("\"uuid\": ", _h.uuid, ", ");
    eosio::print("\"gameuuid\": ", _h.gameuuid, ", ");
    eosio::print("\"user\": \"", _h.user, "\", ");
    eosio::print("\"awards\": ", _h.awards, ", ");
    eosio::print("\"acc_awards\": ", _h.acc_awards, ", ");
    eosio::print("\"ts\": ", _h.ts, ", ");
    eosio::print("\"update_ts\": ", _h.update_ts, ", ");
    eosio::print("\"row\": ", _h.row, ", ");
    eosio::print("\"col\": ", _h.col);
    eosio::print("}");
}

void dice::getmapdetail(eosio::name user, uint64_t gameuuid, uint32_t waitlimit, uint32_t schedlimit, uint32_t herolimit) {
    // 1. number of schedule users
    // 2. number of waiting users
    // 3. random pick 20 waiting users
    // 4. pick latest 20 winners
    require_auth(user);
    auto _game = _games.find(gameuuid);
    eosio_assert(_game != _games.cend(), "not found game");
    eosio::print("{");
    eosio::print("\"game\": ");
    jsonify_game(*_game);
    eosio::print(", ");

    if (waitlimit == 0) {
        // default return 20 users
        waitlimit = 20;
    }
    auto wait_users = _waitingpool.get_index<"gameuuid"_n>();
    auto wait_users_lower_bound = wait_users.lower_bound(gameuuid);
    auto wait_users_upper_bound = wait_users.upper_bound(gameuuid);
    eosio::print("\"waitusers\": [");
    {
        // wait users
        uint32_t wait_count = 0;

        while ((wait_users_lower_bound != wait_users_upper_bound) &&
               (wait_count < waitlimit)) {
            // jsonify
            if (wait_users_lower_bound->sched_flag == 0) {
                jsonify_user(*wait_users_lower_bound);
                wait_count ++;
                wait_users_lower_bound ++;
                if (wait_count == waitlimit ||
                    wait_users_lower_bound == wait_users_upper_bound) {
                    // do nothing
                } else {
                    eosio::print(", ");
                }
            } else {
                wait_users_lower_bound ++;
            }
        }
    }
    eosio::print("], ");

    uint32_t sched_count = 0;
    if (schedlimit == 0) {
        schedlimit = 20;
    }
    eosio::print("\"schedusers\": [");
    // schedule users
    auto sched_users = _scheduled_users.get_index<"gameuuid"_n>();
    {
        auto sched_users_lower_bound = sched_users.lower_bound(gameuuid);
        auto sched_users_upper_bound = sched_users.upper_bound(gameuuid);

        while ((sched_users_lower_bound != sched_users_upper_bound) &&
               (sched_count < schedlimit)) {
            jsonify_user(*sched_users_lower_bound);
            sched_count ++;
            sched_users_lower_bound ++;
            if (sched_count == schedlimit ||
                sched_users_lower_bound == sched_users_upper_bound) {
                // do nothing
            } else {
                eosio::print(", ");
            }
        }
    }
    eosio::print("], ");

    if (herolimit == 0) {
        herolimit = 20;
    }
    eosio::print("\"heroes\": [");
    auto heroes_index = _heroes.get_index<"gameuuid"_n>();

    auto hero_index_lower_bound = heroes_index.lower_bound(gameuuid);
    auto hero_index_upper_bound = heroes_index.upper_bound(gameuuid);

    {
        uint32_t hero_count = 0;

        while ((hero_index_upper_bound != hero_index_lower_bound) &&
               (hero_count < herolimit)) {
            jsonify_hero(*hero_index_lower_bound);
            hero_count ++;
            hero_index_lower_bound ++;
            if (hero_count == herolimit ||
                hero_index_upper_bound == hero_index_lower_bound) {

            } else {
                eosio::print(", ");
            }
        }
    }

    eosio::print("]");
    eosio::print("}");
}

void dice::getmylineno(eosio::name user, uint64_t gameuuid) {
    require_auth(user);
    auto user_index = _scheduled_users.get_index<"username"_n>();
    auto user_index_lower = user_index.lower_bound(user.value);
    auto user_index_upper = user_index.upper_bound(user.value);
    if (user_index_lower == user_index_upper) {
        // not found
        eosio::print("{}");
    } else {
        // found
        auto it = _scheduled_users.cbegin();
        auto cend = _scheduled_users.cend();

        uint32_t lineno = 0;
        while (it != cend) {
            if (it->user == user) {
                break;
            }
            lineno ++;
            it ++;
        }

        jsonify_user(*it, lineno);

    }
}

void dice::getheroes(eosio::name user, uint64_t gameuuid, uint32_t herolimit) {
    require_auth(user);
    if (herolimit == 0) {
        herolimit = 30;
    }
    auto _game = _games.find(gameuuid);
    uint32_t hero_count = 0;

    if (_game == _games.cend()) {
        // not found
        auto hero_begin = _heroes.crbegin();
        eosio::print("[");

        auto hero_end = _heroes.crend();
        while ((hero_begin != hero_end) &&
               (hero_count < herolimit)) {
            jsonify_hero(*hero_begin);
            hero_begin ++;
            hero_count ++;
            if (hero_count == herolimit ||
                hero_begin == hero_end) {

            } else {
                eosio::print(", ");
            }
        }
        eosio::print("]");

    } else {
        // found
        auto heroes_index = _heroes.get_index<"gameuuid"_n>();
        auto hero_index_lower_bound = heroes_index.lower_bound(gameuuid);
        auto hero_index_upper_bound = heroes_index.upper_bound(gameuuid);
        // -- hero_index_upper_bound;
        eosio::print("[");

        while ((hero_index_upper_bound != hero_index_lower_bound) &&
               (hero_count < herolimit)) {
            jsonify_hero(*hero_index_lower_bound);
            hero_count ++;
            hero_index_lower_bound ++;
            if (hero_count == herolimit ||
                hero_index_upper_bound == hero_index_lower_bound) {
            } else {
                eosio::print(", ");
            }
        }
        eosio::print("]");
    }
}
void dice::getmysched(eosio::name user, uint64_t gameuuid, uint32_t schedlimit) {
    require_auth(user);
    if (schedlimit == 0) {
        schedlimit = 20;
    }
    uint32_t sched_count = 0;

    auto _scheduled_user_index = _scheduled_users.get_index<"username"_n>();

    auto _scheduled_user_index_lower = _scheduled_user_index.lower_bound(user.value);
    auto _scheduled_user_index_upper = _scheduled_user_index.upper_bound(user.value);
    eosio::print("[");

    while (_scheduled_user_index_lower != _scheduled_user_index_upper &&
           sched_count < schedlimit) {

        jsonify_user(*_scheduled_user_index_lower);
        _scheduled_user_index_lower ++;
        sched_count ++;

        if (_scheduled_user_index_lower == _scheduled_user_index_upper ||
            sched_count == schedlimit) {

        } else {
            eosio::print(", ");
        }
    }
    eosio::print("]");

}
void dice::getmydetail(eosio::name user) {

    require_auth(user);
    // auto _game = _games.find(gameuuid);

    // if (_game == _games.cend()) {
        // not found
        // 1. wait pool
        // 2. sched pool
        // 3. hero pool
        // 4. all gameuuid I participants
        std::set<uint64_t> gameuuid_set;

        auto wait_index = _waitingpool.get_index<"username"_n>();
        auto wait_index_lower = wait_index.lower_bound(user.value);
        auto wait_index_upper = wait_index.upper_bound(user.value);
        eosio::print("\"wait\": [");
        while (wait_index_lower != wait_index_upper) {
            uint64_t _gameuuid = wait_index_lower->gameuuid;
            gameuuid_set.insert(_gameuuid);

            auto _g = _games.find(_gameuuid);
            jsonify_game(*_g);
            wait_index_lower ++;

            if (wait_index_lower != wait_index_upper) {
                eosio::print(", ");
            }
        }
        eosio::print("], ");
        // sched
        auto sched_index = _scheduled_users.get_index<"username"_n>();
        auto sched_index_lower = _scheduled_users.lower_bound(user.value);
        auto sched_index_upper = _scheduled_users.upper_bound(user.value);
        eosio::print("\"sched\": [");
        while (sched_index_lower != sched_index_upper) {
            uint64_t _gameuuid = sched_index_lower->gameuuid;
            gameuuid_set.insert(_gameuuid);

            auto _g = _games.find(_gameuuid);
            jsonify_game(*_g);

            sched_index_lower ++;
            if (sched_index_lower != sched_index_upper) {
                eosio::print(", ");
            }
        }
        eosio::print("], ");

        eosio::print("\"hero\": [");

        auto hero_index = _heroes.get_index<"username"_n>();

        auto hero_index_lower = hero_index.lower_bound(user.value);
        auto hero_index_upper = hero_index.upper_bound(user.value);
        while (hero_index_lower != hero_index_upper) {
            uint64_t _gameuuid = hero_index_lower->gameuuid;
            gameuuid_set.insert(_gameuuid);

            auto _g = _games.find(_gameuuid);
            jsonify_game(*_g);
            ++ hero_index_lower;

            if (hero_index_lower != hero_index_upper) {
                eosio::print(", ");
            }
        }
        eosio::print("]");
    // } else {
    //     // found
    // }
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
                (version)
                (debug)
                (clear)
                (clear2)
                (delayid)
                (addgame)
                (startgame)
                (move)
                (toss)
                (sendtokens)
                (sched)
                (schedhelper)
                (rmexpired)
                (forcesched)
                (getbriefmaps)
                (getmapdetail)
                (getmylineno)
                (getheroes)
                (getmysched)
                (getmydetail)
    )
