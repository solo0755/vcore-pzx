#!/bin/sh
mangosSqlName=world_full_05_october_2019.sql
user=root
password=root
cmd="mysql -uroot -proot -s"
cmddump="mysqldump --default-character-set=utf8 -u${user} -p${password} "
cmdimport="mysql -uroot -proot -s --default-character-set=utf8"
if [ "${mangosSqlName}" = "" ]; then
	        echo "the databse sql file  are require!"
		        printf "Usage: db_xx.sh world_full_05_october_2019.sql\n"
			        exit 0
fi
printf "begin backup mangos to mangosBak.sql....... \n"
${cmddump} mangos > bakdb/mangosBak.sql
printf "Creating mangos database \n"
${cmd} -e "drop database if exists mangos"
${cmd} -e "CREATE DATABASE mangos DEFAULT CHARACTER SET utf8"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE, ALTER ROUTINE, CREATE ROUTINE ON mangos.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE, ALTER ROUTINE, CREATE ROUTINE ON mangos.* TO mangos@127.0.0.1;"
${cmd} -e "set global max_allowed_packet=2684354560"
printf "importing ${mangosSqlName} \n"
${cmdimport}   mangos < ${mangosSqlName}
printf "importing migrations/world_db_updates.sql \n"
${cmdimport}  mangos < migrations/world_db_updates.sql
printf "install finish,success............\n"
