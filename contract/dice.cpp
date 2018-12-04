#include "dice.hpp"
#include <eosiolib/asset.hpp>

void dice::version() {
    eosio::print(_VERSION.c_str());
}
int dice::random() {
    return 1;
}

void dice::addgame(std::string gamename) {
    eosio::print(gamename.c_str());
    _games.emplace(get_self(), [&](auto &g) {
                                   g.uuid= _games.available_primary_key();
                                   g.game_name = gamename;
                                   g.pos = 0xfffffff0;
                               });
}

void dice::debug() {
    for (auto &_game : _games) {
        _game.debug();
    }

    point pt = point(0xff0000ff);
    for (auto &_game : _games) {
        auto it = _games.find(_game.uuid);
        eosio_assert(it != _games.end(), "could not find the game");

        _games.modify(it, get_self(), [&](auto &g){
                                          set_game_pos(g, pt);
                                      });
    }
    for (auto &_game : _games) {
        _game.debug();
    }
    for (auto &_game : _games) {
        point pt = point(_game.pos);
        eosio::print("row: ", pt.row, ", col: ", pt.col, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    }
}

void dice::transfer() {
    // eosio::name from = "arestest4321"_n;
    // eosio::name to = "arestest1234"_n;
    eosio::name from = "player1"_n;
    eosio::name to = "cardgameacc"_n;
    eosio::name permission = "active"_n;
    // std::vector<eosio::permission_level> auths {eosio::permission_level {from, permission}, eosio::permission_level {to, permission}};
    eosio::asset price = eosio::asset(10000, eosio::symbol("EOS", 4));
    // cleos set account permission your_account active '{"threshold": 1,"keys": [{"key": "EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4","weight": 1}],"accounts": [{"permission":{"actor":"your_contract","permission":"eosio.code"},"weight":1}]}' owner -p your_account
    eosio::action(
        eosio::permission_level{ from, permission },
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(from,to, price, std::string(""))
        ).send();
}

void dice::enter(eosio::name user, std::string room) {
    /**
     * 1. tranfer fee
     * 2. put him/her in waiting room
     */
}
void dice::moveright(uint16_t steps) {

}

void dice::moveleft(uint16_t steps) {

}

void dice::moveup(uint16_t steps) {

}

void dice::movedown(uint16_t steps) {

}

EOSIO_DISPATCH(dice,
               (version)(addgame)(debug)(transfer)(moveright)(moveleft)(moveup)(movedown))
