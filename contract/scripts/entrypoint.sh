#!/bin/bash

cleos wallet open --name eosiomain

if [ $? -eq 1 ]
then
    echo "create a new wallet"
    cleos wallet create -n eosiomain --to-console | tail -1 | sed -e 's/^"//' -e 's/"$//' > eosiomain_wallet_password.txt
    cleos wallet import -n eosiomain --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
    cleos wallet import -n eosiomain --private-key 5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo
fi

if [ $# -eq 0 ]
then
    echo "Run tail -f"
    touch /test.log
    tail -f /test.log
else
    exec $@
fi
