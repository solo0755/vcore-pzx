DROP PROCEDURE IF EXISTS add_migration;
delimiter ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20200413165046');
IF v=0 THEN
INSERT INTO `migrations` VALUES ('20200413165046');
-- Add your query below.


-- remove all incorrect trainers from Magecraft (8250)
DELETE FROM `creature_questrelation` WHERE `quest` = 8250 AND `id` NOT IN(331, 3047, 4567, 7311, 7312);

-- add Magecraft (8250) to correct trainers
INSERT INTO `creature_questrelation` (`id`, `quest`, `patch_min`, `patch_max`) VALUES
(331, 8250, 5, 10),
(3047, 8250, 5, 10),
(7311, 8250, 5, 10),
(7312, 8250, 5, 10);

-- add Journey to the Marsh (1947) to correct trainers
INSERT INTO `creature_questrelation` (`id`, `quest`, `patch_min`, `patch_max`) VALUES
(3048, 1947, 0, 10),
(4568, 1947, 0, 10),
(5144, 1947, 0, 10);
-- (5497, 1947, 0, 10),
-- (5885, 1947, 0, 10);

-- add High Sorcerer Andromath (1939) to Bink (5144)
INSERT INTO `creature_questrelation` (`id`, `quest`, `patch_min`, `patch_max`) VALUES (5144, 1939, 0, 10);

-- add Speak with Un'thuwa (1883) and Report to Anastasia (1959) to Thurston Xane (3049)
INSERT INTO `creature_questrelation` (`id`, `quest`, `patch_min`, `patch_max`) VALUES
(3049, 1959, 0, 10),
(3049, 1883, 0, 10);

-- remove Return to the Marsh (1953) from Uthel'nay (7311)
DELETE FROM `creature_questrelation` WHERE `quest` = 1953 AND `id` = 7311;

-- add Return to the Marsh (1953) to Ursyn Ghull (3048) and Deino (5885)
INSERT INTO `creature_questrelation` (`id`, `quest`, `patch_min`, `patch_max`) VALUES
(3048, 1953, 0, 10),
(5885, 1953, 0, 10);

-- add correct completion text for Speak with Jennea (1860)
UPDATE `quest_template` SET `OfferRewardText` = "You have been sent to aid me in my task, $n?  Good.  It is heartening to see young mages eager to help in our cause...$b$bWhat cause, you ask?  In time, young one.  You shall know... in time." WHERE `entry` = 1860;

-- add correct completion text for Speak with Bink (1879)
UPDATE `quest_template` SET `OfferRewardText` = "Oh, I can see you have promise, yes you do! But it takes more than promise to be a good mage.  And if you're not a good mage, then you're a bad mage.  And bad mages are part of the problem, yes they are!$b$bOops, I'm getting ahead of things.  Let's begin at the start!" WHERE `entry` = 1879;

-- add correct completion text for Speak with Un'thuwa (1883)
UPDATE `quest_template` SET `OfferRewardText` = "Hail, $n.  Praise to you, for my call for aid is now answered.  Let us hope you are up to the task..." WHERE `entry` = 1883;

-- add missing spells to Magis Sparkmantle (1228)
DELETE FROM `npc_trainer` WHERE `entry`=1228;
INSERT INTO `npc_trainer` (`entry`, `spell`, `spellcost`, `reqskill`, `reqskillvalue`, `reqlevel`, `build_min`, `build_max`) VALUES
(1228, 1035, 2000, 0, 0, 20, 0, 5875),
(1228, 1142, 100, 0, 0, 4, 0, 5875),
(1228, 1168, 200, 0, 0, 8, 0, 5875),
(1228, 1173, 100, 0, 0, 6, 0, 5875),
(1228, 1174, 400, 0, 0, 10, 0, 5875),
(1228, 1176, 1800, 0, 0, 18, 0, 5875),
(1228, 1191, 200, 0, 0, 8, 0, 5875),
(1228, 1194, 400, 0, 0, 10, 0, 5875),
(1228, 1196, 2000, 0, 0, 20, 0, 5875),
(1228, 1198, 600, 0, 0, 12, 0, 5875),
(1228, 1200, 2000, 0, 0, 20, 0, 5875),
(1228, 1210, 7000, 0, 0, 28, 0, 5875),
(1228, 1211, 900, 0, 0, 14, 0, 5875),
(1228, 1214, 8000, 0, 0, 30, 0, 5875),
(1228, 1225, 5000, 0, 0, 26, 0, 5875),
(1228, 1228, 15000, 0, 0, 40, 0, 5875),
(1228, 1241, 5000, 0, 0, 26, 0, 5875),
(1228, 1249, 100, 0, 0, 6, 0, 5875),
(1228, 1250, 600, 0, 0, 12, 0, 5875),
(1228, 1251, 3000, 0, 0, 22, 0, 5875),
(1228, 1266, 600, 0, 0, 12, 0, 5875),
(1228, 1267, 1800, 0, 0, 18, 0, 5875),
(1228, 1467, 900, 0, 0, 14, 0, 5875),
(1228, 1472, 10, 0, 0, 1, 0, 5875),
(1228, 1473, 900, 0, 0, 14, 0, 5875),
(1228, 1474, 7000, 0, 0, 28, 0, 5875),
(1228, 1475, 18000, 0, 0, 42, 0, 5875),
(1228, 1481, 2000, 0, 0, 20, 0, 5875),
(1228, 1811, 3000, 0, 0, 22, 0, 5875),
(1228, 1830, 200, 0, 0, 24, 0, 5875),
(1228, 2124, 1500, 0, 0, 16, 0, 5875),
(1228, 2125, 4000, 0, 0, 24, 0, 5875),
(1228, 2141, 100, 0, 0, 6, 0, 5875),
(1228, 2142, 900, 0, 0, 14, 0, 5875),
(1228, 2143, 3000, 0, 0, 22, 0, 5875),
(1228, 2858, 1500, 0, 0, 16, 0, 5875),
(1228, 3142, 1800, 0, 0, 18, 0, 5875),
(1228, 3553, 14000, 0, 0, 38, 0, 5875),
(1228, 3576, 4000, 0, 0, 24, 0, 5875),
(1228, 5146, 200, 0, 0, 8, 0, 5875),
(1228, 5147, 1500, 0, 0, 16, 0, 5875),
(1228, 5148, 4000, 0, 0, 24, 0, 5875),
(1228, 5499, 2000, 0, 0, 20, 0, 5875),
(1228, 5507, 100, 0, 0, 4, 0, 5875),
(1228, 5565, 400, 0, 0, 10, 0, 5875),
(1228, 5566, 2000, 0, 0, 20, 0, 5875),
(1228, 6121, 13000, 0, 0, 34, 0, 5875),
(1228, 6128, 8000, 0, 0, 30, 0, 5875),
(1228, 6130, 10000, 0, 0, 32, 0, 5875),
(1228, 6132, 15000, 0, 0, 40, 0, 5875),
(1228, 6142, 7000, 0, 0, 28, 0, 5875),
(1228, 6144, 3000, 0, 0, 22, 0, 5875),
(1228, 6493, 600, 0, 0, 12, 0, 5875),
(1228, 7323, 2000, 0, 0, 20, 0, 5875),
(1228, 8403, 4000, 0, 0, 24, 0, 5875),
(1228, 8404, 8000, 0, 0, 30, 0, 5875),
(1228, 8405, 13000, 0, 0, 36, 0, 5875),
(1228, 8409, 5000, 0, 0, 26, 0, 5875),
(1228, 8410, 10000, 0, 0, 32, 0, 5875),
(1228, 8411, 14000, 0, 0, 38, 0, 5875),
(1228, 8414, 8000, 0, 0, 30, 0, 5875),
(1228, 8415, 14000, 0, 0, 38, 0, 5875),
(1228, 8420, 10000, 0, 0, 32, 0, 5875),
(1228, 8421, 15000, 0, 0, 40, 0, 5875),
(1228, 8425, 10000, 0, 0, 32, 0, 5875),
(1228, 8426, 15000, 0, 0, 40, 0, 5875),
(1228, 8428, 13000, 0, 0, 36, 0, 5875),
(1228, 8440, 3000, 0, 0, 22, 0, 5875),
(1228, 8441, 8000, 0, 0, 30, 0, 5875),
(1228, 8442, 14000, 0, 0, 38, 0, 5875),
(1228, 8447, 7000, 0, 0, 28, 0, 5875),
(1228, 8448, 12000, 0, 0, 34, 0, 5875),
(1228, 8449, 15000, 0, 0, 40, 0, 5875),
(1228, 8452, 4000, 0, 0, 24, 0, 5875),
(1228, 8453, 13000, 0, 0, 36, 0, 5875),
(1228, 8456, 8000, 0, 0, 30, 0, 5875),
(1228, 8459, 8000, 0, 0, 30, 0, 5875),
(1228, 8460, 15000, 0, 0, 40, 0, 5875),
(1228, 8463, 10000, 0, 0, 32, 0, 5875),
(1228, 8464, 18000, 0, 0, 42, 0, 5875),
(1228, 8493, 12000, 0, 0, 34, 0, 5875),
(1228, 8496, 7000, 0, 0, 28, 0, 5875),
(1228, 8497, 13000, 0, 0, 36, 0, 5875),
(1228, 10055, 28000, 0, 0, 48, 0, 5875),
(1228, 10056, 40000, 0, 0, 58, 0, 5875),
(1228, 10141, 15000, 0, 0, 40, 0, 5875),
(1228, 10142, 32000, 0, 0, 50, 0, 5875),
(1228, 10146, 18000, 0, 0, 42, 0, 5875),
(1228, 10147, 35000, 0, 0, 52, 0, 5875),
(1228, 10152, 18000, 0, 0, 42, 0, 5875),
(1228, 10153, 28000, 0, 0, 48, 0, 5875),
(1228, 10154, 36000, 0, 0, 54, 0, 5875),
(1228, 10155, 42000, 0, 0, 60, 0, 5875),
(1228, 10158, 38000, 0, 0, 56, 0, 5875),
(1228, 10162, 18000, 0, 0, 42, 0, 5875),
(1228, 10163, 32000, 0, 0, 50, 0, 5875),
(1228, 10164, 40000, 0, 0, 58, 0, 5875),
(1228, 10171, 18000, 0, 0, 42, 0, 5875),
(1228, 10172, 36000, 0, 0, 54, 0, 5875),
(1228, 10175, 28000, 0, 0, 48, 0, 5875),
(1228, 10176, 42000, 0, 0, 60, 0, 5875),
(1228, 10178, 35000, 0, 0, 52, 0, 5875),
(1228, 10182, 23000, 0, 0, 44, 0, 5875),
(1228, 10183, 32000, 0, 0, 50, 0, 5875),
(1228, 10184, 38000, 0, 0, 56, 0, 5875),
(1228, 10188, 23000, 0, 0, 44, 0, 5875),
(1228, 10189, 35000, 0, 0, 52, 0, 5875),
(1228, 10190, 42000, 0, 0, 60, 0, 5875),
(1228, 10194, 23000, 0, 0, 44, 0, 5875),
(1228, 10195, 35000, 0, 0, 52, 0, 5875),
(1228, 10196, 42000, 0, 0, 60, 0, 5875),
(1228, 10198, 26000, 0, 0, 46, 0, 5875),
(1228, 10200, 36000, 0, 0, 54, 0, 5875),
(1228, 10203, 26000, 0, 0, 46, 0, 5875),
(1228, 10204, 36000, 0, 0, 54, 0, 5875),
(1228, 10208, 26000, 0, 0, 46, 0, 5875),
(1228, 10209, 35000, 0, 0, 52, 0, 5875),
(1228, 10210, 40000, 0, 0, 58, 0, 5875),
(1228, 10213, 28000, 0, 0, 48, 0, 5875),
(1228, 10214, 38000, 0, 0, 56, 0, 5875),
(1228, 10217, 28000, 0, 0, 48, 0, 5875),
(1228, 10218, 38000, 0, 0, 56, 0, 5875),
(1228, 10221, 32000, 0, 0, 50, 0, 5875),
(1228, 10222, 42000, 0, 0, 60, 0, 5875),
(1228, 10224, 32000, 0, 0, 50, 0, 5875),
(1228, 10226, 42000, 0, 0, 60, 0, 5875),
(1228, 10231, 36000, 0, 0, 54, 0, 5875),
(1228, 12827, 2000, 0, 0, 20, 0, 5875),
(1228, 12828, 15000, 0, 0, 40, 0, 5875),
(1228, 12829, 42000, 0, 0, 60, 0, 5875),
(1228, 13011, 400, 0, 0, 30, 0, 5875),
(1228, 13012, 650, 0, 0, 36, 0, 5875),
(1228, 13014, 900, 0, 0, 42, 0, 5875),
(1228, 13015, 14000, 0, 0, 48, 0, 5875),
(1228, 13016, 1800, 0, 0, 54, 0, 5875),
(1228, 13017, 1890, 0, 0, 60, 0, 5875),
(1228, 13023, 650, 0, 0, 36, 0, 5875),
(1228, 13024, 1150, 0, 0, 44, 0, 5875),
(1228, 13025, 1750, 0, 0, 52, 0, 5875),
(1228, 13026, 2100, 0, 0, 60, 0, 5875),
(1228, 13037, 1170, 0, 0, 46, 0, 5875),
(1228, 13038, 1750, 0, 0, 52, 0, 5875),
(1228, 13039, 2000, 0, 0, 58, 0, 5875),
(1228, 22784, 28000, 0, 0, 46, 0, 5875),
(1228, 22785, 40000, 0, 0, 58, 0, 5875),
(1228, 28403, 2000, 0, 0, 20, 5464, 5875);

-- add missing spells to Kaelystia Hatebringer (4566)
DELETE FROM `npc_trainer` WHERE `entry`=4566;
INSERT INTO `npc_trainer` (`entry`, `spell`, `spellcost`, `reqskill`, `reqskillvalue`, `reqlevel`, `build_min`, `build_max`) VALUES
(4566, 1035, 2000, 0, 0, 20, 0, 5875),
(4566, 1142, 100, 0, 0, 4, 0, 5875),
(4566, 1168, 200, 0, 0, 8, 0, 5875),
(4566, 1173, 100, 0, 0, 6, 0, 5875),
(4566, 1174, 400, 0, 0, 10, 0, 5875),
(4566, 1176, 1800, 0, 0, 18, 0, 5875),
(4566, 1191, 200, 0, 0, 8, 0, 5875),
(4566, 1194, 400, 0, 0, 10, 0, 5875),
(4566, 1196, 2000, 0, 0, 20, 0, 5875),
(4566, 1198, 600, 0, 0, 12, 0, 5875),
(4566, 1200, 2000, 0, 0, 20, 0, 5875),
(4566, 1210, 7000, 0, 0, 28, 0, 5875),
(4566, 1211, 900, 0, 0, 14, 0, 5875),
(4566, 1214, 8000, 0, 0, 30, 0, 5875),
(4566, 1225, 5000, 0, 0, 26, 0, 5875),
(4566, 1228, 15000, 0, 0, 40, 0, 5875),
(4566, 1241, 5000, 0, 0, 26, 0, 5875),
(4566, 1249, 100, 0, 0, 6, 0, 5875),
(4566, 1250, 600, 0, 0, 12, 0, 5875),
(4566, 1251, 3000, 0, 0, 22, 0, 5875),
(4566, 1266, 600, 0, 0, 12, 0, 5875),
(4566, 1267, 1800, 0, 0, 18, 0, 5875),
(4566, 1467, 900, 0, 0, 14, 0, 5875),
(4566, 1472, 10, 0, 0, 1, 0, 5875),
(4566, 1473, 900, 0, 0, 14, 0, 5875),
(4566, 1474, 7000, 0, 0, 28, 0, 5875),
(4566, 1475, 18000, 0, 0, 42, 0, 5875),
(4566, 1481, 2000, 0, 0, 20, 0, 5875),
(4566, 1811, 3000, 0, 0, 22, 0, 5875),
(4566, 1830, 200, 0, 0, 24, 0, 5875),
(4566, 2124, 1500, 0, 0, 16, 0, 5875),
(4566, 2125, 4000, 0, 0, 24, 0, 5875),
(4566, 2141, 100, 0, 0, 6, 0, 5875),
(4566, 2142, 900, 0, 0, 14, 0, 5875),
(4566, 2143, 3000, 0, 0, 22, 0, 5875),
(4566, 2858, 1500, 0, 0, 16, 0, 5875),
(4566, 3142, 1800, 0, 0, 18, 0, 5875),
(4566, 3553, 14000, 0, 0, 38, 0, 5875),
(4566, 3576, 4000, 0, 0, 24, 0, 5875),
(4566, 5146, 200, 0, 0, 8, 0, 5875),
(4566, 5147, 1500, 0, 0, 16, 0, 5875),
(4566, 5148, 4000, 0, 0, 24, 0, 5875),
(4566, 5499, 2000, 0, 0, 20, 0, 5875),
(4566, 5507, 100, 0, 0, 4, 0, 5875),
(4566, 5565, 400, 0, 0, 10, 0, 5875),
(4566, 5566, 2000, 0, 0, 20, 0, 5875),
(4566, 6121, 13000, 0, 0, 34, 0, 5875),
(4566, 6128, 8000, 0, 0, 30, 0, 5875),
(4566, 6130, 10000, 0, 0, 32, 0, 5875),
(4566, 6132, 15000, 0, 0, 40, 0, 5875),
(4566, 6142, 7000, 0, 0, 28, 0, 5875),
(4566, 6144, 3000, 0, 0, 22, 0, 5875),
(4566, 6493, 600, 0, 0, 12, 0, 5875),
(4566, 7323, 2000, 0, 0, 20, 0, 5875),
(4566, 8403, 4000, 0, 0, 24, 0, 5875),
(4566, 8404, 8000, 0, 0, 30, 0, 5875),
(4566, 8405, 13000, 0, 0, 36, 0, 5875),
(4566, 8409, 5000, 0, 0, 26, 0, 5875),
(4566, 8410, 10000, 0, 0, 32, 0, 5875),
(4566, 8411, 14000, 0, 0, 38, 0, 5875),
(4566, 8414, 8000, 0, 0, 30, 0, 5875),
(4566, 8415, 14000, 0, 0, 38, 0, 5875),
(4566, 8420, 10000, 0, 0, 32, 0, 5875),
(4566, 8421, 15000, 0, 0, 40, 0, 5875),
(4566, 8425, 10000, 0, 0, 32, 0, 5875),
(4566, 8426, 15000, 0, 0, 40, 0, 5875),
(4566, 8428, 13000, 0, 0, 36, 0, 5875),
(4566, 8440, 3000, 0, 0, 22, 0, 5875),
(4566, 8441, 8000, 0, 0, 30, 0, 5875),
(4566, 8442, 14000, 0, 0, 38, 0, 5875),
(4566, 8447, 7000, 0, 0, 28, 0, 5875),
(4566, 8448, 12000, 0, 0, 34, 0, 5875),
(4566, 8449, 15000, 0, 0, 40, 0, 5875),
(4566, 8452, 4000, 0, 0, 24, 0, 5875),
(4566, 8453, 13000, 0, 0, 36, 0, 5875),
(4566, 8456, 8000, 0, 0, 30, 0, 5875),
(4566, 8459, 8000, 0, 0, 30, 0, 5875),
(4566, 8460, 15000, 0, 0, 40, 0, 5875),
(4566, 8463, 10000, 0, 0, 32, 0, 5875),
(4566, 8464, 18000, 0, 0, 42, 0, 5875),
(4566, 8493, 12000, 0, 0, 34, 0, 5875),
(4566, 8496, 7000, 0, 0, 28, 0, 5875),
(4566, 8497, 13000, 0, 0, 36, 0, 5875),
(4566, 10055, 28000, 0, 0, 48, 0, 5875),
(4566, 10056, 40000, 0, 0, 58, 0, 5875),
(4566, 10141, 15000, 0, 0, 40, 0, 5875),
(4566, 10142, 32000, 0, 0, 50, 0, 5875),
(4566, 10146, 18000, 0, 0, 42, 0, 5875),
(4566, 10147, 35000, 0, 0, 52, 0, 5875),
(4566, 10152, 18000, 0, 0, 42, 0, 5875),
(4566, 10153, 28000, 0, 0, 48, 0, 5875),
(4566, 10154, 36000, 0, 0, 54, 0, 5875),
(4566, 10155, 42000, 0, 0, 60, 0, 5875),
(4566, 10158, 38000, 0, 0, 56, 0, 5875),
(4566, 10162, 18000, 0, 0, 42, 0, 5875),
(4566, 10163, 32000, 0, 0, 50, 0, 5875),
(4566, 10164, 40000, 0, 0, 58, 0, 5875),
(4566, 10171, 18000, 0, 0, 42, 0, 5875),
(4566, 10172, 36000, 0, 0, 54, 0, 5875),
(4566, 10175, 28000, 0, 0, 48, 0, 5875),
(4566, 10176, 42000, 0, 0, 60, 0, 5875),
(4566, 10178, 35000, 0, 0, 52, 0, 5875),
(4566, 10182, 23000, 0, 0, 44, 0, 5875),
(4566, 10183, 32000, 0, 0, 50, 0, 5875),
(4566, 10184, 38000, 0, 0, 56, 0, 5875),
(4566, 10188, 23000, 0, 0, 44, 0, 5875),
(4566, 10189, 35000, 0, 0, 52, 0, 5875),
(4566, 10190, 42000, 0, 0, 60, 0, 5875),
(4566, 10194, 23000, 0, 0, 44, 0, 5875),
(4566, 10195, 35000, 0, 0, 52, 0, 5875),
(4566, 10196, 42000, 0, 0, 60, 0, 5875),
(4566, 10198, 26000, 0, 0, 46, 0, 5875),
(4566, 10200, 36000, 0, 0, 54, 0, 5875),
(4566, 10203, 26000, 0, 0, 46, 0, 5875),
(4566, 10204, 36000, 0, 0, 54, 0, 5875),
(4566, 10208, 26000, 0, 0, 46, 0, 5875),
(4566, 10209, 35000, 0, 0, 52, 0, 5875),
(4566, 10210, 40000, 0, 0, 58, 0, 5875),
(4566, 10213, 28000, 0, 0, 48, 0, 5875),
(4566, 10214, 38000, 0, 0, 56, 0, 5875),
(4566, 10217, 28000, 0, 0, 48, 0, 5875),
(4566, 10218, 38000, 0, 0, 56, 0, 5875),
(4566, 10221, 32000, 0, 0, 50, 0, 5875),
(4566, 10222, 42000, 0, 0, 60, 0, 5875),
(4566, 10224, 32000, 0, 0, 50, 0, 5875),
(4566, 10226, 42000, 0, 0, 60, 0, 5875),
(4566, 10231, 36000, 0, 0, 54, 0, 5875),
(4566, 12827, 2000, 0, 0, 20, 0, 5875),
(4566, 12828, 15000, 0, 0, 40, 0, 5875),
(4566, 12829, 42000, 0, 0, 60, 0, 5875),
(4566, 13011, 400, 0, 0, 30, 0, 5875),
(4566, 13012, 650, 0, 0, 36, 0, 5875),
(4566, 13014, 900, 0, 0, 42, 0, 5875),
(4566, 13015, 14000, 0, 0, 48, 0, 5875),
(4566, 13016, 1800, 0, 0, 54, 0, 5875),
(4566, 13017, 1890, 0, 0, 60, 0, 5875),
(4566, 13023, 650, 0, 0, 36, 0, 5875),
(4566, 13024, 1150, 0, 0, 44, 0, 5875),
(4566, 13025, 1750, 0, 0, 52, 0, 5875),
(4566, 13026, 2100, 0, 0, 60, 0, 5875),
(4566, 13037, 1170, 0, 0, 46, 0, 5875),
(4566, 13038, 1750, 0, 0, 52, 0, 5875),
(4566, 13039, 2000, 0, 0, 58, 0, 5875),
(4566, 22784, 28000, 0, 0, 46, 0, 5875),
(4566, 22785, 40000, 0, 0, 58, 0, 5875),
(4566, 28403, 2000, 0, 0, 20, 5464, 5875);

-- add correct completion text for Enigma Leggings (8631)
UPDATE `quest_template` SET `OfferRewardText` = "Yes... the worm's skin will make for an excellent protective layer.  Combined with the strongest parts from the Qiraji we've destroyed on our way here, this should make for a formidable piece of armor.  May it aid you in facing the unspeakable horrors that await inside! " WHERE `entry` = 8631;


-- End of migration.
END IF;
END??
delimiter ; 
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
