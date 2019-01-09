#include "challenge.hpp"

auto challenge::get_registration(eosio::name org, eosio::name game) {
    auto org_index = registrations.get_index<"organization"_n>();
    auto org_index_lower = org_index.lower_bound(org.value);
    auto org_index_upper = org_index.upper_bound(org.value);
    while (org_index_lower != org_index_upper) {
        if (org_index_lower->game_name == game) {
            return org_index_lower;
        }
        org_index_lower ++;
    }
    eosio_assert(false, "not found registration");
}
1
void challenge::enroll(eosio::name org, eosio::name game, uint64_t min_duration, uint64_t max_duration) {
    require_auth(org);
    registrations.emplace(get_self(), [&](auto &r) {
                                          r.id = registrations.available_primary_key();
                                          r.organization = org;
                                          r.game_name = game;
                                          r.min_duration = min_duration;
                                          r.max_duration = max_duration;
                                      });
}

void challenge::startgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target,
                          uint64_t bet, eosio::name referuser, uint64_t ts) {
    require_auth(get_self());

    auto registration = get_registration(org, game);

    games.emplace(get_self(), [&](auto &g) {
                                  g.id = games.available_primary_key();
                                  g.registration_id = registration->id;
                                  g.user = user;
                                  g.refer_user = referuser;
                                  g.bet = bet;
                                  g.target = target;
                                  g.duration = -1;
                              });
    // genarate duration randomly again
}

void challenge::endgame(eosio::name user, eosio::name org, eosio::name game, uint64_t target, uint64_t achievement,
                        uint64_t duration,
                        eosio::name referuser, uint64_t ts) {
    require_auth(get_self());
    auto registration = get_registration(org, game);

    auto game_index = games.get_index<"registration"_n>();
    auto game_index_lower = game_index.lower_bound(registration->id);
    auto game_index_upper = game_index.upper_bound(registration->id);

    while (game_index_lower != game_index_upper) {
        if (game_index_lower->user == user &&
            game_index_lower->target == target &&
            game_index_lower->duration == duration &&
            game_index_lower->finished == 0) {
            eosio_assert(game_index_lower->achievement == 0, "error happens");
            games.modify(*game_index_lower, get_self(),
                         [&](auto &g) {
                             g.achievement = achievement;
                             g.finished = 1;
                         });
            // TODO: add information to hero table
            break;
        }
        game_index_lower ++;
    }
}


EOSIO_DISPATCH(challenge,
               (enroll)
               (startgame)
               (endgame)
    )
