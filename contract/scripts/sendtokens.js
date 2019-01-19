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
let table = 'winnertbl';

const options = {
  authorization: 'matrixcasino@active',
  sign: true
};

let eos = Eos({ keyProvider: config.defaultPrivateKey,
                httpEndpoint: config.httpEndpoint,
                chainId: config.chainId });


const queryTable = async function () {

  let results = await eos.getTableRows(true, contract, scope, table);
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
