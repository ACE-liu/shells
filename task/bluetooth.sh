#! /bin/bash
#
# Copyright (C) 2017 Albert Lv <altair.albert@yahoo.com>
#
#

MAC_ADDR=F9E5C750114A
sudo systemctl stop ego
source /opt/ego/runtime/env.sh
aplay ./notice/start_bluetooth_test.wav
# enter usermode, start scan, start connect to MAC_ADDR, quit
echo 2 s c $MAC_ADDR q q | /opt/ego/runtime/bin/run.sh bluetooth_config
sleep 2
# enter usermode, test door_control, quit
ret=`echo 2 t t q q | /opt/ego/runtime/bin/run.sh bluetooth_config | grep '8'`
echo $ret
if [ -n "$ret" ];then
    aplay ./notice/bluetooth_door_control_success.wav
else
    aplay ./notice/bluetooth_door_control_fail.wav
fi
