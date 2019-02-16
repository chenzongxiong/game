var list_game = [];//地图列表
var list_shecd = [];//排队列表
var list_join_game = [];//已加入的地图列表 id
var account_name = "";

function  getListGame(arr) {
    //console.log(arr);
    var list=[];
    for (let i=0;i<arr.length;i++){
        list.push(arr[i]);
        list[i].pos=parserPosition(list[i].pos);
        var goals=[];
        for (let l=0;l<arr[i].goals.length;l++){
            let point=parserPosition(arr[i].goals[l]);
            if(point.row<list[i].board_height&&point.col<list[i].board_width){
                goals.push(point);
            }
        }
        list[i].goals=goals;
    }
    return list;
}


var userGameTime = 120;//玩家游戏时间,单位秒 
/**
 * 解析坐标
 * 一个position 用32bit 的整型 ，高 16 bit 表示 row， 低16 bit 表示 col
 * 适用于人物和电话的位置坐标解析
 * @param {Object} pos{row, column}
 */
function parserPosition(pos){
    var point = {row:0, col:0};
    point.row =  (pos >> 16) & 0x0000ffff;
    point.col =  pos & 0x0000ffff;
    return point;
}
/**
 * 移动4向位置解析
 * 
 * pos 结构 4个字节组成
 * ------------------------------
 * 0x0000  0x0000  0x0000  0x0000
 * right    left     up     down
 * ------------------------------
 * 
 * @param {Object} pos
 */
function parserMovePos(pos) {
    var point = {right:0, left:0, up:0, down:0};
    point.right = bigInt(pos).shiftRight(48).and(0xffff).valueOf();
    point.left = bigInt(pos).shiftRight(32).and(0xffff).valueOf();
    point.up = bigInt(pos).shiftRight(16).and(0xffff).valueOf();
    point.down = bigInt(pos).and(0xffff).valueOf();
    return point;
}
/**
 * 移动合成位置坐标数
 * 
 * pos 结构 4个字节组成
 * ------------------------------
 * 0x0000  0x0000  0x0000  0x0000
 * right    left     up     down
 * ------------------------------
 * 
 * @param {Object} left
 * @param {Object} right
 * @param {Object} up
 * @param {Object} dow
 */
function mergeMovePos(right, left, up, dow) {
    var pos = bigInt();
    pos = bigInt(right).shiftLeft(48);
    pos = pos.or(bigInt(left).shiftLeft(32));
    pos = pos.or(bigInt(up).shiftLeft(16));
    pos = pos.or(bigInt(dow));
    return pos.valueOf();
}


//---------------游戏交互方法定义-----------------
const eos_network = {
    protocol: "https",
    blockchain: 'eos',
    host: "jungle2.cryptolions.io",
    port: 443,
    chainId: "e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473"
}
const simple_required_fields = {accounts:[eos_network]};
const eos_contract_name = "matrixcasino";
const eosOptions = { expireInSeconds:60 };

ScatterJS.plugins( new ScatterEOS() );//设置使用的协议

var _isInitEosIdentity = false;//是否做了授权认证

//初始化Eos,并进行授权登录
function getUserInfo() {
    // First we need to connect to the user's Scatter.
    ScatterJS.scatter.connect(eos_contract_name).then(connected => {
        // If the user does not have Scatter or it is Locked or Closed this will return false;
        if(!connected) return false;
        const scatter = ScatterJS.scatter;
        //window.ScatterJS = null;
    
        // Now we need to get an identity from the user.
        // We're also going to require an account that is connected to the network we're using.
        const requiredFields = {
            //personal:['firstname', 'lastname'],
            //location:['phone', 'address', 'city', 'state', 'country', 'zipcode'],
            accounts:[eos_network] 
        };
        scatter.getIdentity(requiredFields).then(() => {
            // Always use the accounts you got back from Scatter. Never hardcode them even if you are prompting
            // the user for their account name beforehand. They could still give you a different account.
            const account = scatter.identity.accounts.find(account => account.blockchain === 'eos');
            account_name = account.name;
            _isInitEosIdentity = true;
            
            //console.log(account);
            easy.MyEvent.sendEvent("eos_user", {data:account}, true);
			easy.MessageTips.showMessage(easy.LocalizationManager.getText("LoginSuccess"));
        }).catch(err => {
            if (err.type){
                if (err.type == "locked") {
                    console.log("eos_account_locked");
                    easy.MyEvent.sendEvent("eos_account_locked", null, true);//账号没解锁
                } else if (err.type == "identity_rejected"){//账号没授权
                    console.log("eos_no_authorization");
                    easy.MyEvent.sendEvent("eos_no_authorization", null, true);
                }
            }
            console.error(err);
        });
    }).catch(err =>{
        console.log("eos_no_scatter");
        easy.MyEvent.sendEvent("eos_no_scatter", null, true);
        console.error(err);
    });
}
/**
 * 加入游戏
 * @param {Object} uname
 * @param {Object} gameid
 */
function joinGame(gameid, amount) {
    const scatter = ScatterJS.scatter;
    scatter.getIdentity(simple_required_fields).then(() => {
        const account = ScatterJS.scatter.identity.accounts.find(account => account.blockchain === 'eos')
        
        const eos = scatter.eos(eos_network, Eos, eosOptions);
        const transactionOptions = {
            authorization: [
                `${account.name}@${account.authority}`,
            ]
        };
        
        var memo = "gameuuid:" + gameid;     /* memo must be set in such format: "key:val" */
        // key is `gameuuid` and value is the gameuuid a user wants to enter
        eos.contract("eosio.token", {simple_required_fields}).then(
            contract => {
                // transfer(from_user, to_user, quantity);
                // `from_user` transfers `quantity` TOKENS to `to_user`
                // `from_user` and `to_user` must be different, or it throws exceptions
                contract.transfer(account.name, eos_contract_name, amount + " EOS", memo, transactionOptions).then(trx => {
                    //console.log("缴费完成 transfer tokens to our platform", trx);
                    //是否成功,要查询交易状况来决策
                    easy.MyEvent.sendEvent("eos_joingame", {"data":true}, true);
					easy.MessageTips.showMessage(easy.LocalizationManager.getText("JoinGameSuccess"));
                }).catch(e => {
                    console.log("缴费失败 error", e);
                    easy.MyEvent.sendEvent("eos_joingame", {"data":false}, true);
					easy.MessageTips.showMessage(easy.LocalizationManager.getText("JoinGameFaild"));
                    if (e.toString().includes("overdrawn balance")) {
                        alert("No money, go back to Getting Started and refill")
                    }
                });
            }
        ); 
        
    }).catch(err =>{
        easy.MyEvent.sendEvent("eos_joingame", {"data":false}, true);
        console.error(err);
    });
}
/**
 * 投骰子
 * @param {Object} uname 用户名
 * @param {Object} gameid 游戏的id
 * @param {Object} lucknum 用户输入的幸运数字
 */
function toss(gameid, lucknum){
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
                contract.toss(account.name, gameid, lucknum, transactionOptions).then(trx => {
                    //console.log("请求抛骰子 成功", trx);
                    //是否成功,要查询交易状况来决策
                    var res = "{dice_number: 0, user: '', gameuuid: 0, toss: false}";
                    if (trx.processed && trx.processed.action_traces && trx.processed.action_traces.length > 0 && trx.processed.action_traces[0].act && trx.processed.action_traces[0].console) {
                        res = trx.processed.action_traces[0].console;
                    }
                    //console.log("请求抛骰子 res", res);
                    var resObj = JSON.parse(res);
                    if (resObj.toss === true) {
						easy.MessageTips.showMessage(easy.LocalizationManager.getText("TossSuccess"));
                        //console.log("请求抛骰子 success res.num", resObj.dice_number);
                        easy.MyEvent.sendEvent("eos_toss", {"data":resObj.dice_number}, true);//成功
                    } else {
						easy.MessageTips.showMessage(easy.LocalizationManager.getText("TossFaild"));
                        //console.log("请求抛骰子 faild res.num", resObj.dice_number);
                        easy.MyEvent.sendEvent("eos_toss", {"data":0}, true);//失败
                    }
//                  var num = Math.ceil(Math.random()*6);
//                  easy.MyEvent.sendEvent("eos_toss", {"data":num}, true);//失败
                }).catch(e => {
                    console.log("请求抛骰子失败 error", e);
					easy.MessageTips.showMessage(easy.LocalizationManager.getText("TossFaild"));
                    easy.MyEvent.sendEvent("eos_toss", {"data":0}, true);
                });
            }
        ); 
        
    }).catch(err =>{
        easy.MyEvent.sendEvent("eos_toss", {"data":0}, true);
        console.error(err);
    });
}

/**
 * 移动步数
 * @param {Object} right
 * @param {Object} left
 * @param {Object} up
 * @param {Object} dow
 */
function move(gameid, right, left, up, dow) {
//  easy.MyEvent.sendEvent("eos_move", {"data":0}, true);
//  return;
    
    var num = mergeMovePos(right, left, up, dow);//合成移动数据
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
                contract.move(account.name, gameid, num, transactionOptions).then(trx => {
                    //console.log("请求移动 成功", trx);
                    var res = "{'award': 0, 'user': '', 'gameuuid': 0, 'win': false}";
                    if (trx.processed && trx.processed.action_traces && trx.processed.action_traces.length > 0 && trx.processed.action_traces[0].act && trx.processed.action_traces[0].console) {
                        res = trx.processed.action_traces[0].console;
                    }
                    //console.log("请求移动 res", res);
					easy.MessageTips.showMessage(easy.LocalizationManager.getText("MoveSuccess"));
                    var resObj = JSON.parse(res);
                    if (resObj.win === true) {
                        //console.log("请求移动 success res.win", resObj.win);
                        easy.MyEvent.sendEvent("eos_move", {"data":resObj.awards}, true);//成功
                    } else {
						
                        //console.log("请求移动 faild res.win", resObj.win);
                        easy.MyEvent.sendEvent("eos_move", {"data":0}, true);//无奖励
                    }
                    //是否成功,要查询交易状况来决策
                }).catch(e => {
                    easy.MyEvent.sendEvent("eos_move", {"data":-1}, true);
					easy.MessageTips.showMessage(easy.LocalizationManager.getText("MoveFaild"));
                    console.log("请求移动失败 error", e);
                });
            }
        ); 
        
    }).catch(err =>{
        easy.MyEvent.sendEvent("eos_move", {"data":-1}, true);
        console.error(err);
    });
}
/**
 * 排行榜数据
 */
function getListRank(){
	/*
    var row = [
        {user:"test1", acc_award:210},
        {user:"arestest1234", acc_award:180},
        {user:"test2", acc_award:150},
        {user:"qazwsxedc123", acc_award:80},
        {user:"test3", acc_award:60},
        {user:"dafdae", acc_award:20}
    ];
    easy.MyEvent.sendEvent("eos_list_rank", {"data":row}, true);
    return;
    */
	
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = "herotbl";
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:"50",
        json: true
    }).then(function(res) {
        if (res && res.rows){
			var temp = {};
			for(var i = 0; i < res.rows.length; i++) {
				if (temp[res.rows[i].user]) {
					temp[res.rows[i].user].acc_awards = res.rows[i].acc_awards;
					temp[res.rows[i].user].awards = res.rows[i].awards;
				} else {
					temp[res.rows[i].user] = res.rows[i];
				}
			}
			var datas = [];
			for(var key in temp) {
				temp[key]["acc_award"] = temp[key].acc_awards;
				temp[key]["award"] = temp[key].awards;
				datas.push(temp[key]);
			}
            easy.MyEvent.sendEvent("eos_list_rank", {"data":datas}, true);
        } else {
            easy.MyEvent.sendEvent("eos_list_rank", {"data":[]}, true);
        }
    });
}

/**
 * 最近奖金获得列表
 */
function getListWinner(gameId){
	/*
    var row = [
        {user:"test1", awards:30, acc_awards:30, gameuuid:gameId},
        {user:"arestest1234", awards:180, acc_awards:180, gameuuid:gameId},
        {user:"test2", awards:50, acc_awards:50, gameuuid:gameId},
        {user:"qazwsxedc123", awards:80, acc_awards:80, gameuuid:gameId},
        {user:"test3", awards:20, acc_awards:20, gameuuid:gameId},
        {user:"dafdae", awards:60, acc_awards:60, gameuuid:gameId}
    ];
    easy.MyEvent.sendEvent("eos_list_winner", {"gameuuid":gameId, "data":row}, true);
    return;
    */
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = "herotbl";
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:'100',
        json: true
    }).then(function(res) {
        if (res && res.rows){
			if (gameId == "-1"){
				easy.MyEvent.sendEvent("eos_list_winner", {"gameuuid":gameId, "data":res.rows}, true);
			} else {
				var temp = [];
				for(var i = 0; i < res.rows.length; i++) {
					if (res.rows[i].gameuuid == gameId) {
						temp.push(res.rows[i]);
					}
				}
				easy.MyEvent.sendEvent("eos_list_winner", {"gameuuid":gameId, "data":temp}, true);
			}
            
        } else {
            easy.MyEvent.sendEvent("eos_list_winner", {"gameuuid":gameId, "data":[]}, true);
        }
    });
}

/**
 * 游戏列表信息
 */
function getListGameInfo(){
    getTableInfo("gametbl", "1000");
}
/**
 * 排队列表
 */
function getListSchedInfo() {
    getTableInfo("schedtbl", "1000");
}

/**
 * 等待队列
 */
function getListWaitInfo() {
    getTableInfo("waittbl", "1000");
}
/**
 * 获取表格信息
 * @param {Object} tableName
 */
function getTableInfo(table_name, limit) {
    //if (!_isInitEosIdentity){
    //    return;
    //}
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = table_name;
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:limit,
        json: true
    }).then(function(res) {
        if (res && res.rows){
            if (temp_table == "gametbl") {
                list_game = res.rows;
            } else if (temp_table == "schedtbl") {
                //-----for test 处理时间-----
//              var seconds = new Date().getTime()/1000;
//              seconds += userGameTime;
//              seconds = Math.floor(seconds);
//              for(var i = 0; i < res.rows.length; i++) {
//                  res.rows[i].update_ts = seconds;
//              }
                //--------------
                list_shecd = res.rows;
            }
            
            //for temp code 临时处理的代码
            if (temp_table == "schedtbl" || temp_table == "waittbl") {
                for(var i = 0; i < res.rows.length; i++) {
                    if (res.rows[i].expired_ts && res.rows[i].update_ts) {
                        res.rows[i].update_ts = res.rows[i].expired_ts;
                    }
                }
            }
            
            easy.MyEvent.sendEvent("table_info", {"table":temp_table, "data":res.rows}, true);
        }
    });
}

/**
 * 推荐的地图
 * 调用这个数据之前,确保已经调用getListUserJoinedGame()方法,并且获得数据
 */
function getRecommendGame() {
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = "gametbl";
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:'10000',
        json: true
    }).then(function(res) {
        if (res && res.rows){
            //var row = res.rows[res.rows.length - 1];//暂时使用最后一条数据
            var index = Math.floor(Math.random()*res.rows.length);
            var row = res.rows[index];
            if (list_join_game.length > 0 && list_join_game.length < res.rows.length){
                for(var i = 0; i < res.rows.length; i++) {
                    if(list_join_game.indexOf(res.rows[i].uuid) < 0) {
                        row = res.rows[i];
                        break;
                    }
                }
            }
            //console.log("getRecommendGame=" + JSON.stringify(row));
            easy.MyEvent.sendEvent("game_recommend", {"data":row}, true);
        }
    });
}
/**
 * 获得用户的游戏地图列表
 */
function getListUserJoinedGame() {
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = "waittbl";
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:'10000',
        key_type:'name',
        index_position:4,
        lower_bound:account_name,
        upper_bound:account_name,
        json: true
    }).then(function(res) {
//      console.log("用户的游戏列表=" + JSON.stringify(res));
        if (res && res.rows){
            list_join_game = [];
            for(var i = 0; i < res.rows.length; i++) {
                if (res.rows[i].user == account_name && list_join_game.indexOf(res.rows[i].gameuuid) < 0) {
                    list_join_game.push(res.rows[i].gameuuid);
                }
            }
            //console.log("getListGameByUser=" + JSON.stringify(list_join_game));
            game.GameData.userGameList=list_join_game;
            easy.MyEvent.sendEvent("eos_user_join_game_list", {"data":list_join_game}, true);
            getRecommendGame();//计算推荐地图
        }
    });
}

//游戏缴费信息 gameid:"-1" 查询所有，其他值：对应游戏的数据 数据格式{user:"qazwsxedc123", gameuuid:"-1", fee:80}
function getUseGameFee(gameid) {
    //easy.MyEvent.sendEvent("eos_user_game_fee", {"data":{user:"qazwsxedc123", gameuuid:"-1", fee:180}}, true);
    //return
    
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = "waittbl";
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:'10000',
        key_type:'name',
        index_position:6,
        lower_bound:account_name,
        upper_bound:account_name,
        json: true
    }).then(function(res) {
        //console.log("用户的游戏列表=" + JSON.stringify(res));
        if (res && res.rows && res.rows.length > 0){
            for(var i = 1; i < res.rows.length; i++) {//全部汇总到第一条,返回第一条的数据
                if (gameid != "-1" && res.rows[i].gameuuid == gameid) {
                    easy.MyEvent.sendEvent("eos_user_game_fee", {"data":res.rows[i]}, true);
                    return;
                } else {
                    res.rows[0].fee += res.rows[i].fee;
                }
            }
            if (gameid == "-1"){
                res.rows[0].gameuuid = "-1";
                easy.MyEvent.sendEvent("eos_user_game_fee", {"data":res.rows[0]}, true);
            } else {
                easy.MyEvent.sendEvent("eos_user_game_fee", {"data":{user:account_name, gameuuid:gameid, fee:0}}, true);
            }
        } else {
            easy.MyEvent.sendEvent("eos_user_game_fee", {"data":{user:account_name, gameuuid:gameid, fee:0}}, true);
        }
    });
}
//游戏奖励信息 gameid:"-1" 查询所有，其他值：对应游戏的数据 数据格式{user:"qazwsxedc123", gameuuid:"-1", acc_award:80, award:86}
function getUserHeroInfo(gameid){//改游戏的数据
    //easy.MyEvent.sendEvent("eos_user_hero_info", {"data":{user:"qazwsxedc123", gameuuid:"-1", acc_award:80, award:86}}, true);
    //return;
    
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = "herotbl";
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:'10000',
        key_type:'name',
        index_position:6,
        lower_bound:account_name,
        upper_bound:account_name,
        json: true
    }).then(function(res) {
        //console.log("用户的游戏列表=" + JSON.stringify(res));
        if (res && res.rows && res.rows.length > 0){
            for(var i = 0; i < res.rows.length; i++) {
                if (gameid != "-1" && res.rows[i].gameuuid == gameid) {
                    easy.MyEvent.sendEvent("eos_user_hero_info", {"data":res.rows[i]}, true);
                    return;
                } else {
                    res.rows[0].acc_awards += res.rows[i].acc_awards;
                    res.rows[0].awards += res.rows[i].awards;
                }
            }
            if (gameid == "-1"){
                res.rows[0].gameuuid = "-1";
                easy.MyEvent.sendEvent("eos_user_hero_info", {"data":res.rows[0]}, true);
            } else {
                easy.MyEvent.sendEvent("eos_user_hero_info", {"data":{user:account_name, gameuuid:gameid, acc_awards:0, awards:0}}, true);
            }
        } else {
            easy.MyEvent.sendEvent("eos_user_hero_info", {"data":{user:account_name, gameuuid:gameid, acc_awards:0, awards:0}}, true);
        }
    });
}

//玩家在某张地图上,缴纳费用的列表(可以得到时间和缴纳的费用)
function getUserWaiterInfo(gameid){
//  var tempData = [
//      {"uuid":0, "gameuuid":gameid, "steps":0,"no":0xfff, "user":account_name, "ts":1545173089, "update_ts":1545173089},
//      {"uuid":1, "gameuuid":gameid, "steps":0,"no":0xfff, "user":account_name, "ts":1545173089, "update_ts":1545173089},
//      {"uuid":2, "gameuuid":gameid, "steps":0,"no":0xfff, "user":account_name, "ts":1545173095, "update_ts":1545173095}
//  ];
//  easy.MyEvent.sendEvent("eos_user_waiter_info", {"data":tempData}, true);
//  return;
    
    const scatter = ScatterJS.scatter;
    const eos = scatter.eos(eos_network, Eos, eosOptions);
    const temp_table = "waiter";
    
    //获取表格记录
    eos.getTableRows({
        code: eos_contract_name,
        scope: eos_contract_name,
        table: temp_table,
        limit:'2000',
        key_type:'name',
        index_position:6,
        lower_bound:account_name,
        upper_bound:account_name,
        json: true
    }).then(function(res) {
        //console.log("用户的游戏列表=" + JSON.stringify(res));
        if (res && res.rows && res.rows.length > 0){
            var temp = [];
            for(var i = 1; i < res.rows.length; i++) {//全部汇总到第一条,返回第一条的数据
                if (res.rows[i].gameuuid == gameid) {
                    temp.push(res.rows[i]);
                }
            }
            temp.sort(sortUuidAsc);
            easy.MyEvent.sendEvent("eos_user_waiter_info", {"data":temp}, true);
        } else {
            easy.MyEvent.sendEvent("eos_user_waiter_info", {"data":[]}, true);
        }
    });
}
//uuid排序
function sortUuidAsc(a,b){
    return a.uuid - b.uuid;
}
//深度克隆
function deepClone(obj){
    var result,oClass=isClass(obj);
    //确定result的类型
    if(oClass==="Object"){
        result={};
    }else if(oClass==="Array"){
        result=[];
    }else{
        return obj;
    }
    for(key in obj){
        var copy=obj[key];
        if(isClass(copy)=="Object"){
            result[key]=deepClone(copy);//递归调用
        }else if(isClass(copy)=="Array"){
            result[key]=deepClone(copy);
        }else{
            result[key]=obj[key];
        }
    }
    return result;
}
//返回传递给他的任意对象的类
function isClass(o){
    if(o===null) return "Null";
    if(o===undefined) return "Undefined";
    return Object.prototype.toString.call(o).slice(8,-1);
}
