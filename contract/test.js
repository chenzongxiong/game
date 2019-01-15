'use strict';

const ecc = require('eosjs-ecc');

// ecc.randomKey().then(privateKey => {
//   console.log('Private Key:\t', privateKey) // wif
//   console.log('Public Key:\t', ecc.privateToPublic(privateKey)) // EOSkey...
//   let digest = '0d4ddcf51bbf59e4281bb943f5cd3684b572688a5e3c12c678be1e1cd088185a';
//   let sig = ecc.signHash(digest, privateKey);
//   console.log('sig: ', sig);
// });
let privateKey = '5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo';
let publicKey = ecc.privateToPublic(privateKey);
let digest = '0d4ddcf51bbf59e4281bb943f5cd3684b572688a5e3c12c678be1e1cd088185a';
let sig = ecc.signHash(digest, privateKey);
console.log('public key: ', publicKey);
console.log('sig: ', sig);
