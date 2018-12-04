#!/usr/bin/env bash
set -o errexit

# set PATH
# PATH="$PATH:/opt/eosio/bin"

CONTRACTSPATH="$( pwd -P )"
cleos='docker exec -it eos /opt/eosio/bin/cleos --url http://127.0.0.1:8888'
# make new directory for compiled contract files
# mkdir -p ./compiled_contracts

mkdir -p ./compiled_contracts/dice

COMPILEDCONTRACTSPATH="$( pwd -P )/compiled_contracts/dice"
cd $COMPILEDCONTRACTSPATH


eosio-cpp -o dice.wasm $CONTRACTSPATH/dice.cpp --abigen
# # compile smart contract to wast and abi files
# (
#   eosiocpp -o "$COMPILEDCONTRACTSPATH/$1/$1.wast" "$CONTRACTSPATH/$1/$1.cpp" &&
#   eosiocpp -g "$COMPILEDCONTRACTSPATH/$1/$1.abi" "$CONTRACTSPATH/$1/$1.cpp"
# ) &&


# # set (deploy) compiled contract to blockchain
# unlock the wallet, ignore error if already unlocked
if [ ! -z $3 ]; then cleos wallet unlock -n $3 --password $4 || true; fi
# $cleos set contract $2 "$COMPILEDCONTRACTSPATH/$1/" --permission $2
