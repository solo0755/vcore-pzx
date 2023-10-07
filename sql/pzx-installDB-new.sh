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
printf "Creating logs_${mangosSqlName} database \n"
${cmd} -e "drop database if exists logs_${mangosSqlName}"
${cmd} -e "CREATE DATABASE logs_${mangosSqlName} DEFAULT CHARACTER SET utf8"
printf "Creating realmd_${mangosSqlName} database \n"
${cmd} -e "drop database if exists realmd_${mangosSqlName}"
${cmd} -e "CREATE DATABASE realmd_${mangosSqlName} DEFAULT CHARACTER SET utf8"
printf "Creating characters_${mangosSqlName} database \n"
${cmd} -e "drop database if exists characters_${mangosSqlName}"
${cmd} -e "CREATE DATABASE characters_${mangosSqlName} DEFAULT CHARACTER SET utf8"
printf "Creating mangos_${mangosSqlName} database \n"
${cmd} -e "drop database if exists mangos_${mangosSqlName}"
${cmd} -e "CREATE DATABASE mangos_${mangosSqlName} DEFAULT CHARACTER SET utf8"

${cmd} -e "DROP USER 'mangos'@'localhost'"
${cmd} -e "CREATE USER 'mangos'@'localhost' IDENTIFIED BY 'mangos'"
${cmd} -e "DROP USER 'mangos'@'127.0.0.1'"
${cmd} -e "CREATE USER 'mangos'@'127.0.0.1' IDENTIFIED BY 'mangos'"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE, ALTER ROUTINE, CREATE ROUTINE ON mangos_${mangosSqlName}.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON logs_${mangosSqlName}.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON realmd_${mangosSqlName}.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON characters_${mangosSqlName}.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE, ALTER ROUTINE, CREATE ROUTINE ON mangos_${mangosSqlName}.* TO mangos@127.0.0.1;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON logs_${mangosSqlName}.* TO mangos@127.0.0.1;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON realmd_${mangosSqlName}.* TO mangos@127.0.0.1;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON characters_${mangosSqlName}.* TO mangos@127.0.0.1;"
#创建基础数据
#修改缓存大小256M(1024*1024*256),检查方法show global variables like 'max_allowed_packet';
${cmd} -e "set global max_allowed_packet=2684354560"
printf "importing logs_${mangosSqlName} \n"
${cmdimport} logs_${mangosSqlName} <logs.sql
printf "importing realmd_${mangosSqlName} \n"
${cmdimport}  realmd_${mangosSqlName} < logon.sql
printf "importing characters \n"
${cmdimport}  characters_${mangosSqlName} < characters.sql




printf "importing migrations/logon_db_updates.sql \n"
${cmdimport}  realmd < migrations/logon_db_updates.sql
printf "importing migrations/logs_db_updates.sql \n"
${cmdimport}  logs < migrations/logs_db_updates.sql
printf "install finish,success............\n"

#for sql_file in `ls sql/base/dbc/original_data/*.sql`; do mysql -uroot -proot --database=classicmangos < $sql_file ; done
#for sql_file in `ls sql/base/dbc/cmangos_fixes/*.sql`; do mysql -uroot -proot --database=classicmangos < $sql_file ; done
#