#! /bin/sh
#
# network.sh
# Copyright (C) 2017 Albert Lv <altair.albert@yahoo.com>
#
# Distributed under terms of the MIT license.
#

notify_audio(){
    if [ $? -eq 0 ];then
        aplay ./notice/install_success.wav
    else
        aplay ./notice/install_fail.wav
    fi
}
trap  notify_audio EXIT

DEBS=`ls package/*.deb`
if [ ! -z "$DEBS" ];then
    for f in $DEBS
    do
        sudo dpkg -i "./$f"
        id=$?
        if [ $id -ne 0 ];then
            exit $id
        fi
    done
fi
