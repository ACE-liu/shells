#! /bin/sh
#
# recognition.sh
# Copyright (C) 2018 Albert Lv <altair.albert@yahoo.com>
#
# Distributed under terms of the MIT license.
#


sudo systemctl stop ego
./task/data_transfer.sh -iy

if [ $? -eq 0 ];then
    aplay ./notice/import_data_success.wav
    sudo systemctl start ego
else
    aplay ./notice/import_data_fail.wav
fi
