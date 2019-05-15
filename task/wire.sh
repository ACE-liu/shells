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
