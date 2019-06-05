#!/bin/bash
SERVER=maker.ifttt.com
KEY=YOUR-IFTTT-MAKER-API-KEY
CIPHERLIST=AES256-SHA256:AES256-GCM-SHA384:AES256-SHA:AES128-GCM-SHA256:AES128-SHA256:AES128-SHA
EVENT=new_temperature_event

if [ "$#" -ne "2" ]; then
    echo "syntax error: missing arguments"
    exit 1
fi
EVENT=$1
VALUE=$2

curl -k --ciphers $CIPHERLIST -X POST -H "Content-Type: application/json" -d "{\"value1\":\"$VALUE\"}" https://$SERVER/trigger/$EVENT/with/key/$KEY &> /dev/null
exit $?
