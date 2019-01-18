cleos=/opt/eosio/bin/cleos

admin=player1
$cleos push action matrixcasino clear '{}' -p $admin -f
echo "====================VERSION===================="
$cleos push action matrixcasino version '{}' -p $admin -f
echo "====================add game===================="
$cleos push action matrixcasino addgame '{"width": 23, "height": 23, "status": 1, "fee": 10000, "gamename": "test"}' -p $admin -f
$cleos push action matrixcasino startgame '{"gameuuid": 0}' -p $admin -f
echo "====================debug===================="
$cleos push action matrixcasino debug '{}' -p $admin -f
echo "====================enter===================="
$cleos transfer player1 matrixcasino "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 matrixcasino "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 matrixcasino "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 matrixcasino "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 matrixcasino "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 matrixcasino "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 matrixcasino "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 matrixcasino "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 matrixcasino "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 matrixcasino "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 matrixcasino "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 matrixcasino "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 matrixcasino "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 matrixcasino "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 matrixcasino "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 matrixcasino "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 matrixcasino "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 matrixcasino "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 matrixcasino "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 matrixcasino "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 matrixcasino "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 matrixcasino "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 matrixcasino "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 matrixcasino "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 matrixcasino "1 EOS" "gameuuid:0" -p player5 -f

$cleos transfer player1 matrixcasino "1 EOS" "gameuuid:0" -p player1 -f
$cleos transfer player2 matrixcasino "1 EOS" "gameuuid:0" -p player2 -f
$cleos transfer player3 matrixcasino "1 EOS" "gameuuid:0" -p player3 -f
$cleos transfer player4 matrixcasino "1 EOS" "gameuuid:0" -p player4 -f
$cleos transfer player5 matrixcasino "1 EOS" "gameuuid:0" -p player5 -f

echo "====================debug===================="
$cleos push action matrixcasino debug '{}' -p $admin -f
# echo "====================schedusers===================="
# $cleos push action matrixcasino schedusers '{"gameuuid": 0, "total": 25}' -p matrixcasino -f
# echo "====================debug===================="
# $cleos push action matrixcasino debug '{}' -p $admin -f
# echo "====================move==================="
# # $cleos push action matrixcasino move '{"user": "player1", "gameuuid": 0, "steps": 0x0001000100010001}' -p player1 -f
# echo "====================debug===================="
# $cleos push action matrixcasino debug '{}' -p matrixcasino -f
# echo "====================intransefer===================="
# $cleos push action matrixcasino intransfer '{"from": "matrixcasino", "to": "player1", "amount": 2}' -p matrixcasino -f

# $cleos push action matrixcasino sendtokens '{"gameuuid": 0, "user": "player1"}' -p matrixcasino -f


# $cleos push action matrixcasino getbriefmaps '{"user": "player1", "gameuuid": 0xfffff, "status": 0xfffff}' -p player1
# $cleos push action matrixcasino getmylineno '{"user": "player1", "gameuuid": 0}' -p player1
# $cleos push action matrixcasino getmapdetail '{"user": "player1", "gameuuid": 0, "wait_limit": 20, "sched_limit": 20, "hero_limit": 20}' -p player1
