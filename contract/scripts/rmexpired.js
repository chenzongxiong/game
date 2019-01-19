'use strict';

const Eos = require('eosjs');
const config = require('config.json');

let eosnet  = process.env.EOS_NET || 'testnet';

if (eosnet === 'testnet') {
  config = config.testnet;
} else if (eosnet === 'mainet') {
  config = config.mainet;
}
console.log("========================================");
console.log(config);
console.log("========================================");

let contract = config.contract;
let scope = config.scope;
let table = 'schedtbl';

const options = {
  authorization: 'matrixcasino@active',
  sign: true
};

let eos = Eos({ keyProvider: config.defaultPrivateKey,
                httpEndpoint: config.httpEndpoint,
                chainId: config.chainId });

console.log("start to call action: rmexpired");

let GAME_CLOSE = 0x01;
let GAME_START = 0x02;
let GAME_OVER  = 0x04;
let MAX_SCHED_USER_IN_POOL = 10;


let game_params = {
  json: true,
  code: contract,
  scope: scope,
  table: "gametbl",
};

let sched_params = {
  json: true,
  code: contract,
  scope: scope,
  key_type: 'i64',
  index_position: '2',
  table: 'schedtbl',
  lower_bound: -1,
  upper_bound: -1,
  limit: 1000,
};

const queryTable = async function () {
  let curr_ts = Date.now()/1000;
  let game_results = await eos.getTableRows(game_params);

  for (let row of game_results.rows) {
    if (row.status != GAME_START) {
      continue;
    }
    sched_params.lower_bound = row.uuid;
    sched_params.upper_bound = row.uuid;
    let sched_results = await eos.getTableRows(sched_params);
    console.log("========================================");
    console.log("gameuuid: ", row.uuid, ", sched_result.length: ", sched_results.rows.length);
    console.log("========================================");
    if (sched_results.rows.length != 0) {
      let row = sched_results.rows[0];
      if (row.expired_ts < curr_ts) {
        console.log("********************remove expired********************");
        eos.contract(contract).then(
          ctx => {
            ctx.rmexpired(options).then(trx => {
              console.log(trx.transaction_id);
            }).catch(e => {
              console.log("error", e);
              process.exit(1);
            });
          }
        );
      } else {
        console.log("no expired");
      }
    }
  }
};

function wrapper() {
  let intervalHandle = setInterval(() => {
    try {
      queryTable();
    } catch (error) {
      console.log("catch error:");
      console.log(error);
      clearInterval(intervalHandle);
    }
  }, config.rmexpired_interval);
}

wrapper();
