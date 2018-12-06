cleos=/opt/eosio/bin/cleos

$cleos push action cardgameacc version '{}' -p cardgameacc -f
$cleos push action cardgameacc addgame '{}' -p cardgameacc -f
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
$cleos push action cardgameacc enter '{"user": "player1", "gameuuid": 0}' -p cardgameacc -f
$cleos push action cardgameacc schedusers '{"gameuuid": 0, "total": 2}' -p cardgameacc -f
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
$cleos push action cardgameacc enter '{"user": "player1", "gameuuid": 0}' -p cardgameacc -f
$cleos push action cardgameacc moveright '{"user": "player1", "gameuuid": 0, "steps": 2}' -p cardgameacc -f
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
