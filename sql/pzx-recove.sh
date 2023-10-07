#!/bin/sh
#mangosSqlName=$1

hostip=127.0.0.1
uname=root
upasswd=root



#read -s -n1 -p "Press 'y' to continue ... "    
#printf "Your inputs: $REPLY \n"    

#ip
echo "Press input host ip ... "  
read ipaddress
if [ "$ipaddress" = "" ]; then
	
	echo "use ip default "
else
	hostip=$ipaddress
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

echo "Press 'yes' to continue ... "  
read YES_OR_NO
if [ "$YES_OR_NO" = "yes" ]; then
	echo "Your inputs: $REPLY " 
else
	echo "no continue ,to exit "
	exit 0
fi


cmd="mysql -u$uname -p$upasswd -h$hostip --max_allowed_packet=2684354560 --net_buffer_length=16384 --default-character-set=utf8 -s"
#创建库
printf "Creating logs database \n"
${cmd} -e "drop database if exists logs"
${cmd} -e "CREATE DATABASE logs DEFAULT CHARACTER SET utf8"
printf "Creating realmd database \n"
${cmd} -e "drop database if exists realmd"
${cmd} -e "CREATE DATABASE realmd DEFAULT CHARACTER SET utf8"
printf "Creating characters database \n"
${cmd} -e "drop database if exists characters"
${cmd} -e "CREATE DATABASE characters DEFAULT CHARACTER SET utf8"
printf "Creating mangos database \n"
${cmd} -e "drop database if exists mangos"
${cmd} -e "CREATE DATABASE mangos DEFAULT CHARACTER SET utf8"

${cmd} -e "DROP USER 'mangos'@'localhost'"
${cmd} -e "CREATE USER 'mangos'@'localhost' IDENTIFIED BY 'mangos'"
${cmd} -e "DROP USER 'mangos'@'127.0.0.1'"
${cmd} -e "CREATE USER 'mangos'@'127.0.0.1' IDENTIFIED BY 'mangos'"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE, ALTER ROUTINE, CREATE ROUTINE ON mangos.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON logs.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON realmd.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON characters.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE, ALTER ROUTINE, CREATE ROUTINE ON mangos.* TO mangos@127.0.0.1;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON logs.* TO mangos@127.0.0.1;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON realmd.* TO mangos@127.0.0.1;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON characters.* TO mangos@127.0.0.1;"
#创建基础数据
#修改缓存大小256M(1024*1024*256),检查方法show global variables like 'max_allowed_packet';
#${cmd} -e "set global max_allowed_packet=2684354560"
printf "importing logs \n"
${cmd} logs < bakdb/logsBak.sql
printf "importing realmd \n"
${cmd}  realmd < bakdb/realmdBak.sql
printf "importing characters \n"
${cmd}  characters < bakdb/charactersBak.sql
#安装最新全量数据库
printf "importing ${mangosSqlName} \n"
${cmd}   mangos < bakdb/mangosBak.sql
printf "finish"
