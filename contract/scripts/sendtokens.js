'use strict';

const Eos = require('eosjs');
const configs = require('./config.json');

let eosnet  = process.env.EOS_NET || 'testnet';

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


let winner_params = {
  json: true,
  code: config.contract,
  scope: config.scope,
  table: "winnertbl",
};

const queryTable = async function () {

  let results = await eos.getTableRows(winner_params);
  console.log("========================================");

  if (results.rows.length != 0) {
    let row = results.rows[0];
    eos.contract(contract).then(
      ctx => {
        ctx.sendtokens(row['user'], row['gameuuid'], options).then(trx => {
          console.log(trx.transaction_id);
        }).catch(e => {
          console.log("error", e);
          process.exit(1);
        });
      }
    );
  } else {
    console.log("no winner");
  }
};

setInterval(queryTable, config.sendtoken_interval);
