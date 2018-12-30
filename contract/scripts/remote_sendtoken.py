import time
import subprocess
import json


cleos="/opt/eosio/bin/cleos"
contract = "arestest1234"
scope = "arestest1234"
action = "sendtokens"
admin = "arestest1234"
tablename = "winnertbl"

options = ["--url", "http://jungle2.cryptolions.io:80"]

check_winnertbl_cmd = [cleos] + options + ["get", "table", contract, scope, tablename]
sendtokens_cmd = [cleos] + options + ["push", "action", contract, action] 
permission = ["-p", admin]
interval = 2


if __name__ == "__main__":
    while True:

        results = subprocess.check_output(check_winnertbl_cmd)
        results = json.loads(results)
        if len(results["rows"]) > 0:
            print("send tokens")
            for row in results["rows"]: 
                print(row)
                parameters = json.dumps({"user": row["user"], "gameuuid": row["gameuuid"]})
                final_cmd = sendtokens_cmd + [parameters] + permission
                print(final_cmd)
                subprocess.call(final_cmd)
        else:
            print("empty results")
        print("========================================")
        time.sleep(interval)
