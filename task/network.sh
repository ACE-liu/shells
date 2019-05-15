#! /bin/sh
#
# network.sh
# Copyright (C) 2017 Albert Lv <altair.albert@yahoo.com>
#
# Distributed under terms of the MIT license.
#


SSID=iim_robot_5g
PSK=zxzq8888

sudo nmcli dev wifi con $SSID password $PSK

if [ $? -eq 0 ];then
    aplay ./notice/network_connect.wav
else
    aplay ./notice/network_disconnect.wav
fi
