'use strict';

const Eos = require('eosjs');
const configs = require('./config.json');

let eosnet = process.env.EOS_NET || 'testnet';

console.log("========================================");
console.log(eosnet);
console.log("========================================");

let config = configs[eosnet];

console.log("========================================");
console.log(config);
console.log("========================================");

const options = {
  authorization: 'matrixcasino@active',
  sign: true
};

let eos = Eos({ keyProvider: config.defaultPrivateKey,
                httpEndpoint: config.httpEndpoint,
                chainId: config.chainId });

console.log("start to call action: forcesched");

let GAME_CLOSE = 0x01;
let GAME_START = 0x02;
let GAME_OVER  = 0x04;
let MAX_SCHED_USER_IN_POOL = 10;

let game_params = {
  json: true,
  code: config.contract,
  scope: config.scope,
  table: 'gametbl',
};
let sched_params = {
  json: true,
  code: config.contract,
  scope: config.scope,
  key_type: 'i64',
  index_position: '2',
  table: 'schedtbl',
  lower_bound: -1,
  upper_bound: -1,
  limit: 1000,
};
let wait_params = {
  json: true,
  code: config.contract,
  scope: config.scope,
  key_type: 'i64',
  index_position: '2',
  table: 'waittbl',
  limit: 1000,
};

let max = config.random_max;
let min = config.random_min;

const queryTable = async function () {
  let game_results = await eos.getTableRows(game_params);

  for (let row of game_results.rows) {
    if (row.status == GAME_START) {
      sched_params.lower_bound = row.uuid;
      sched_params.upper_bound = row.uuid;
      let sched_results = await eos.getTableRows(sched_params);

      console.log("========================================");
      console.log("gameuuid: ", row.uuid, ", sched_result.length: ", sched_results.rows.length);
      console.log("========================================");

      if (sched_results.rows.length < MAX_SCHED_USER_IN_POOL) {
        wait_params.lower_bound = row.uuid;
        wait_params.upper_bound = row.uuid;
        let wait_results = await eos.getTableRows(wait_params);
        for (let i in wait_results.rows) {
          console.log("Item: ", i, ", gameuuid: ", wait_results.rows[i].gameuuid);
          if (wait_results.rows[i].sched_flag === 0) {
            console.log("force sched");
            let contract_instance = await eos.contract(config.contract);

            try {
              let seed = Math.floor(Math.random() * (max - min)) + min;
              let param = {
                gameuuid: row.uuid,
                seed: seed
              };
              let trx = await contract_instance.forcesched(param, options);
              console.log("transaction.id", trx.transaction_id);
            } catch (error) {
              console.log("error", error);
              process.exit(1);
            }
            break;
          }
        }
      }
    }
  }
};

function wrapper() {
  let intervalHandle = setInterval(() => {
    try {
      queryTable();
    } catch (error) {
      console.log("catch error: ");
      console.log(error);
      clearInterval(intervalHandle);
      process.exit(1);
    }
  }, config.sched_interval);
}

wrapper();
