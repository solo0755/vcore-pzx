DROP PROCEDURE IF EXISTS add_migration;
delimiter ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20191210164802');
IF v=0 THEN
INSERT INTO `migrations` VALUES ('20191210164802');
-- Add your query below.


-- Delete custom Lord Victor Nefarius.
DELETE FROM `creature_template` WHERE `entry`=10163;


-- End of migration.
END IF;
END??
delimiter ; 
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
