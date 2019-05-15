#! /bin/sh
dst_path=tx2_old

ls_date=`date +%Y%m%d`
echo "ls_date:"$ls_date
echo $?
mkdir -p ${ls_date}/${dst_path}

echo $?
echo $*

if [ $# -gt 0 ]
then
 echo $*
else
echo "argc ==0"
fi


for i in `seq 1 10`
do 
  echo $i "input your data"
done 
#c++ shell tools ffmpeg opencv 
#suanfa xieyi mysql 
