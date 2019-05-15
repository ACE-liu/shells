#! /bin/bash

bakPath=/opt/ego/runtime
# logPath=/var/data/log
SUDO=
MAC_ADDR=F9E5C750114A

aplay ./notice/clear2reset.wav
# clear local register data
$SUDO rm -rf /var/data/ego_java_server_data/temp

# clear record_image
$SUDO rm -rf /var/data/ego_java_server_data/record_image

# clear user settings
data=`find $bakPath -name "*.bak" -o -name "*.usr"`
for f in $data
do 
    $SUDO rm -f $f
done

   
# clear /var/data 
echo "clear /var/data"
data=`find /var/data -type f`
for f in $data
do 
    $SUDO rm -f $f
done

# clear old system log
data=`find /var/log -type f -name "*.gz"`
for f in $data
do 
    $SUDO rm -f $f
done

#clear bluetooth
sudo systemctl stop ego
source /opt/ego/runtime/env.sh
echo 2 s c $MAC_ADDR q q | /opt/ego/runtime/bin/run.sh bluetooth_config




#clear mysql tables
cat > /tmp/.my.cnf <<EOF
[client]
user=root
password=iim@1234
EOF

clean(){
   rm -rf /tmp/.my.cnf
   echo "exit..."
}
trap clean EXIT

clean_database(){
   tables=`mysql --defaults-file=/tmp/.my.cnf -DIIMRobot -e "show tables;" | grep user_`
   for t in $tables
   do 
       echo $t
       mysql --defaults-file=/tmp/.my.cnf -DIIMRobot -e "truncate table $t;"
   done
}

clean_database


# clear network connection history
uuid=`nmcli -f uuid connection show  | sed -n '2,$p'`
[ ! -z "$uuid" ]  && sudo nmcli connection delete uuid $uuid

echo "清理完成!"
aplay ./notice/clear2reset_finish.wav


