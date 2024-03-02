#!/bin/bash
source $HOME/fdungeon_creds.sh
HEADER="Content-Type: application/x-www-form-urlencoded; charset=utf-8"
/usr/bin/curl -k -H "${HEADER}" --data-urlencode "text=$(iconv -f cp1251 -t utf-8 send_note.txt)" "https://api.telegram.org/${BOTID}:${TOKEN}/sendMessage?chat_id=${PRIVCHAT}&parse_mode=Markdown"
