#!/bin/sh
mangosSqlName=$1
user=root
password=root
cmd="mysql -uroot -proot -s"
cmddump="mysqldump --default-character-set=utf8 -u${user} -p${password} "
printf "begin backup db ....... \n"
${cmddump} mangos > bakdb/mangosBak.sql
printf "backup mangos success \n"
${cmddump} characters > bakdb/charactersBak.sql
printf "backup characters success \n"
${cmddump} realmd > bakdb/realmdBak.sql
printf "backup realmd success \n"
${cmddump} logs > bakdb/logsBak.sql
printf "backup logs success \n"
printf "backup db finish ............\n"
