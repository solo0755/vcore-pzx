#!/bin/sh
folderName=$1
dbname=characters
hostip=127.0.0.1
uname=root
upasswd=root

if [ "${folderName}" = "" ]; then
echo "the folder Name are require!"
printf "Usage: pzx_xx.sh <folder Name>\n"
exit 0
fi

#ip
echo "Press input host ip ... "  
read ipaddress
if [ "$ipaddress" = "" ]; then
	
	echo "use ip default "
else
	hostip="$ipaddress"
	echo "use ip: $hostip "
fi

#username
echo "Press input username ...[root] "  
read username
if [ "$username" = "" ]; then
	
	echo "use username default [root] "
else
	uname=$username
	echo "use username: $uname "
fi

#password
echo "Press input passwd ... [root]"  
read passwd
if [ "$passwd" = "" ]; then
	
	echo "use password default [root] "
else
	upasswd=$passwd
	echo "use password: $upasswd "
fi


cmd="mysql -u$uname -p$upasswd -h$hostip --max_allowed_packet=2147483648 --net_buffer_length=16384 --default-character-set=utf8 -s"
#cmdimport="mysql -u$uname -p$upasswd -h$hostip -s --default-character-set=utf8"

${cmd} -e "set global max_allowed_packet=2684354560"
echo "use max_allowed_packet: 2684354560 "

for sql_file in `ls ${folderName}/*.sql`; 
	do 
		echo "start to importing file: $sql_file"
		${cmd} --database=${dbname} < $sql_file ; 
		echo "importing file: $sql_file done."
	done
#for sql_file in `ls sql/base/dbc/cmangos_fixes/*.sql`; do mysql -uroot -proot --database=classicmangos < $sql_file ; done
#
