#! /bin/sh
#
# copy_rules.sh
# Copyright (C) 2018 Albert Lv <altair.albert@yahoo.com>
#
# Distributed under terms of the MIT license.
#


cp ./file/01-b150.rules /etc/udev/rules.d/
udevadm trigger /dev/ttyUSB*
