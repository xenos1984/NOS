#!/bin/bash

# printf "NICK :nos-circleci\r\nUSER nos-circleci nos-circleci nos-circleci :NOS CircleCI bot\r\nJOIN :#nos\r\nPRIVMSG #nos :NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results.\r\nQUIT :Bye!\r\n" | nc -q 60 irc.freenode.net 6667

server=irc.freenode.net
port=6667
channel="#nos"

ii -s $server -p $port -i /root -n nos-circleci -f "NOS CircleCI bot" &

sleep 30

echo "/j $channel" > /root/$server/in
echo "NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results." > "/root/$server/$channel/in"
echo "/quit" > /root/$server/in

