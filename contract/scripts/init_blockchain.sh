#!/usr/bin/env bash
set -o errexit

echo "=== setup blockchain accounts and smart contract ==="

# set PATH
PATH="$PATH:/opt/eosio/bin:/opt/eosio/bin/scripts"

set -m

# start nodeos ( local node of blockchain )
# run it in a background job such that docker run could continue
nodeos -e -p eosio -d /mnt/dev/data \
  --config-dir /mnt/dev/config \
  --http-validate-host=false \
  --plugin eosio::producer_plugin \
  --plugin eosio::history_plugin \
  --plugin eosio::chain_api_plugin \
  --plugin eosio::history_api_plugin \
  --plugin eosio::http_plugin \
  --plugin eosio::chain_plugin \
  --http-server-address=0.0.0.0:8888 \
  --access-control-allow-origin=* \
  --contracts-console \
  --verbose-http-errors &
sleep 1s
  until curl localhost:8888/v1/chain/get_info
do
  sleep 1s
done

# Sleep for 2 to allow time 4 blocks to be created so we have blocks to reference when sending transactions
sleep 2s
echo "=== setup wallet: eosiomain ==="
# First key import is for eosio system account
cleos wallet create -n eosiomain --to-console | tail -1 | sed -e 's/^"//' -e 's/"$//' > eosiomain_wallet_password.txt
cleos wallet import -n eosiomain --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

echo "=== setup wallet: cardgamewal ==="
# key for eosio account and export the generated password to a file for unlocking wallet later
cleos wallet create -n cardgamewal --to-console | tail -1 | sed -e 's/^"//' -e 's/"$//' > cardgame_wallet_password.txt
# Owner key for cardgamewal wallet
cleos wallet import -n cardgamewal --private-key 5JD9AGTuTeD5BXZwGQ5AtwBqHK21aHmYnTetHgk1B3pjj7krT8N
# Active key for cardgamewal wallet
cleos wallet import -n cardgamewal --private-key 5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo

# create account for cardgameacc with above wallet's public keys
cleos create account eosio cardgameacc EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
# cleos create account eosio oraclizeconn EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account cardgameacc oraclizeconn EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio eosio.token EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio player1 EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio player2 EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio player3 EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio player4 EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio player5 EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio platform EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw
cleos create account eosio divident EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw

# create account for MYEOS token
cleos create account eosio myeos EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw

# echo "=== deploy smart contract ==="
# $1 smart contract name
# $2 account holder name of the smart contract
# $3 wallet for unlock the account
# $4 password for unlocking the wallet
# deploy_contract.sh cardgame cardgameacc cardgamewal $(cat cardgame_wallet_password.txt)
cleos set contract eosio.token /opt/eosio/contracts/eosio.token --abi eosio.token.abi -p eosio.token@active
cleos push action eosio.token create '[ "cardgameacc", "1000000000.0000 EOS"]' -p eosio.token@active
cleos push action eosio.token issue '[ "cardgameacc", "100000000.0000 EOS", "cardgameacc" ]' -p cardgameacc@active
cleos push action eosio.token issue '[ "player1", "100000000.0000 EOS", "player1" ]' -p cardgameacc@active
cleos push action eosio.token issue '[ "player2", "100000000.0000 EOS", "player2" ]' -p cardgameacc@active
cleos push action eosio.token issue '[ "player3", "100000000.0000 EOS", "player3" ]' -p cardgameacc@active
cleos push action eosio.token issue '[ "player4", "100000000.0000 EOS", "player4" ]' -p cardgameacc@active
cleos push action eosio.token issue '[ "player5", "100000000.0000 EOS", "player5" ]' -p cardgameacc@active

cleos set contract myeos /opt/eosio/bin/contract/compiled_contracts/matr0x -p myeos@active
cleos push action myeos create '["myeos", "1 MYEOS", ""]' -p myeos@active
cleos push action myeos issue '["divident", "520000000.000000 MYEOS", "to divident pool"]' -p myeos@active
cleos push action myeos issue '["platform", "480000000.000000 MYEOS", "to platform"]' -p myeos@active

echo "=== create user accounts ==="
# script for create data into blockchain
create_accounts.sh

echo "=== end of setup blockchain accounts and smart contract ==="
# create a file to indicate the blockchain has been initialized
touch "/mnt/dev/data/initialized"

# put the background nodeos job to foreground for docker run
fg %1
