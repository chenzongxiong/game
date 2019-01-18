'use strict';

const Eos = require('eosjs');

const defaultPrivateKey = "5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo";

let contract = 'matrixcasino';
let scope = 'matrixcasino';
let table = 'schedtbl';

const options = {
  authorization: 'matrixcasino@active',
  sign: true
};

let eos = Eos({ keyProvider: defaultPrivateKey,
                httpEndpoint: 'http://jungle2.cryptolions.io:80',
                chainId: 'e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473' });

console.log("start to call action: rmexpired");

const queryTable = async function () {
  let results = await eos.getTableRows(true, contract, scope, table);
  console.log("========================================");
  let curr_ts = Date.now();

  if (results.rows.length != 0){
    let row = results.rows[0];
    console.log(row);
    if (row.expired_ts < curr_ts) {
      console.log("remove expired");
      eos.contract(contract).then(
        ctx => {
          ctx.rmexpired(options).then(trx => {
            console.log(trx.transaction_id);
          }).catch(e => {
            console.log("error", e);
          });
        }
      );
    }
  } else {
    console.log("no expired");
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
  }, 3000);
}

wrapper();
