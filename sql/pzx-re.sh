#!/bin/sh
mangosSqlName=$1
user=root
password=root
cmd="mysql -uroot -proot -s"
cmddump="mysqldump --default-character-set=utf8 -u${user} -p${password} "
cmdimport="mysql -uroot -proot -s --default-character-set=utf8"
if [ "${mangosSqlName}" = "" ]; then
echo "the databse sql file  are require!"
printf "Usage: db_xx.bak.sql\n"
exit 0
fi
#备份库


#${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON mangos.* TO mangos@localhost;"
#创建基础数据
#修改缓存大小256M(1024*1024*256),检查方法show global variables like 'max_allowed_packet';
${cmd} -e "set global max_allowed_packet=2684354560"
#安装最新全量数据库
printf "importing ${mangosSqlName} \n"
${cmdimport}   mangosbak < ${mangosSqlName}

printf "recovery success............\n"
