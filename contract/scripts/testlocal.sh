cleos=/opt/eosio/bin/cleos

$cleos push action cardgameacc clear '{}' -p cardgameacc -f
echo "====================VERSION===================="
$cleos push action cardgameacc version '{}' -p cardgameacc -f
echo "====================add game===================="
$cleos push action cardgameacc addgame '{"width": 23, "height": 23, "status": 1, "fee": 10000, "gamename": "test"}' -p cardgameacc -f
$cleos push action cardgameacc startgame '{"gameuuid": 0}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
echo "====================enter===================="
# $cleos push action cardgameacc enter '{"user": "player1", "gameuuid": 0}' -p cardgameacc -f
$cleos transfer player1 cardgameacc "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 cardgameacc "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 cardgameacc "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 cardgameacc "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 cardgameacc "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 cardgameacc "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 cardgameacc "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 cardgameacc "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 cardgameacc "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 cardgameacc "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 cardgameacc "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 cardgameacc "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 cardgameacc "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 cardgameacc "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 cardgameacc "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 cardgameacc "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 cardgameacc "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 cardgameacc "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 cardgameacc "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 cardgameacc "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 cardgameacc "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 cardgameacc "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 cardgameacc "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 cardgameacc "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 cardgameacc "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 cardgameacc "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 cardgameacc "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 cardgameacc "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 cardgameacc "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 cardgameacc "1 EOS" "gameuuid:0" -p player5 -f

echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
echo "====================schedusers===================="
$cleos push action cardgameacc schedusers '{"gameuuid": 0, "total": 25}' -p cardgameacc -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p cardgameacc -f
# echo "====================move==================="
# # $cleos push action cardgameacc move '{"user": "player1", "gameuuid": 0, "steps": 0x0001000100010001}' -p player1 -f
# echo "====================debug===================="
# $cleos push action cardgameacc debug '{}' -p cardgameacc -f
# echo "====================intransefer===================="
# $cleos push action cardgameacc intransfer '{"from": "cardgameacc", "to": "player1", "amount": 2}' -p cardgameacc -f

# $cleos push action cardgameacc sendtokens '{"gameuuid": 0, "user": "player1"}' -p cardgameacc -f
