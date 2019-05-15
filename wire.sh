#! /bin/sh

path=/opt/ego/runtime/bin/
curPath=
#path=./
file=system_settings.xml
deal()
{
  rm ${file}.* 2>/dev/null
  sed -i "s/bluetooth/wire/g" $file
  echo "替换bluetooth为wire成功！"
  cd $curPath
}

uuid=`nmcli -f uuid connection show|sed -n '2,$p'`
for f in $uuid
do
  echo $f
done

curPath=`pwd`
cd $path 2>/dev/null

result=$?

#echo 'cd $path执行结果: '$result

if [ $result -eq 0 ]
then
   deal
   aplay ./notice/change_opendoor_type.wav
else
   echo "未找到$path文件夹！"  
   aplay ./notice/change_opendoor_type_fail.wav
fi
