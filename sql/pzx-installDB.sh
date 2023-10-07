#!/bin/sh
mangosSqlName=$1
cmd="mysql -uroot -proot -s"
cmdimport="mysql -uroot -proot -s --default-character-set=utf8"
if [ "${mangosSqlName}" = "" ]; then
echo "the databse sql file  are require!"
printf "Usage: db_xx.sh world_full_26_august_2018.sql\n"
exit 0
fi


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
${cmd} -e "set global max_allowed_packet=2684354560"
printf "importing logs \n"
${cmdimport} logs <logs.sql
printf "importing realmd \n"
${cmdimport}  realmd < logon.sql
printf "importing characters \n"
${cmdimport}  characters < characters.sql
#安装最新全量数据库
printf "importing ${mangosSqlName} \n"
${cmdimport}   mangos < ${mangosSqlName}


#安装补丁更新
printf "importing migrations/world_db_updates.sql \n"
${cmdimport}  mangos < migrations/world_db_updates.sql
printf "importing migrations/logon_db_updates.sql \n"
${cmdimport}  realmd < migrations/logon_db_updates.sql
printf "importing migrations/logs_db_updates.sql \n"
${cmdimport}  logs < migrations/logs_db_updates.sql
printf "install finish,success............\n"

#for sql_file in `ls sql/base/dbc/original_data/*.sql`; do mysql -uroot -proot --database=classicmangos < $sql_file ; done
#for sql_file in `ls sql/base/dbc/cmangos_fixes/*.sql`; do mysql -uroot -proot --database=classicmangos < $sql_file ; done
#