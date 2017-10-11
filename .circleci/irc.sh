#!/bin/bash

server=irc.freenode.net
port=6667
channel="#nos"
prefix=/root
nick=nos-circleci

echo "msg #nos NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results." > ~/.ircrc
echo "quit" >> ~/.ircrc

irc -d -c "$channel" $nick $server:$port:$IRCPASS

#printf "NICK :nos-circleci\r\nPASS $IRCPASS\r\nUSER nos-circleci nos-circleci nos-circleci :NOS CircleCI bot\r\nJOIN :$channel\r\nPRIVMSG $channel :NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results.\r\nQUIT :Bye!\r\n" | nc -q 60 $server $port

exit 0

ii -s $server -p $port -i $prefix -n nos-circleci -f "NOS CircleCI bot" -k IRCPASS &

sleep 60

echo "/j $channel" > $prefix/$server/in
echo "NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results." > "$prefix/$server/$channel/in"
echo "/quit" > $prefix/$server/in

sleep 30
pkill ii

