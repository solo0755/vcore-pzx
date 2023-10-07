#!/bin/sh
mangosSqlName=$1
cmd="mysql -uroot -proot -s"
cmdimport="mysql -uroot -proot -s --default-character-set=utf8"

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

#赋予mangos用户权限

${cmd} -e "CREATE USER 'mangos'@'localhost' IDENTIFIED BY 'mangos'"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE, ALTER ROUTINE, CREATE ROUTINE ON mangos.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON logs.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON realmd.* TO mangos@localhost;"
${cmd} -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER, LOCK TABLES, CREATE TEMPORARY TABLES, EXECUTE ON characters.* TO mangos@localhost;"