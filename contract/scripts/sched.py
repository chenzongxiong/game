import time
import subprocess
import json

check_waittbl_cmd = "/opt/eosio/bin/cleos --url http://jungle2.cryptolions.io:80 get table arestest1234 arestest1234 waittbl".split()
check_schedtbl_cmd = "/opt/eosio/bin/cleos --url http://jungle2.cryptolions.io:80 get table arestest1234 arestest1234 schedtbl".split()
sched_cmd = ["/opt/eosio/bin/cleos",  "--url",  "http://jungle2.cryptolions.io:80", "push", "action", "arestest1234", "schedusers", '{"gameuuid": 0, "total": 10}', "-p", "arestest1234"]
interval = 10


if __name__ == "__main__":
    while True:
        results = subprocess.check_output(check_waittbl_cmd)
        results = json.loads(results)
        print("number of results {}".format(len(results["rows"])))
        if len(results["rows"]) > 0:
            subprocess.call(sched_cmd)
            # print("outputs: ", outputs)

        results = subprocess.check_output(check_schedtbl_cmd)
        results = json.loads(results)
        if len(results["rows"]) > 0:
            print("force to remove expired users")
            diff_ts = time.time() - results["rows"][0]["update_ts"]
            steps = results["rows"][0]["steps"]
            # force call it to remove expired users
            if steps !=0 and diff_ts >= 120:
                subprocess.call(sched_cmd)
            if steps == 0 and diff_ts >= 300:
                subprocess.call(sched_cmd)

        time.sleep(interval)
