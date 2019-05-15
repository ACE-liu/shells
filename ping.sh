#! /bin/sh

ip_addr=192.168.2.30

sleep 5
for i in `seq 0 5`
do
echo "stsart ping..."
 ping -c 1 -i 1 $ip_addr>>1.log
 echo $?
done 


echo "done..."
