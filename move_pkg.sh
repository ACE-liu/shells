#!/bin/bash
#./make_pkg.sh -bx86 -cpointgrey
#cd build/runtime/
filePath=/var/tmp
dstPath=tx2_old
prj=ego-runtime
cur_date=`date +%Y%m%d`
ver=`git describe | sed s/version-//g`+x86-*

ssh iim@192.168.1.171 > /dev/null 2>&1 << eof
cd share/EgoPackage/testing
mkdir -p ${cur_date}/${dstPath}
exit
eof

sftp iim@192.168.1.171 > /dev/null 2>&1 << eof
cd share/EgoPackage/testing/${cur_date}/${dstPath}
put ${filePath}/${prj}_${ver}
exit
eof

echo "done!"

