cleos=/opt/eosio/bin/cleos

admin=player1
$cleos push action cardgameacc clear '{}' -p $admin -f
echo "====================VERSION===================="
$cleos push action cardgameacc version '{}' -p $admin -f
echo "====================add game===================="
$cleos push action cardgameacc addgame '{"width": 23, "height": 23, "status": 1, "fee": 10000, "gamename": "test"}' -p $admin -f
$cleos push action cardgameacc startgame '{"gameuuid": 0}' -p $admin -f
echo "====================debug===================="
$cleos push action cardgameacc debug '{}' -p $admin -f
echo "====================enter===================="
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
$cleos push action cardgameacc debug '{}' -p $admin -f
# echo "====================schedusers===================="
# $cleos push action cardgameacc schedusers '{"gameuuid": 0, "total": 25}' -p cardgameacc -f
# echo "====================debug===================="
# $cleos push action cardgameacc debug '{}' -p $admin -f
# echo "====================move==================="
# # $cleos push action cardgameacc move '{"user": "player1", "gameuuid": 0, "steps": 0x0001000100010001}' -p player1 -f
# echo "====================debug===================="
# $cleos push action cardgameacc debug '{}' -p cardgameacc -f
# echo "====================intransefer===================="
# $cleos push action cardgameacc intransfer '{"from": "cardgameacc", "to": "player1", "amount": 2}' -p cardgameacc -f

# $cleos push action cardgameacc sendtokens '{"gameuuid": 0, "user": "player1"}' -p cardgameacc -f


# $cleos push action cardgameacc getbriefmaps '{"user": "player1", "gameuuid": 0xfffff, "status": 0xfffff}' -p player1
# $cleos push action cardgameacc getmylineno '{"user": "player1", "gameuuid": 0}' -p player1
# $cleos push action cardgameacc getmapdetail '{"user": "player1", "gameuuid": 0, "wait_limit": 20, "sched_limit": 20, "hero_limit": 20}' -p player1
