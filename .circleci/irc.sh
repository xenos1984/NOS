#!/bin/bash

server=irc.freenode.net
port=6667
channel="#nos"
prefix=/root
nick=nos-circleci
base64=`printf "$nick\0$nick\0$IRCPASS" | base64`

(
	printf "CAP REQ :sasl\r\n"
	printf "AUTHENTICATE PLAIN\r\n"
	printf "AUTHENTICATE $base64\r\n"
	printf "CAP END\r\n"
	printf "NICK :nos-circleci\r\n"
	printf "USER nos-circleci nos-circleci nos-circleci :NOS CircleCI bot\r\n"
	printf "JOIN :$channel\r\n"
	printf "PRIVMSG $channel :NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results.\r\n"
	printf "QUIT :Bye!\r\n"
) | nc -q 60 $server $port

exit 0

echo "NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results." > /root/irc.txt

sh -c 'sleep 30; cat /root/irc.txt' | irc -d -c "$channel" $nick $server:$port:$IRCPASS

ii -s $server -p $port -i $prefix -n nos-circleci -f "NOS CircleCI bot" -k IRCPASS &

sleep 60

echo "/j $channel" > $prefix/$server/in
echo "NOS build #$CIRCLE_BUILD_NUM finished - see $CIRCLE_BUILD_URL for results." > "$prefix/$server/$channel/in"
echo "/quit" > $prefix/$server/in

sleep 30
pkill ii

