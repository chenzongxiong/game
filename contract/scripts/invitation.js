'use strict';

const express = require('express');
const utils = require('./utils.js');


let eos = utils.getEosInstance();
let config = utils.getConfig();
let options = utils.getOptions();

const app = express();
const port = config.invitation_port;

let invite_params = {
  json: true,
  code: config.contract,
  scope: config.scope,
  table: 'registration',
  limit: 1000,
};

const invite = async function(username, referuser) {
  let results = null;

  invite_params.key_type = 'name';
  invite_params.index_position = '1';
  invite_params.lower_bound = referuser;
  invite_params.upper_bound = referuser;

  results = await eos.getTableRows(invite_params);
  if (results.rows.length === 0) {
    console.log(results);
    throw new Error(`referuser ${referuser} doesn't exist`);
  }

  invite_params.lower_bound = username;
  invite_params.upper_bound = username;

  results = await eos.getTableRows(invite_params);

  if (results.rows.length !== 0) {
    console.log(results);
    throw new Error(`username ${username} already exists`);
  }

  eos.contract(config.contract).then(
    ctx => {
      ctx.invite(username, referuser, options).then(trx => {
        console.log(trx.transaction_id);
      }).catch(e => {
        console.log(e);
      });
    }
  )
};

/**
 * @swagger
 * /:
 *   get:
 *     summary: accept invitation link via GET method
 *     parameters:
 *       - in: "path"
 *         name: "ref"
 *         required: true
 *       - in: "path"
 *         name: "username"
 *         required: true
 *     responses:
 *       - 200:
 *           description: request successfully
 */
app.get('/newuser', async (req, res) => {

  let username = req.query.username;
  let referuser = req.query.ref;

  if (! referuser || ! username) {
    // res.json({msg: 'Invalid invitation params',
    //           status: 'success' });
    console.log('invalid invitation params');
    res.redirect('/');
    return;
  }
  try {
    // check referuser/username is valid EOS account
    await eos.getAccount(referuser);
    await eos.getAccount(username);

    await invite(username, referuser);
    console.log(`Insert ${username} into registration table.`);
    // res.json({ msg: `Insert ${username} into registration table.`,
    //            status: 'success'});
    res.redirect('/');
  } catch (error) {
    // res.json({ msg: new String(error),
    //            status: 'fail' });
    console.log(error);
    res.redirect('/');
  }
});

app.listen(port, (err) => {
  if (err) {
    process.exit(1);
  }
});
