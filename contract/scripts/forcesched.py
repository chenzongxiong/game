import time
import subprocess
import json

cleos="/opt/eosio/bin/cleos"
contract = "cardgameacc"
scope = "cardgameacc"
action = "forcesched"
admin = "player1"
tablename = "waittbl"
interval = 10
check_waittbl_cmd = [cleos, "get", "table", contract, scope, tablename, "--index", 5, "--lower", 0, "--upper", 1]
sched_cmd = [cleos, "push", "action", contract, action, '{}', "-p", admin]

if __name__ == "__main__":
    while True:
        results = subprocess.check_output(check_waittbl_cmd)
        results = json.loads(results)
        if len(results["rows"]) > 0:
            update_ts = results["rows"][0]["update_ts"]
            now = time.time()
            diff = now - update_ts
            if diff > 60:
                print("remove expired users in schedule table")
                subprocess.call(sched_cmd)

        time.sleep(interval)
