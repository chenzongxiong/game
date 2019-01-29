FROM eosio/eos-dev:v1.4.1

RUN apt-get update -y && apt-get install -y nodejs npm
RUN npm install eosjs@16.0.9 express@4.16.4

ADD ./scripts/entrypoint.sh /opt/eosio/bin/entrypoint.sh

ADD ./scripts/rmexpired.js /opt/eosio/bin/rmexpired.js
ADD ./scripts/sendtokens.js /opt/eosio/bin/sendtokens.js
ADD ./scripts/forcesched.js /opt/eosio/bin/forcesched.js

ADD ./scripts/utils.js /opt/eosio/bin/utils.js
ADD ./scripts/invitation.js /opt/eosio/bin/invitation.js

ENTRYPOINT ["/opt/eosio/bin/entrypoint.sh"]
