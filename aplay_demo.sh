# #! /bin/sh
#! /bin/sh
PAH=/opt/ego/runtime/execution/text2speech/audio

aplay /opt/ego/runtime/execution/text2speech/expired_remind.wav &
echo "sleep 1"

sleep 1

aplay ${PAH}/go_out.wav &
echo "sleep 2"
sleep 2
aplay ${PAH}/welcome_chn.wav &
echo "sleep 3"
sleep 3
echo "exit!"
