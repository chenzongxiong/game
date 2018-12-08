#!/usr/bin/env bash

# change to script's directory
rm -rf data && mkdir -p data
script="./scripts/init_blockchain.sh"

echo "=== run docker container from the eosio/eos-dev image ==="
docker run --rm --name eos -d \
-p 8888:8888 -p 9876:9876 \
-v "$(pwd)"/config:/mnt/dev/config \
--mount type=bind,src="$(pwd)",dst=/opt/eosio/bin/contract/ \
--mount type=bind,src="$(pwd)"/scripts,dst=/opt/eosio/bin/scripts \
--mount type=bind,src="$(pwd)"/data,dst=/mnt/dev/data \
-w "/opt/eosio/bin/" eosio/eos-dev:v1.4.1 /bin/bash -c "$script"

if [ "$1" != "--nolog" ]
then
  echo "=== follow eosio_cardgame_container logs ==="
  docker logs eos --follow
fi
