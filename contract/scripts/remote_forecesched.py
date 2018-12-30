import time
import subprocess
import json

cleos="/opt/eosio/bin/cleos"
contract = "arestest1234"
scope = "arestest1234"
action = "forcesched"
admin = "arestest1234"
tablename = "waittbl"

interval = 30
options = ["--url", "http://jungle2.cryptolions.io:80"]
check_waittbl_cmd = [cleos] + options + ["get", "table", contract, scope, tablename, "--index", "5", "--lower", "0", "--upper", "1", "--key-type", "i64"]
sched_cmd = [cleos] + options + ["push", "action", contract, action, '{}', "-p", admin]

if __name__ == "__main__":
    while True:
        results = subprocess.check_output(check_waittbl_cmd)
        results = json.loads(results)
        if len(results["rows"]) > 0:
            for row in results["rows"]:
                sched_flag = results["rows"][0]["sched_flag"]
                if sched_flag == 0:
                    print("force sched")
                    subprocess.call(sched_cmd)
                    break
        print("========================================")
        time.sleep(interval)
