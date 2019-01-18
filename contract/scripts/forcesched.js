'use strict';

const Eos = require('eosjs');

const defaultPrivateKey = "5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo";

let contract = 'matrixcasino';
let scope = 'matrixcasino';
let table = 'waittbl';

const options = {
  authorization: 'matrixcasino@active',
  sign: true
};

let eos = Eos({ keyProvider: defaultPrivateKey,
                httpEndpoint: 'http://jungle2.cryptolions.io:80',
                chainId: 'e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473' });

console.log("start to call action: forcesched");

let GAME_CLOSE = 0x01;
let GAME_START = 0x02;
let GAME_OVER  = 0x04;
let MAX_SCHED_USER_IN_POOL = 10;

let game_params = {
  json: true,
  code: contract,
  scope: scope,
  table: 'gametbl',
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
  // table_key:
};
let wait_params = {
  json: true,
  code: contract,
  scope: scope,
  // key_type: 'i64',
  // index_position: '2',
  table: 'waittbl',
};

const queryTable = async function () {
  try {
    let game_results = await eos.getTableRows(game_params);

    // console.log(game_results.rows);
    for (let row of game_results.rows) {
      if (row.status != GAME_OVER) {
        sched_params.lower_bound = row.uuid;
        sched_params.upper_bound = row.uuid;
        let sched_results = await eos.getTableRows(sched_params);

        console.log("========================================");
        console.log(sched_results.rows[0].gameuuid);
        if (sched_results.rows.length < MAX_SCHED_USER_IN_POOL) {
          // TODO: call force sched function offline

        }
      }
    }
    // game_results.rows.forEach(row => {



    //   console.log(row.uuid);
    // });
    // console.log(sched_results.rows);
    // let wait_results = await eos.getTableRows(wait_params);
    // console.log("========================================");
    // let curr_ts = Date.now();

    // if (wait_results.rows.length != 0){
    //   let row = wait_results.rows[0];
    //   console.log(row);
    //   if (row.expired_ts < curr_ts) {
    //     console.log("force sched");
    //     eos.contract(contract).then(
    //       ctx => {
    //         ctx.forcesched(options).then(trx => {
    //           console.log(trx.transaction_id);
    //         }).catch(e => {
    //           console.log("error", e);
    //           process.exit(1);
    //         });
    //       }
    //     );
    //   }
    // } else {
    //   console.log("no expired");
    // }
  } catch (error) {
    console.log("error");
    console.log(error);
    process.exit(1);
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
    }
  }, 3000);
}

wrapper();
