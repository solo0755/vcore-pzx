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

 //#include "scriptPCH.h"
 //#include "custom.h"
 //#include "Config/Config.h"

#include "custom.h"


void PSendSysMessage(Player *player, const char *format, ...)
{
	va_list ap;
	char str[2048];
	va_start(ap, format);
	vsnprintf(str, 2048, format, ap);
	va_end(ap);

	WorldPacket data(SMSG_SERVER_MESSAGE, (strlen(str) + 1));              // guess size
	data << uint32(SERVER_MSG_CUSTOM);
	data << str;

	if (player&&player->GetSession())
		player->GetSession()->SendPacket(&data);

}
void clickClassTran(Player *pPlayer, const ObjectGuid guid) {
	pPlayer->PlayerTalkClass->ClearMenus();
	switch (pPlayer->GetClass())
	{
	case CLASS_DRUID:
		pPlayer->PlayerTalkClass->SendGossipMenu(4913, guid);
		break;
	case CLASS_HUNTER:
		pPlayer->PlayerTalkClass->SendGossipMenu(10090, guid);
		break;
	case CLASS_MAGE:
		pPlayer->PlayerTalkClass->SendGossipMenu(328, guid);
		break;
	case CLASS_PALADIN:
		pPlayer->PlayerTalkClass->SendGossipMenu(1635, guid);
		break;
	case CLASS_PRIEST:
		pPlayer->PlayerTalkClass->SendGossipMenu(4436, guid);
		break;
	case CLASS_ROGUE:
		pPlayer->PlayerTalkClass->SendGossipMenu(4797, guid);
		break;
	case CLASS_SHAMAN:
		pPlayer->PlayerTalkClass->SendGossipMenu(5003, guid);
		break;
	case CLASS_WARLOCK:
		pPlayer->PlayerTalkClass->SendGossipMenu(5836, guid);
		break;
	case CLASS_WARRIOR:
		pPlayer->PlayerTalkClass->SendGossipMenu(4985, guid);
		break;
	}
}
bool resetIntance(Player *player, const uint32 instanceID, bool modify) {
	if (player->GetGroup() || player->GetMapRef()->IsDungeon()) {
		PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍或者副本中|h|r，请先脱离队伍或离开副本区域.");
		return false;
	}
	bool isok = false;
	Player::BoundInstancesMap &binds = player->GetBoundInstances();
	for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
	{
		if (instanceID != 999999 && instanceID != itr->first)
		{
			++itr;
			continue;
		}
		if (itr->first != player->GetMapId())//不可在副本中重置副本,或者自己在副本中重置自己
		{
			PSendSysMessage(player, u8"[系统消息]:副本%d重置成功,欢迎再次体验.", itr->first);
			player->UnbindInstance(itr);
			isok = true;
		}
		else
			++itr;
	}
	if (modify && !isok)
		PSendSysMessage(player, u8"[系统消息]:您暂时还没有 副本进度. ");
	return isok;
}

void saveInstance(Player * player, const uint32 bossindex, const char *  szData, uint32 /**/mapid = 469, uint32 ins_satus = 0) {

	uint32 id = sMapMgr.GenerateInstanceId();
	uint32 count = 0;
	while (!CharacterDatabase.DirectPExecute("INSERT INTO instance VALUES ('%u', '%u', '%u', '%s')", id, mapid, 0, szData)) {
		count++;
		if (count > 100) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000系统繁忙，稍后再试|h|r");//防止死循环
			return;
		}
		id = sMapMgr.GenerateInstanceId();
		continue;//插入成功为止
	}
	//CharacterDatabase.DirectPExecute("INSERT INTO character_instance VALUES ('%u', '%u', '%u')", player->GetGUIDLow(), id, 1);

	time_t mutetime = time(nullptr) + 60 * 48 * MINUTE;//2天后再刷新，一般公会不会连续开荒2天
	if (mapid != 469) {///TAQ
		for (size_t i = 0; i < bossindex + 1; i++)
		{
			const uint32*   getkillCreature = taqbossIndex[i];
			while (*getkillCreature) {
				CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", *getkillCreature, uint64(mutetime), id, mapid);
				if (mapid == 531 && bossindex == 4 && ins_satus == 4) {//创建双子进度的时候，需要清理掉5狗
					CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 88015, uint64(mutetime), id, mapid);
					CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 88016, uint64(mutetime), id, mapid);
					CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 88017, uint64(mutetime), id, mapid);
					CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 88018, uint64(mutetime), id, mapid);
					CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 88019, uint64(mutetime), id, mapid);
				}
				getkillCreature++;
			}
		}


	}
	else {//黑翼

		CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 84389, uint64(mutetime), id, mapid);
		CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 84390, uint64(mutetime), id, mapid);
		CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", 84391, uint64(mutetime), id, mapid);
		for (size_t i = 0; i < bossindex - 1; i++)
		{
			CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", bossIndex[i], uint64(mutetime), id, mapid);
		}

	}

	MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(mapid);

	DungeonPersistentState *state = (DungeonPersistentState*)sMapPersistentStateMgr.AddPersistentState(mapEntry, id, uint64(mutetime), true);
	if (state) player->BindToInstance(state, true, false);
	sMapPersistentStateMgr.LoadCreatureRespawnTimes();
	sMapPersistentStateMgr.LoadGameobjectRespawnTimes();


}




bool check(Player *player, bool modify) {
	bool isok = true;
	const static uint32 NUM_BREATHS = sizeof(all) / sizeof(all[0]);
	for (uint32 id = 0; id < NUM_BREATHS; id++) {
		const initClazz clzz = all[id];
		if (clzz.clazz > 0 && player->GetClass() == clzz.clazz) {
			//检查技能
			const uint32* spells = clzz.checkSpells;
			while (spells != nullptr&&*spells > 0) {
				if (!player->HasSpell(*spells)) {
					isok = false;
					if (modify) {
						player->LearnSpell(*spells, false);
					}
					else
						break;
				}
				spells++;
			}

			//检查物品
			const uint32* items = clzz.checkItems;
			while (items != nullptr&&*items > 0) {
				if (!player->HasItemCount(*items, 1, true)) {
					isok = false;
					if (modify) {
						ItemPosCountVec dest;
						InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, *items, 1);
						if (msg == EQUIP_ERR_OK)
						{
							Item* item = player->StoreNewItem(dest, *items, true);
							//player->SendNewItem(item, 1, false, true);
							player->SendNewItem(item, 1, true, false);
							PSendSysMessage(player, u8"[系统消息]:%s 已经添加到你包中", item->GetProto()->Name1);
						}
						else
						{
							player->SendEquipError(msg, nullptr, nullptr, *items);
							PSendSysMessage(player, u8"[系统消息]:请保持包包有足够空间");
							isok = false;
						}
					}
					else
						break;//检查模式就停止
				}
				items++;
			}

		}
		if (!modify && !isok) {
			break;
		}
	}
	return isok;
}
bool addHoner(Player *player, float points, bool modify) {
	bool isok = true;
	if (player->GetHonorMgr().GetRankPoints() < points) { //.ho setrp 50000
		if (modify) {
			player->GetHonorMgr().SetRankPoints(points);//高阶督军/大元帅 需要60000.0f
			player->GetHonorMgr().Update();
			return true;
		}
		isok = false;
	}
	return isok;
}
bool addRep(Player *player, bool modify) {
	bool isok = true;
	const uint32* fas = player->GetTeam() == HORDE ? factionID[0] : factionID[1];
	//const static uint32 NUM_BREATHS = sizeof(fas) / sizeof(fas[0]);
	for (uint32 id = 0; id < 3; id++) {
		FactionEntry const *factionEntry = sObjectMgr.GetFactionEntry(fas[id]);//faction ID 参考DPS
		if (player->GetReputationMgr().GetReputation(factionEntry) < 42000) {
			if (modify) {
				player->GetReputationMgr().SetReputation(factionEntry, 42001);//声望值
			}
			isok = false;
		}
	}
	float rp = sPzxConfig.GetFloatDefault("openRP", 50000.0f);//12级别
	if (player->GetHonorMgr().GetRankPoints() < rp) { //.ho setrp 50000
		if (modify) {
			player->GetHonorMgr().SetRankPoints(rp);//高阶督军/大元帅 需要60000.0f
			player->GetHonorMgr().Update();
		}
		isok = false;
	}
	return isok;
}

void addItemSet(Player *player, uint8 itemindex) {
	uint32 itemsetid = IDS[player->GetClass()][itemindex];
	if (itemsetid) {
		for (uint32 id = 0; id < sItemStorage.GetMaxEntry(); id++)
		{
			ItemPrototype const *pProto = sItemStorage.LookupEntry<ItemPrototype>(id);
			if (!pProto)
				continue;

			if (pProto->ItemSet == itemsetid)
			{
				if (player->HasItemCount(pProto->ItemId, 1, true)) {//已经有一件了
					continue;
				}
				ItemPosCountVec dest;
				InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, pProto->ItemId, 1);
				if (msg == EQUIP_ERR_OK)
				{
					Item* item = player->StoreNewItem(dest, pProto->ItemId, true);

					// remove binding (let GM give it to another player later)

					//player->SendNewItem(item, 1, false, true);
					player->SendNewItem(item, 1, true, false);
					PSendSysMessage(player, u8"[系统消息]:%s 已经添加到你包中", item->GetProto()->Name1);
				}
				else
				{
					player->SendEquipError(msg, nullptr, nullptr, pProto->ItemId);
					PSendSysMessage(player, u8"[系统消息]:请保持包包有足够空间");
					//PSendSysMessage(LANG_ITEM_CANNOT_CREATE, pProto->ItemId, 1);
				}
			}
		}

	}
}

void Enchant(Player* player, Item* item, const uint32 enchantid)
{
	if (!item)
	{
		PSendSysMessage(player, u8"请先装备好你的物品再附魔");
		return;
	}

	if (!enchantid)
	{
		PSendSysMessage(player, u8"未找到附魔项目");
		return;
	}

	item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
	item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
	PSendSysMessage(player, u8"%s 附魔成功", item->GetProto()->Name1);
}

bool GossipSelect_Enchant(Player* player, const ObjectGuid objectGuid, uint32 sender, const uint32 action)
{
	if (sender != GOSSIP_SENDER_MAIN)
		return true;

	if (action >= 30000)
	{
		switch (action)
		{
		case _EQUIPMENT_SLOT_CHEST:
			player->ADD_GOSSIP_ITEM(5, u8"所有属性+4", GOSSIP_SENDER_MAIN, CHEST_STATS);
			break;
		case _EQUIPMENT_SLOT_BACK:
			player->ADD_GOSSIP_ITEM(5, u8"敏捷+3", GOSSIP_SENDER_MAIN, CLOAK_AGILITY);
			player->ADD_GOSSIP_ITEM(5, u8"护甲+70", GOSSIP_SENDER_MAIN, CLOAK_ARMOR);
			player->ADD_GOSSIP_ITEM(5, u8"躲闪+1%", GOSSIP_SENDER_MAIN, CLOAK_DODGE);
			player->ADD_GOSSIP_ITEM(5, u8"狡诈", GOSSIP_SENDER_MAIN, CLOAK_SUB);
			break;
		case _EQUIPMENT_SLOT_WRISTS:
			player->ADD_GOSSIP_ITEM(5, u8"耐力+9", GOSSIP_SENDER_MAIN, BRACER_STAM);
			player->ADD_GOSSIP_ITEM(5, u8"力量+9", GOSSIP_SENDER_MAIN, BRACER_STR);
			player->ADD_GOSSIP_ITEM(5, u8"治疗强度+24", GOSSIP_SENDER_MAIN, BRACER_HEAL);
			player->ADD_GOSSIP_ITEM(5, u8"智力+7", GOSSIP_SENDER_MAIN, BRACER_INT);
			break;
		case _EQUIPMENT_SLOT_HANDS:
			player->ADD_GOSSIP_ITEM(5, u8"敏捷+15", GOSSIP_SENDER_MAIN, GLOVES_AGI);
			player->ADD_GOSSIP_ITEM(5, u8"火焰伤害+20", GOSSIP_SENDER_MAIN, GLOVES_FIRE);
			player->ADD_GOSSIP_ITEM(5, u8"冰霜伤害+20", GOSSIP_SENDER_MAIN, GLOVES_FROST);
			player->ADD_GOSSIP_ITEM(5, u8"暗影伤害+20", GOSSIP_SENDER_MAIN, GLOVES_SHADOW);
			player->ADD_GOSSIP_ITEM(5, u8"治疗强度+30", GOSSIP_SENDER_MAIN, GLOVES_HEALING);
			break;
		case _EQUIPMENT_SLOT_FEET:
			player->ADD_GOSSIP_ITEM(5, u8"耐力+7", GOSSIP_SENDER_MAIN, BOOTS_STAM);
			player->ADD_GOSSIP_ITEM(5, u8"微略提高移动速度", GOSSIP_SENDER_MAIN, BOOTS_SPEED);
			player->ADD_GOSSIP_ITEM(5, u8"敏捷+5", GOSSIP_SENDER_MAIN, BOOTS_AGI);
			break;
		case _EQUIPMENT_SLOT_MAINHAND:
			player->ADD_GOSSIP_ITEM(5, u8"十字军", GOSSIP_SENDER_MAIN, WEP_CRUSADER);
			player->ADD_GOSSIP_ITEM(5, u8"单手敏捷+15", GOSSIP_SENDER_MAIN, WEP1H_AGILITY);
			player->ADD_GOSSIP_ITEM(5, u8"双手敏捷+25", GOSSIP_SENDER_MAIN, WEP2H_AGILITY);
			player->ADD_GOSSIP_ITEM(5, u8"法术强度+30", GOSSIP_SENDER_MAIN, WEP_SPELLPOWER);
			player->ADD_GOSSIP_ITEM(5, u8"治疗强度+55", GOSSIP_SENDER_MAIN, WEP_HEAL);
			break;
		case _EQUIPMENT_SLOT_OFFHAND:
			player->ADD_GOSSIP_ITEM(5, u8"精神+9", GOSSIP_SENDER_MAIN, OFFHAND_SPIRIT);
			player->ADD_GOSSIP_ITEM(5, u8"耐力+7", GOSSIP_SENDER_MAIN, OFFHAND_STAM);
			player->ADD_GOSSIP_ITEM(5, u8"冰霜抗性+8", GOSSIP_SENDER_MAIN, OFFHAND_FROSTRES);
			break;
		}
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, objectGuid);
	}
	else
	{
		Item* item = nullptr;
		uint32 id = 0;
		switch (action)
		{
		case WEP2H_SUPERIOR_IMPACT:
		case WEP2H_AGILITY:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
			if (item && (action == WEP2H_AGILITY || action == WEP2H_SUPERIOR_IMPACT))
			{
				if (item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 && item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_MACE2
					&& item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 && item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM
					&& item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
				{
					PSendSysMessage(player, u8"需要双手武器");
					player->CLOSE_GOSSIP_MENU();
					return true;
				}
			}
			if (action == WEP2H_SUPERIOR_IMPACT)
				id = 1896;
			else if (action == WEP2H_AGILITY)
				id = 2646;
			break;

		case WEP_CRUSADER:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
			id = 1900;
			break;
		case WEP1H_AGILITY:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
			id = 2564;
			break;
		case WEP_SPELLPOWER:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
			id = 2504;
			break;
		case WEP_HEAL:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
			id = 2505;
			break;

		case OFFHAND_SPIRIT:
		case OFFHAND_STAM:
		case OFFHAND_FROSTRES:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
			if (item && item->GetProto()->SubClass != ITEM_SUBCLASS_ARMOR_SHIELD)
			{
				PSendSysMessage(player, u8"需要盾牌");
				player->CLOSE_GOSSIP_MENU();
				return true;
			}
			if (action == OFFHAND_SPIRIT)
				id = 1890;
			else if (action == OFFHAND_FROSTRES)
				id = 926;
			else if (action == OFFHAND_STAM)
				id = 929;
			break;
		case CHEST_STATS:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST);
			id = 1891;
			break;
		case CLOAK_DODGE:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
			id = 2622;
			break;
		case CLOAK_SUB:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
			id = 2621;
			break;
		case CLOAK_ARMOR:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
			id = 1889;
			break;
		case CLOAK_AGILITY:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
			id = 849;
			break;
		case BRACER_STAM:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
			id = 1886;
			break;
		case BRACER_STR:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
			id = 1885;
			break;
		case BRACER_HEAL:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
			id = 2566;
			break;
		case BRACER_INT:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
			id = 1883;
			break;
		case GLOVES_AGI:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
			id = 2564;
			break;
		case GLOVES_FIRE:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
			id = 2616;
			break;
		case GLOVES_FROST:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
			id = 2615;
			break;
		case GLOVES_SHADOW:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
			id = 2614;
			break;
		case GLOVES_HEALING:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
			id = 2617;
			break;
		case BOOTS_AGI:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
			id = 904;
			break;
		case BOOTS_SPEED:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
			id = 911;
			break;
		case BOOTS_STAM:
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
			id = 929;
			break;
		}
		Enchant(player, item, id);
		player->CLOSE_GOSSIP_MENU();
	}
	return true;
}


void Menu_Teleport(Player *player, const ObjectGuid guid, const uint32 action)
{
	switch (action)
	{
	case 10001: // Cities [HORDE]
		player->ADD_GOSSIP_ITEM(5, u8"奥格瑞玛", GOSSIP_SENDER_MAIN, 20);
		player->ADD_GOSSIP_ITEM(5, u8"幽暗城", GOSSIP_SENDER_MAIN, 21);
		player->ADD_GOSSIP_ITEM(5, u8"雷霆崖", GOSSIP_SENDER_MAIN, 22);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 10002: // Cities [ALLIANCE]
		player->ADD_GOSSIP_ITEM(5, u8"暴风城", GOSSIP_SENDER_MAIN, 23);
		player->ADD_GOSSIP_ITEM(5, u8"铁炉堡", GOSSIP_SENDER_MAIN, 24);
		player->ADD_GOSSIP_ITEM(5, u8"达纳苏斯", GOSSIP_SENDER_MAIN, 25);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 10003: // Starting Places [HORDE]

		//player->ADD_GOSSIP_ITEM(5, u8"亡灵出生地", GOSSIP_SENDER_MAIN, 40);
		player->ADD_GOSSIP_ITEM(5, u8"塔纳利斯", GOSSIP_SENDER_MAIN, 41);
		player->ADD_GOSSIP_ITEM(5, u8"荆棘谷", GOSSIP_SENDER_MAIN, 42);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 10004: // Starting Places [ALLIANCE]
		player->ADD_GOSSIP_ITEM(5, u8"塔纳利斯", GOSSIP_SENDER_MAIN, 43);
		player->ADD_GOSSIP_ITEM(5, u8"荆棘谷", GOSSIP_SENDER_MAIN, 44);
		//player->ADD_GOSSIP_ITEM(5, u8"暗夜精灵出生地", GOSSIP_SENDER_MAIN, 45);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 10005: // Instances [PAGE 1]
		player->ADD_GOSSIP_ITEM(5, u8"哀嚎洞穴", GOSSIP_SENDER_MAIN, 1249);
		player->ADD_GOSSIP_ITEM(5, u8"死亡矿井", GOSSIP_SENDER_MAIN, 1250);
		player->ADD_GOSSIP_ITEM(5, u8"影牙城堡", GOSSIP_SENDER_MAIN, 1251);
		player->ADD_GOSSIP_ITEM(5, u8"黑暗深渊", GOSSIP_SENDER_MAIN, 1252);
		player->ADD_GOSSIP_ITEM(5, u8"剃刀沼泽", GOSSIP_SENDER_MAIN, 1254);
		player->ADD_GOSSIP_ITEM(5, u8"剃刀高地", GOSSIP_SENDER_MAIN, 1256);
		player->ADD_GOSSIP_ITEM(5, u8"血色修道院", GOSSIP_SENDER_MAIN, 1257);
		player->ADD_GOSSIP_ITEM(7, u8"[更多] ->", GOSSIP_SENDER_MAIN, 5551);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 5551: // Instances [PAGE 2]
		player->ADD_GOSSIP_ITEM(5, u8"奥达曼", GOSSIP_SENDER_MAIN, 1258);
		player->ADD_GOSSIP_ITEM(5, u8"祖尔法拉克", GOSSIP_SENDER_MAIN, 1259);
		player->ADD_GOSSIP_ITEM(5, u8"玛拉顿", GOSSIP_SENDER_MAIN, 1260);
		player->ADD_GOSSIP_ITEM(5, u8"沉默的神庙", GOSSIP_SENDER_MAIN, 1261);
		player->ADD_GOSSIP_ITEM(5, u8"黑石深渊", GOSSIP_SENDER_MAIN, 1262);
		player->ADD_GOSSIP_ITEM(5, u8"厄运之槌", GOSSIP_SENDER_MAIN, 1263);
		player->ADD_GOSSIP_ITEM(5, u8"黑石塔", GOSSIP_SENDER_MAIN, 1264);
		player->ADD_GOSSIP_ITEM(5, u8"斯坦索姆", GOSSIP_SENDER_MAIN, 1265);
		player->ADD_GOSSIP_ITEM(5, u8"通灵学院", GOSSIP_SENDER_MAIN, 1266);
		player->ADD_GOSSIP_ITEM(7, u8"返回", GOSSIP_SENDER_MAIN, 10005);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 101: // Raids
		player->ADD_GOSSIP_ITEM(5, u8"祖尔格拉布", GOSSIP_SENDER_MAIN, 4000);
		player->ADD_GOSSIP_ITEM(5, u8"奥妮克希亚的巢穴", GOSSIP_SENDER_MAIN, 4001);
		player->ADD_GOSSIP_ITEM(5, u8"熔火之心", GOSSIP_SENDER_MAIN, 4002);
		player->ADD_GOSSIP_ITEM(5, u8"黑翼之巢", GOSSIP_SENDER_MAIN, 4003);
		player->ADD_GOSSIP_ITEM(5, u8"安其拉废墟", GOSSIP_SENDER_MAIN, 4004);
		player->ADD_GOSSIP_ITEM(5, u8"安其拉神庙", GOSSIP_SENDER_MAIN, 4005);
		player->ADD_GOSSIP_ITEM(5, u8"纳克萨玛斯", GOSSIP_SENDER_MAIN, 4006);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 6010: // Kalimdor
		player->ADD_GOSSIP_ITEM(5, u8"灰谷", GOSSIP_SENDER_MAIN, 601);
		player->ADD_GOSSIP_ITEM(5, u8"黑海岸", GOSSIP_SENDER_MAIN, 602);
		player->ADD_GOSSIP_ITEM(5, u8"凄凉之地", GOSSIP_SENDER_MAIN, 603);
		player->ADD_GOSSIP_ITEM(5, u8"杜隆塔尔", GOSSIP_SENDER_MAIN, 604);
		player->ADD_GOSSIP_ITEM(5, u8"尘泥池沼", GOSSIP_SENDER_MAIN, 605);
		player->ADD_GOSSIP_ITEM(5, u8"菲拉斯", GOSSIP_SENDER_MAIN, 606);
		player->ADD_GOSSIP_ITEM(5, u8"希利苏斯", GOSSIP_SENDER_MAIN, 607);
		player->ADD_GOSSIP_ITEM(5, u8"石爪山脉", GOSSIP_SENDER_MAIN, 608);
		player->ADD_GOSSIP_ITEM(5, u8"塔纳利斯", GOSSIP_SENDER_MAIN, 609);
		player->ADD_GOSSIP_ITEM(5, u8"贫瘠之地", GOSSIP_SENDER_MAIN, 610);
		player->ADD_GOSSIP_ITEM(5, u8"千针石林", GOSSIP_SENDER_MAIN, 611);
		player->ADD_GOSSIP_ITEM(5, u8"冬泉谷", GOSSIP_SENDER_MAIN, 612);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 6020: // Eastern Kingdoms
		player->ADD_GOSSIP_ITEM(5, u8"阿拉希高地", GOSSIP_SENDER_MAIN, 613);
		player->ADD_GOSSIP_ITEM(5, u8"荒芜之地", GOSSIP_SENDER_MAIN, 614);
		player->ADD_GOSSIP_ITEM(5, u8"丹莫罗", GOSSIP_SENDER_MAIN, 615);
		player->ADD_GOSSIP_ITEM(5, u8"暮色森林", GOSSIP_SENDER_MAIN, 616);
		player->ADD_GOSSIP_ITEM(5, u8"东瘟疫之地", GOSSIP_SENDER_MAIN, 617);
		player->ADD_GOSSIP_ITEM(5, u8"艾尔文森林", GOSSIP_SENDER_MAIN, 618);
		player->ADD_GOSSIP_ITEM(5, u8"希尔斯布莱德丘陵", GOSSIP_SENDER_MAIN, 619);
		player->ADD_GOSSIP_ITEM(5, u8"赤脊山", GOSSIP_SENDER_MAIN, 620);
		//player->ADD_GOSSIP_ITEM(5, u8"银松森林", GOSSIP_SENDER_MAIN, 621);
		player->ADD_GOSSIP_ITEM(5, u8"荆棘谷", GOSSIP_SENDER_MAIN, 622);
		player->ADD_GOSSIP_ITEM(5, u8"悲伤沼泽", GOSSIP_SENDER_MAIN, 623);
		player->ADD_GOSSIP_ITEM(5, u8"辛特兰", GOSSIP_SENDER_MAIN, 624);
		player->ADD_GOSSIP_ITEM(5, u8"提瑞斯法林地", GOSSIP_SENDER_MAIN, 625);
		player->ADD_GOSSIP_ITEM(5, u8"西部荒野", GOSSIP_SENDER_MAIN, 626);
		player->ADD_GOSSIP_ITEM(5, u8"湿地", GOSSIP_SENDER_MAIN, 627);
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;



		// ----### CITIES ###----

		// ### HORDE ###

	case 20: // Orgrimmar
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 1541.0f, -4426.0f, 11.24f, 0.85f);
		break;
	case 21: // Undercity
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 1822.0999f, 238.638855f, 60.694809f, 0.0f);
		break;
	case 22: // Thunderbluff
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -1272.703735f, 116.886490f, 131.016861f, 0.0f);
		break;

		// ### ALLIANCE ###

	case 23: // Stormwind
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -8828.231445f, 627.927490f, 94.055664f, 0.0f);
		break;
	case 24: // Ironforge
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -4917.0f, -955.0f, 502.0f, 0.0f);
		break;
	case 25: // Darnassus
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 9962.712891f, 2280.142822f, 1341.394409f, 0.0f);
		break;

		// ----### STARTING PLACES ####----

		// ### HORDE ###
	case 40: // Shadow Grave
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 1663.517f, 1678.187744f, 120.5303f, 0.0f);
		break;
	case 41: // Valley of Trials
		player->CLOSE_GOSSIP_MENU();
		//player->TeleportTo(1, -602.1253f, -4262.4208f, 38.956341f, 0.0f);
		player->TeleportTo(1, -7177.8f, -3769.0f, 8.5f, 0.0f);
		break;
	case 42: // Camp Narache
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -14289.2f, 545.9f, 8.85f, 0.0f);
		//player->TeleportTo(1, -2914.16992f, -266.061798f, 53.658211f, 0.0f);
		break;

		// ### ALLIANCE ###

	case 43: // Nortshire Valley
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -7177.8f, -3769.0f, 8.5f, 0.0f);
		//player->TeleportTo(1, -7177.8f, -3769.0f, 8.5f, 0.0f);
		//player->TeleportTo(0, -8943.133789f, -132.934921f, 83.704269f, 0.0f);
		break;
	case 44: // Coldridge Valley
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -14289.2f, 545.9f, 8.85f, 0.0f);
		//player->TeleportTo(0, -6231.106445f, 332.270477f, 383.153931f, 0.0f);
		break;
	case 45: // Shadowglen
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 10329.918945f, 833.500305f, 1326.260620f, 0.0f);
		break;

		// ----### INSTANCES ###----

	case 50: // Dire Maul
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -3762.340820f, 1216.537598f, 127.434608f, 0.0f);
		break;
	case 51: // Blackrock Spire
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -7528.554688f, -1222.907227f, 285.732941f, 0.0f);
		break;
	case 52: // Zul'Gurub
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -11916.179688f, -1190.977051f, 85.137901f, 0.0f);
		break;
	case 53: // Onyxia's Lair
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -4682.391602f, -3709.857422f, 46.792862f, 0.0f);
		break;
	case 54: // Searing Gorge (Moltencore, Blackwinglair)
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -7342.270020f, -1096.863892f, 277.06930f, 0.0f);
		break;
	case 55: // Naxxramas
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 3121.061768f, -3689.973389f, 133.458786f, 0.0f);
		break;
	case 56: // Stratholme Backdoor Entrance
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 3160.416992f, -4038.750244f, 104.177376f, 0.0f);
		break;
	case 57: // Stratholme Main Entrance
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 3357.214355f, -3379.713135f, 144.780853f, 0.0f);
		break;
	case 58: // Scholomance
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 1252.319824f, -2587.151123f, 92.886772f, 0.0f);
		break;

		// ---### ZONES ###---

	case 70: // Silithus
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -7430.070313f, 1002.554565f, 1.249787f, 0.0f);
		break;
	case 71: // Durotar
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 1006.426819f, -4439.258789f, 11.352882f, 0.0f);
		break;
	case 72: // Ashenvale
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 1922.842651f, -2169.429688f, 94.327400f, 0.0f);
		break;
	case 73: // Tanaris
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -7180.401367f, -3773.328369f, 8.728320f, 0.0f);
		break;
	case 74: // Pretbc Mall
			 //player->CLOSE_GOSSIP_MENU();
			 //player->TeleportTo(1, 16201.107422f, 16205.1875f, 0.140072f, 1.630427f);
		break;
	case 4015:// Gurubashi
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -13261.3f, 168.294f, 35.0792f, 1.00688f);
		break;
	case 4017:// Razor Hill
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 315.721f, -4743.4f, 10.4867f, 0.0f);
		break;
	case 4018:// Goldshire
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -9464.0f, 62.0f, 56.0f, 0.0f);
		break;
	case 1249://teleport player to the Wailing Caverns
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -722.53f, -2226.30f, 16.94f, 2.71f);
		break;
	case 1250://teleport player to the Deadmines
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -11212.04f, 1658.58f, 25.67f, 1.45f);
		break;
	case 1251://teleport player to Shadowfang Keep
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -254.47f, 1524.68f, 76.89f, 1.56f);
		break;
	case 1252://teleport player to Blackfathom Deeps
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 4254.58f, 664.74f, -29.04f, 1.97f);
		break;
	case 1254://teleport player to Razorfen Kraul
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -4484.04f, -1739.40f, 86.47f, 1.23f);
		break;
	case 1256://teleport player to Razorfen Downs
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -4645.08f, -2470.85f, 85.53f, 4.39f);
		break;
	case 1257://teleport player to the Scarlet Monastery
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 2843.89f, -693.74f, 139.32f, 5.11f);
		break;
	case 1258://teleport player to Uldaman
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -6119.70f, -2957.30f, 204.11f, 0.03f);
		break;
	case 1259://teleport player to Zul'Farrak
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -6839.39f, -2911.03f, 8.87f, 0.41f);
		break;
	case 1260://teleport player to Maraudon
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -1433.33f, 2955.34f, 96.21f, 4.82f);
		break;
	case 1261://teleport player to the Sunken Temple
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -10346.92f, -3851.90f, -43.41f, 6.09f);
		break;
	case 1262://teleport player to Blackrock Depths
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -7301.03f, -913.19f, 165.37f, 0.08f);
		break;
	case 1263://teleport player to Dire Maul
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -3982.47f, 1127.79f, 161.02f, 0.05f);
		break;
	case 1264://teleport player to Blackrock Spire
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -7535.43f, -1212.04f, 285.45f, 5.29f);
		break;
	case 1265://teleport player to Stratholme
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 3263.54f, -3379.46f, 143.59f, 0.00f);
		break;
	case 1266://teleport player to Scholomance
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 1219.01f, -2604.66f, 85.61f, 0.50f);
		break;
	case 4000:// Teleport to Zul'Gurub
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -11916.7f, -1212.82f, 92.2868f, 4.6095f);
		break;
	case 4001:// Teleport to Onyxia's Lair
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -4707.44f, -3726.82f, 54.6723f, 3.8f);
		break;
	case 4002:// Teleport to Molten Core
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(230, 1121.451172f, -454.316772f, -101.329536f, 3.5f);
		break;
	case 4003:// Teleport to Blackwing Lair
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(469, -7665.55f, -1102.49f, 400.679f, 0.0f);
		break;
	case 4004:// Ruins of Ahn'Qiraj
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -8409.032227f, 1498.830933f, 27.361542f, 2.497567f);
		break;
	case 4005:// Temple of Ahn'Qiraj
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -8245.837891f, 1983.736206f, 129.071686f, 0.936195f);
		break;
	case 4006:// Naxxramas
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(533, 3005.87f, -3435.0f, 293.89f, 0.0f);
		break;
	case 601: // Kalimdor -> Ashenvale
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 3469.43f, 847.62f, 6.36476f, 0.0f);
		break;
	case 602: // Kalimdor -> Darkshore
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 6207.5f, -152.833f, 80.8185f, 0.0f);
		break;
	case 603: // Kalimdor -> Desolace
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -93.1614f, 1691.15f, 90.0649f, 0.0f);
		break;
	case 604: // Kalimdor -> Durotar
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 341.42f, -4684.7f, 31.9493f, 0.0f);
		break;
	case 605: // Kalimdor -> Duswallow Marsh
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -3463.26f, -4123.13f, 18.1043f, 0.0f);
		break;
	case 606: // Kalimdor -> Ferelas
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -4458.93f, 243.415f, 65.6136f, 0.0f);
		break;
	case 607: // Kalimdor -> Silithus
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -6824.15f, 821.273f, 50.6675f, 0.0f);
		break;
	case 608: // Kalimdor -> Stonetalon Mountains
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 1145.00f, 85.664f, -6.64f, 0.0f);
		break;
	case 609: // Kalimdor -> Tanaris
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -7373.69f, -2950.2f, 11.7598f, 0.0f);
		break;
	case 610: // Kalimdor -> The Barrens
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -456.263f, -2652.7f, 96.615f, 0.0f);
		break;
	case 611: // Kalimdor -> Thousand Needles
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -4941.66f, -1595.42f, -33.07f, 0.0f);
		break;
	case 612: // Kalimdor -> Winterspring
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, 6107.62f, -4181.6f, 853.322f, 0.0f);
		break;
	case 613: // Eastern Kingdoms -> Arathi Highlands
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -1544.93f, -2495.01f, 54.11f, 0.0f);
		break;
	case 614: // Eastern Kingdoms -> Badlands
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -6084.95f, -3328.44f, 253.76f, 0.0f);
		break;
	case 615: // Eastern Kingdoms -> Dun Morogh
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -5660.33f, 755.299f, 390.605f, 0.0f);
		break;
	case 616: // Eastern Kingdoms -> Duskwood
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -10879.85f, -327.06f, 37.78f, 0.0f);
		break;
	case 617: // Eastern Kingdoms -> Eastern Plaguelands
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 2280.12f, -5313.16f, 87.74f, 0.0f);
		break;
	case 618: // Eastern Kingdoms -> Elwynn Forest
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -9545.78f, -51.81f, 56.72f, 0.0f);
		break;
	case 619: // Eastern Kingdoms -> Hillsbrad Foothills
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -436.03f, -583.27f, 53.58f, 0.0f);
		break;
	case 620: // Eastern Kingdoms -> Reridge Mountains
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -9633.80f, -1875.09f, 68.39f, 0.0f);
		break;
	case 621: // Eastern Kingdoms -> Silverpine Forest
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 641.48f, 1297.84f, 85.45f, 0.0f);
		break;
	case 622: // Eastern Kingdoms -> Stranglethorn Vale
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -11393.77f, -288.94f, 58.99f, 0.0f);
		break;
	case 623: // Eastern Kingdoms -> Swamp of Sorrows
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -10522.55f, -3028.30f, 21.79f, 0.0f);
		break;
	case 624: // Eastern Kingdoms -> The Hinterlands
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 155.14f, -2014.95f, 122.55f, 0.0f);
		break;
	case 625: // Eastern Kingdoms -> Tirishfal Glades
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 2255.5f, 288.511f, 35.1138f, 0.0f);
		break;
	case 626: // Eastern Kingdoms -> Westfall
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -10919.09f, 998.47f, 35.47f, 0.0f);
		break;
	case 627: // Eastern Kingdoms -> Wetlands
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, -4086.36f, -2610.95f, 47.0143f, 0.0f);
		break;
	case 104:
		// HORDE
		if (player->GetTeam() == HORDE)
		{
			player->ADD_GOSSIP_ITEM(2, u8"传送-->主城", GOSSIP_SENDER_MAIN, 10001);
			player->ADD_GOSSIP_ITEM(2, u8"传送-->中立地(练级测试)", GOSSIP_SENDER_MAIN, 10003);
		}
		// ALLIANCE
		else
		{
			player->ADD_GOSSIP_ITEM(2, u8"传送-->主城", GOSSIP_SENDER_MAIN, 10002);
			player->ADD_GOSSIP_ITEM(2, u8"传送-->中立地(练级测试)", GOSSIP_SENDER_MAIN, 10004);
		}
		//if (player->IsGameMaster()|| player->HasItemCount(49999, 1)) {
		player->ADD_GOSSIP_ITEM(5, u8"传送-->副本", GOSSIP_SENDER_MAIN, 10005);
		player->ADD_GOSSIP_ITEM(5, u8"传送-->团队副本", GOSSIP_SENDER_MAIN, 101);
		player->ADD_GOSSIP_ITEM(5, u8"传送-->古巴拉什竞技场", GOSSIP_SENDER_MAIN, 4015);
		player->ADD_GOSSIP_ITEM(5, u8"传送-->地区-卡利姆多", GOSSIP_SENDER_MAIN, 6010);
		player->ADD_GOSSIP_ITEM(5, u8"传送-->地区-东部王国", GOSSIP_SENDER_MAIN, 6020);
		//}
		player->ADD_GOSSIP_ITEM(7, u8"<--返回主菜单", GOSSIP_SENDER_MAIN, 100);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 121:
	{
		ObjectGuid target_guid;
		if (player->GetLevel() < 40) {
			player->GiveLevel(40);
			player->InitTalentForLevel();
			player->SetUInt32Value(PLAYER_XP, 0);
			player->UpdateSkillsToMaxSkillsForLevel();
		}
		player->LearnSpell(33389, false);
		ItemPosCountVec dest;
		InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, (player->GetTeam() == HORDE) ? 15277 : 2411, 1);

		if (msg == EQUIP_ERR_OK)
		{
			Item* item = player->StoreNewItem(dest, (player->GetTeam() == HORDE) ? 15277 : 2411, true);
			player->SendNewItem(item, 1, true, false);
		}
		else {
			player->SendEquipError(msg, nullptr, nullptr, (player->GetTeam() == HORDE) ? 15277 : 2411);
			PSendSysMessage(player, u8"[系统消息]:请保持包包有足够空间");
		}
	}
	player->CLOSE_GOSSIP_MENU();
	break;
	case 122:
	{
		//player->LearnSpell(33389, false);
		ObjectGuid target_guid;
		if (player->GetLevel() < 60) {
			player->GiveLevel(60);
			player->InitTalentForLevel();
		}
		player->LearnSpell(33389, false);
		player->SetUInt32Value(PLAYER_XP, 0);
		player->UpdateSkillsToMaxSkillsForLevel();
		if (sPzxConfig.GetIntDefault("initItemSet", 1) <= 6) {
			addItemSet(player, sPzxConfig.GetIntDefault("initItemSet", 1));//增加T1套装
		}
		if (!player->HasSkill(129) || player->GetSkillValue(129) < 300) {
			player->SetSkill(129, 300, 300);//初始急救
			LearnSkillRecipesHelper(player, 129);
		}
	}
	player->CLOSE_GOSSIP_MENU();
	break;
	case 102:
	{ //世界BUFF菜单，需要creature
		try
		{

			if (sPzxConfig.GetIntDefault("show.combotBuff", 1) && player->IsInCombat()) {
				PSendSysMessage(player, u8"[系统消息]:战斗中禁止施放世界BUFF.");
				player->CLOSE_GOSSIP_MENU();
				break;
			}
			uint32 zone, area;
			player->GetZoneAndAreaId(zone, area);
			if (sPzxConfig.GetIntDefault("show.arrowCityBuff", 1) && zone == 1637 && area == 1637) {
				PSendSysMessage(player, u8"[系统消息]:奥格瑞玛区域内禁止施放世界BUFF.");
				player->CLOSE_GOSSIP_MENU();
				break;
			}
			if (sPzxConfig.GetIntDefault("show.arrowCityBuff", 1) && zone == 1537 && area == 1537) {
				PSendSysMessage(player, u8"[系统消息]:铁炉堡区域内禁止施放世界BUFF.");
				player->CLOSE_GOSSIP_MENU();
				break;
			}
			if (!player->CustomPlayerActionTimeCheck(sPzxConfig.GetIntDefault("timecool.fullbuff", 15), PLAYED_TIME_BUFF)) {
				//PSendSysMessage(player, u8"[系统消息]:技能冷却中.");
				player->CLOSE_GOSSIP_MENU();
				break;
			}
			/*
			25898 :124 = "强效王者祝福", 142 = "使团队或小队中所有与目标职业相同的玩家都获得强效王者祝福的效果，使其所有属性提高$s1%，持续$d。每个圣骑士在同一时间内只能给目标施加一种祝福，同类型的祝福不能重叠。", 151 = "所有属性提高$s1%。"
			21564 : 124 = "坚韧祷言", 133 = "等级 2", 142 = "圣洁的能量充满全队成员的身体，令其耐力提高$s1点，持续$d。", 151 = "耐力提高$s1点。"
			27683 : 124 = "暗影防护祷言", 133 = "等级 1", 142 = "使目标所在小队的所有成员的暗影抗性提高$s1点，持续$d。", 151 = "暗影抗性提高$s1点。"
			27681 : 124 = "精神祷言", 133 = "等级 1", 142 = "使目标所在小队的所有成员的精神提高$s1点，持续$d。", 151 = "精神提高$s1点。"
			21850 : 124 = "野性赐福", 133 = "等级 2", 142 = "使目标所在的队伍获得野性赐福的效果，护甲值提高$s1点，所有属性提高$s2点，所有抗性提高$s3点。持续$d。", 151 = "护甲提高$s1点，所有属性提高$s2点，所有抗性提高$s3点。"
			16877 : 124 = "荆棘术", 133 = "等级 6", 142 = "使友方目标身上环绕着荆棘，对任何使用近战手段击中被保护者的敌人造成$s1点自然伤害，持续$d。", 151 = "对攻击者造成$s1点自然伤害。"
			25289 :124="战斗怒吼",133="等级 7",142="战士发出怒吼，使半径$a1码范围内的所有小队成员的攻击强度提高$s1点，持续$d。",151="攻击强度提高$s1点。"
			23028  奥术光辉
			*/
			if (sPzxConfig.GetIntDefault("show.worldBuff2", 1) || player->IsGameMaster()) {
				player->CastSpell(player, 22888, true);
				player->CastSpell(player, 16609, true);
				player->CastSpell(player, 24425, true);
			}

			uint32 randomSpell = pubID[urand(0, 1)];
			bool find_palading = false;
			if (player->GetGroup() && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER)) {
				for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
				{
					Player* pl = itr->getSource();

					if (!pl || !pl->GetSession() || pl->IsInCombat())
						continue;
					if (pl->GetClass() == CLASS_PALADIN) {
						find_palading = true;
						break;
					}
				}
			}
			//如果是团长为全团增加buff
			if (player->GetGroup() && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER)) {
				for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
				{
					Player* pl = itr->getSource();
					if (!pl || !pl->GetSession() || pl->IsInCombat() || !pl->IsAlive() || player == pl || !player->IsWithinLOSInMap(pl)) {
						//需要目标存活、在视野内，而且不是团长自己
						continue;
					}
					if (pl->IsAlive()&&pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl)) {//可能野外也要用到，先不限制副本内使用

						if (sPzxConfig.GetIntDefault("show.morebuff", 1) || player->IsGameMaster()) {
							if(!pl->HasAura(21564))
							player->CastSpell(pl, 21564, true);
							if (!pl->HasAura(27683))
							player->CastSpell(pl, 27683, true);
							if (!pl->HasAura(21850))
							player->CastSpell(pl, 21850, true);
							//player->CastSpell(pl, 16877, true);
							if (pl->GetPowerType() == POWER_MANA&&pl->GetClass() != CLASS_HUNTER) {//蓝条职业
								if (!pl->HasAura(23028))
								player->CastSpell(pl, 23028, true);//奥术光辉
								if (!pl->HasAura(27681))
								player->CastSpell(pl, 27681, true);
							}
							else {
								if (!pl->HasAura(25289))
								player->CastSpell(pl, 25289, true);//战斗怒吼
							}
						}
						if (sPzxConfig.GetIntDefault("show.worldBuff1", 1) || player->IsGameMaster()) {
							if (!pl->HasAura(15366))
							player->CastSpell(pl, 15366, true);//风歌夜曲
							if (pl->GetPowerType() == POWER_MANA&&pl->GetClass() != CLASS_HUNTER) {//蓝条职业
								if (!pl->HasAura(22820))
								player->CastSpell(pl, 22820, true);//厄运三buff,3%致命
						
							}
							else {
								if (!pl->HasAura(22817))
								player->CastSpell(pl, 22817, true);//厄运三buff,强度200
							}
							if (find_palading) {
								player->CastSpell(pl, 25898, true);//队伍有QS，全团王者
							}
							if (!pl->HasAura(randomSpell))
								player->CastSpell(pl, randomSpell, true);//随机释放
							if (!pl->HasAura(22818))
							player->CastSpell(pl, 22818, true);//厄运三buff,耐力
						}
						//修改血量
						pl->SetHealth(pl->GetMaxHealth());
						if (pl->GetPowerType() == POWER_RAGE) {
							pl->SetPowerPercent(POWER_RAGE, 99.0f);
						}
						else if (pl->GetPowerType() == POWER_MANA) {
							pl->SetPowerPercent(POWER_MANA, 99.0f);
						}
					}
				}
			}
			//最后给自己加BUFF
			if (sPzxConfig.GetIntDefault("show.buffNOGroup", 1)) {//控制非团长使用功能
					if (sPzxConfig.GetIntDefault("show.morebuff", 1) || player->IsGameMaster()) {
						if (!player->HasAura(21564))
						player->CastSpell(player, 21564, true);
						if (!player->HasAura(27683))
						player->CastSpell(player, 27683, true);
						if (!player->HasAura(21850))
						player->CastSpell(player, 21850, true);
						//player->CastSpell(player, 16877, true);
						if (player->GetPowerType() == POWER_MANA&&player->GetClass() != CLASS_HUNTER) {//蓝条职业
							if (!player->HasAura(23028))
							player->CastSpell(player, 23028, true);//奥术光辉
							if (!player->HasAura(27681))
							player->CastSpell(player, 27681, true);
						}
						else {
							if (!player->HasAura(25289))
							player->CastSpell(player, 25289, true);//战斗怒吼
						}
					}

					if (sPzxConfig.GetIntDefault("show.worldBuff", 1) || player->IsGameMaster()) {
						if (!player->HasAura(15366))
						player->CastSpell(player, 15366, true);//风歌夜曲
						if (player->GetPowerType() == POWER_MANA&&player->GetClass() != CLASS_HUNTER) {//蓝条职业
							if (!player->HasAura(22820))
							player->CastSpell(player, 22820, true);//厄运三buff,3%致命
						}
						else {
							if (!player->HasAura(22817))
							player->CastSpell(player, 22817, true);//厄运三buff,强度200
						}
						if (!player->HasAura(randomSpell))
							player->CastSpell(player, randomSpell, true);//随机释放
						if (!player->HasAura(22818))
						player->CastSpell(player, 22818, true);//厄运三buff,耐力
					}
				}
				else {
					PSendSysMessage(player, u8"|cffff0000[系统消息]:|h|r高峰期此此功能禁用");
				}
	
			//加完BUFF 团长满蓝满血
				player->SetHealth(player->GetMaxHealth());
				if (player->GetPowerType() == POWER_RAGE) {
					player->SetPowerPercent(POWER_RAGE, 99.0f);
				}
				else if (player->GetPowerType() == POWER_MANA) {
					player->SetPowerPercent(POWER_MANA, 99.0f);
				}
			//PSendSysMessage(player, u8"[系统消息]:奔跑吧,勇士你已经足够强大了");
		}
		catch (...)
		{
			sLog.outError("[pzx-exception-SHE-for102]");
		}
	}

	player->CLOSE_GOSSIP_MENU();

	break;
	case 103: //武器技能
		player->UpdateSkillsToMaxSkillsForLevel();
		PSendSysMessage(player, u8"[系统消息]:您的所有武器技能熟练度已经提升.");
		player->CLOSE_GOSSIP_MENU();
		break;
	case 109://送T0
		addItemSet(player, 0);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 110://送T0.5
		addItemSet(player, 1);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 111://送T1
		addItemSet(player, 2);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 112://送T2
		addItemSet(player, 3);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 113://送T2.5
		addItemSet(player, 4);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 114://送zug
		addItemSet(player, 5);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 115://送T3
		addItemSet(player, 6);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 120://增加荣誉
		addRep(player, true);
		player->CLOSE_GOSSIP_MENU();
		break;
	case 105:
		player->ADD_GOSSIP_ITEM(5, u8"胸部", GOSSIP_SENDER_MAIN, _EQUIPMENT_SLOT_CHEST);
		player->ADD_GOSSIP_ITEM(5, u8"披风", GOSSIP_SENDER_MAIN, _EQUIPMENT_SLOT_BACK);
		player->ADD_GOSSIP_ITEM(5, u8"护腕", GOSSIP_SENDER_MAIN, _EQUIPMENT_SLOT_WRISTS);
		player->ADD_GOSSIP_ITEM(5, u8"手套", GOSSIP_SENDER_MAIN, _EQUIPMENT_SLOT_HANDS);
		player->ADD_GOSSIP_ITEM(5, u8"鞋子", GOSSIP_SENDER_MAIN, _EQUIPMENT_SLOT_FEET);
		player->ADD_GOSSIP_ITEM(5, u8"武器", GOSSIP_SENDER_MAIN, _EQUIPMENT_SLOT_MAINHAND);
		player->ADD_GOSSIP_ITEM(5, u8"盾牌", GOSSIP_SENDER_MAIN, _EQUIPMENT_SLOT_OFFHAND);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 106:
		player->ADD_GOSSIP_ITEM(6, u8"创建 <黑翼之巢> 新进度 |cff6247c8瓦拉斯塔兹|h|r", GOSSIP_SENDER_MAIN, 301);
		player->ADD_GOSSIP_ITEM(6, u8"创建 <黑翼之巢> 新进度 |cff6247c8勒西雷尔|h|r", GOSSIP_SENDER_MAIN, 302);
		player->ADD_GOSSIP_ITEM(6, u8"创建 <黑翼之巢> 新进度 |cff6247c8费尔默|h|r", GOSSIP_SENDER_MAIN, 303);
		player->ADD_GOSSIP_ITEM(6, u8"创建 <黑翼之巢> 新进度 |cff6247c8克洛玛古斯|h|r", GOSSIP_SENDER_MAIN, 304);
		player->ADD_GOSSIP_ITEM(6, u8"创建 <黑翼之巢> 新进度 |cff6247c8奈法利安|h|r", GOSSIP_SENDER_MAIN, 305);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;
	case 107:
		if (sPzxConfig.GetIntDefault("openT0", 1)) {
			player->ADD_GOSSIP_ITEM(0, u8"请送我|cffe31bd2T0套装|h|r", GOSSIP_SENDER_MAIN, 109);
		}
		if (sPzxConfig.GetIntDefault("openT0.5", 0)) {
			player->ADD_GOSSIP_ITEM(0, u8"请送我|cffe31bd2T0.5套装|h|r", GOSSIP_SENDER_MAIN, 110);
		}
		if (sPzxConfig.GetIntDefault("openT1", 1)) {
			player->ADD_GOSSIP_ITEM(0, u8"请送我|cffe31bd2T1套装|h|r", GOSSIP_SENDER_MAIN, 111);
		}
		if (sPzxConfig.GetIntDefault("openT2", 1)) {
			player->ADD_GOSSIP_ITEM(0, u8"请送我|cffe31bd2T2套装|h|r", GOSSIP_SENDER_MAIN, 112);
		}
		if (sPzxConfig.GetIntDefault("openT2.5", 1)) {
			player->ADD_GOSSIP_ITEM(0, u8"请送我|cffe31bd2T2.5套装|h|r", GOSSIP_SENDER_MAIN, 113);
		}
		if (sPzxConfig.GetIntDefault("openT3", 1)) {
			player->ADD_GOSSIP_ITEM(0, u8"请送我|cffe31bd2T3套装|h|r", GOSSIP_SENDER_MAIN, 115);
		}
		if (sPzxConfig.GetIntDefault("openZug", 1)) {
			player->ADD_GOSSIP_ITEM(0, u8"请送我|cffe31bd2祖尔格拉布套装|h|r", GOSSIP_SENDER_MAIN, 114);
		}

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, guid);
		break;

	case 108:
		if (player->GetGroup()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍中|h|r，请先脱离队伍.");
		}
		else {
			resetIntance(player, 469, true);
		}
		player->CLOSE_GOSSIP_MENU();
		break;
	case 301:
		if (player->GetGroup()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍中|h|r，请先脱离队伍.");
		}
		else {
			resetIntance(player, 469, true);
			char *buf = new char[100];
			sprintf(buf, "3 0 0 0 0 0 0 0 0 %d %d 0 0 0", urand(0, 19), urand(0, 19));
			saveInstance(player, 2, buf);
			delete[] buf;
			PSendSysMessage(player, u8"[系统消息]:您的 |cffff0000 <黑翼之巢> |h|r进度已经设置成 |cffe31bd2%s|h|r", u8"瓦拉斯塔兹");
		}
		player->CLOSE_GOSSIP_MENU();
		break;
	case 302:
		if (player->GetGroup()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍中|h|r，请先脱离队伍.");
		}
		else {
			resetIntance(player, 469, true);
			char *buf = new char[100];
			sprintf(buf, "3 3 0 0 0 0 0 0 3 %d %d 2 0 0", urand(0, 19), urand(0, 19));
			saveInstance(player, 3, buf);
			delete[] buf;
			PSendSysMessage(player, u8"[系统消息]:您的 |cffff0000 <黑翼之巢> |h|r进度已经设置成 |cffe31bd2%s|h|r", u8"勒什雷尔");
		}
		player->CLOSE_GOSSIP_MENU();
		break;
	case 303:
		if (player->GetGroup()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍中|h|r，请先脱离队伍.");
		}
		else {
			resetIntance(player, 469, true);
			char *buf = new char[100];
			sprintf(buf, "3 3 3 0 0 0 0 0 3 %d %d 2 0 0", urand(0, 19), urand(0, 19));
			saveInstance(player, 4, buf);
			delete[] buf;
			PSendSysMessage(player, u8"[系统消息]:您的 |cffff0000 <黑翼之巢> |h|r进度已经设置成 |cffe31bd2%s|h|r", u8"费尔默");
		}
		player->CLOSE_GOSSIP_MENU();
		break;
	case 304:
		if (player->GetGroup()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍中|h|r，请先脱离队伍.");
		}
		else {
			resetIntance(player, 469, true);
			char *buf = new char[100];
			sprintf(buf, "3 3 3 3 3 3 0 0 3 %d %d 2 0 0", urand(0, 19), urand(0, 19));
			saveInstance(player, 7, buf);
			delete[] buf;
			PSendSysMessage(player, u8"[系统消息]:您的 |cffff0000 <黑翼之巢> |h|r进度已经设置成 |cffe31bd2%s|h|r", u8"克洛玛古斯");
		}
		player->CLOSE_GOSSIP_MENU();
		break;

	case 305:
		if (player->GetGroup()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍中|h|r，请先脱离队伍.");
		}
		else {
			resetIntance(player, 469, true);
			char *buf = new char[100];
			sprintf(buf, "3 3 3 3 3 3 3 0 3 %d %d 2 0 0", urand(0, 19), urand(0, 19));
			saveInstance(player, 8, buf);
			delete[] buf;
			PSendSysMessage(player, u8"[系统消息]:您的 |cffff0000 <黑翼之巢> |h|r进度已经设置成 |cffe31bd2%s|h|r", u8"奈法利安");
		}
		player->CLOSE_GOSSIP_MENU();
		break;
	case 201:
		check(player, true); //学习职业技能
		player->CLOSE_GOSSIP_MENU();
		break;
	case 205:
		if (player->GetPet() && player->GetPet()->getPetType() == HUNTER_PET) {
			uint32 maxlevel = std::min(sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL), player->GetLevel());
			Pet* HunterPet = player->GetPet();
			if (HunterPet->GetLevel() < maxlevel || HunterPet->GetLoyaltyLevel() < LoyaltyLevel(BEST_FRIEND)) {

				//player->ADD_GOSSIP_ITEM(3, u8"提升 我的宠物忠诚度和等级 ", GOSSIP_SENDER_MAIN, 205);
				player->GetPet()->GivePetXP(99999999);
				player->GetPet()->ModifyLoyalty(1000000.0);
			}
			else {
				PSendSysMessage(player, u8"[系统消息]:|cff0000ff 您的宠物已经强化完成!|h|r");
			}
		}
		else {
			PSendSysMessage(player, u8"[系统消息]:请先|cffff0000 驯服或者召唤出|h|r一只要强化的宠物");
		}

		player->CLOSE_GOSSIP_MENU();
		break;

	case 100: // Main Menu
		MainMenu(player, guid);
		break;
	}
}
void MainMenu(Player *player, const ObjectGuid guid) //创建非creature相关的菜单
{
	//player->PrepareGossipMenu(_Creature, 0);
	if (sPzxConfig.GetIntDefault("openT", 1)) {
		player->ADD_GOSSIP_ITEM(7, u8"请送我一组|cff6247c8职业套装|h|r", GOSSIP_SENDER_MAIN, 107);
	}

	player->ADD_GOSSIP_ITEM(7, u8"-->|cff6247c8传送|h|r目的地", GOSSIP_SENDER_MAIN, 104);

	if (sPzxConfig.GetIntDefault("openEnt", 1)) {
		player->ADD_GOSSIP_ITEM(0, u8"请为我的装备|cff6247c8附魔|h|r", GOSSIP_SENDER_MAIN, 105);
	}
	if (sPzxConfig.GetIntDefault("openHonor", 1)) {
		if (!addRep(player, false)) {
			player->ADD_GOSSIP_ITEM(0, u8"提升奥山、阿拉希、战歌|cff6247c8声望和荣誉值|h|r", GOSSIP_SENDER_MAIN, 120);
		}
	}
	if (sPzxConfig.GetIntDefault("openWeap", 1)) {
		player->ADD_GOSSIP_ITEM(3, u8"提升我的|cff6247c8武器熟练度|h|r", GOSSIP_SENDER_MAIN, 103);
	}
	if (sPzxConfig.GetIntDefault("openPre", 0)) {
		if (player->GetLevel() >= 60 && !check(player, false)) {//暂定60级才能学习
			const char* getmenu = all[player->GetClass()].menuName;
			player->ADD_GOSSIP_ITEM(3, getmenu, GOSSIP_SENDER_MAIN, 201);//  职业菜单
		}
	}
	if (player->GetClass() == CLASS_HUNTER) {
		player->ADD_GOSSIP_ITEM(3, u8"提升 我的宠物|cff6247c8忠诚度和等级|h|r ", GOSSIP_SENDER_MAIN, 205);
	}
	if (sPzxConfig.GetIntDefault("openBWL", 1)) {
		player->ADD_GOSSIP_ITEM(7, u8"|cff6247c8创建|h|r <黑翼之巢> 新进度", GOSSIP_SENDER_MAIN, 106);
		player->ADD_GOSSIP_ITEM(6, u8"|cff6247c8重置|h|r团队副本  <黑翼之巢>  ", GOSSIP_SENDER_MAIN, 108);
	}
}



bool  ExtractUInt32(char** args, uint32& val) { return ExtractUInt32Base(args, val, 10); }
bool  ExtractUInt32Base(char** args, uint32& val, uint32 base)
{
	if (!*args || !**args)
		return false;

	char* tail = *args;

	unsigned long valRaw = strtoul(*args, &tail, base);

	if (tail != *args && isWhiteSpace(*tail))
		*(tail++) = '\0';
	else if (tail && *tail)                                 // some not whitespace symbol
		return false;                                       // args not modified and can be re-parsed

	if (valRaw > std::numeric_limits<uint32>::max())
		return false;

	// value successfully extracted
	val = uint32(valRaw);
	*args = tail;

	SkipWhiteSpaces(args);
	return true;
}
bool  ExtractOptUInt32(char** args, uint32& val, uint32 defVal)
{
	if (!*args || !**args)
	{
		val = defVal;
		return true;
	}

	return ExtractUInt32(args, val);
}
void SkipWhiteSpaces(char** args)
{
	if (!*args)
		return;

	while (isWhiteSpace(**args))
		++(*args);
}
bool  ExtractFloat(char** args, float& val)
{
	if (!*args || !**args)
		return false;

	char* tail = *args;

	double valRaw = strtod(*args, &tail);

	if (tail != *args && isWhiteSpace(*tail))
		*(tail++) = '\0';
	else if (tail && *tail)                                 // some not whitespace symbol
		return false;                                       // args not modified and can be re-parsed

															// value successfully extracted
	val = float(valRaw);
	*args = tail;

	SkipWhiteSpaces(args);
	return true;
}
void AddSC_zero_scripts()
{
	//AddSC_zero_creatures();
	AddSC_custom_creatures();
	AddSC_custom_items();

}