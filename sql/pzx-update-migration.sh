#!/bin/sh
mangosSqlName=$1
user=root
password=root
cmd="mysql -uroot -proot -s"
cmddump="mysqldump --default-character-set=utf8 -u${user} -p${password} "
cmdimport="mysql -uroot -proot -s --default-character-set=utf8"
printf "begin backup mangos to mangosBak.sql....... \n"
${cmddump} mangos > bakdb/mangosBak.sql
${cmd} -e "set global max_allowed_packet=2684354560"
printf "importing migrations/world_db_updates.sql \n"
${cmdimport}  mangos < migrations/world_db_updates.sql
printf "install finish,success............\n"
