#! /bin/sh

PREFIX=build/runtime
echo  ${PREFIX%%\*} 

copy_project_files()
{
    echo "start copy..."
    set_files=`find -name *.xml| grep -v ${PREFIX%%\*} |grep -v runtime`
    for f in $set_files
   do
        echo $f  
   done
}
