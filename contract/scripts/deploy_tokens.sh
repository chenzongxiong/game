# cleos -u http://jungle2.cryptolions.io:80 set contract matrixtokens /opt/eosio/bin/contract/compiled_contracts/matr0x -p matrixtokens@active

cleos -u http://jungle2.cryptolions.io:80 push action matrixtokens create '["matrixtokens", "1000000000.000000 MYEOS", "create tokens"]' -p matrixtokens@active
cleos -u http://jungle2.cryptolions.io:80 push action matrixtokens issue '["matrixmaster", "520000000.000000 MYEOS", "to matrixmaster pool"]' -p matrixtokens@active
cleos -u http://jungle2.cryptolions.io:80 push action matrixtokens issue '["matrixcasino", "480000000.000000 MYEOS", "to matrixcasino"]' -p matrixtokens@active
