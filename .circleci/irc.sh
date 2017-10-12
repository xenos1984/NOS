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

