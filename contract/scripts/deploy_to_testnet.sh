cleos=/opt/eosio/bin/cleos

# $cleos -u http://jungle2.cryptolions.io:80 set account permission matrixcasino active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"matrixcasino","permission":"eosio.code"},"weight":1}]}' owner -p matrixcasino

## testnet interaction
# $cleos --url http://jungle2.cryptolions.io:80 system buyram arestest1234 arestest1234 --kbytes 5000
# $cleos --url http://jungle2.cryptolions.io:80 system delegatebw arestest1324 arestest1324 "5 EOS" "10 EOS"
# $cleos -u http://jungle2.cryptolions.io:80 set contract arestest1234 /opt/eosio/bin/contract/compiled_contracts/empty --permission arestest1234

$cleos -u http://jungle2.cryptolions.io:80 set contract matrixcasino /opt/eosio/bin/contract/compiled_contracts/dice --permission matrixcasino

$cleos -u http://jungle2.cryptolions.io:80 push action matrixcasino setrate '[120]' -p matrixcasino
$cleos -u http://jungle2.cryptolions.io:80 push action matrixcasino setairdrop '[1]' -p matrixcasino

# $cleos -u http://jungle2.cryptolions.io:80 set contract challenge123 /opt/eosio/bin/contract/compiled_contracts/challenge --permission challenge123

# $cleos -u http://jungle2.cryptolions.io:80 set account permission arestest1234 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"arestest1234","permission":"eosio.code"},"weight":1}]}' owner -p arestest1234
