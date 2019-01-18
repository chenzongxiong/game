#include "dice.hpp"


// action
#if DEBUG
void dice::version() {
    eosio::print(this->_VERSION.c_str());
}

void dice::clear2(std::string tbl) {
    require_auth(admin);

    if (tbl == "gametbl") {
        auto it1 = _games.begin();
        while (it1 != _games.end()) {
            it1 = _games.erase(it1);
        }
    }
    if (tbl == "waittbl") {
        auto it2 = _waitingpool.begin();
        while (it2 != _waitingpool.end()) {
            it2 = _waitingpool.erase(it2);
        }
    }
    if (tbl == "schedtbl") {
        auto it3 = _scheduled_users.begin();
        while (it3 != _scheduled_users.end()) {
            it3 = _scheduled_users.erase(it3);
        }
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
 * administrator actions
 ********************************************************************************/
void dice::addgame(eosio::name gamename, uint32_t width,
                   uint32_t height, uint32_t status, int64_t fee) {

    require_auth(admin);

    eosio_assert(fee > 0, "fee < 0");
    eosio_assert(MAXSIZE > width, "width > MAXSIZE, invalid");
    eosio_assert(MAXSIZE > height, "height > MAXSIZE, invalid");
    eosio_assert(width >= 23, "width < 23, invalid");
    eosio_assert(height >= 23, "height < 23, invalid");
    eosio_assert(status == GAME_CLOSE || status == GAME_START, "invalid status");
    // NOTE: only access by our platform, so it's safe to use timestamp as a random number see
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
                                   g.status = status;
                                   g.board_width = width;
                                   g.board_height = height;
                                   g.fee = fee;
                                   g.awards = 0;
                                   g.shadow_awards = 0;
                                   g.total_number = 0;
                                   g.total_sched_number = 0;
                                   g.gamename = gamename;
                                   for (auto _gl : goals) {
                                       g.goals.push_back(_gl);
                                   }
                               });
}

void dice::setgamestat(uint64_t gameuuid, uint32_t status) {
    require_auth(admin);
    if (status != GAME_START ||
        status != GAME_CLOSE ||
        status != GAME_OVER) {
        eosio_assert(false, "invalid st_game status");
    }
    auto _game = _games.find(gameuuid);
    eosio_assert(_game != _games.cend(), "st_game not foud");
    _games.modify(_game, get_self(), [&](auto &g) {
                                         g.status = status;
                                     });
}

void dice::rmexpired() {
    require_auth(admin);

    uint32_t count = 0;
    time_t curr_ts = now();

    auto it3 = _scheduled_users.begin();
    while (it3 != _scheduled_users.end()) {
        if (curr_ts >= it3->expired_ts) {
            it3 = _scheduled_users.erase(it3);
            auto _game = _games.find(it3->gameuuid);
            if (_game != _games.cend()) {
                _games.modify(_game, get_self(), [&](auto &g){
                                                     g.total_sched_number --;
                                                 });
            }

            count ++;
        } else {
            it3 ++;
        }
    }
    // desc_num_sched_users(count);
    eosio::print("{");
    eosio::print("\"count\":", count, ", ");
    eosio::print("\"msg\": \"remove expired user successfully\"");
    eosio::print("}");
}

void dice::forcesched(uint64_t seed) {
    require_auth(admin);

    time_t curr_ts = now();

    pcg32_srandom_r(seed, (uint64_t)curr_ts);
    uint128_t proof = 0;
    proof |= (uint128_t)seed;
    proof <<= 64;
    proof |= (uint128_t)curr_ts;

    for (auto &g : _games) {
        if (g.status != GAME_START) { // only do scheduling for start game
            continue;
        }

        uint64_t sched_num = g.total_sched_number;
        uint64_t max_sched_user_in_pool = get_max_sched_user_in_pool();
        if (sched_num >= max_sched_user_in_pool) {
            eosio::print("error here !!!");
            eosio::print("sched_number: ", sched_num);
        } else {
            uint64_t num_to_sched = max_sched_user_in_pool - sched_num;
            std::vector<st_users> latest_scheduling_users;
            for (auto _user : _waitingpool) {
                if (_user.gameuuid == g.uuid) {
                    latest_scheduling_users.push_back(_user);
                }
            }
            if (latest_scheduling_users.size() < num_to_sched) {
                num_to_sched = latest_scheduling_users.size();
            }
            uint64_t i = 0;
            while (i < num_to_sched) {         // forever looping ?
                uint64_t idx = pcg32_boundedrand_r(num_to_sched);
                auto _user = latest_scheduling_users[idx];
                sched(_user.uuid, _user.gameuuid, curr_ts, proof);
                i ++;
            }
        }
    }
    // eosio::print("{");
    // eosio::print("\"count\": ", count, ", ");
    // eosio::print("\"msg\": \"forcesched successfully.\"");
    // eosio::print("}");
}

void dice::sched(uint64_t user_id, uint64_t gameuuid, time_t ts, uint128_t seed) {
    require_auth(admin);

    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "bug ? st_game not found.");
    eosio_assert(_game->status == GAME_START, "bug ? st_game does not start");
    uint128_t proof = seed;
    time_t curr_ts = now();
    uint64_t platform_lucky_number = -1;

    time_t expired_ts = curr_ts + (_game->total_sched_number + 1) * SCHED_TIMEOUT;
    auto _user = _waitingpool.find(user_id);
    eosio_assert(_user != _waitingpool.cend(), "bug? not found user");
    uint64_t sched_user_id = _scheduled_users.available_primary_key();
    _scheduled_users.emplace(get_self(), [&](auto &u) {
                                             // u.uuid = user_id;
                                             u.uuid = sched_user_id;
                                             u.gameuuid = gameuuid;
                                             u.steps = 0;
                                             u.no = platform_lucky_number;
                                             u.user = _user->user;
                                             u.ts = ts;
                                             u.update_ts = curr_ts;
                                             u.expired_ts = expired_ts;
                                             u.proof = proof;
                                             u.sched_flag = 1;
                                         });
    _games.modify(_game, _self, [&](auto &g) {
                                    g.total_sched_number ++;
                                });

    // mapping with waiting pool
    // _waitingpool.erase(_user);
    // remove: set log for waiting user
    // eosio::action(
    //     eosio::permission_level{_self, "active"_n},
    //     _self, "setloguser"_n,
    //     std::make_tuple(
    //         _user->uuid,
    //         _user->gameuuid,
    //         _user->steps,
    //         _user->no,
    //         _user->user,
    //         _user->ts,
    //         _user->update_ts,
    //         _user->expired_ts,
    //         _user->proof,
    //         1
    //         )
    //     ).send();

    _waitingpool.modify(*_user, get_self(),  [&](auto &u) {
                                               u.sched_flag = 1;
                                               u.expired_ts = expired_ts;
                                             });
}
void dice::sendtokens(eosio::name user, uint64_t gameuuid) {
    require_auth(admin);

    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "not found st_game");

    auto _winner = is_won_user_in_game(user, gameuuid);
    eosio_assert(_winner != _winners.cend(), "winner not in st_game");

    std::vector<eosio::name> participants;

    for (auto _u : _waitingpool) {
        if (_u.sched_flag == 0) {
            participants.push_back(_u.user);
        }
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
        // clear all waiting users in this st_game
        const uint32_t action_per_txn = 10;
        auto it2 = _waitingpool.begin();
        eosio::transaction txn {};
        while (it2 != _waitingpool.end()) {
            if (it2->gameuuid == _game->uuid) {
                _games.modify(_game, get_self(), [&](auto &g){
                                                     g.total_number --;
                                                     });
                it2 = _waitingpool.erase(it2);
            } else {
                it2 ++;
            }
        }
        // clear all sched users in this st_game
        txn.actions.clear();
        auto it3 = _scheduled_users.begin();
        while (it3 != _scheduled_users.end()) {
            if (it3->gameuuid == _game->uuid) {
                // auto _game = _games.find(it3->gameuuid);
                _games.modify(_game, get_self(), [&](auto &g){
                                                     g.total_sched_number --;
                                                 });
                it3 = _scheduled_users.erase(it3);
            } else {
                it3 ++;
            }
        }
    }
}

void dice::setrate(uint64_t rate) {
    require_auth(admin);
    auto cfg = _config.get_or_default({});
    cfg.token_exchange_rate = rate;
    if (cfg.token_exchange_rate == 0) {
        cfg.token_exchange_rate = 1;
    }
    _config.set(cfg, get_self());
}

void dice::setairdrop(uint32_t flag) {
    require_auth(admin);
    auto cfg = _config.get_or_default({});
    cfg.airdrop_flag = flag;
    _config.set(cfg, get_self());
}

void dice::showconfig() {
    require_auth(admin);
    auto cfg = _config.get_or_default({});
    eosio::print("sender id: ", cfg.sender_id, ", ");
    eosio::print("stop_remove_sched: ", cfg.stop_remove_sched, ", ");
    eosio::print("sched_no: ", cfg.sched_no, ", ");
    eosio::print("num_sched_users: ", cfg.num_sched_users, ", ");
    eosio::print("token_exchange_rate: ", cfg.token_exchange_rate, ", ");
    eosio::print("airdrop_flag: ", cfg.airdrop_flag, ", ");
    eosio::print("global_seed: ", cfg.global_seed, ", ");

}

void dice::setschednum(uint64_t schednum) {
    require_auth(admin);
    auto cfg = _config.get_or_default({});
    cfg.max_sched_user_in_pool = schednum;
    _config.set(cfg, get_self());
}

void dice::setloguser(uint64_t uuid, uint64_t gameuuid, uint32_t steps,
                      uint128_t no, eosio::name user, time_t ts, time_t update_ts,
                      time_t expired_ts, uint128_t proof, uint8_t sched_flag) {
    require_auth(_self);
}

void dice::setloghero(
        uint64_t uuid,
        uint64_t gameuuid,
        eosio::name user,
        int64_t awards,
        int64_t acc_awards,
        uint32_t row,
        uint32_t col,
        time_t ts,
        time_t update_ts) {
    require_auth(_self);
}
/********************************************************************************
 * Users actions
 ********************************************************************************/
void dice::enter(eosio::name user) {
    /**
     * NOTE: a user can enter st_game multiple times
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
        eosio_assert(_game != _games.cend(), "not found st_game.");
        eosio_assert(_game->status != GAME_OVER, "st_game is over");

        if (data.quantity.amount >= _game->fee) { // pay enough fee
            // check st_game
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
            // log a user enter our game
            eosio::action(
                eosio::permission_level{_self, "active"_n},
                _self, "setloguser"_n,
                std::make_tuple(user_id,
                                _game->uuid,
                                0,
                                -1,
                                data.from,
                                now(),
                                now(),
                                -1,
                                -1,
                                0)).send();
            // airdrop
            if (check_airdrop_flag()) {
                // check balance
                uint32_t rate = get_token_exchange_rate();
                uint64_t amt = data.quantity.amount * rate * 100;
                eosio::symbol sym = eosio::symbol("MYEOS", 6);
                eosio::asset airdrop_amt = eosio::asset(amt, sym);

                accounts acnts(token_account, _self.value);
                auto my_account_itr = acnts.find(sym.code().raw());
                if (my_account_itr != acnts.cend() &&
                    my_account_itr->balance.amount >= amt){

                    std::string memo = std::string("User: ") + data.from.to_string() + std::string(" -- Enjoy airdrop! Play: matr0x");
                    eosio::action(
                        eosio::permission_level{_self, "active"_n},
                        token_account, "transfer"_n,
                        std::make_tuple(_self, data.from, airdrop_amt, memo)
                        ).send();
                } else {
                    auto cfg = _config.get_or_default({});
                    cfg.airdrop_flag = 0;
                    _config.set(cfg, get_self());
                }
            }

            _games.modify(_game, get_self(), [&](auto &g) {
                                                 g.total_number ++;
                                             });
            uint64_t seed = data.from.value + (uint64_t)(now());
            update_global_seed(seed);
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

// void dice::schedhelper(uint64_t user_id, uint64_t gameuuid, time_t ts, uint128_t sender_id) {
//     // only this contract can run
//     require_auth(get_self());
//     // uint128_t _sender_id = next_sender_id();

//     uint32_t block_num = tapos_block_num();
//     uint32_t block_prefix = tapos_block_prefix();

//     eosio::transaction txn {};
//     txn.actions.emplace_back(
//         eosio::permission_level{get_self(), "active"_n},
//         get_self(), "sched"_n,
//         std::make_tuple(user_id, gameuuid, now(), sender_id)
//         );
//     // NOTE: delay `x` (3 - 25) seconds, to avoid one man controls too many accounts
//     // this random number generator might be vunerable, but it's fine since
//     // it's only affect the order of the scheduled table.
//     // a hacker might be able to occupy at leading position.
//     // but a common user still has chance to be put at leading position
//     // and a common user can also enter this st_game multiple time to increase
//     // the chance to be put at leading position
//     pcg32_srandom_r(now(), now());
//     uint32_t delayed = pcg32_boundedrand_r(22) + 3;
//     txn.delay_sec = delayed;
//     txn.send(sender_id, get_self(), false);
// }

void dice::toss(eosio::name user, uint64_t gameuuid, uint32_t seed) {
    // NOTE: each user can only toss once
    // STEPS:
    // 1. user authentication
    // 2. randomly generate a dice number
    // 3. assign that number to that user

    require_auth(user);
    auto end = _scheduled_users.cend();
    bool once = false;
    bool expired = false;
    for (auto _user = _scheduled_users.cbegin(); _user != end; _user ++) {
        if (_user->gameuuid == gameuuid) { // in this st_game
            if (_user->user == user) {
                // ok. in first loop, you are the first user in the scheduled table
                if (_user->steps == 0) {
                    // this user doesn't toss yet
                    if (_user->expired_ts >= now()) {
                        // this user is still valid, not expired
                        // size_t trx_size = transaction_size();
                        std::string user_seed_str = std::to_string(seed);
                        eosio::checksum256 user_seed = eosio::sha256(user_seed_str.c_str(),
                                                                     user_seed_str.length());

                        std::string global_seed_str = std::to_string(get_global_seed());
                        eosio::checksum256 global_seed = eosio::sha256(global_seed_str.c_str(),
                                                                       global_seed_str.length());
                        update_global_seed(seed); // normal and hacker takes part in this game to update seed

                        std::string platform_seed_str = std::to_string((uint64_t)_user->proof);
                        eosio::checksum256 platform_seed = eosio::sha256(platform_seed_str.c_str(),
                                                                         platform_seed_str.length());

                        st_seed mixed_seed;
                        mixed_seed.seed1 = user_seed;
                        mixed_seed.seed2 = global_seed;
                        mixed_seed.seed3 = platform_seed;

                        eosio::checksum256 final_seed = eosio::sha256((char *)&mixed_seed.seed1, sizeof(st_seed));
                        auto final_seed_arr = final_seed.get_array();

                        uint128_t proof = 0;
                        for (const auto &e : final_seed_arr) {
                            proof += e;
                        }

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
                        // user is expired, force remove it offline
                        eosio::print("{");
                        eosio::print("\"dice_number\": ", _user->steps, ", ");
                        eosio::print("\"user\": \"", user, "\", ");
                        eosio::print("\"gameuuid\": ", gameuuid, ", ");
                        eosio::print("\"update_ts\": ", now(), ", ");
                        eosio::print("\"expired_ts\": ", _user->expired_ts, ", ");
                        eosio::print("\"expired\":", true, ", ");
                        eosio::print("\"toss\": ", false);
                        eosio::print("}");
                        expired = true;
                    }
                } else {
                    // this user already toss the dice, return the dice number directly
                    eosio::print("{");
                    eosio::print("\"dice_number\": ", _user->steps, ", ");
                    eosio::print("\"user\": \"", user, "\", ");
                    eosio::print("\"gameuuid\": ", gameuuid, ", ");
                    eosio::print("\"update_ts\": ", now(), ", ");
                    eosio::print("\"expired_ts\": ", _user->expired_ts, ", ");
                    eosio::print("\"expired\":", false, ", ");
                    eosio::print("\"toss\": ", true);
                    eosio::print("}");
                    once = true;
                }
            } else {
                // sorry. you are not the first person in this st_game,
                // you cannot toss this round
                eosio::print("{");
                eosio::print("\"dice_number\": ", -1, ", ");
                eosio::print("\"user\": \"", user, "\", ");
                eosio::print("\"gameuuid\": ", gameuuid, ", ");
                eosio::print("\"update_ts\": ", now(), ", ");
                eosio::print("\"expired_ts\": ", _user->expired_ts, ", ");
                eosio::print("\"msg\": ", "\"user found\"", ", ");
                eosio::print("\"expired\":", NULL, ", ");
                eosio::print("\"toss\": ", false);
                eosio::print("}");
                once = true;
            }

        }
        if (once || expired) {
            // if first gameuuid found, then *once* or *expired* must be true
            // then break is executed
            break;              // this st_game only executes once
        }
    }
    if (! once) {
        if (! expired) {
            eosio::print("{");
            eosio::print("\"dice_number\": ", -1, ", ");
            eosio::print("\"user\": \"", user, "\", ");
            eosio::print("\"gameuuid\": ", gameuuid, ", ");
            eosio::print("\"update_ts\": ", -1, ", ");
            eosio::print("\"expired_ts\": ", -1, ", ");
            eosio::print("\"msg\": ", "\"user not found\"", ", ");
            eosio::print("\"expired\": ", NULL, ", ");
            eosio::print("\"toss\": ", false);
            eosio::print("}");
        }
    }
}

void dice::move(eosio::name user, uint64_t gameuuid, uint64_t steps) {
    // STEPS:
    // 1. user authentication
    // 2. check the user is in the given st_game
    // 3. validate the steps from client is the same as we generate from dice.
    // 4. update position
    // 5. distribute awards
    require_auth(user);

    auto _game = get_game_by_uuid(gameuuid);
    eosio_assert(_game != _games.cend(), "not found st_game");
    eosio_assert(_game->status == GAME_START, "st_game does not start");
    incr_game_awards(*_game, _game->fee);

    auto _user = is_sched_user_in_game(user, gameuuid);
    eosio_assert(_user != _scheduled_users.cend(), "user not in st_game");

    bool expired = (_user->expired_ts > now());
    eosio_assert(expired, "bug ? expired user");

    uint32_t inner_steps = _user->steps;
    eosio_assert(inner_steps <= 6 && inner_steps > 0, "inner steps error");

    // 2. check steps is smaller than or equal 6
    // right: steps & 0xffff 0000 0000 0000
    // left : steps & 0x0000 ffff 0000 0000
    // up   : steps & 0x0000 0000 ffff 0000
    // down : steps & 0x0000 0000 0000 ffff
    uint32_t right = (steps & 0xffff000000000000) >> 48;
    uint32_t left  = (steps & 0x0000ffff00000000) >> 32;
    uint32_t up    = (steps & 0x00000000ffff0000) >> 16;
    uint32_t down  = (steps & 0x000000000000ffff);

    eosio_assert(right <= 6 && right >= 0, (std::string("right steps error: ") + std::to_string(right)).c_str());
    eosio_assert(left <= 6 && left >= 0, (std::string("left steps error: ") + std::to_string(left)).c_str());
    eosio_assert(up <= 6 && up >= 0, (std::string("up steps error: ") + std::to_string(up)).c_str());
    eosio_assert(down <= 6 && down >= 0, (std::string("down steps error: ") + std::to_string(down)).c_str());

    if ((right * left != 0 && up    + down == 0) ||
        (up    * down != 0 && right + left == 0)) {
        eosio_assert(false, "invalid combination of steps.");
    }
    uint32_t _steps = (right + left + up + down);
    // 3. check steps is equal to given steps
    eosio_assert(_steps == inner_steps, "total steps error");

    // 4. arrange the sequence
    // DONE: right <--> left
    bool right_first = true;
    bool up_first = true;
    point pt = point(_game->pos);

    point origin_pt = point(_game->pos);

    bool right_valid = ((pt.col + right >= pt.col) && // no overflow
                        (pt.col + right <= _game->board_width)); // still in the game board

    if (right_valid) {
        // 1. move right is correct
        // update virtual position
        pt.col += right;
        // move left
        bool left_valid = (pt.col - left <= pt.col);
        if (left_valid) {
            // ok
            right_first = true;
        } else {
            eosio_assert(false, "error");
        }
    } else {
        // 2. move right is wrong
        // try move left first
        bool left_valid = (pt.col - left <= pt.col); // no overflow
        if (left_valid) {
            // 1. move left is correct
            // update virtual position
            pt.col -= left;
            bool right_valid = ((pt.col + right >= pt.col) && // no overflow
                                (pt.col + right <= _game->board_width)); // still in the game board
            if (right_valid) {
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
    bool up_valid = (pt.row - up <= pt.row);

    if (up_valid) {
        // 1. move up is correct
        // update virutal position
        pt.row -= up;
        bool down_valid = ((pt.row + down >= pt.row) && // no overflow
                           (pt.row + down <= _game->board_height)); // still in the game board
        if (down_valid) {
            // ok
            up_first = true;
        } else {
            eosio_assert(false, "error");
        }
    } else {
        bool down_valid = ((pt.row + down >= pt.row) && // no overflow
                           (pt.row + down <= _game->board_height)); // still in the game board

        if (down_valid) {
            pt.row += down;
            bool up_valid = (pt.row - up <= pt.row);
            if (up_valid) {
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
        // check whether all goals are reached
        close_game(*_game);
        // print information to console
        point pt = point(_game->pos);

        eosio::print("{");
        eosio::print("\"gameuuid\": ", gameuuid, ", ");
        eosio::print("\"user\": \"", user, "\", ");
        eosio::print("\"awards\": ", _game->awards, ", ");
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

        if (_game->status == GAME_OVER) {
            eosio::print("\"last_goal\": ", true, ", ");
        } else {
            eosio::print("\"last_goal\": ", false, ", ");
        }
        eosio::print("\"win\": ", true);

        eosio::print("}");
        // add it to winner_table
        uint64_t winner_id = _winners.available_primary_key();
        uint64_t awards = 0;
        if (_game->status == GAME_OVER) {
            // last goal
            // NOTE: how to deal with the rest people when st_game is closed
            // CURRENT: remove them from waitting pool and the last winner gets all prize
            _winners.emplace(get_self(), [&](auto &w) {
                                             w.uuid = winner_id;
                                             w.user = user;
                                             w.gameuuid = _game->uuid;
                                             w.ts = now();
                                             w.update_ts = now();
                                             w.awards = _game->shadow_awards;
                                             w.row = pt.row;
                                             w.col = pt.col;
                                             w.acc_awards = 0;
                                         });
            awards = _game->shadow_awards;

            desc_game_awards(*_game, _game->awards);
            desc_game_shadow_awards(*_game, _game->shadow_awards);
            // TOOD: remove all users in this game from waiting table and scheduled table
        } else {
            _winners.emplace(get_self(), [&](auto &w) {
                                             w.uuid = winner_id;
                                             w.user = user;
                                             w.gameuuid = _game->uuid;
                                             w.ts = now();
                                             w.update_ts = now();
                                             w.awards = _game->awards;
                                             w.row = pt.row;
                                             w.col = pt.col;
                                             w.acc_awards = 0;
                                   });
            awards = _game->awards;

            desc_game_awards(*_game, _game->awards);
            desc_game_shadow_awards(*_game, _game->awards);
        }

        // log for add winer user into pool
        eosio::action(
            eosio::permission_level{_self, "active"_n},
            _self, "setloghero"_n,
            std::make_tuple(
                winner_id,
                _game->uuid,
                user,
                awards,
                0,
                pt.row,
                pt.col,
                now(),
                now()
                )).send();
    } else {
        point pt = point(_game->pos);
        eosio::print("{");
        eosio::print("\"gameuuid\": ", gameuuid, ", ");
        eosio::print("\"user\": \"", user, "\", ");
        eosio::print("\"awards\": ", _game->awards, ", ");
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
    _games.modify(_game, get_self(), [&](auto &g){
                                         g.total_sched_number --;
                                     });
    eosio::action(
        eosio::permission_level{_self, "active"_n},
        _self, "setloguser"_n,
        std::make_tuple(
            _user->uuid,
            _user->gameuuid,
            _user->steps,
            _user->no,
            _user->user,
            _user->ts,
            _user->update_ts,
            _user->expired_ts,
            _user->proof,
            1
            )
        ).send();
    _scheduled_users.erase(_user);

    // we descrease total waiting number here
    _games.modify(_game, get_self(), [&](auto &g) {
                                         g.total_number --;
                                     });
}

/*********************************************************************************
 * private inner functions
 *********************************************************************************/
void dice::moveright(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    // DONE check valid user in given st_game;
    // DONE: check valid steps
    // DONE: update steps
    if (steps == 0) return;

    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    bool overflow = (pt.col + steps < pt.col);
    eosio_assert(! overflow, "upper overflow");
    pt.col = pt.col + steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        eosio::print("right steps: ", steps, ", ");
        eosio_assert(false, "invalid right steps");
    }

    update_game_pos(*_game, pt);
}

void dice::moveleft(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    if (steps == 0) return;

    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    bool overflow = (pt.col - steps > pt.col);
    eosio_assert(! overflow, "lower overflow");

    pt.col = pt.col - steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        eosio::print("left steps: ", steps, ", ");
        eosio_assert(false, "invalid left steps");
    }

    update_game_pos(*_game, pt);
}

void dice::moveup(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    if (steps == 0) return;

    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    // overflow
    bool overflow = (pt.row - steps > pt.row);
    eosio_assert(! overflow, "upper overflow");

    pt.row = pt.row - steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        // _game->debug();
        eosio::print("up steps: ", steps, ", ");
        eosio_assert(false, "invlaid up steps");
    }

    update_game_pos(*_game, pt);
}

void dice::movedown(eosio::name user, uint64_t gameuuid, uint32_t steps) {
    if (steps == 0) return;

    auto _game = prepare_movement(user, gameuuid, steps);

    point pt = point(_game->pos);
    bool overflow = (pt.row + steps < pt.row);
    eosio_assert(! overflow, "lower overflow");
    pt.row = pt.row + steps;
    bool valid = is_valid_pos(*_game, pt);
    if (! valid) {
        // _game->debug();
        eosio::print("down steps: ", steps, ", ");
        eosio_assert(false, "invalid down steps");
    }

    update_game_pos(*_game, pt);
}

bool dice::reach_goal(const st_game &_game) {
    eosio_assert(_game.status == GAME_START, "invalid st_game status");
    int idx = 0;
    for (auto &_goal : _game.goals) {
        if (_goal == _game.pos && _goal != DELETED_GOAL) {
            // someone reaches exactly the goal
            // remove this goal, set it to DELETED_GOAL, just a flag
            _games.modify(_game, get_self(), [&](auto &g){
                                                 g.goals[idx] = DELETED_GOAL;
                                             });
            return true;
        }
        idx ++;
    }
    return false;
}

void dice::distribute(const st_game& _game,
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
    // int64_t last_goal_amount = awards * LAST_GOAL_PERCENT;

    // to winner
    if (winner_amount > 0) {
        inner_transfer(get_self(), winner, winner_amount, 0);
    } else if (awards > 0) {
        inner_transfer(get_self(), winner, awards, 0);
    } else {
        eosio_assert(false, "bug ? no awards in pool");
    }

    // to dividend pool
    if (dividend_pool_amount > 0) {
        inner_transfer(get_self(), dividend_account, dividend_pool_amount, 0);
    }
    // to our platform
    if (platform_amount > 0) {
        inner_transfer(get_self(), platform, platform_amount, 0);
    }

    // to all participants, for fun
    // here we use defer transaction to make sure it will be successfully even though
    // the size of participants is rather larger
    pcg32_srandom_r(now(), initseq);
    if (participants_amount > 0) {
        for (auto part : participants) {
            uint32_t delay = pcg32_boundedrand_r(30) + 2;
            inner_transfer(get_self(), part, participants_amount, delay);
        }
    }

    incr_game_awards(_game, next_goal_amount);
}

void dice::inner_transfer(eosio::name from, eosio::name to, int64_t amount, int delay) {

    eosio::name permission = "active"_n;
    eosio::asset quantity = eosio::asset(amount, eosio::symbol("EOS", 4));

    // https://blog.csdn.net/ITleaks/article/details/83069431
    // https://blog.csdn.net/ITleaks/article/details/83378319
    eosio::transaction txn {};
    std::string memo = std::string("Congratulations. You got ") + std::to_string(amount) + std::string(" EOS");
    txn.actions.emplace_back(
        eosio::permission_level{ from, permission },
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(from, to, quantity, memo)
        );
    // delay 0 seconds
    // no need introduce deferred action in our case,
    // since this action only trigger by our platform
    // never access by outdoors
    txn.delay_sec = delay;

    txn.send(next_sender_id(), from, false);
}


// void dice::debug(uint32_t steps) {
//     require_auth(admin);

//     if (steps == 0) {
//         eosio::print("hello world");
//         return;
//     }
//     eosio::print("step is not zero");

// }

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
                (clear2)
                (addgame)
                (setgamestat)
                (move)
                (toss)
                (sendtokens)
                // (sched)
                // (schedhelper)
                (rmexpired)
                (forcesched)
                (setloguser)
                (setloghero)
                (setrate)
                (setairdrop)
                (showconfig)
                (setschednum)
                // (debug)
    )
