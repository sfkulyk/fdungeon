#!/bin/bash
source $HOME/fdungeon_creds.sh
HEADER="Content-Type: application/x-www-form-urlencoded; charset=utf-8"
/usr/bin/curl -k -H "${HEADER}" --data-urlencode "text=${1}" "https://api.telegram.org/${BOTID}:${TOKEN}/sendMessage?chat_id=${PUBCHAT}&parse_mode=Markdown"
