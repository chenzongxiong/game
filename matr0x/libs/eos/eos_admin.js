//添加游戏地图
function addGame(){
    const scatter = ScatterJS.scatter;
    scatter.getIdentity(simple_required_fields).then(() => {
        const account = ScatterJS.scatter.identity.accounts.find(account => account.blockchain === 'eos')
        
        const eos = scatter.eos(eos_network, Eos, eosOptions);
        const transactionOptions = {
            authorization: [
                `${account.name}@${account.authority}`,
            ]
        };
        var gameName = "game" + Math.floor(Math.random()*100);
        
        eos.contract(eos_contract_name, {simple_required_fields}).then(
            contract => {
                contract.addgame(gameName, 25, 25, 1, 100, transactionOptions).then(trx => {
                    console.log("addGame 成功", trx);
                    //是否成功,要查询交易状况来决策
                }).catch(e => {
                    console.log("addGame error", e);
                });
            }
        ); 
    }).catch(err =>{
        console.error(err);
    });
}
//清除游戏内容
function clear(){
    const scatter = ScatterJS.scatter;
    scatter.getIdentity(simple_required_fields).then(() => {
        const account = ScatterJS.scatter.identity.accounts.find(account => account.blockchain === 'eos')
        
        const eos = scatter.eos(eos_network, Eos, eosOptions);
        const transactionOptions = {
            authorization: [
                `${account.name}@${account.authority}`,
            ]
        };
        eos.contract(eos_contract_name, {simple_required_fields}).then(
            contract => {
                contract.clear(transactionOptions).then(trx => {
                    console.log("clear 成功", trx);
                    //是否成功,要查询交易状况来决策
                }).catch(e => {
                    console.log("clear error", e);
                });
            }
        ); 
        
    }).catch(err =>{
        console.error(err);
    });
}

//开始游戏
function startGame(gameid) {
    const scatter = ScatterJS.scatter;
    scatter.getIdentity(simple_required_fields).then(() => {
        const account = ScatterJS.scatter.identity.accounts.find(account => account.blockchain === 'eos')
        
        const eos = scatter.eos(eos_network, Eos, eosOptions);
        const transactionOptions = {
            authorization: [
                `${account.name}@${account.authority}`,
            ]
        };
        eos.contract(eos_contract_name, {simple_required_fields}).then(
            contract => {
                contract.startgame(gameid,transactionOptions).then(trx => {
                    console.log("startGame 成功", trx);
                    //是否成功,要查询交易状况来决策
                }).catch(e => {
                    console.log("startGame error", e);
                });
            }
        ); 
        
    }).catch(err =>{
        console.error(err);
    });
}
//停止游戏
function closeGame(gameid) {
    const scatter = ScatterJS.scatter;
    scatter.getIdentity(simple_required_fields).then(() => {
        const account = ScatterJS.scatter.identity.accounts.find(account => account.blockchain === 'eos')
        
        const eos = scatter.eos(eos_network, Eos, eosOptions);
        const transactionOptions = {
            authorization: [
                `${account.name}@${account.authority}`,
            ]
        };
        eos.contract(eos_contract_name, {simple_required_fields}).then(
            contract => {
                contract.closegame(gameid,transactionOptions).then(trx => {
                    console.log("closeGame 成功", trx);
                    //是否成功,要查询交易状况来决策
                }).catch(e => {
                    console.log("closeGame error", e);
                });
            }
        ); 
        
    }).catch(err =>{
        console.error(err);
    });
}

//产生调度队列
function schedUser(gameid) {
    const scatter = ScatterJS.scatter;
    scatter.getIdentity(simple_required_fields).then(() => {
        const account = ScatterJS.scatter.identity.accounts.find(account => account.blockchain === 'eos')
        
        const eos = scatter.eos(eos_network, Eos, eosOptions);
        const transactionOptions = {
            authorization: [
                `${account.name}@${account.authority}`,
            ]
        };
        eos.contract(eos_contract_name, {simple_required_fields}).then(
            contract => {
                contract.schedusers(gameid,1, transactionOptions).then(trx => {
                    console.log("schedUser 成功", trx);
                }).catch(e => {
                    console.log("schedUser error", e);
                });
            }
        ); 
        
    }).catch(err =>{
        console.error(err);
    });
}
