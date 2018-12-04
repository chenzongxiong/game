#!/bin/bash

# PATH="$PATH:/opt/eosio/bin"

# /opt/eosio/bin/scripts/deploy_contract.sh cardgame cardgameacc cardgamewal $(cat /opt/eosio/bin/cardgame_wallet_password.txt)
# cleos='docker exec -it eos /opt/eosio/bin/cleos --url http://127.0.0.1:8888'
cleos=/opt/eosio/bin/cleos
$cleos wallet unlock -n cardgamewal --password $(cat /opt/eosio/bin/cardgame_wallet_password.txt)
$cleos set contract cardgameacc /opt/eosio/bin/contract/compiled_contracts/dice --permission cardgameacc
$cleos push action cardgameacc version '{}' -p cardgameacc

# $cleos -u http://jungle2.cryptolions.io:80 set contract arestest1234 /opt/eosio/bin/contract/compiled_contracts/dice --permission arestest1234
# $cleos --url http://jungle2.cryptolions.io:80 system buyram arestest1234 arestest1234 --kbytes 5000
