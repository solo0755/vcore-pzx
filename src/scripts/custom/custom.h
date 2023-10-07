/* Copyright (C) 2009 - 2010 ScriptDevZero <http://github.com/scriptdevzero/scriptdevzero> 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */ 
#include "scriptPCH.h"
#include "Config/Config.h"
#include "Config/PzxConfig.h"
#include "ScriptedAI.h"
#include <ctime>
#include <string> 
#include <codecvt>
#include <regex>
#include "Chat.h"
#include "Language.h"
#include "PzxMgr.h"
#include "PlayerDump.h"
using  namespace  std;
const uint32 factionID[2][3] =
{

	{ 510,889,729},//污染者，战歌，霜狼
	{ 509,890,730 }//阿拉索，银翼，雷矛
};

//暗月马戏团 BUFF 随机获取吧
//23766 : 124 = "塞格的黑暗塔罗牌：智力", 151 = "智力提高$s1%。"
//23767 : 124 = "塞格的黑暗塔罗牌：护甲", 151 = "护甲提高$s1%。"
//23768 : 124 = "塞格的黑暗塔罗牌：伤害", 151 = "伤害提高$s1%。"
//23769 : 124 = "塞格的黑暗塔罗牌：抗性", 151 = "所有魔法抗性提高$s1点。"
//23735 : 124 = "塞格的黑暗塔罗牌：力量", 151 = "力量提高$s1%。"
//23736 : 124 = "塞格的黑暗塔罗牌：敏捷", 151 = "敏捷提高$s1%。"
//23737 : 124 = "塞格的黑暗塔罗牌：耐力", 151 = "耐力提高$s1%。"
//23738 : 124 = "塞格的黑暗塔罗牌：精神", 151 = "精神提高$s1%。"

const uint32  display_id_meel[6] = { 23767, 23768, 23735, 23736, 23737,23769 };
const uint32  display_id[6] = { 23766, 23767,23737, 23738,23768,23769 };
const uint32  pubID[3] = { 23768,23737 };
struct initClazz
{
	uint8 clazz;
	char * menuName;
	uint32 checkSpells[6];
	uint32 checkItems[5];
	char * talenNames[3];

};

enum EquipmentSlotsPzx                                         // 19 slots
{
	_EQUIPMENT_SLOT_HEAD = 30000,
	_EQUIPMENT_SLOT_NECK,
	_EQUIPMENT_SLOT_SHOULDERS,
	_EQUIPMENT_SLOT_BODY,
	_EQUIPMENT_SLOT_CHEST,
	_EQUIPMENT_SLOT_WAIST,
	_EQUIPMENT_SLOT_LEGS,
	_EQUIPMENT_SLOT_FEET,
	_EQUIPMENT_SLOT_WRISTS,
	_EQUIPMENT_SLOT_HANDS,
	_EQUIPMENT_SLOT_FINGER1,
	_EQUIPMENT_SLOT_FINGER2,
	_EQUIPMENT_SLOT_TRINKET1,
	_EQUIPMENT_SLOT_TRINKET2,
	_EQUIPMENT_SLOT_BACK,
	_EQUIPMENT_SLOT_MAINHAND,
	_EQUIPMENT_SLOT_OFFHAND,
	_EQUIPMENT_SLOT_RANGED,
	_EQUIPMENT_SLOT_TABARD,
	_EQUIPMENT_SLOT_END
};
char * const ItemQualityColorsStr[MAX_ITEM_QUALITY] = {
	"|cff9d9d9d",        // GREY
	"|cffffffff",        // WHITE
	"|cff1eff00",        // GREEN
	"|cff0070dd",        // BLUE
	"|cffa335ee",        // PURPLE
	"|cffff8000",        // ORANGE
	"|cffe6cc80"        // LIGHT YELLOW
};
//15275
static const uint32 taqbossIndex[7][5] = { {88075}, {87602,87603,87601 }, {87648 }, { 87911 }, { 88014 }, { 88076,88077 } ,{ 15517 } };
static const uint32 naxxbossIndex[16] = { 88346,88304,88479,88496,88303,88205,88755,88753,88226,88483,88100,88298,88754,88480,88752,88460 };//88756
static const uint32 naxxbossDefind[9] = { 88346,88304,88479,88496,88303,88205,88100,88298,88460 };
char * const TAQINSTANCE_Names[7] = { u8"吉祥三宝",u8"沙尔图拉",u8"范克瑞斯",u8"哈霍兰公主",u8"双子大帝",u8"奥罗",u8"克苏恩" };
char * const TAQINSTANCE_DATA[7] = {
	"3 0 0 0 0 0 0 0 0 0 ",//吉祥三宝
	"3 0 0 0 0 0 3 0 0 0 ",//沙尔图拉
	"3 3 0 0 0 0 3 0 0 0 ",//范克瑞斯
	"3 3 3 0 0 0 3 0 0 0 ",//哈霍兰
	"3 3 3 3 0 0 3 0 0 0 ",//双子
	"3 3 3 3 3 0 3 0 0 0 ",//奥罗
	"3 3 3 3 3 0 3 0 3 0 "//克苏恩
};
//char * const TAQINSTANCE_DATA[7] = {
//	"3 0 0 0 0 0 0 0 0 0 ",//吉祥三宝
//	"3 0 0 0 0 0 3 0 0 0 ",//沙尔图拉
//	"3 3 0 0 0 0 3 0 0 0 ",//范克瑞斯
//	"3 3 3 0 0 0 3 0 0 0 ",//哈霍兰
//	"3 3 3 3 0 0 3 0 0 0 ",//双子
//	"3 3 3 3 3 0 3 0 0 0 ",//奥罗
//	"3 3 3 3 3 0 3 0 3 0 "//克苏恩
//};
const initClazz all[] = {
	{ 0, u8"未定义",{},{} ,{}},
	{ CLASS_WARRIOR,u8"学习 |cff6247c8狂暴姿态、防御姿态|h|r",{ 2458, 71, 355, 7386, 20617 },{},{u8"武器",u8"狂怒",u8"防护"} },
	{ CLASS_PALADIN,u8"学习 |cff6247c8救赎|h|r 职业技能",{ 7328 },{} ,{u8"神圣",u8"惩戒",u8"防护"} },
	{ CLASS_HUNTER,u8"学习 |cff6247c8驯服野兽|h|r 职业技能",{ 1515, 5149, 883, 2641, 6991, 982 },{} ,{u8"野兽控制",u8"射击",u8"生存"} },
	{ CLASS_ROGUE,u8"学习 |cff6247c8制毒|h|r 职业技能",{ 2842 },{} ,{ u8"刺杀",u8"战斗",u8"敏锐" } },
	{ CLASS_PRIEST,u8"未定义",{},{} ,{ u8"戒律",u8"神圣",u8"暗影" } },
	{ 0, u8"未定义",{},{} ,{ u8"",u8"",u8"" } },
	{ CLASS_SHAMAN,u8"获取 |cff6247c8风火水地图腾|h|r",{},{ 5175 ,5176,5177,5178 } ,{ u8"元素",u8"增强",u8"恢复" } },
	{ CLASS_MAGE,u8"未定义",{},{} ,{ u8"奥术",u8"火焰",u8"冰霜" } },
	{ CLASS_WARLOCK,u8"学习 |cff6247c8召唤小鬼|h|r 职业技能",{ 688, 712,691 },{} ,{ u8"痛苦",u8"恶魔学识",u8"毁灭" } },
	{ 0, u8"未定义",{},{} ,{ u8"",u8"",u8"" } },
	{ CLASS_DRUID,u8"学习 |cff6247c8熊形态|h|r 职业技能",{ 5487,9634 },{} ,{ u8"平衡",u8"野性战斗",u8"恢复" } }
};
enum
{
	NPC_RAZORGORE = 12435,
	NPC_VAELASTRASZ = 13020,
	NPC_LASHLAYER = 12017,
	NPC_FIREMAW = 11983,
	NPC_EBONROC = 14601,
	NPC_FLAMEGOR = 11981,
	NPC_CHROMAGGUS = 14020,
	NPC_NEFARIAN = 11583,

	MAX_BREATHS = 5,
	SPELL_INCINERATE = 23308,                    // Incinerate 23308,23309
	SPELL_TIME_LAPSE = 23310,                    // Time lapse 23310, 23311(old threat mod that was removed in 2.01)
	SPELL_CORROSIVE_ACID = 23313,                    // Corrosive Acid 23313, 23314
	SPELL_IGNITE_FLESH = 23315,                    // Ignite Flesh 23315,23316
	SPELL_FROST_BURN = 23187,                    // Frost burn 23187, 23189

};
//0.5~2.5,勇士、督军 
const uint32 IDS[13][7] =
{
	{ 0,0, 0, 0, 0, 0 ,0},
	{ 189,511, 209, 218,496,474 ,523},//CLASS_WARRIOR
	{ 188, 516, 208, 217,505,475 ,528},//CLASS_PALADIN
	{ 186, 515, 206, 215,509,477,530},//CLASS_HUNTER
	{ 184,512, 204, 213,497,478,524},//CLASS_ROGUE
	{ 182,514, 202, 211,507,480,525 },//CLASS_PRIEST
	{ 0,0, 0, 0, 0, 0 ,0},
	{ 187,519, 207, 216,501,476,527},//CLASS_SHAMAN
	{ 181, 517, 201, 210 ,503,482,526},//CLASS_MAGE
	{ 183, 518, 203, 212 ,499,481,529},//CLASS_WARLOCK
	{ 0,0, 0, 0 , 0, 0,0 },
	{ 185,513, 205, 214 ,493,479,521 },//CLASS_DRUID
};
const static uint32  WeaponDisplay_id1[4] = { 19019,12584,18584,18583 };
const static uint32  WeaponDisplay_id2[4] = { 18876,18873,18869,19364 };
enum Enchants
{
	WEP2H_SUPERIOR_IMPACT = 20000,
	WEP2H_AGILITY,
	WEP_CRUSADER,
	WEP1H_AGILITY,
	WEP_SPELLPOWER,
	WEP_HEAL,
	OFFHAND_SPIRIT,
	OFFHAND_STAM,
	OFFHAND_FROSTRES,
	CHEST_STATS,
	CLOAK_DODGE,
	CLOAK_SUB,
	CLOAK_ARMOR,
	CLOAK_AGILITY,
	BRACER_STAM,
	BRACER_STR,
	BRACER_HEAL,
	BRACER_INT,
	GLOVES_AGI,
	GLOVES_FIRE,
	GLOVES_FROST,
	GLOVES_SHADOW,
	GLOVES_HEALING,
	BOOTS_AGI,
	BOOTS_SPEED,
	BOOTS_STAM,
};



struct talentST
{
	uint32 guid;
	uint32 t1;
	uint32 t2;
	uint32 t3;
	std::string   spells;

};
const static uint32 NUM_BREATHS = sizeof(all) / sizeof(all[0]);

static const uint32 bossIndex[8] = { 84388, 84512, 84387, 84572, 85780, 85783, 85784, 84572 };

static const uint32 aPossibleBreaths[MAX_BREATHS] = { SPELL_INCINERATE, SPELL_TIME_LAPSE, SPELL_CORROSIVE_ACID, SPELL_IGNITE_FLESH, SPELL_FROST_BURN };
static const char * breathTxt[MAX_BREATHS] = { u8"焚烧", u8"时间流逝", u8"腐蚀酸液", u8"点燃躯体", u8"冰霜灼烧" };
void PSendSysMessage(Player *player, const char *format, ...);
void LearnSkillRecipesHelper(Player *player, uint32 skill_id);
bool resetIntance(Player *player, const uint32 instanceID, bool modify);
void saveInstance(Player * player, const uint32 bossindex, const char *  szData, uint32 /**/mapid,uint32 ins_satus);
bool check(Player *player, bool modify);
bool addRep(Player *player, bool modify);
bool addHoner(Player *player, float points,bool modify);
void addItemSet(Player *player, uint8 itemindex);
void Enchant(Player* player, Item* item, const uint32 enchantid);
void MainMenu(Player *player, const ObjectGuid guid);
bool GossipSelect_Enchant(Player* player, const ObjectGuid objectGuid, uint32 sender, const uint32 action);
void Menu_Teleport(Player *player, ObjectGuid guid, const uint32 action);
bool modifyTimeLength(Player *player, uint32 timelength, uint32 needval,uint32 action);
bool cutPointAndlearnSpell(Player *player, uint32 spellid, uint32 needval, uint32 action);
bool cutPointAndAddItem(Player *player, uint32 items, uint32 needval, uint32 count);
bool simpleAction(Player *player, MenuTree & tree, uint32 action, char const* reStr);
bool teleToPoint(Player *player, string xyz, uint32 needval);
bool  ExtractUInt32Base(char** args, uint32& val, uint32 base);
bool GOHello_scarab_meeting(Player* player, GameObject* goinfo);

bool  ExtractOptUInt32(char** args, uint32& val, uint32 defVal);
bool  ExtractFloat(char** args, float& val);
void SkipWhiteSpaces(char** args);
void AddSC_custom_creatures();
void AddSC_custom_items();
//void AddSC_zero_creatures();

// used to call all scripts
void AddSC_zero_scripts();
