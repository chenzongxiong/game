cleos=/opt/eosio/bin/cleos
## testnet interaction
# $cleos --url http://jungle2.cryptolions.io:80 system buyram arestest1234 arestest1234 --kbytes 5000
# $cleos -u http://jungle2.cryptolions.io:80 set contract arestest1234 /opt/eosio/bin/contract/compiled_contracts/empty --permission arestest1234

$cleos -u http://jungle2.cryptolions.io:80 set contract arestest1234 /opt/eosio/bin/contract/compiled_contracts/dice --permission arestest1234
# $cleos -u http://jungle2.cryptolions.io:80 set contract qazwsxedc123 /opt/eosio/bin/contract/compiled_contracts/dice --permission qazwsxedc123

# $cleos -u http://jungle2.cryptolions.io:80 set account permission arestest1234 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"arestest1234","permission":"eosio.code"},"weight":1}]}' owner -p arestest1234
