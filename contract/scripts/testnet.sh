cleos=/opt/eosio/bin/cleos

echo "====================VERSION===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 version '{}' -p arestest1234 -f
echo "====================add game===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 addgame '{}' -p arestest1234 -f
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 startgame '{"gameuuid": 0}' -p arestest1234 -f
echo "====================debug===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 debug '{}' -p arestest1234 -f
echo "====================enter===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 enter '{"user": "arestest4321", "gameuuid": 0}' -p arestest4321 -f
echo "====================schedusersr===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 schedusers '{"gameuuid": 0, "total": 2}' -p arestest1234 -f
echo "====================debug===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 debug '{}' -p arestest1234 -f
echo "====================enter==================="=
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 enter '{"user": "arestest4321", "gameuuid": 0}' -p arestest4321 -f
echo "====================toss===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 toss '{"user": "arestest4321", "gameuuid": 3}' -p arestest4321 -f
echo "====================move right==================="=
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 moveright '{"user": "player1", "gameuuid": 0, "steps": 2}' -p arestest1234 -f
echo "====================debug===================="
$cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 debug '{}' -p arestest1234 -f

# $cleos --url http://jungle2.cryptolions.io:80 push action arestest1234 version '{}' -p arestest1234
