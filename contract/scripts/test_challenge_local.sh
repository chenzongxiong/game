cleos=/opt/eosio/bin/cleos

echo "====================ENROLL===================="
$cleos push action cardgameacc enroll '{"org": "player1", "game": "test1", "min_duration": 1, "max_duration": 60}' -p player1 -f
echo "====================GET ORG==================="
$cleos push action cardgameacc enroll '{"org": "player1", "game": "test1"}' -p player1 -f
# $cleos transfer player2 cardgameacc "1 EOS" "organization:player1;game:test1;bet:1;ts:0;referuser:player3" -p player2
# org, game, referuser, target, ts,
$cleos transfer player2 cardgameacc "1 EOS" "player1:test1::12:0:123" -p player2
$cleos push action cardgameacc '{"user": "player2", "org": "player1", "game": "test1", "target": 12, "achievement": 15, "duration": 30, "referuser": "", "ts": 0}' -p player2
