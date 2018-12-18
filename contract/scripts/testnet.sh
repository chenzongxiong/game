cleos=/opt/eosio/bin/cleos

echo "====================CLEAR===================="
# $cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 clear '{}' -p arestest1234 -f
echo "====================VERSION===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 version '{}' -p arestest1234 -f
echo "====================add game===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 addgame '{"width": 23, "height": 23, "status": 2, "fee": 10000, "gamename": "test1"}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 addgame '{"width": 23, "height": 23, "status": 2, "fee": 10000, "gamename": "test2"}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 addgame '{"width": 23, "height": 23, "status": 2, "fee": 10000, "gamename": "test3"}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 addgame '{"width": 23, "height": 23, "status": 2, "fee": 10000, "gamename": "test4"}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 startgame '{"gameuuid": 0}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 startgame '{"gameuuid": 1}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 startgame '{"gameuuid": 2}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 startgame '{"gameuuid": 3}' -p arestest1234 -f
echo "====================debug===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 debug '{}' -p arestest1234 -f
echo "====================enter===================="
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 arestest1234 "1 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 arestest1234 "1 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 arestest1234 "1 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 arestest1234 "1 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 arestest1234 "1 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 arestest1234 "1 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 arestest1234 "1 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 arestest1234 "1 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f


$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 arestest1234 "1 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 arestest1234 "1 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 arestest1234 "1 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 arestest1234 "1 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 arestest1234 "1 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 arestest1234 "1 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 arestest1234 "1 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 arestest1234 "1 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 arestest1234 "1 EOS" "gameuuid:0" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 arestest1234 "1 EOS" "gameuuid:0" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 arestest1234 "1 EOS" "gameuuid:0" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 arestest1234 "1 EOS" "gameuuid:0" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:0" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 arestest1234 "1 EOS" "gameuuid:1" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 arestest1234 "1 EOS" "gameuuid:1" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 arestest1234 "1 EOS" "gameuuid:1" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 arestest1234 "1 EOS" "gameuuid:1" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:1" -p qazwsxedc123 -f

$cleos --url http://jungle2.cryptolions.io:80 transfer arestest4321 arestest1234 "1 EOS" "gameuuid:2" -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2134 arestest1234 "1 EOS" "gameuuid:2" -p arestest2134 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest2143 arestest1234 "1 EOS" "gameuuid:2" -p arestest2143 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer arestest1324 arestest1234 "1 EOS" "gameuuid:2" -p arestest1324 -f
$cleos --url http://jungle2.cryptolions.io:80 transfer qazwsxedc123 arestest1234 "1 EOS" "gameuuid:2" -p qazwsxedc123 -f

echo "====================schedusers===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 schedusers '{"gameuuid": 0, "total": 10}' -p arestest1234 -f
echo "====================debug===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 debug '{}' -p arestest1234 -f
# echo "====================toss===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 toss '{"user": "arestest4321", "gameuuid": 0, "seed": 3}' -p arestest4321 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 toss '{"user": "arestest4321", "gameuuid": 0, "seed": 3}' -p arestest4321 -f

# echo "====================move==================="=
# $cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 move '{"user": "arestest4321", "gameuuid": 0, "steps": 2}' -p arestest1234 -f
# echo "====================debug===================="

# $cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 sendtokens '{"user": "arestest4321", "gameuuid": 0}' -p arestest1234 -f
