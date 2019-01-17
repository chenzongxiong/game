'use strict';

const Eos = require('eosjs');

const defaultPrivateKey = "5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo";

let contract = 'matrixcasino';
let scope = 'matrixcasino';
let table = 'winnertbl';

const options = {
  authorization: 'matrixcasino@active',
  sign: true
};


const queryTable = async function () {
  let eos = Eos({ keyProvider: defaultPrivateKey,
                  httpEndpoint: 'http://jungle2.cryptolions.io:80',
                  chainId: 'e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473' });

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
        });
      }
    );
  } else {
    console.log("no winner");
  }
};

setInterval(queryTable, 3000);
