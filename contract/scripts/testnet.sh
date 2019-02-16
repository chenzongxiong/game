cleos=/opt/eosio/bin/cleos

echo "====================CLEAR===================="
$cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino clear '{}' -p matrixcasino -f
echo "====================VERSION===================="
$cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino version '{}' -p matrixcasino -f
# echo "====================add game===================="
$cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino addgame '{"width": 23, "height": 23, "status": 2, "fee": 10, "gamename": "test1"}' -p matrixcasino -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino addgame '{"width": 23, "height": 23, "status": 2, "fee": 1, "gamename": "test2"}' -p matrixcasino -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino addgame '{"width": 23, "height": 23, "status": 2, "fee": 1, "gamename": "test3"}' -p matrixcasino -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino addgame '{"width": 23, "height": 23, "status": 2, "fee": 1, "gamename": "test4"}' -p matrixcasino -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino startgame '{"gameuuid": 0}' -p matrixcasino -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino startgame '{"gameuuid": 1}' -p matrixcasino -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino startgame '{"gameuuid": 2}' -p matrixcasino -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino startgame '{"gameuuid": 3}' -p matrixcasino -f
# echo "====================debug===================="
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino debug '{}' -p matrixcasino -f
echo "====================enter===================="
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.0001 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.0001 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.0001 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.0001 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.0001 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.0001 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.0001 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.001 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 matrixcasino "0.01 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 matrixcasino "0.01 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 matrixcasino "0.01 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 matrixcasino "0.01 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.01 EOS" "gameuuid:0" -p qazwsxedc123 -f


$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 matrixcasino "0.1 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 matrixcasino "0.1 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 matrixcasino "0.1 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 matrixcasino "0.1 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.1 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.001 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 matrixcasino "0.001 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.001 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 matrixcasino "0.001 EOS" "gameuuid:1" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 matrixcasino "0.001 EOS" "gameuuid:1" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 matrixcasino "0.001 EOS" "gameuuid:1" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 matrixcasino "0.001 EOS" "gameuuid:1" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.001 EOS" "gameuuid:1" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 matrixcasino "0.01 EOS" "gameuuid:2" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 matrixcasino "0.01 EOS" "gameuuid:2" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 matrixcasino "0.01 EOS" "gameuuid:2" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 matrixcasino "0.01 EOS" "gameuuid:2" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 matrixcasino "0.01 EOS" "gameuuid:2" -p qazwsxedc123 -f

# echo "====================schedusers===================="
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino schedusers '{"gameuuid": 0, "total": 10}' -p matrixcasino -f
# echo "====================debug===================="
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino debug '{}' -p matrixcasino -f
# # echo "====================toss===================="
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino toss '{"user": "arestest4321", "gameuuid": 0, "seed": 3}' -p arestest4321 -f
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino toss '{"user": "arestest4321", "gameuuid": 0, "seed": 3}' -p arestest4321 -f

# echo "====================move==================="=
# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino move '{"user": "arestest4321", "gameuuid": 0, "steps": 2}' -p matrixcasino -f
# echo "====================debug===================="

# $cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino sendtokens '{"user": "arestest4321", "gameuuid": 0}' -p matrixcasino -f

cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino addgame '{"width": 23, "height": 23, "status": 2, "fee": 10, "gamename": "free1"}' -p matrixcasino -f

cleos --url http://jungle2.cryptolions.io:80 push action matrixcasino setawards '{"gameuuid": 7, "awards": "5000", "sawards": "200000"}' -p matrixcasino -f
