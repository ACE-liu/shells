#! /bin/bash
#
# data_transfer.sh
# Copyright (C) 2018 Albert Lv <albert.lv@iim.ltd>
#

set -e

DATADIRS=(
"/var/data/ego_java_server_data/record_image"
)
LOGDIRS=("/var/data/log")
ROBOTDIRS=(
"/var/data/ego_java_server_data/IIM_Feature"
"/var/data/ego_java_server_data/IIM_Features"
#"/var/data/ego_java_server_data/IIM_Setting"
"/var/data/ego_java_server_data/IIM_Images"
"/var/data/ego_java_server_data/IIM_Images_Online"
"/var/data/ego_java_server_data/IIM_Feature_Online"
)

WORKDIR="/tmp/transfer"
OUTFILE="transfer.tgz"


# mysql password
cat > /tmp/.my.cnf << EOF
[client]
user=root
password=iim@1234
EOF


# clean
clean(){
    rm -rf /tmp/.my.cnf
    rm -rf $WORKDIR
}
trap clean EXIT


# dump database
export_database()
{
   mysqldump --defaults-file=/tmp/.my.cnf IIMRobot > $WORKDIR/data.sql 
}

import_database(){
    mysql --defaults-file=/tmp/.my.cnf -DIIMRobot < $WORKDIR/data.sql
}

# truncate data
clean_database(){
   tables=`mysql --defaults-file=/tmp/.my.cnf -DIIMRobot -e "show tables;" | grep user_`
   for t in $tables
   do 
       mysql --defaults-file=/tmp/.my.cnf -DIIMRobot -e "truncate table $t;"
   done
}

# dump files
export_files(){
    DIRS=$@
    for d in ${DIRS};
    do 
        echo "Export data from $d"
        mkdir -p $WORKDIR/$d
        for f in `ls $d`;
        do 
            cp -apP $d/$f $WORKDIR/$d
        done
    done
}

import_files(){
    DIRS=$@
    for d in ${DIRS}
    do
        echo "Import data to $d"
        mkdir -p $d
        [ -d $WORKDIR/$d ] && mv $WORKDIR/$d `dirname $d`
    done
}


# compress 
compress_transfer(){
    if [ -f $OUTFILE ];then
        read -p "File exsit! Delete $OUTFILE?[y/N] " del
        ([ "x$del"="xN" ] || [ "x$del"="xn" ]) && exit 2
    fi
    rm -f $OUTFILE
    [ -d $WORKDIR ] && tar -czf $OUTFILE $WORKDIR
}

# decompress
decompress_transfer(){
    if [ -f "$OUTFILE" ] ;then
        [ ! -d $WORKDIR ] && mkdir -p $WORKDIR
        tar -xzf $OUTFILE -C $WORKDIR
    else
        echo "Error : can't found $OUTFILE"
    fi
}

clean_old_data(){
    DIRS=$@
    for d in ${DIRS}
    do 
        echo "Clean $d"
        for f in `ls $d`
        do 
            rm -rf $d/$f
        done
    done
    echo "Clean database"
    clean_database
}

usage(){
    cat  << EOF
Usage:  
    $0  -[e|i|c] -[yd]    
        -e  export data
        -i  import data
        -c  just clean old data
        -d  import or export acitivty record, default false
        -y  clean old data,no promot
        -f  out put or input filename
EOF
}

# main
ENABLE_DATA=false
CLEAN_OLD_DATA=
cmd=
# parse args
while getopts 'eicdyf:' OPT &> /dev/null;
do
	case $OPT in
		e)
            cmd=e
            ;;
        i)
            cmd=i
            ;;
        c)
            cmd=c
            ;;
        d)
            ENABLE_DATA=true
            ;;
        y)
            CLEAN_OLD_DATA=true
            ;;
        f)
            OUTFILE=$OPTARG
            ;;
	*)
            usage
			exit 7 ;;
	esac 
done
shift $[$OPTIND-1]
case $cmd in
    e)    
        mkdir -p $WORKDIR
        $ENABLE_DATA && export_files ${DATADIRS[@]} ${LOGDIRS[@]}
        export_files ${ROBOTDIRS[@]}
        export_database
        compress_transfer
        ;;
    i)
        if [ $CLEAN_OLD_DATA ];then
            choose=y
        else
            read -p "Clear old data.[y/N] " choose 
        fi
        ([ x"$choose" = "xy" ] || [ x"$choose" = "xY" ]) && clean_old_data ${DATADIRS[@]} ${ROBOTDIRS[@]} ${LOGDIRS[@]}
        decompress_transfer
        $ENABLE_DATA && import_files ${DATADIRS[@]} ${LOGDIRS[@]}
        import_files ${ROBOTDIRS[@]}
        import_database
        ;;
    c)
        if [ $CLEAN_OLD_DATA ];then
            choose=y
        else
            read -p "Clear old data.[y/N] " choose 
        fi
        ([ x"$choose" = "xy" ] || [ x"$choose" = "xY" ]) && clean_old_data ${DATADIRS[@]} ${ROBOTDIRS[@]} ${LOGDIRS[@]}
        ;;
    *)
        usage
        ;;
esac


