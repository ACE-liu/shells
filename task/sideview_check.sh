#!/bin/bash

source /opt/ego/runtime/env.sh
./task/sideview_check_test

#echo $?
val=$?
if [ $val -eq 0 ];then
    aplay ./notice/left_ok_right_ok.wav
elif [ $val -eq 2 ];then
    aplay ./notice/left_ok_right_error.wav
elif [[ $val -eq 3 ]];then
    aplay ./notice/left_ok_right_ok.wav
elif [[ $val -eq 5 ]];then
    aplay ./notice/left_wrong_right_wrong.wav
elif [[ $val -eq 6 ]];then
    aplay ./notice/left_wrong_right_error.wav
elif [[ $val -eq 7 ]];then
    aplay ./notice/left_wrong.wav
elif [[ $val -eq 8 ]];then
    aplay ./notice/left_error_right_ok.wav
elif [[ $val -eq 9 ]];then
    aplay ./notice/left_error_right_wrong.wav
elif [[ $val -eq 10 ]];then
    aplay ./notice/left_error_right_error.wav
elif [[ $val -eq 11 ]];then
    aplay ./notice/left_error_right_nopaper.wav
elif [[ $val -eq 12 ]];then
    aplay ./notice/left_ok_right_ok.wav
elif [[ $val -eq 13 ]];then
    aplay ./notice/left_nopaper_right_wrong.wav
elif [[ $val -eq 14 ]];then
    aplay ./notice/left_nopaper_right_error.wav
elif [[ $val -eq 15 ]];then
    aplay ./notice/left_nopaper_right_nopaper.wav
else 
    echo "other"
fi

