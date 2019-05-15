#! /bin/sh

PATH=/opt/ego/runtime/execution/text2speech/audio

aplay ./notice/enter_maintenance_mode.wav
sleep 1

aplay $PATH/expired_remind.wav

sleep
