import time
import subprocess
import json

check_schedtbl_cmd = "/opt/eosio/bin/cleos get table cardgameacc cardgameacc schedtbl".split()
sched_cmd = ["/opt/eosio/bin/cleos", "push", "action", "cardgameacc", "rmexpired", '{}', "-p", "player1"]
interval = 10


if __name__ == "__main__":
    while True:

        results = subprocess.check_output(check_schedtbl_cmd)
        results = json.loads(results)
        if len(results["rows"]) > 0:
            print("results: ", results)
            expired_ts = results["rows"][0]["expired_ts"]
            now = time.time()
            if now > expired_ts:
                print("remove expired users in schedule table")
                subprocess.call(sched_cmd)

        time.sleep(interval)
