cleos=/opt/eosio/bin/cleos

echo "====================VERSION===================="
$cleos push action cardgameacc version '{}' -p cardgameacc -f
echo "====================add game===================="
$cleos push action cardgameacc addgame '{"width": 25, "height": 30, "status": 1, "fee": 10000}' -p cardgameacc -f
$cleos push action cardgameacc startgame '{"gameuuid": 0}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
echo "====================enter===================="
$cleos push action cardgameacc enter '{"user": "player1", "gameuuid": 0}' -p cardgameacc -f
echo "====================schedusersr===================="
$cleos push action cardgameacc schedusers '{"gameuuid": 0, "total": 2}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
# echo "====================enter==================="=
# $cleos push action cardgameacc enter '{"user": "player1", "gameuuid": 0}' -p cardgameacc -f
$cleos transfer player1 cardgameacc "1 EOS" "gameuuid:0" -f
echo "====================move right==================="
# $cleos push action cardgameacc moveright '{"user": "player1", "gameuuid": 0, "steps": 2}' -p cardgameacc -f
$cleos push action cardgameacc toss '{"user": "player4", "gameuuid": 3, "seed": 2}' -p cardgameacc -f
$cleos push action cardgameacc mover '{"user": "player1", "gameuuid": 0, "steps": 2}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
echo "====================intransefer===================="
$cleos push action cardgameacc intransfer '{"from": "cardgameacc", "to": "player1", "amount": 2}' -p cardgameacc -f
