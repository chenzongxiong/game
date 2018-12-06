cleos=/opt/eosio/bin/cleos

echo "====================VERSION===================="
$cleos push action cardgameacc version '{}' -p cardgameacc -f
echo "====================add game===================="
$cleos push action cardgameacc addgame '{}' -p cardgameacc -f
$cleos push action cardgameacc startgame '{"gameuuid": 0}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
echo "====================enter===================="
$cleos push action cardgameacc enter '{"user": "player1", "gameuuid": 0}' -p cardgameacc -f
echo "====================schedusersr===================="
$cleos push action cardgameacc schedusers '{"gameuuid": 0, "total": 2}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
echo "====================enter==================="=
$cleos push action cardgameacc enter '{"user": "player1", "gameuuid": 0}' -p cardgameacc -f
echo "====================move right==================="=
$cleos push action cardgameacc moveright '{"user": "player1", "gameuuid": 0, "steps": 2}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
