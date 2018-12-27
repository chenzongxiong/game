import time
import subprocess
import json


cleos="/opt/eosio/bin/cleos"
contract = "arestest1234"
scope = "arestest1234"
action = "rmexpired"
admin = "arestest1234"
tablename = "schedtbl"

options = ["--url", "http://jungle2.cryptolions.io:80"]

check_schedtbl_cmd = [cleos] + options + ["get", "table", contract, scope, tablename]
sched_cmd = [cleos] + options + ["push", "action", contract, action, '{}', "-p", admin]
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
        else:
            print("empty results")
        time.sleep(interval)
