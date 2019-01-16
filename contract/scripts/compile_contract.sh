#!/usr/bin/env bash
set -o errexit

# set PATH
# PATH="$PATH:/opt/eosio/bin"

CONTRACTSPATH="$( pwd -P )"

# cleos='docker exec -it eos /opt/eosio/bin/cleos --url http://127.0.0.1:8888'
# make new directory for compiled contract files

mkdir -p ./compiled_contracts/dice
mkdir -p ./compiled_contracts/empty
mkdir -p ./compiled_contracts/challenge
mkdir -p ./compiled_contracts/matr0x

COMPILEDCONTRACTSPATH="$( pwd -P )/compiled_contracts/dice"
cd $COMPILEDCONTRACTSPATH
eosio-cpp -o dice.wasm $CONTRACTSPATH/dice.cpp --abigen -I=/usr/local/include

# cd $CONTRACTSPATH
# COMPILEDCONTRACTSPATH="$( pwd -P )/compiled_contracts/empty"
# cd $COMPILEDCONTRACTSPATH
# eosio-cpp -o empty.wasm $CONTRACTSPATH/empty.cpp --abigen

# cd $CONTRACTSPATH
# COMPILEDCONTRACTSPATH="$( pwd -P )/compiled_contracts/challenge"
# cd $COMPILEDCONTRACTSPATH

# eosio-cpp -o challenge.wasm $CONTRACTSPATH/challenge.cpp --abigen -I=/usr/local/include

cd $CONTRACTSPATH
COMPILEDCONTRACTSPATH="$( pwd -P )/compiled_contracts/matr0x"
cd $COMPILEDCONTRACTSPATH
eosio-cpp -o matr0x.wasm $CONTRACTSPATH/matr0x/matr0x.cpp --abigen

# # unlock the wallet, ignore error if already unlocked
# if [ ! -z $3 ]; then cleos wallet unlock -n $3 --password $4 || true; fi
# # $cleos set contract $2 "$COMPILEDCONTRACTSPATH/$1/" --permission $2
