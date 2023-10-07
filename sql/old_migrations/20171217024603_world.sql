DROP PROCEDURE IF EXISTS add_migration;
delimiter ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20171217024603');
IF v=0 THEN
INSERT INTO `migrations` VALUES ('20171217024603');
-- Add your query below.

REPLACE INTO `gameobject` (`guid`, `id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(112895, 180654, 30, 713.369, -457.524, 68.5122, 0.173563, 0, 0, 0.0866726, 0.996237, 300, 100, 1),
(112896, 180654, 30, 663.165, -258.335, 14.6348, 4.77835, 0, 0, 0.683404, -0.73004, 300, 100, 1),
(112897, 180654, 30, 383.217, -370.09, -18.2239, 5.40746, 0, 0, 0.424005, -0.90566, 300, 100, 1),
(112898, 180654, 30, 441.107, -243.111, -19.4384, 1.38779, 0, 0, 0.639537, 0.76876, 300, 100, 1),
(112899, 180654, 30, 319.974, -177.098, -30.7619, 2.12292, 0, 0, 0.873069, 0.487596, 300, 100, 1),
(112900, 180654, 30, 290.743, -90.8554, -39.989, 1.69881, 0, 0, 0.750887, 0.66043, 300, 100, 1),
(112901, 180654, 30, 331.946, -4.27387, -2.21438, 0.661298, 0, 0, 0.324657, 0.945832, 300, 100, 1),
(112902, 180654, 30, 269.75, 40.629, 20.6186, 2.64521, 0, 0, 0.969359, 0.245649, 300, 100, 1),
(112903, 180654, 30, 393.96, -126.64, -8.37789, 5.64857, 0, 0, 0.312007, -0.95008, 300, 100, 1),
(112904, 180654, 30, 510.265, -79.9827, 13.2593, 0.951894, 0, 0, 0.45818, 0.888859, 300, 100, 1),
(112905, 180654, 30, 382.65, -257.878, -40.0292, 3.81624, 0, 0, 0.943644, -0.330962, 300, 100, 1),
(112906, 180654, 30, 292.685, -404.206, 16.4788, 0.0730329, 0, 0, 0.0365083, 0.999333, 300, 100, 1),
(112907, 180654, 30, 187.871, -347.493, 49.9014, 0.794812, 0, 0, 0.387028, 0.922068, 300, 100, 1),
(112908, 180654, 30, 107.732, -259.346, 25.1696, 0.524635, 0, 0, 0.25932, 0.965792, 300, 100, 1),
(112909, 180654, 30, 6.7223, -350.19, 18.5508, 4.91501, 0, 0, 0.631965, -0.774997, 300, 100, 1),
(112910, 180654, 30, -30.5515, -393.019, 22.7206, 4.90951, 0, 0, 0.634093, -0.773257, 300, 100, 1),
(112911, 180654, 30, -60.9539, -491.516, 45.2524, 3.47538, 0, 0, 0.986106, -0.166118, 300, 100, 1),
(112912, 180654, 30, -131.512, -559.269, 47.5295, 3.56884, 0, 0, 0.977269, -0.212002, 300, 100, 1),
(112913, 180654, 30, -171.517, -474.433, 28.6923, 2.25487, 0, 0, 0.903314, 0.42898, 300, 100, 1),
(112914, 180654, 30, -275.328, -460.374, 28.3529, 2.90518, 0, 0, 0.993022, 0.117933, 300, 100, 1),
(112915, 180654, 30, -341.738, -352.906, 10.0064, 1.94463, 0, 0, 0.826193, 0.563386, 300, 100, 1),
(112916, 180654, 30, -374.579, -238.116, 14.4059, 2.3177, 0, 0, 0.916343, 0.400394, 300, 100, 1),
(112917, 180654, 30, -377.27, -134.087, 26.4231, 0.201835, 0, 0, 0.100746, 0.994912, 300, 100, 1),
(112918, 180654, 30, -416.699, -43.0144, 35.6197, 2.37582, 0, 0, 0.92759, 0.3736, 300, 100, 1),
(112919, 180654, 30, -268.635, -28.047, 41.0637, 2.0915, 0, 0, 0.865302, 0.501251, 300, 100, 1),
(112920, 180654, 30, -170.045, -95.8946, 35.1276, 4.58907, 0, 0, 0.749335, -0.662191, 300, 100, 1),
(112921, 180654, 30, -107.962, -147.486, 20.0322, 1.25898, 0, 0, 0.588734, 0.808327, 300, 100, 1),
(112922, 180654, 30, -35.2907, -188.867, 21.9315, 6.22191, 0, 0, 0.0306314, -0.999531, 300, 100, 1),
(112923, 180654, 30, -332.611, -243.971, 14.2602, 0.685642, 0, 0, 0.336145, 0.94181, 300, 100, 1),
(112924, 180654, 30, -391.983, -319.717, 19.2212, 2.61144, 0, 0, 0.965072, 0.261984, 300, 100, 1),
(112925, 180654, 30, -453.542, -349.82, 33.1161, 4.78385, 0, 0, 0.681395, -0.731916, 300, 100, 1),
(112926, 180654, 30, -529.263, -363.65, 34.1552, 3.17928, 0, 0, 0.999822, -0.0188435, 300, 100, 1),
(112927, 180654, 30, -469.793, -441.135, 37.7274, 5.88655, 0, 0, 0.19702, -0.980399, 300, 100, 1),
(112928, 180654, 30, -588.135, -284.45, 54.6961, 2.1889, 0, 0, 0.888676, 0.458537, 300, 100, 1),
(112929, 180654, 30, -598.035, -355.309, 61.6211, 4.64798, 0, 0, 0.729509, -0.683971, 300, 100, 1),
(112930, 180654, 30, -687.065, -354.415, 60.9971, 2.15277, 0, 0, 0.880247, 0.474515, 300, 100, 1),
(112931, 180654, 30, -727.579, -313.932, 64.9773, 3.01513, 0, 0, 0.998002, 0.0631869, 300, 100, 1),
(112932, 180654, 30, -761.414, -392.392, 79.5707, 4.00709, 0, 0, 0.907816, -0.419369, 300, 100, 1),
(112933, 180654, 30, -906.423, -336.417, 55.4283, 2.59023, 0, 0, 0.962241, 0.272201, 300, 100, 1),
(112934, 180654, 30, -903.362, -493.481, 46.2971, 4.24585, 0, 0, 0.851409, -0.524503, 300, 100, 1),
(112935, 180654, 30, -917.213, -574.985, 65.2434, 4.13276, 0, 0, 0.879692, -0.475544, 300, 100, 1),
(112936, 180654, 30, -934.978, -526.52, 54.9837, 2.61223, 0, 0, 0.965176, 0.261602, 300, 100, 1),
(112937, 180654, 30, -1024.04, -427.249, 51.7721, 2.35069, 0, 0, 0.922823, 0.385224, 300, 100, 1),
(112938, 180654, 30, -1168.4, -441.957, 58.9765, 2.36404, 0, 0, 0.925374, 0.379055, 300, 100, 1),
(112939, 180654, 30, -1148.24, -370.189, 55.1857, 1.47026, 0, 0, 0.670683, 0.741744, 300, 100, 1),
(112940, 180654, 30, -1098.88, -287.895, 58.3042, 2.60987, 0, 0, 0.964867, 0.262739, 300, 100, 1),
(112941, 180654, 30, -1023.15, -266.23, 63.7499, 1.09798, 0, 0, 0.521826, 0.853052, 300, 100, 1),
(112942, 180654, 30, -952.973, -216.597, 70.0072, 5.94389, 0, 0, 0.168836, -0.985644, 300, 100, 1),
(112943, 180654, 30, -898.396, -279.974, 64.4548, 6.26433, 0, 0, 0.00942756, -0.999956, 300, 100, 1),
(112944, 180654, 30, -812.467, -263.533, 61.2332, 0.308656, 0, 0, 0.153716, 0.988115, 300, 100, 1),
(112945, 180654, 30, -826.867, -206.29, 81.1801, 2.66564, 0, 0, 0.971817, 0.235738, 300, 100, 1),
(112946, 180654, 30, -751.005, -304.337, 67.9169, 0.265461, 0, 0, 0.132341, 0.991204, 300, 100, 1),
(112947, 180654, 30, -774.599, -276.901, 79.6937, 1.20087, 0, 0, 0.565001, 0.82509, 300, 100, 1),
(112948, 180654, 30, -1027.96, -345.436, 57.9763, 3.28846, 0, 0, 0.997305, -0.0733664, 300, 100, 1),
(112949, 180654, 30, -1085.46, -416.242, 53.1229, 3.69529, 0, 0, 0.961921, -0.273328, 300, 100, 1),
(112950, 180654, 30, -1083.35, -524.103, 49.6222, 4.05972, 0, 0, 0.896468, -0.443108, 300, 100, 1),
(112951, 180654, 30, -1084.97, -637.372, 57.0936, 5.79388, 0, 0, 0.24222, -0.970221, 300, 100, 1),
(112952, 180654, 30, -1174.9, -651.996, 64.9471, 3.50366, 0, 0, 0.983658, -0.180045, 300, 100, 1),
(112953, 180654, 30, -1239.32, -587.034, 52.7883, 2.33813, 0, 0, 0.920384, 0.391015, 300, 100, 1),
(112954, 180654, 30, -1300.5, -579.354, 60.908, 2.54861, 0, 0, 0.956368, 0.292165, 300, 100, 1),
(112955, 180654, 30, -1356.81, -540.745, 54.1318, 4.59536, 0, 0, 0.747249, -0.664544, 300, 100, 1),
(112956, 180654, 30, -1385.98, -466.664, 49.9666, 4.12098, 0, 0, 0.882477, -0.470355, 300, 100, 1),
(112957, 180654, 30, -1331.21, -395.291, 50.6683, 2.29729, 0, 0, 0.912209, 0.409725, 300, 100, 1),
(112958, 180654, 30, -1262.1, -436.518, 57.4549, 3.07954, 0, 0, 0.999519, 0.0310191, 300, 100, 1),
(112959, 180654, 30, -1290.14, -423.533, 65.2731, 4.787, 0, 0, 0.680243, -0.732987, 300, 100, 1),
(112960, 180654, 30, -1194.3, -414.699, 58.366, 0.177496, 0, 0, 0.0886317, 0.996064, 300, 100, 1),
(112961, 180654, 30, -1154.95, -293.355, 39.1069, 4.95272, 0, 0, 0.617243, -0.786773, 300, 100, 1),
(112962, 180654, 30, -970.251, -309.063, 65.5942, 5.90305, 0, 0, 0.188925, -0.981992, 300, 100, 1),
(112963, 180654, 30, -811.955, -345.576, 53.6397, 0.328295, 0, 0, 0.163411, 0.986558, 300, 100, 1),
(112964, 180654, 30, -663.423, -398.289, 60.754, 0.713925, 0, 0, 0.34943, 0.936963, 300, 100, 1),
(112965, 180654, 30, -551.439, -418.423, 55.5601, 4.96371, 0, 0, 0.612908, -0.790154, 300, 100, 1),
(112966, 180654, 30, -465.786, -398.398, 28.1647, 1.58415, 0, 0, 0.711811, 0.702371, 300, 100, 1),
(112967, 180654, 30, -172.513, -399.726, 15.8117, 5.72634, 0, 0, 0.274841, -0.96149, 300, 100, 1),
(112968, 180654, 30, -99.5573, -376.942, 15.8158, 1.60378, 0, 0, 0.718672, 0.695349, 300, 100, 1),
(112969, 180654, 30, -32.7714, -523.832, 47.5873, 6.02479, 0, 0, 0.12884, -0.991665, 300, 100, 1),
(112970, 180654, 30, 7.6156, -433.974, 45.3332, 1.86375, 0, 0, 0.802739, 0.59633, 300, 100, 1),
(112971, 180654, 30, 84.1501, -465.508, 49.1185, 6.10647, 0, 0, 0.0882434, -0.996099, 300, 100, 1),
(112972, 180654, 30, 64.5926, -440.894, 49.684, 3.01828, 0, 0, 0.9981, 0.0616151, 300, 100, 1),
(112973, 180654, 30, 136.004, -320.534, 49.1353, 5.28808, 0, 0, 0.477275, -0.878754, 300, 100, 1),
(112974, 180654, 30, 256.619, -450.356, 49.4244, 0.402907, 0, 0, 0.200094, 0.979777, 300, 100, 1),
(112975, 180654, 30, 491.954, -316.141, -12.9273, 0.0981722, 0, 0, 0.0490664, 0.998796, 300, 100, 1),
(112976, 180654, 30, 588.82, -348.094, 32.6343, 4.10684, 0, 0, 0.885779, -0.464107, 300, 100, 1),
(112977, 180654, 30, 649.332, -273.921, 31.1966, 1.18831, 0, 0, 0.559807, 0.828623, 300, 100, 1),
(112978, 180654, 30, 596.836, -114.791, 41.8419, 2.13864, 0, 0, 0.876873, 0.480721, 300, 100, 1),
(112979, 180654, 30, 694.055, -108.678, 51.906, 6.12139, 0, 0, 0.0808088, -0.99673, 300, 100, 1),
(112980, 180654, 30, 604.648, -43.066, 40.3963, 1.75144, 0, 0, 0.768004, 0.640445, 300, 100, 1),
(112981, 180654, 30, 659.018, -4.70805, 55.5548, 0.571768, 0, 0, 0.282006, 0.959413, 300, 100, 1),
(112982, 180654, 30, 652.94, 33.3443, 70.3421, 1.17574, 0, 0, 0.55459, 0.832124, 300, 100, 1),
(112983, 180654, 30, 541.548, -62.9618, 37.852, 4.2168, 0, 0, 0.858937, -0.512081, 300, 100, 1),
(112984, 180654, 30, 558.834, -6.05473, 45.4063, 2.06403, 0, 0, 0.858335, 0.51309, 300, 100, 1),
(112985, 180654, 30, 489.569, 13.109, 24.3065, 2.8565, 0, 0, 0.989857, 0.142066, 300, 100, 1),
(112986, 180654, 30, 414.552, -36.1106, 0.967913, 4.02281, 0, 0, 0.904492, -0.426492, 300, 100, 1);

REPLACE INTO `game_event_gameobject` (`guid`, `event`) VALUES
(112895, 2),(112896, 2),(112897, 2),(112898, 2),(112899, 2),(112900, 2),(112901, 2),(112902, 2),(112903, 2),
(112904, 2),(112905, 2),(112906, 2),(112907, 2),(112908, 2),(112909, 2),(112910, 2),(112911, 2),(112912, 2),
(112913, 2),(112914, 2),(112915, 2),(112916, 2),(112917, 2),(112918, 2),(112919, 2),(112920, 2),(112921, 2),
(112922, 2),(112923, 2),(112924, 2),(112925, 2),(112926, 2),(112927, 2),(112928, 2),(112929, 2),(112930, 2),
(112931, 2),(112932, 2),(112933, 2),(112934, 2),(112935, 2),(112936, 2),(112937, 2),(112938, 2),(112939, 2),
(112940, 2),(112941, 2),(112942, 2),(112943, 2),(112944, 2),(112945, 2),(112946, 2),(112947, 2),(112948, 2),
(112949, 2),(112950, 2),(112951, 2),(112952, 2),(112953, 2),(112954, 2),(112955, 2),(112956, 2),(112957, 2),
(112958, 2),(112959, 2),(112960, 2),(112961, 2),(112962, 2),(112963, 2),(112964, 2),(112965, 2),(112966, 2),
(112967, 2),(112968, 2),(112969, 2),(112970, 2),(112971, 2),(112972, 2),(112973, 2),(112974, 2),(112975, 2),
(112976, 2),(112977, 2),(112978, 2),(112979, 2),(112980, 2),(112981, 2),(112982, 2),(112983, 2),(112984, 2),
(112985, 2),(112986, 2);

REPLACE INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`, `condition_id`) 
VALUES (17489, 21038, 100, 0, 5, 5, 0);
UPDATE `gameobject_template` SET `data1`=17489 WHERE `entry`=180654;

REPLACE INTO `item_template` (`entry`, `patch`, `class`, `subclass`, `name`, `displayid`, `Quality`, `Flags`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `dmg_min3`, `dmg_max3`, `dmg_type3`, `dmg_min4`, `dmg_max4`, `dmg_type4`, `dmg_min5`, `dmg_max5`, `dmg_type5`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `Duration`, `ExtraFlags`) 
VALUES (21038, 0, 0, 8, 'Hardpacked Snowball', 29169, 1, 64, 5, 10, 0, 0, 32767, -1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25677, 0, -1, 0, 30000, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2597, 0, 0, '', 0, 0, 0, 0, 300, 0);

INSERT INTO `spell_mod` (`Id`, `AttributesEx3`, `Comment`) VALUES (26218, 1048576, 'Mistletoe Death Persistent');

-- End of migration.
END IF;
END??
delimiter ; 
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
