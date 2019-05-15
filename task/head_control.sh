#! /bin/bash
#
# Copyright (C) 2017 Albert Lv <altair.albert@yahoo.com>
#
#

sudo systemctl stop ego
source /opt/ego/runtime/env.sh
aplay ./notice/start_head_control_test.wav
/opt/ego/runtime/bin/run.sh head_controlTest
