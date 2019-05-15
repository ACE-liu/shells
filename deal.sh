#! /bin/bash

file=
var=0
#buffile="buf.log"

echo $0
echo $#
# read -p "get data of var :" var
# echo $var
# echo `expr $var + 1`

# sed -i 's/192.168.1.171/0.0.0.0/g' /etc/.. 

deal()
{
    if [ -z $file ]
    then
        echo "file name is empty."
    else
        total=`grep -c "RECOGNIZED" $file`
        echo "总处理数： ${total}"
        #grep -n "score_non_online" $file | awk '{if ($8 >= "0.75"){var++; print var;}; print var}'
        eval$(grep -n "RECOGNIZED" $file | awk '{if ($8 >= "0.745"){var++; print var>"buf.log";};}')
        var=`grep -c ".*" buf.log`
        echo "大于阈值的总数：${var}"
        rm buf.log
    fi
}

if [ $# -lt 1 ] 
then
    echo "请输入要处理的文件名！"
else
    file=$1
    #echo $file
    deal
fi
