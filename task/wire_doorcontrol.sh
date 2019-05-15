#! /bin/sh
#
# wire_doorcontrol.sh
# Copyright (C) 2018 Albert Lv <altair.albert@yahoo.com>
#
# Distributed under terms of the MIT license.
#


wire_test(){
    echo 0 > /sys/class/gpio/gpio298/value
    sleep 1
    echo 1 > /sys/class/gpio/gpio298/value
}

aplay ./notice/start_wire_test.wav

for i in `seq 1 4`
do
    wire_test 
    sleep 1
done
