#!/bin/bash

server=irc.libera.chat
port=6667
channel="#nos"
prefix=/root
nick=nos-circleci
base64=`printf "$nick\0$nick\0$IRCPASS" | base64`

(
	sleep 10
	printf "CAP REQ :sasl\r\n"
	printf "AUTHENTICATE PLAIN\r\n"
	printf "AUTHENTICATE $base64\r\n"
	printf "CAP END\r\n"
	sleep 5
	printf "NICK :nos-circleci\r\n"
	printf "USER nos-circleci nos-circleci nos-circleci :NOS CircleCI bot\r\n"
	sleep 5
	printf "JOIN :$channel\r\n"
	sleep 5
	printf "PRIVMSG $channel :NOS build #$CIRCLE_BUILD_NUM - $CIRCLE_JOB finished - see $CIRCLE_BUILD_URL for results.\r\n"
	sleep 5
	printf "QUIT :Bye!\r\n"
) | nc -q 15 $server $port

