#! /bin/bash

ARCH=
for i in `seq 1 100`
do
echo haha
done


for i in $(seq 1 20)
do 
echo $i
done
echo 111$?
sudo nmcli device wifi connect iim_robot_5g password zxzq888
echo $?


p=10
q=20
w=`expr $p + $q`
echo "w is: "$w
[ "aarch64" = `uname -p` ] && ARCH=arm64
[ "x86_64" = `uname -p` ] && ARCH=amd64

echo $ARCH
