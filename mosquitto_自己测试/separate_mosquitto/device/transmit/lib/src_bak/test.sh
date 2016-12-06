#!/bin/sh

while true;do
	./mosquitto_pub -t event/ideahos/server_1/device_1 -m llllllllllllllll
	sleep 1
	./mosquitto_pub -t event/ideahos/server_1/device_1 -m -----------------
	sleep 1
done
