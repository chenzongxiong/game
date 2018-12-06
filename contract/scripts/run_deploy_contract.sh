#!/bin/bash

# PATH="$PATH:/opt/eosio/bin"

# /opt/eosio/bin/scripts/deploy_contract.sh cardgame cardgameacc cardgamewal $(cat /opt/eosio/bin/cardgame_wallet_password.txt)
# cleos='docker exec -it eos /opt/eosio/bin/cleos --url http://127.0.0.1:8888'
cleos=/opt/eosio/bin/cleos
$cleos wallet unlock -n cardgamewal --password $(cat /opt/eosio/bin/cardgame_wallet_password.txt)
$cleos set contract cardgameacc /opt/eosio/bin/contract/compiled_contracts/dice --permission cardgameacc
$cleos push action cardgameacc version '{}' -p cardgameacc

$cleos set account permission player1 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"cardgameacc","permission":"eosio.code"},"weight":1}]}' owner -p player1
$cleos set account permission player2 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"cardgameacc","permission":"eosio.code"},"weight":1}]}' owner -p player2
$cleos set account permission player3 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"cardgameacc","permission":"eosio.code"},"weight":1}]}' owner -p player3
$cleos set account permission player4 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"cardgameacc","permission":"eosio.code"},"weight":1}]}' owner -p player4
$cleos set account permission player5 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"cardgameacc","permission":"eosio.code"},"weight":1}]}' owner -p player5
$cleos set account permission cardgameacc active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"cardgameacc","permission":"eosio.code"},"weight":1}]}' owner -p cardgameacc

## testnet interaction
# $cleos --url http://jungle2.cryptolions.io:80 system buyram arestest1234 arestest1234 --kbytes 5000
# $cleos -u http://jungle2.cryptolions.io:80 set contract arestest1234 /opt/eosio/bin/contract/compiled_contracts/dice --permission arestest1234

# $cleos -u http://jungle2.cryptolions.io:80 set account permission arestest1234 active '{"threshold": 1,"keys": [{"key": "EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw","weight": 1}],"accounts": [{"permission":{"actor":"arestest1234","permission":"eosio.code"},"weight":1}]}' owner -p arestest1234
