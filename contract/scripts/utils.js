'use strict';

const Eos = require('eosjs');
const configs = require('./config.json');

module.exports = {
  getConfig: () => {
    let eosnet  = process.env.EOS_NET || 'testnet';
    let config = configs[eosnet];
    return config;
  },

  getEosInstance: () => {
    let config = module.exports.getConfig();
    let eos = Eos({ keyProvider: config.defaultPrivateKey,
                    httpEndpoint: config.httpEndpoint,
                    chainId: config.chainId });
    return eos;
  },

  getOptions: () => {
    const options = {
      authorization: 'matrixcasino@active',
      sign: true
    };
    return options;
  }
}
