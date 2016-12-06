#!/bin/sh

mosquitto_pub -t device/nodeDataEvent/little_dark_led -f aa

mosquitto_sub -t server/action/# 

