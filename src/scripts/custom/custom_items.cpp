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


#include "custom.h"
#include <regex>

const talentST  readTalents(Player *player) {
	uint32 cost_0 = 0;
	uint32 cost_1 = 0;
	uint32 cost_2 = 0;
	//vector<uint32> talent_spells;
	//ostringstream talentName;
	ostringstream spells;

	for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
	{
		TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);

		if (!talentInfo) continue;

		TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);

		if (!talentTabInfo)
			continue;

		// unlearn only talents for character class
		// some spell learned by one class as normal spells or know at creation but another class learn it as talent,
		// to prevent unexpected lost normal learned spell skip another class talents
		if ((player->GetClassMask() & talentTabInfo->ClassMask) == 0)
			continue;
		uint32 rankCount = 0;
		for (uint32 j : talentInfo->RankID)
		{
			rankCount++;
			//SpellEntry const* pInfos = sSpellMgr.GetSpellEntry(j);
			if (player->HasSpell(j)) {
				if (talentTabInfo->tabpage == 0) {
					cost_0 += rankCount;
				}
				else if (talentTabInfo->tabpage == 1) {
					cost_1 += rankCount;
				}
				else if (talentTabInfo->tabpage == 2) {
					cost_2 += rankCount;
				}
				spells << j << ",";
				//talent_spells.push_back(j);
			}

			//if (j)
			//	player->RemoveSpell(j, !Spells::IsPassiveSpell(j), false);
		}

		//保存后更换名称，并且替换缓存的天赋名称
		//切换后

	}
	const talentST pt = { player->GetGUIDLow(), cost_0, cost_1, cost_2,spells.str() };
	//PSendSysMessage(player, u8"当前天赋树信息:%d,%d,%d,", cost_0, cost_1, cost_2);
	return pt;
}
talentST readFromDB(Player *player, bool update)
{
	const talentST  currentTalent = readTalents(player);
	QueryResult* result = CharacterDatabase.PQuery("SELECT guid,t1,t2,t3,spells FROM tf where guid='%u'", player->GetGUIDLow());
	if (!result)
	{


		BarGoLink bar(1);
		bar.step();
		if (update)
			CharacterDatabase.PExecute("insert into tf values (%d,%d,%d,%d, '%s')", player->GetGUIDLow(), 0, 0, 0, "");

		sLog.outString();
		sLog.outString(">> Loaded 0 creature respawn time.");
		return{};
	}
	BarGoLink bar(result->GetRowCount());
	do
	{
		bar.step();
		Field* fields = result->Fetch();

		uint32 guid = fields[0].GetUInt32();
		uint32 t1 = fields[1].GetUInt32();
		uint32 t2 = fields[2].GetUInt32();
		uint32 t3 = fields[3].GetUInt32();
		string spells = fields[4].GetCppString();
		if (update) {
			CharacterDatabase.PExecute("update  tf set t1=%d,t2=%d,t3=%d,spells= '%s' where guid=%d", currentTalent.t1, currentTalent.t2, currentTalent.t3, currentTalent.spells.c_str(), player->GetGUIDLow());
			player->ResetTalents(true);
			player->CastSpell(player, 14867, true);//播放声音
			ChatHandler(player).SendSysMessage(LANG_RESET_TALENTS);
			//变更自己的天赋为旧天赋
			Tokens data = StrSplit(spells, ",");
			for (uint8 slot = 0; slot < data.size(); slot++)
			{
				uint32 item_id = GetUInt32ValueFromArray(data, slot);
				if (item_id > 0 && !player->HasSpell(item_id)) {
					player->LearnSpell(item_id, false, true);
				}
			}

		}
		const talentST pt = { player->GetGUIDLow(), t1, t2, t3,spells };
		return pt;
	} while (result->NextRow());
	//FIXME: remove pet before or after unlearn spells? for now after unlearn to allow removing of talent related, pet affecting auras
	//player->RemovePet(PET_SAVE_REAGENTS);
	return{};
}


bool GossipHello_ItemPzxManager(Player *player, Item *_item) {


	player->ADD_GOSSIP_ITEM(0, u8"复活拉团", GOSSIP_SENDER_MAIN, 404);
	if (sPzxConfig.GetIntDefault("open.fullbuff", 1) || player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"加满BUFF", GOSSIP_SENDER_MAIN, 102);
	}
	if (sPzxConfig.GetIntDefault("RiadTooldelbuff", 1)|| player->IsGameMaster()){
		if (sPzxConfig.GetIntDefault("show.allHiddenBuff", 1)) {//团长让全团隐形
			player->ADD_GOSSIP_ITEM(0, u8"全团隐身", GOSSIP_SENDER_MAIN, 410);
			player->ADD_GOSSIP_ITEM(0, u8"移除隐身", GOSSIP_SENDER_MAIN, 407);
		}
		if (sPzxConfig.GetIntDefault("show.cthunPostion", 0)) {//团长让全团隐形
			player->ADD_GOSSIP_ITEM(0, u8"分配全团到|cff6247c8克苏恩站位点|h|r--【分配站位专用】", GOSSIP_SENDER_MAIN, 412);
		}
		if (sPzxConfig.GetIntDefault("show.allKillBuff", 1)) {//团长让全团重新开始
			player->ADD_GOSSIP_ITEM(0, u8"秒杀全团", GOSSIP_SENDER_MAIN, 411);
		}
		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_cooldown", 0))
			player->ADD_GOSSIP_ITEM(0, u8"重置全团技能CD", GOSSIP_SENDER_MAIN, 413);

		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_15_add", 0)) 
		player->ADD_GOSSIP_ITEM(0, u8"减伤光环 |cff6247c8减伤15%|h|r", GOSSIP_SENDER_MAIN, 405);

		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_50_add", 0))
		player->ADD_GOSSIP_ITEM(0, u8"减伤光环 |cff6247c8减伤35%|h|r", GOSSIP_SENDER_MAIN, 406);

		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_15_del", 0))
		player->ADD_GOSSIP_ITEM(0, u8"为|cff6247c8目标|h|r增加护盾，使所受伤害|cff6247c8减少15%|h|r", GOSSIP_SENDER_MAIN, 408);
		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_50_del", 0))
		player->ADD_GOSSIP_ITEM(0, u8"为|cff6247c8目标|h|r增加护盾，使所受伤害|cff6247c8减少35%|h|r", GOSSIP_SENDER_MAIN, 409);

	}
	player->ADD_GOSSIP_ITEM(0, u8"原地复活 死后工会频道输入up回车", GOSSIP_SENDER_MAIN, 600);

	
	uint32 accID = player->GetSession()->GetAccountId();
	if (sPzxConfig.GetIntDefault("show.theendboss", 0) || player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8创建|h|r一个冰龙萨菲隆的进度", GOSSIP_SENDER_MAIN, 997);
		player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8创建|h|r一个克尔苏加德的进度", GOSSIP_SENDER_MAIN, 998);
		if(sPzxConfig.GetIntDefault("show.theendboss2", 0) || player->IsGameMaster()) {
			player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8创建|h|r一个守关BOSS(四骑士,电男,蘑菇男,迈克斯纳)的进度", GOSSIP_SENDER_MAIN, 996);
		}
	}

	if (sPzxConfig.GetIntDefault("RiadTooldExtMenu", 0)|| player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8创建|h|r一个安其拉的新进度", GOSSIP_SENDER_MAIN, 500);
		player->ADD_GOSSIP_ITEM(7, u8"-->|cff6247c8传送|h|r到目的地", GOSSIP_SENDER_MAIN, 104);
		//player->ADD_GOSSIP_ITEM(0, u8"武器随机幻化", GOSSIP_SENDER_MAIN, 888);
	}


	if (sPzxConfig.GetIntDefault("show.additem", 0)) {
		player->ADD_GOSSIP_ITEM_EXTENDED(6, u8"我需要一个物品<|cffff0000危险操作|h|r>|n|cff00f700   **使用说明**|h|r:在弹出框中输入|cff6247c8物品ID|h|r", GOSSIP_SENDER_MAIN, 777, "reload config 配置abc", true);
	}


	if (player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"重新加载服务器个性化配置", GOSSIP_SENDER_MAIN, 999);
	}
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
	return true;

}

static constexpr float eyeTentaclePositions[8][3] =
{
	{ -8547.269531f, 1986.939941f, 100.490351f },
	{ -8556.047852f, 2008.144653f, 100.598129f },
	{ -8577.246094f, 2016.939941f, 100.320351f },
	{ -8598.457031f, 2008.178467f, 100.320351f },
	{ -8607.269531f, 1986.987671f, 100.490351f },
	{ -8598.525391f, 1965.769043f, 100.490351f },
	{ -8577.340820f, 1956.940063f, 100.536636f },
	{ -8556.115234f, 1965.667725f, 100.598129f }
};

void telePlaytoPosition(Player *player,uint8 memberId) {

	const auto& eyeTentaclePosition = eyeTentaclePositions[memberId];
			float x = eyeTentaclePosition[0];
			float y = eyeTentaclePosition[1];
			float z = eyeTentaclePosition[2];
				uint8 indexid = memberId + 1;
				player->TeleportTo(player->GetMapId(), x, y, z + 2.0f, 0, TELE_TO_GM_MODE);
				PSendSysMessage(player, u8"玩家 |cffff0000[%s]|h|r 分配到了%d号触须刷新点", player->GetName(), indexid);
		
		
}

bool GossipHello_axnsItemManager(Player *player, Item *_item) {
	if (player->HasItemCount(50001, 40, false)) {//够40了
												 //WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
												 //data << player->GetObjectGuid();
												 //player->GetSession()->SendPacket(&data);
		player->ADD_GOSSIP_ITEM(0, u8"集齐40个【|cffff8000|Hitem:50001:0:0:0|h[埃辛诺斯的碎片]|h|r】即可兑换神器【|cffe6cc80|Hitem:18582:0:0:0|h[埃辛诺斯双刃]|h|r】", GOSSIP_SENDER_MAIN, 601);
	}
	else {//不够40个
		player->ADD_GOSSIP_ITEM(0, u8"碎片集齐40个再来兑换吧", GOSSIP_SENDER_MAIN, 602);
	}

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
	return false;
}

bool GossipSelect_axnsItemPzxManager(Player *player, Item *item, uint32 sender, const uint32 action, char const* reStr) {
	uint32 toadditems = 50001;
	try
	{
		if (action == 601) {
			player->DestroyItemCount(toadditems, 40, true, false, true);
			ItemPosCountVec dest;
			InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, toadditems,1);
			if (msg == EQUIP_ERR_OK)
			{
				//检查点数是否够
					uint32 accID = player->GetSession()->GetAccountId();
					Item* item = player->StoreNewItem(dest, toadditems, true);
					sLog.out(LOG_LOOTS, u8"Account %u guid %u : create 埃辛双刀  %u", accID, player->GetGUIDLow(), toadditems);
					player->SendNewItem(item, 1, true, false);
					ItemPrototype const* pProto = item->GetProto();
					ostringstream outs;
					//outputItemColorFormat(player, outs, toadditems, );
					ItemPrototype const* topProto = ObjectMgr::GetItemPrototype(toadditems);
					if (topProto) {
						const char * colorItemPre = ItemQualityColorsStr[topProto->Quality];
						outs << colorItemPre << topProto->Name1 << "|h|r";
						PSendSysMessage(player, u8"[系统消息]:%s已经添加到你包裹中", outs.str().c_str());
					}
					player->SaveToDB(true);//保存物品
			}
			else
			{
				player->SendEquipError(msg, nullptr, nullptr, 50001);
				PSendSysMessage(player, u8"[系统消息]:请检查包包空间");
			}
			player->CLOSE_GOSSIP_MENU();
			return true;
		}
		else if (action == 602) {
			return GossipHello_axnsItemManager(player, item);
		}

	}
	catch (...)
	{
		sLog.outError("[pzx-exception-SHE](%d)-[%s] action %d", player->GetGUIDLow(), player->GetName(), action);
		return false;
	}
}

bool GossipSelect_ItemPzxManager(Player *player, Item *item, uint32 sender, const uint32 action, char const* reStr) {
	try
	{
		//判断是否超期

		//查看数据库中是否有记录的时长
		//uint64 timeDur = sPzxMgr->getTimeLength(player->GetGUIDLow(), 49999);
		//if (timeDur == 0|| (timeDur + 60 * 60 * 24 * 30) < uint64(time(nullptr))){
		//	  //判断超期,摧毁物品，
		//		player->DestroyItemCount(49999, 1, true, false, true);
		//		sLog.out(LOG_LOOTS, u8"GUID %d 过期自动摧毁%d", player->GetGUIDLow(), 49999);
		//		PSendSysMessage(player, u8"您的团长工具已经过期,请重新购买");
		//		player->CLOSE_GOSSIP_MENU();
		//		return false;
		//}


		if (action == 777) {
		/*	if (!player->HasItemCount(sPzxConfig.GetIntDefault("vipItemID", 40003), 1, true)) {
				PSendSysMessage(player, u8"[系统消息]:需要VIP认证卡才可以使用本功能，请联系GM获取");
				player->CLOSE_GOSSIP_MENU();

				return false;
			}*/
			sLog.outString("[pzx] get Input str =%s", reStr);
			uint32 getItemID;
			try {
				getItemID = std::stoi(reStr);
				if (getItemID <= 0) {
					PSendSysMessage(player, u8"[系统消息]:请输入正确的物品ID");
					player->CLOSE_GOSSIP_MENU();

					return false;
				}
			}
			catch (...) {
				PSendSysMessage(player, u8"[系统消息]:请输入正确的物品ID");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}


			ItemPrototype const *pProto = sItemStorage.LookupEntry<ItemPrototype>(getItemID);
			if (pProto) {

				if (pProto->Quality < sPzxConfig.GetIntDefault("item.quality", ITEM_QUALITY_LEGENDARY)&& pProto->ItemLevel < sPzxConfig.GetIntDefault("item.level", 80))
				{
					if (player->HasItemCount(pProto->ItemId, 1, true)) {//已经有一件了
						PSendSysMessage(player, u8"[系统消息]:该物品唯一");
					}
					ItemPosCountVec dest;
					InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, pProto->ItemId, 1);
					if (msg == EQUIP_ERR_OK)
					{
						Item* item = player->StoreNewItem(dest, pProto->ItemId, true);
						player->SendNewItem(item, 1, true, false);
						PSendSysMessage(player, u8"[系统消息]:%s 已经添加到你包中", item->GetProto()->Name1);
					}
					else
					{
						player->SendEquipError(msg, nullptr, nullptr, pProto->ItemId);
						PSendSysMessage(player, u8"[系统消息]:请保持包包有足够空间");
					}
				}
				else {
					PSendSysMessage(player, u8"[系统消息]:获取物品等级过高，请联系GM");
				}
			}
			else
				PSendSysMessage(player, u8"[系统消息]:物品未找到");

			player->CLOSE_GOSSIP_MENU();
			return false;

		}
	
	sLog.outString("[pzx](%d)-[%s] action %d", player->GetGUIDLow(), player->GetName(), action);
	switch (action)
	{

	case 404://团长工具
	case 405:
	case 406:
	case 407:
	case 408:
	case 409://+减速buff
	case 410:
	case 411:
	case 412:
	case 413:
	{
		if (player->IsInCombat()&&action!= 411) {//杀死全团可以在战斗中使用
			PSendSysMessage(player, u8"|cffff0000[系统消息]:|h|r不可以在战斗中能使用此功能");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		if (!(player->GetGroup() && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER))) {
			PSendSysMessage(player, u8"|cffff0000[系统消息]:|h|r只有团队领袖才能使用此功能");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		if (action == 410&&sPzxConfig.GetIntDefault("forbidden.area", 0) && sPzxConfig.GetIntDefault("forbidden.area", 0) == player->GetMapId()) {
			PSendSysMessage(player, u8"[系统消息]:此区域内禁止开启特殊光环");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
	}
	break;
	default:
		break;
	}
	
	if (action <= 508 && action >= 501) {
		if (player->GetGroup() || player->GetMapRef()->IsDungeon()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍或者副本中|h|r，请先脱离队伍或离开副本区域.");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		uint32 id = action == 508 ? 4 : action - 501;//双子不带5狗换成ID=4
		//约定一些清理状态
		uint32 ins_satus = 0;
		if (action == 508) {
			ins_satus = 4;
		}
		resetIntance(player, 531, true);
		saveInstance(player, id, TAQINSTANCE_DATA[id], 531, ins_satus);
		
		PSendSysMessage(player, u8"您的TAQ新进度（%s）已经创建成功,可以进入副本了。", TAQINSTANCE_Names[id]);
		player->CLOSE_GOSSIP_MENU();
		return true;
	}
	switch (action)
	{
	case 600:
		return GossipHello_ItemPzxManager(player, item);
	case 500:
	{
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8吉祥三宝|h|r进度", GOSSIP_SENDER_MAIN, 501);
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8沙尔图拉|h|r进度", GOSSIP_SENDER_MAIN, 502);
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8范克瑞斯|h|r进度", GOSSIP_SENDER_MAIN, 503);
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8哈霍兰|h|r进度", GOSSIP_SENDER_MAIN, 504);
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8双子皇帝|h|r进度", GOSSIP_SENDER_MAIN, 505);
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8双子皇帝(不带五狗)|h|r进度", GOSSIP_SENDER_MAIN, 508);
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8奥罗|h|r进度", GOSSIP_SENDER_MAIN, 506);
		player->ADD_GOSSIP_ITEM(7, u8"创建|cff6247c8克苏恩|h|r进度", GOSSIP_SENDER_MAIN, 507);
		player->ADD_GOSSIP_ITEM(7, u8"-->|cff6247c8返回|h|r", GOSSIP_SENDER_MAIN, 100);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
		return true;
	}
	case 999:
		sPzxMgr->Reload();
		sPzxConfig.Reload();
		player->CLOSE_GOSSIP_MENU();
		break;
	case 996:
	case 997:
	case 998:
	{
		if (player->GetGroup() || player->GetMapRef()->IsDungeon()) {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经在队伍或者副本中|h|r，请先脱离队伍或离开副本区域.");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		resetIntance(player, 533, true);

		uint32 id = sMapMgr.GenerateInstanceId();
		uint32 count = 0;
		while (!CharacterDatabase.DirectPExecute("INSERT INTO instance VALUES ('%u', '%u', '%u', '%s')", id, 533, 0, action== 997?"3 3 3 3 3 3 3 3 3 3 3 3 3 0 0":(action == 998 ? "3 3 3 3 3 3 3 3 3 3 3 3 3 3 0":"3 3 0 3 3 0 3 3 0 3 3 3 0 0 0 "))) {
			count++;
			if (count > 100) {
				PSendSysMessage(player, u8"[系统消息]:|cffff0000系统繁忙，稍后再试|h|r");//防止死循环
				player->CLOSE_GOSSIP_MENU();
				return false;
			}
			id = sMapMgr.GenerateInstanceId();																		 
			continue;//插入成功为止
		}
		time_t mutetime = time(nullptr) + 60 * 48 * MINUTE;//2天后再刷新，一般公会不会连续开荒2天
		if (action == 997 || action == 998) {
			for (size_t i = 0; i < 16; i++)
			{
				CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", naxxbossIndex[i], uint64(mutetime), id, 533);
			}
		}else {//创建守关BOSS
			for (size_t i = 0; i < 9; i++)
			{
				CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", naxxbossDefind[i], uint64(mutetime), id, 533);
			}
		}

		MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(533);
		DungeonPersistentState *state = (DungeonPersistentState*)sMapPersistentStateMgr.AddPersistentState(mapEntry, id, uint64(mutetime), true);
		if (state) player->BindToInstance(state, true, false);
		sMapPersistentStateMgr.LoadCreatureRespawnTimes();
		sMapPersistentStateMgr.LoadGameobjectRespawnTimes();
		PSendSysMessage(player, action == 997?(u8"您的冰龙进度已经创建成功,可以进入副本了。"):(u8"您的老克进度已经创建成功,可以进入副本了。"));
		player->CLOSE_GOSSIP_MENU();
	}
		break;
	case 888://幻化武器为风剑
	{

		if (player->IsTwoHandUsed()) {
			player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + EQUIPMENT_SLOT_MAINHAND * MAX_VISIBLE_ITEM_OFFSET, WeaponDisplay_id2[urand(0, 3)]);// 随机释放
		}
		else {
			if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
				player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + EQUIPMENT_SLOT_MAINHAND * MAX_VISIBLE_ITEM_OFFSET, WeaponDisplay_id1[urand(0, 3)]);// 随机释放
			if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
				player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + EQUIPMENT_SLOT_OFFHAND * MAX_VISIBLE_ITEM_OFFSET, WeaponDisplay_id1[urand(0, 3)]);// 随机释放
		}
		PSendSysMessage(player, u8"您的武器幻化完成");
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 405:
	case 406:
	case 407:
	case 413:
	{
		if (action == 405 || action == 406) { //NAXX禁止免伤和隐身
			//uint32 zone, area;
			//player->GetZoneAndAreaId(zone, area);
			if (sPzxConfig.GetIntDefault("forbidden.area", 0)&&sPzxConfig.GetIntDefault("forbidden.area", 0) == player->GetMapId()) {
				PSendSysMessage(player, u8"[系统消息]:此区域内禁止开启特殊光环");
				player->CLOSE_GOSSIP_MENU();
				break;
			}
		}
		if (action == 413&&!player->CustomPlayerActionTimeCheck(sPzxConfig.GetIntDefault("timecool.coolspell", 30), PLAYED_TIME_COOL)) {
			//PSendSysMessage(player, u8"[系统消息]:技能冷却中.");
			player->CLOSE_GOSSIP_MENU();
			break;
		}
		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
			{
				Player* pl = itr->getSource();

				if (!pl || !pl->GetSession())
					continue;
				if (pl->IsAlive()) {
					if (action == 405 || action == 406) {//添加隐身光环
						uint32 tocheckSpell = action == 405 ? 8299 : 21751;
						if(!pl->HasAura(tocheckSpell))
							pl->CastSpell(pl, tocheckSpell, true);//21751  减伤50%
					}
					else if (action == 413) {//冷却所有技能
						if (pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl)) {
							pl->RemoveAllSpellCooldown();
							pl->CastSpell(pl, 14867, true);//播放声音
						}else {
							//PSendSysMessage(player,  u8"您的团队成员|cffff0000%s|h|r不在视野范围内",pl->GetName());
						}
					}else{//移除隐身 action=407

						if (pl->HasAura(21079))
							pl->RemoveAurasDueToSpell(21079);
					}
				}
			}

		PSendSysMessage(player, action == 407 ? u8"[|cffff0000系统消息|h|r]:您的全团成员已经|cffff0000移除护盾|h|r" : (action == 413 ? u8"[|cffff0000系统消息|h|r]::您的全团成员已经 |cff0000ff重置完技能冷却时间 |h|r": u8"[|cffff0000系统消息|h|r]::您的全团成员已经|cff0000ff增加护盾|h|r"));
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 408:
	case 409://指定成员增加减伤BUFF
	{
		if (player->GetSelectedPlayer()) {

				for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
				{
					Player* pl = itr->getSource();

					if (!pl || !pl->GetSession())
						continue;
					if (pl->IsAlive() && player->GetSelectedPlayer() == pl) {
						pl->CastSpell(pl, action == 408 ? 8299 : 21751, true);//21751  减伤50%
						PSendSysMessage(player, u8"您已经为成员cff00ff00%sh|r增加护盾%s--", pl->GetName(), (action == 408) ? u8"减免15%伤害" : u8"减免35%伤害");
						break;
					}
				}
			}
			else {
				PSendSysMessage(player, u8"[|cffff0000系统消息|h|r]:请先选定一个团队成员");
			}

		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 410://分配站位
	{

		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
		{
			Player* pl = itr->getSource();

		
			if (!pl || !pl->GetSession()|| pl->IsInCombat()|| !pl->IsAlive()) {
				continue;
			}
			
			if (pl == player) {
				if(!pl->HasAura(21079))
					pl->CastSpell(pl, 21079, false);//阿克蒙德的回响
				continue;
			}
			if (pl->IsAlive() && pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl)) {
				//小于50码而且在同一地图，不召唤
				if (!pl->HasAura(21079))
					pl->CastSpell(pl, 21079, false);//阿克蒙德的回响
			}
			else {
				//PSendSysMessage(player, u8"团队成员[|cffff0000%s|h|r] 不在视野中", pl->GetName());
			}
		}
		PSendSysMessage(player, u8"[|cffff0000系统消息|h|r]:您的全团成员已经|cff00ff00增加隐身光环|h|r");
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 412://分配克苏恩的站位
	{
		//-8577.26 1986.93 100.4
		uint32 zone, area;
		player->GetZoneAndAreaId(zone, area);
		if (zone != 3428) {
			PSendSysMessage(player, u8"需要在安其拉神殿副本内才能使用此功能");
			player->CLOSE_GOSSIP_MENU();
			break;
		}
		if(Creature* pCreature = player->FindNearestCreature(15727, 90.0f, true)) {

		}
		else {
			PSendSysMessage(player, u8"需要在克苏恩房间附近才能使用此功能");
			player->CLOSE_GOSSIP_MENU();
			break;
		}

		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
		{
			Player* pl = itr->getSource();


			if (!pl || !pl->GetSession()) {
				continue;
			}
			if (pl->IsInCombat()) {
				PSendSysMessage(player, u8"团队成员[|cffff0000%s|h|r]正在战斗中", pl->GetName());
				continue;
			}
			if (!pl->IsAlive()) {
				continue;
			}
			uint8 memberId=	player->GetGroup()->GetMemberGroup(pl->GetGUID());
			if (pl == player) {
				pl->CastSpell(pl, 21079, false);//阿克蒙德的回响
				telePlaytoPosition(pl, memberId);
				continue;
			}

			if (pl->IsAlive() && pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl)) {
				//小于50码而且在同一地图，不召唤
				pl->CastSpell(pl, 21079, false);//阿克蒙德的回响
				telePlaytoPosition(pl, memberId);
			}
			else {
				PSendSysMessage(player, u8"团队成员[|cffff0000%s|h|r] 不在视野中", pl->GetName());
			}
		}
		PSendSysMessage(player, u8"您的全团成员已经分配完|cff00ff00克苏恩站位|h|r");
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 411://杀死玩家
	{
		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
		{
			Player* pl = itr->getSource();


			if (!pl || !pl->GetSession())
				continue;
			/*if (pl->IsInCombat()) {
				PSendSysMessage(player, u8"团队成员[|cffff0000%s|h|r]正在战斗中", pl->GetName());
				continue;
			}*/
			if (pl->IsAlive() &&pl->GetGroup()->IsMember(player->GetGUIDLow())&& pl->GetMap()&&pl->GetMap()->IsDungeon()) {
				//杀死副本内所有玩家
				player->DealDamage(pl, pl->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
				//PSendSysMessage(player, u8"团队成员[|cff00ff00%s|h|r]已经被杀死",pl->GetName());
			}
		}
		//
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 404://队长集合+复活
	{
		
		if (!player->CustomPlayerActionTimeCheck(sPzxConfig.GetIntDefault("timecool.pullplayer", 15), PLAYED_TIME_PULL)) {
			//PSendSysMessage(player, u8"[系统消息]:技能冷却中.");
			player->CLOSE_GOSSIP_MENU();
			break;
		}
		InstancePlayerBind *playBind=nullptr;
		DungeonPersistentState* save=nullptr;
		if (player->GetMap() && player->GetMap()->IsDungeon()) {
			playBind = player->GetBoundInstance(player->GetMapId());//团长的进度信息
			save = ((DungeonMap*)player->GetMap())->GetPersistanceState();//团长的副本信息
		}
		//std::list<Player*> reivePlayers;
		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
		{
			Player* pl = itr->getSource();
			

			if (!pl || !pl->GetSession() )
				continue;

			if (pl == player) {//防止团长无限复活人后，身上还带隐身光环
				if (player->IsAlive()&&player->HasAura(21079))
					player->RemoveAurasDueToSpell(21079);
				continue;
			}

			// check online security
			if ( pl->IsInCombat() || pl->IsBeingTeleported()|| pl->IsTaxiFlying()) {//战斗中正在传送中不可再次使用
				PSendSysMessage(player, u8"团队成员[|cffff0000%s|h|r]正在战斗中", pl->GetName());
				continue;
			}

			uint32 teleFlags = TELE_TO_GM_MODE;

			if (save) {//团长在副本内
				uint32 instanceId = save->GetInstanceId();
				DungeonPersistentState* pl_save = nullptr;//队友的副本信息
				if(pl->GetMap() && pl->GetMap()->IsDungeon())
					pl_save = ((DungeonMap*)pl->GetMap())->GetPersistanceState();//队友的副本信息
				InstancePlayerBind *pl_bind = pl->GetBoundInstance(player->GetMapId());//队友存档
				//强制忽略存档不一致的玩家



				if (playBind) {//团长有存档
					if (!pl_bind) {//没有保存进度，强制切换地图
						teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
					}else {//有这个副本的进度
						  //1.判断是否在同一个本
						if (pl_bind->state->GetInstanceId() != instanceId) {//进度ID不同
							pl->UnbindInstance(player->GetMapId());
							pl->BindToInstance(save, true, false);
							teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
							PSendSysMessage(pl, u8"[|cffff0000系统消息|h|r]:团长正在召唤你、请尽快出本清理CD");//强制出本，不做召唤
							pl->RemoveAllSpellCooldown();
							continue;
						}
					}
				}else {//团长无存档(新进度)，有副本ID，没保存信息
					
					if (!pl_save) {//玩家当前地图无存档
						if (!pl_bind|| pl_bind->state->GetInstanceId() != instanceId) {//与团长绑定的ID不一致
							teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
						}
					}else {//玩家当前地图有存档，有存档就一定有副本bind？可能存档是其他副本
							if (pl_bind&&pl_bind->state->GetInstanceId() != instanceId) {//进度ID不同
								pl->UnbindInstance(player->GetMapId());
								teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
								PSendSysMessage(pl, u8"[|cffff0000系统消息|h|r]:团长正在召唤你、请尽快出本清理CD");//强制出本，不做召唤
								pl->RemoveAllSpellCooldown();
								continue;
							}
					}


				}
			

			}else {
				if (pl->GetMap()->IsDungeon()) {//玩家在副本内直接忽略
					PSendSysMessage(pl, u8"[|cffff0000系统消息|h|r]:团长在召唤你、请尽快出本重置CD");
					continue;
					//teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
				}
			}
			// stop flight if need
			if (pl->IsTaxiFlying())
			{
				sLog.outString("[pzx](%d)-[%s] IsTaxiFlying", pl->GetGUIDLow(), pl->GetName());
				pl->GetMotionMaster()->MovementExpired();
				pl->GetTaxi().ClearTaxiDestinations();
				continue;
			}
			// save only in non-flight case
			else
				pl->SaveRecallPosition();

			if (pl->IsAlive() && pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl) && teleFlags != TELE_TO_FORCE_MAP_CHANGE) {
				//小于50码而且在同一地图，不召唤
				continue;
			}
			if (pl->IsAlive()) {
				player->CastSpell(pl, 7720, true);
			}
			else {
				try
				{
						float x, y, z;
						player->GetPosition(x, y, z);
						Corpse* pcor = pl->GetCorpse();
						if (!pcor) {//没释放，还没有尸体
							if (player->IsWithinLOSInMap(pl)) {
								player->CastSpell(pl, 20770, true);
							}
							else {//不在视野的直接拉过来
								pl->TeleportTo(player->GetMapId(), x, y, z + 2.0f, pl->GetAngle(player));
							}
							continue;
						}
				//释放了,直接复活再拉人
			
						pl->ResurrectPlayer(0.99f);
						pl->SpawnCorpseBones();
						if (pl->IsAlive()) {
							player->CastSpell(pl, 7720, true);
						}
				}
				catch (const std::exception&)
				{
					//PSendSysMessage(player, u8"召唤玩家 |cffff0000[%s]|h|r 失败", pl->GetName());
					sLog.outError("[pzx-exception-call] (%d)-[%s]", player->GetGUIDLow(), player->GetName());
				}


			}

			//PSendSysMessage(player, u8"召唤玩家 |cff00ff00[%s]|h|r 成功", pl->GetName());
		}

			PSendSysMessage(player, u8"[|cffff0000系统消息|h|r]:您的队伍已经|cff00ff00集合完毕|h|r");
			player->CLOSE_GOSSIP_MENU();

	}
	break;
	case 100:
		return GossipHello_ItemPzxManager(player, item);
	default:
		Menu_Teleport(player, item->GetGUID(), action);
		break;
	}
	return true;

	}
	catch (...)
	{
		sLog.outError("[pzx-exception-SHE](%d)-[%s] action %d", player->GetGUIDLow(), player->GetName(), action);
		return false;
	}
}

string changeName(string _name, Player *player, MenuTree* tree) {
	string name(_name);
	uint32 accID = player->GetSession()->GetAccountId();

	string  pointT = to_string(tree->needval);
	regex  patternT("DDD");
	name = regex_replace(name, patternT, pointT);

	uint32 mypoint = sPzxMgr->getMyPoint(accID);
	string  timeT = to_string(mypoint);
	regex  patternD("TTT");
	name = regex_replace(name, patternD, timeT);

	regex pattern("[$]{1}\\d{4,}");
	//$60001|t  菜单与类型

	smatch result;
	//regex pattern("\\d{4}");	//匹配四个数字

								//迭代器声明
	string::const_iterator iterStart = name.begin();
	string::const_iterator iterEnd = name.end();
	string temp;
	while (regex_search(iterStart, iterEnd, result, pattern))
	{
		temp = result[0];
		iterStart = result[0].second;	//更新搜索起始位置,搜索剩下的字符串
	}
	if (temp.length() > 0) {
		string id = temp.substr(1);//截取$获取字符串
		int i_id = stoi(id);
		MenuTree _tree = sPzxMgr->getTreeByID(i_id);
		uint32 ty = _tree.type;
		if (ty == 3) {
			uint64 dbtime = sPzxMgr->getTimeLength(player->GetGUIDLow(), i_id);
			uint32 dist;
			if ((_tree.itemid + dbtime) < time(nullptr)) {//超期了
				dist = 0;
				return "";
			}
			else {
				dist = (_tree.itemid + dbtime - time(nullptr)) / (60*60*24);
				if (dist <1) {
					dist = 1;
				}
			}
			name = regex_replace(name, pattern, to_string(dist));
		}
	}
	//物品内置时长
	iterStart = name.begin();
	iterEnd = name.end();
	temp="";
	regex ItemIDpattern("[#]{1}\\d{2,}");
	while (regex_search(iterStart, iterEnd, result, ItemIDpattern))
	{
		temp = result[0];
		iterStart = result[0].second;	//更新搜索起始位置,搜索剩下的字符串
	}
	if (temp.length() > 0) {
		string id = temp.substr(1);//截取$获取字符串
		int item_id = stoi(id);

		if (!player->HasItemCount(item_id, 1, true))
			return "";//没有物品返回
		
		//查看数据库中是否有记录的时长
		uint64 timeDur =sPzxMgr->getTimeLength(player->GetGUIDLow(), item_id);
		if (timeDur == 0) {//没有插入数据的
			sPzxMgr->updateTime(item_id, player->GetGUIDLow(), accID,uint64(time(nullptr)));//更新当前时间
			timeDur = 60 * 60 * 24 * 30;//默认30天
		}
		else {//有数据
			//判断超期
			int32 leftLen = (timeDur+ 60 * 60 * 24 * 30)- uint64(time(nullptr));
			if (leftLen<=0) {
				//摧毁物品，
				player->DestroyItemCount(item_id, 1, true, false, true);
				sLog.out(LOG_LOOTS, u8"GUID %d 过期自动摧毁%d", player->GetGUIDLow(), item_id);
				PSendSysMessage(player, u8"[|cffff0000系统消息|h|r]:您的团长工具已经过期,请重新购买");
				name = u8"您的团长工具道具已经过期,请重新购买";
			}
			else {
				uint32 dist= (leftLen) / (60 * 60 * 24);
				if (dist <1) {
					dist = 1;
				}
				name = regex_replace(name, ItemIDpattern, to_string(dist));
			}

		}
		
	}

	return name;
}
bool GossipHello_ItemPzx(Player *player, Item *_item)
{
	uint32 guid = player->GetGUIDLow();
	CharaMenuMap mmap = sPzxMgr->treeMenu;
	CharaMenuMap::iterator it;
	for (it = mmap.begin(); it != mmap.end(); it++) {

		uint32 id = it->first;
		MenuTree tree = it->second;
		uint32 unionid = tree.unionID;//时长判断
		uint32 unioncheck = tree.unioncheck;
		uint32 raceMask = tree.racemask;
		if (raceMask && ((player->GetRaceMask()&raceMask) == 0)) {
			continue;
		}
		//sLog.outString(">>menuInfo %d %s", id, tree.name);
		string name = changeName(tree.name, player, &tree);//菜单名字
		if (name.length() > 0) {//一般都是过期或者未开通
			if (unionid) {//有关联项检查
				if (unionid == 4001) {
					MenuTree unionTree = sPzxMgr->getTreeByID(unionid);
						uint64 oldVel = sPzxMgr->getTimeLength(guid, unionid);
						if (oldVel == 0) {

							if (!unioncheck) {//第一次开通（没开通过），做异常检查
								player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
							}
						}
						else {
							if (time(nullptr) > oldVel + unionTree.itemid) {//过期了，重新开通
								if (!unioncheck) {//互斥检查，异常检查
									player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
								}
							}
							else {//没有过期
								if (unioncheck) {//互斥检查，正常检查
									player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
								}
							}
						}
				}
			}
			else
				player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
		}
	}
	if (player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM_EXTENDED(0, u8"重新加载服务器个性化配置", GOSSIP_SENDER_MAIN, 999, "reload config", false);
	}

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
	return true;
}



bool GossipSelect_ItemPzx(Player *player, Item *_item, uint32 sender, const uint32 action, char const* reStr)
{



	CharaMenuMap allmap = sPzxMgr->allMenu;
	bool fit = false;
	switch (action)
	{
	case 100://返回主菜单
		GossipHello_ItemPzx(player, _item);
		fit = true;
		break;
	case 999:
		sLog.outString("[pzx] reload server config =%s", reStr);
		sPzxMgr->Reload();
		sPzxConfig.Reload();
		player->CLOSE_GOSSIP_MENU();
		fit = true;
		break;
	case 1005://action=1005 双天赋的切换代码代码，硬编码
	{


		const talentST  readPlay = readTalents(player);
		ostringstream oss2;
		oss2 << u8"->您|cff6247c8目前|h|r使用天赋【";
		for (uint32 id = 0; id < NUM_BREATHS; id++) {
			const initClazz clzz = all[id];
			if (clzz.clazz > 0 && player->GetClass() == clzz.clazz) {
				char * const * tnames = clzz.talenNames;
				oss2 << "|cff00ff00" << (tnames)[0] << "|h|r" << readPlay.t1;
				oss2 << "|cff00ff00" << (tnames)[1] << "|h|r" << readPlay.t2;
				oss2 << "|cff00ff00" << (tnames)[2] << "|h|r" << readPlay.t3;
				break;
			}
		}
		oss2 << u8"】";
		player->ADD_GOSSIP_ITEM(7, oss2.str().c_str(), GOSSIP_SENDER_MAIN, 401);

		const talentST  readdb = readFromDB(player, false);
		ostringstream oss;
		oss << u8"切换至|cff6247c8第二套|h|r天赋 【";
		for (uint32 id = 0; id < NUM_BREATHS; id++) {
			const initClazz clzz = all[id];
			if (clzz.clazz > 0 && player->GetClass() == clzz.clazz) {
				char * const * tnames = clzz.talenNames;
				oss << "|cff00ff00" << tnames[0] << "|h|r" << readdb.t1;
				oss << "|cff00ff00" << tnames[1] << "|h|r" << readdb.t2;
				oss << "|cff00ff00" << tnames[2] << "|h|r" << readdb.t3;
				break;
			}
		};
		oss << u8"】";
		player->ADD_GOSSIP_ITEM(3, oss.str().c_str(), GOSSIP_SENDER_MAIN, 402);
		player->ADD_GOSSIP_ITEM(0, u8"重置|cff6247c8当前天赋|h|r", GOSSIP_SENDER_MAIN, 403);
		player->ADD_GOSSIP_ITEM(0, u8"返回主菜单", GOSSIP_SENDER_MAIN, 100);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
		fit = true;
		break;
	}
	case 401://子菜单
		GossipSelect_ItemPzx(player, _item, sender, 1005, nullptr);
		fit = true;
		break;
	case 403://重置天赋
		player->ResetTalents(true);
		PSendSysMessage(player, u8"[|cffff0000系统消息|h|r]:您的当前天赋树|cffff0000已经重置|h|r，请再打开天赋面板修改天赋点数");
		player->CLOSE_GOSSIP_MENU();
		fit = true;
		break;
	case 402://切换天赋
	{
		talentST  readPlay = readFromDB(player, true);
		ostringstream oss;
		oss << u8"[|cffff0000系统消息|h|r]:|h|r天赋树切换成功.当前天赋已经变更为";
		for (uint32 id = 0; id < NUM_BREATHS; id++) {
			const initClazz clzz = all[id];
			if (clzz.clazz > 0 && player->GetClass() == clzz.clazz) {
				char * const * tnames = clzz.talenNames;
				oss << "|cff00ff00" << (tnames)[0] << "|h|r" << readPlay.t1;
				oss << "|cff00ff00" << (tnames)[1] << "|h|r" << readPlay.t2;
				oss << "|cff00ff00" << (tnames)[2] << "|h|r" << readPlay.t3;
				break;
			}
		}
		oss << u8"】";
		PSendSysMessage(player, oss.str().c_str());
		player->CLOSE_GOSSIP_MENU();
		fit = true;
		break;
	}
	default:
		break;
	}

	if (fit) {
		return true;
	}

	CharaMenuMap::const_iterator got = allmap.find(action);
	if (got != allmap.end()) {
		MenuTree getTree = got->second;
		CharaMenuMap mmap = getTree.children;
		uint32 newActionID = getTree.id;
		if (mmap.size() > 0) {//涵盖子菜单
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
			CharaMenuMap::iterator it;
			for (it = mmap.begin(); it != mmap.end(); it++) {

				uint32 id = it->first;
				MenuTree tree = it->second;
				uint32 raceMask = tree.racemask;
				if (raceMask && ((player->GetRaceMask()&raceMask) == 0)) {
					continue;
				}
				//TODO 此处不坚持union项
				string name = changeName(tree.name, player, &tree);//菜单名字
				if (tree.popMenu) {
					player->ADD_GOSSIP_ITEM_EXTENDED(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id, "abcdefg", true);
				}
				else {
					if (tree.unionID) {
						if(sPzxMgr->getAcctType(player->GetSession()->GetAccountId(), tree.unionID) == 0)//第一次开通（没开通过），有抽奖
							player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
					}
					else {
						player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
					}
				}
			}
			player->ADD_GOSSIP_ITEM(0, u8"返回主菜单", GOSSIP_SENDER_MAIN, 100);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
			return true;
		}
		else {//动作菜单
			//0菜单, 1开关节点，2购买物品，3增加时长，4学习技能, 5传送地点
			switch (getTree.type)
			{
			case 2:
				cutPointAndAddItem(player, getTree.itemid, getTree.needval, getTree.item_count);
				break;
			case 3:
				modifyTimeLength(player, getTree.itemid, getTree.needval, action);
				break;
			case 4:
				cutPointAndlearnSpell(player, getTree.itemid, getTree.needval, action);
				break;
			case 5:
				teleToPoint(player, getTree.xyz, getTree.needval);
				break;
			case 6:
				resetIntance(player, getTree.itemid, true);//重置副本
				break;
			case 7:
			case 8:
			case 13://提升宠物快乐度
			case 14://提升60级+T0
				simpleAction(player, getTree, action, reStr);//鸟点功能和角色功能
				break;
			case 15://加荣誉值
			{
				if (addHoner(player, (float)(getTree.itemid), false)) {
					PSendSysMessage(player, u8"[系统消息]:|cffff0000您已经购买过该荣誉等级了，系统将稍后退还赞助点数|h|r]");
				}
				else {
					simpleAction(player, getTree, action, reStr);//鸟点功能和角色功能
				}
			}
			break;
			case 9:
			case 10:
			case 11:
			case 12:
			case 16://转服功能
			case 17://转服功能

				if (simpleAction(player, getTree, action, reStr))//鸟点功能和角色功能
				{
					return true; //因为要返回到人物列表
				}
				break;
			default:
				break;
			}

			player->CLOSE_GOSSIP_MENU();
		}
	}
	else {//没有子菜单
		player->CLOSE_GOSSIP_MENU();
	}

	return true;
}
bool simpleAction(Player *player, MenuTree & tree, uint32 action, char const* reStr) {
	uint32 accID = player->GetSession()->GetAccountId();
	uint32 mypoint = sPzxMgr->getMyPoint(accID);
	uint32 needval = tree.needval;

	if (tree.type == 16&&player->GetSession()->GetSecurity() == SEC_PLAYER&&sPzxMgr->m_GMONOFFOPTONS[GMOPTIONS_O1]) {//在线配置，1为禁用
		PSendSysMessage(player, u8"[系统消息]:该功能GM在测试中...");
		return false;
	}

	if (player->GetLevel()<60 || player->GetTotalPlayedTime()<3600&&player->GetSession()->GetSecurity() == SEC_PLAYER) {
		PSendSysMessage(player, u8"[系统消息]:该功能只对在线1小时以上的60玩家开放");
		return false;
	}
	
	if (tree.type == 16 && reStr&&strlen(reStr) > 0 && strcmp(player->GetName(), reStr) != 0) {
		
		PSendSysMessage(player, u8"[系统消息]:请输入正确的玩家名称，来确认转服信息.");
		return false;
	}
	if (mypoint < needval) {//点数不够
		PSendSysMessage(player, u8"[系统消息]:你当前点数[|cff00ff00%d|h|r]不够支付", mypoint);
		return false;
	}
	//TODO 修改数据库并且增加时长
	if (needval == 0 || sPzxMgr->cutPoints(needval, accID)) {//如果是免费的
		if (needval > 0)
			PSendSysMessage(player, u8"[系统消息]:系统扣减[|cffff0000%d|h|r]后剩余[|cff00ff00%d|h|r]赞助点。", needval, sPzxMgr->getMyPoint(accID));
		switch (tree.type)
		{
		case 7:
			player->SetTaxiCheater(true);
			player->SaveToDB(true);
			PSendSysMessage(player, u8"[系统消息]:您的鸟点已经临时全部打开");
			break;
		case 8:
		{
			for (uint8 i = 0; i < PLAYER_EXPLORED_ZONES_SIZE; ++i)
			{
				player->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF); //开
				//player->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0); //关
			}
			player->SaveToDB(true);
			PSendSysMessage(player, u8"[系统消息]:您的地图已经全部探索完成");
		}
		break;
		case 9://重命角色名
			CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '1' WHERE guid = '%u'", player->GetGUIDLow());
			player->SetAtLoginFlag(AT_LOGIN_RENAME);
			player->SaveToDB();
			player->GetSession()->LogoutPlayer(false);
			break;
		case 11://更换性别
			{
				uint8 gender = player->GetGender();
				CharacterDatabase.PExecute("UPDATE characters SET gender = '%u' WHERE guid = '%u'", gender == 0 ? 1 : 0, player->GetGUIDLow());
				//player->SaveToDB(false);
				player->GetSession()->LogoutPlayer(false);
			}
		break;
		case 16://角色转移
		{
				uint32 guid = player->GetGUIDLow();
				string pdump_path = sPzxConfig.GetStringDefault("pdump.path", "pdump");
				string file = pdump_path+"/" + to_string(guid)+"_"+ player->GetSession()->GetUsername();
				if (PlayerDumpWriter().WriteDump(file, guid) == DUMP_SUCCESS)
				{
					//写一个结束文件
					string endfile = file + ".end";
					FILE* fout = fopen(endfile.c_str(), "w");
					if (!fout) {
						return false;
					}
					fprintf(fout, "%s\n", "ok");
					fclose(fout);

					player->GetSession()->LogoutPlayer(false);
					player->DeleteFromDB(guid, accID, true, true);
					return true;

				}
				else {
					return false;
				}
				
		}
		break;
		case 17://GM批量在线转移角色
		{


			if (player->GetSession()->GetSecurity() == SEC_PLAYER) {//仅仅GM使用
				return false;
			}
			uint32 count = 0;
			uint32 Pguid = player->GetGUIDLow();
			QueryResult* result = CharacterDatabase.PQuery("select c.guid,a.username from  characters c left join account a on a.id = c.account  where c.guid != %u  LIMIT 0, %u", Pguid,sPzxConfig.GetIntDefault("characters.trans.numm",1000));
			if (!result)
			{
				BarGoLink bar(1);
				bar.step();

				sLog.outString();
				sLog.outString(">> [pzx char trancf]  count select is 0");
			}

			BarGoLink bar(result->GetRowCount());
			string pdump_path = sPzxConfig.GetStringDefault("pdump.path", "pdump");
			do
			{
				bar.step();
				Field* fields = result->Fetch();
				uint32 guid = fields[0].GetUInt32();
				string name = fields[1].GetCppString();
				//开始转移
				//uint32 guid = player->GetGUIDLow();
				string file = pdump_path + "/" + to_string(guid) + "_" + name;
				if (PlayerDumpWriter().WriteDump(file, guid) == DUMP_SUCCESS)
				{
					//写一个结束文件
					string endfile = file + ".end";
					FILE* fout = fopen(endfile.c_str(), "w");
					if (!fout) {
						PSendSysMessage(player, u8"[系统消息]:[%u-%s] 异常", guid, name);
						sLog.outString(u8"[系统消息]:[%u-%s] 异常", guid, name);
						continue;
					}
					fprintf(fout, "%s\n", "ok");
					fclose(fout);

					//player->GetSession()->LogoutPlayer(false);
					player->DeleteFromDB(guid, accID, true, true);
					PSendSysMessage(player, u8"[系统消息]:[%u-%s] 成功", guid, name);
					sLog.outString(u8"[系统消息]:[%u-%s]已经转移成功", guid, name);
					

				}
				else {
					sLog.outString(u8"[系统消息]:[%u-%s]已经转移失败", guid, name);
					continue;
				}


			} while (result->NextRow());

			delete result;

			sLog.outString(">> [pzx char trancf]   %u  success", count);

	

		}
		break;
		case 12://更换造型
		{
			if (reStr&&strlen(reStr) > 0) {
				uint32 toCopyGuid = 0;
				Player* pl = sObjectMgr.GetPlayer(reStr);
				if (!pl) {
					PlayerCacheData *pdata = sObjectMgr.GetPlayerDataByName(reStr);
					if (pdata) {
						if (pdata->uiGender != player->GetGender() || pdata->uiRace != player->GetRace()) {
							sPzxMgr->cutPoints(0 - needval, accID);//补偿
							PSendSysMessage(player, u8"[系统消息]:目标角色与当前角色|cffff0000%s不符|h|r", pdata->uiGender != player->GetGender() ? u8"性别" : u8"种族");
						}
						else {
							//更新造型
							toCopyGuid = pdata->uiGuid;

						}
					}
					else {
						sPzxMgr->cutPoints(0 - needval, accID);//补偿
						PSendSysMessage(player, u8"[系统消息]:未找到对应的角色名");
					}
				}
				else {
					toCopyGuid = pl->GetGUIDLow();
				}
				if (toCopyGuid > 0) {

					bool change = CharacterDatabase.PExecute("update characters a join characters b on a.guid='%u' and b.guid='%u' set a.playerBytes = b.playerBytes,a.playerBytes2=b.playerBytes2", player->GetGUIDLow(), toCopyGuid);
					if (change) {
						PSendSysMessage(player, u8"[系统消息]:造型变更成功");
						player->GetSession()->LogoutPlayer(false);
						return true;
					}
					else {
						PSendSysMessage(player, u8"[系统消息]:造型变更失败222");
						return false;
					}
				}
				else {
					PSendSysMessage(player, u8"[系统消息]:造型变更失败");
					return false;
				}
			}
			else {
				sPzxMgr->cutPoints(0 - needval, accID);//补偿
				PSendSysMessage(player, u8"[系统消息]:请输入角色名字");
				return false;
			}
		}
		break;
		case 10://更换种族
		{
			if (reStr&&strlen(reStr) > 0) {
				uint32 race = 0;
				Player* pl = sObjectMgr.GetPlayer(reStr);
				if (!pl) {
					PlayerCacheData *pdata = sObjectMgr.GetPlayerDataByName(reStr);
					if (pdata) {
						if (pdata->uiGender != player->GetGender()) {
							sPzxMgr->cutPoints(0 - needval, accID);//补偿
							PSendSysMessage(player, u8"[系统消息]:目标角色与当前角色性别不符");
						}
						else
							race = (pdata)->uiRace;
					}
					else {
						sPzxMgr->cutPoints(0 - needval, accID);//补偿
						PSendSysMessage(player, u8"[系统消息]:未找到对应的角色名");
					}
				}
				else {
					race = pl->GetRace();
				}
				if (race&&player->ChangeRace(race))
				{
					return true;
					//PSendSysMessage(player, u8"[系统消息]:种族变更成功");
				}
				else {
					PSendSysMessage(player, u8"[系统消息]:种族变更失败");
					return false;

				}
			}
			else {
				sPzxMgr->cutPoints(0 - needval, accID);//补偿
				PSendSysMessage(player, u8"[系统消息]:请输入角色名字");
				return false;
			}
		}
		break;
		case 13://提升宠物等级
		{
			if (player->GetPet() && player->GetPet()->getPetType() == HUNTER_PET) {
				uint32 maxlevel = std::min(sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL), player->GetLevel());
				Pet* HunterPet = player->GetPet();
				if (HunterPet->GetLevel() < maxlevel || HunterPet->GetLoyaltyLevel() < LoyaltyLevel(BEST_FRIEND)) {

					//player->ADD_GOSSIP_ITEM(3, u8"提升 我的宠物忠诚度和等级 ", GOSSIP_SENDER_MAIN, 205);
					player->GetPet()->GivePetXP(99999999);
					player->GetPet()->ModifyLoyalty(1000000.0);
				}
				else {
					PSendSysMessage(player, u8"[系统消息]:|cff0000ff 您的宠物忠诚度已经提升完成!|h|r");
				}
			}
			else {
				PSendSysMessage(player, u8"[系统消息]:请先|cffff0000 驯服或者召唤出|h|r一只要强化的宠物");
			}
		}

		break;
		case 14://免费-提升到60级+T0套装
		{
			//player->LearnSpell(33389, false);
			ObjectGuid target_guid;
			if (player->GetLevel() < 60) {
				player->GiveLevel(60);
				player->InitTalentForLevel();
			}
			player->SetUInt32Value(PLAYER_XP, 0);
			player->UpdateSkillsToMaxSkillsForLevel();
			addItemSet(player, 0);//增加T1套装
		}

		break;
		case 15://提升荣誉等级
		{
			if (!addHoner(player, (float)(tree.itemid), true)) {
				PSendSysMessage(player, u8"[系统消息]:|cffff0000您的荣|h|r]");
				sPzxMgr->cutPoints(0 - needval, accID);//补偿
			}
			else {
				PSendSysMessage(player, u8"[系统消息]:|cffff0000您的荣誉等级已经提升,请到荣誉面板查看等级|h|r");
			}
		}
		default:
			break;
		}

	}
	else {//扣减异常
		PSendSysMessage(player, u8"[系统消息]:|cffff0000您赞助点系统异常，请联系GM-1008|h|r]");
	}

	return true;

}

bool modifyTimeLength(Player *player, uint32 timelength, uint32 needval, uint32 action) {
	const char* txt;
	
	switch (action)
	{
	case 4001:
		txt = u8"双天赋功能";
		break;
		
	default:
		txt = u8"瞬飞功能";
		break;
	}
	uint32 accID = player->GetSession()->GetAccountId();
	uint32 guid = player->GetGUIDLow();
	uint64 oldVel = sPzxMgr->getTimeLength(guid, action);

	if (oldVel) {
		//修改数据库，更改长度
		if (time(nullptr) > oldVel + timelength) {//过期了，重新开通

			uint32 mypoint = sPzxMgr->getMyPoint(accID);

			if (mypoint < needval) {//点数不够
				PSendSysMessage(player, u8"[系统消息]:你当前点数[|cffff0000%d|h|r]不够支付", mypoint);
				return false;
			}
			//TODO 修改数据库并且增加时长
			if (sPzxMgr->cutPoints(needval, accID) && sPzxMgr->updateTime(action, guid, accID,  uint64(time(nullptr)))) {
	
				PSendSysMessage(player, u8"[系统消息]:系统扣减[|cffff0000%d|h|r]后剩余[|cff00ff00%d|h|r]赞助点", needval, sPzxMgr->getMyPoint(accID));
				PSendSysMessage(player, u8"[系统消息]:您的%s激活成功", txt);
			}
			else {
				PSendSysMessage(player, u8"[系统消息]:|cffff0000您赞助点系统异常，请联系GM-1004|h|r");
			}

		}
		else {//没过期
			PSendSysMessage(player, u8"[系统消息]:您的%s时间还在有效期", txt);
		}
		time_t mutetime = time(nullptr) + timelength;//2天后再刷新，一般公会不会连续开荒2天
	}
	else {//还没卡通过，第一次开通
		uint32 mypoint = sPzxMgr->getMyPoint(accID);

		if (mypoint < needval) {//点数不够
			PSendSysMessage(player, u8"[系统消息]:你当前点数[|cffff0000%d|h|r]不够支付", mypoint);
			return false;
		}
		//TODO 修改数据库并且增加时长
		if (sPzxMgr->cutPoints(needval, accID) && sPzxMgr->updateTime(action, guid, accID, uint64(time(nullptr)))) {

			PSendSysMessage(player, u8"[系统消息]:系统扣减[|cffff0000%d|h|r]后剩余[|cff00ff00%d|h|r]赞助点", needval, sPzxMgr->getMyPoint(accID));
			if (action==4001) {
				talentST  readPlay = readFromDB(player, true);
				//PSendSysMessage(player, u8"|cffff0000[系统消息]:|h|r您的第2套天赋功能已经开通，现在可以通过[|cffeb8920懦夫救星|h|r]设置新的天赋树");
				//player->CLOSE_GOSSIP_MENU();
			}
			PSendSysMessage(player, u8"[系统消息]:您的%s开通成功", txt);
		}
		else {
			PSendSysMessage(player, u8"[系统消息]:|cffff0000您赞助点系统异常，请联系GM-1005|h|r");
		}
	}
	return true;
}

bool teleToPoint(Player *player, string xyz, uint32 needval) {
	uint32 accID = player->GetSession()->GetAccountId();
	uint32 mypoint = sPzxMgr->getMyPoint(accID);



	if (mypoint < needval) {//点数足够
		PSendSysMessage(player, u8"[系统消息]:你当前点数[|cff00ff00%d|h|r]不够支付", mypoint);
			return false;
	}
	const char * _args = xyz.c_str();
	char * args = const_cast<char*>(_args);
	sLog.outString(u8">> [PZX] xyzzzzzz=%s", args);
	float x;
	if (!ExtractFloat(&args, x)) {
		sLog.outString(u8">> [PZX] xyz1=%s", args);
		return false;
	}

	float y;
	if (!ExtractFloat(&args, y)) {
		sLog.outString(u8">> [PZX] xyz2=%s", args);
		return false;
	}

	float z;
	if (!ExtractFloat(&args, z)) {
		sLog.outString(u8">> [PZX] xyz3=%s", args);
		return false;
	}

	uint32 mapid;
	if (!ExtractOptUInt32(&args, mapid, player->GetMapId())) {
		sLog.outString(u8">> [PZX] xyz4=%s", args);
		return false;
	}
	if (sPzxMgr->cutPoints(needval, accID)) {
		PSendSysMessage(player, u8"[系统消息]:系统扣减[|cffff0000%d|h|r]后剩余[|cff00ff00%d|h|r]赞助点", needval, sPzxMgr->getMyPoint(accID));
		return player->TeleportTo(mapid, x, y, z, 0.0f);
	}
	else
	{
		PSendSysMessage(player, u8"[系统消息]:你当前点数[|cff00ff00%d|h|r]不够支付", mypoint);
		return false;
	}
}

bool cutPointAndlearnSpell(Player *pPlayer, uint32 skill, uint32 needval, uint32 action) {
	if (pPlayer->GetFreePrimaryProfessionPoints() == 0 && !(skill == 185 || skill == 129))
	{//附魔和急救
		PSendSysMessage(pPlayer, u8"[系统消息]:你已经学习了2项专业技能了.");
		return false;
	}

	if (!pPlayer->HasSkill(skill) || pPlayer->GetSkillValue(skill) < 300) {
		//ChatHandler handler(pPlayer->GetSession());
		//检查点数是否够
		uint32 accID = pPlayer->GetSession()->GetAccountId();
		uint32 mypoint = sPzxMgr->getMyPoint(accID);

		if (mypoint >= needval) {//点数足够
			SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
			if (!SkillInfo)
			{
				PSendSysMessage(pPlayer, u8"[系统消息]:您赞助点系统异常，请联系GM-1001");
				sLog.outError("Profession NPC: received non-valid skill ID %d" + skill);
				return false;
			}
			if (sPzxMgr->cutPoints(needval, accID)) {
				PSendSysMessage(pPlayer, u8"[系统消息]:系统扣减[|cffff0000%d|h|r]后剩余[|cff00ff00%d|h|r]赞助点", needval, sPzxMgr->getMyPoint(accID));
				pPlayer->SetSkill(SkillInfo->id, 300, 300);
				LearnSkillRecipesHelper(pPlayer, SkillInfo->id);
				char* skill_name = SkillInfo->name[sWorld.GetDefaultDbcLocale()];
				PSendSysMessage(pPlayer, u8"所有 %s 配方已经学习完成", skill_name);
				return true;
			}
			else {
				PSendSysMessage(pPlayer, u8"[系统消息]:2您赞助点系统异常，请联系GM-1002");

			}
		}
		else {
			PSendSysMessage(pPlayer, u8"[系统消息]:你当前点数[|cff00ff00%d|h|r]不够支付", mypoint);
		}
		return false;
	}
	else {
		PSendSysMessage(pPlayer, u8"[系统消息]:你已经练满该专业");
		return false;
	}
	//PSendSysMessage(pPlayer, u8"系统错误.");


}
void outputItemColorFormat(Player *player, ostringstream &itemret, uint32 itemID, const char *format) {
	ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemID);
	if (pProto) {
		const char * colorItemPre = ItemQualityColorsStr[pProto->Quality];
		itemret << colorItemPre << pProto->Name1 << "|h|r";
		PSendSysMessage(player, format, itemret.str().c_str());
	}
	else {
		PSendSysMessage(player, u8"未找到物品ID-%d", itemID);
	}
	itemret.str(std::string());
}

bool cutPointAndAddItem(Player *player, uint32 items, uint32 needval, uint32 count) {
	ostringstream outs;
	ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(items);
	if (player->HasItemCount(items, 1, true) && pProto->MaxCount == 1) {//唯一物品检测
		outputItemColorFormat(player, outs, items, u8"[系统消息]:%s已经在物品清单中");
		return true;
	}
	ItemPosCountVec dest;
	InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, items, items == 49999?1:count);
	if (msg == EQUIP_ERR_OK)
	{
		//检查点数是否够
		uint32 accID = player->GetSession()->GetAccountId();
		uint32 mypoint = sPzxMgr->getMyPoint(accID);

		if (mypoint >= needval) {//点数足够
			Item* item = player->StoreNewItem(dest, items, true);
			if (sPzxMgr->cutPoints(needval, accID)) {
				sLog.out(LOG_LOOTS, "Account %u guid %u : getitem from zz %u", accID , player->GetGUIDLow(),items);
				PSendSysMessage(player, u8"[系统消息]:系统扣减[|cffff0000%d|h|r]后剩余[|cff00ff00%d|h|r]赞助点", needval, sPzxMgr->getMyPoint(accID));
				player->SendNewItem(item, 1, true, false);
				ItemPrototype const* pProto = item->GetProto();
				if (pProto&&pProto->RequiredSkill == 762) {//物品是坐骑自动学习骑术
					player->LearnSpell(33389, false);
					player->LearnSpell(33392, false);
				}
				if (items == 49999) {//团长助手需要插入时间
					uint64 leftTime = uint64(time(nullptr)) - (30 - count) * 60 * 60 * 24;
					sPzxMgr->updateTime(49999, player->GetGUIDLow(), accID, leftTime);
					//根据cont时长控制,如果conten小于30，则把数据库记录时间推迟
					
					//时间长度
				}
				if (items == 60003) {//免费体验，插入一条记录，证明体验过
					sPzxMgr->updateTime(items, player->GetGUIDLow(), accID,uint64(time(nullptr)));
				}
				outputItemColorFormat(player, outs, items, u8"[系统消息]:%s已经添加到你包裹中");
				player->SaveToDB(true);//保存物品
			}
			else {
				PSendSysMessage(player, u8"[系统消息]:您赞助点系统异常，请联系GM-1003");
			}

		}
		else {
			PSendSysMessage(player, u8"[系统消息]:你当前点数[|cff00ff00%d|h|r]不够支付", mypoint);
		}

	}
	else
	{
		player->SendEquipError(msg, nullptr, nullptr, items);
		PSendSysMessage(player, u8"[系统消息]:您无法拥有该物品,请检查包包空间或装备唯一性");
	}
	return true;
}


struct go_Meeting_doorAI : GameObjectAI
{
	explicit go_Meeting_doorAI(GameObject* pGo) : GameObjectAI(pGo)
	{

	}

	bool OnUse(Unit* pCaster) override
	{
		Player* player = pCaster->ToPlayer(); 
		if (!player) {
			return false;
		}
		ObjectGuid guid = player->GetSession()->GetPlayer()->GetSelectionGuid();
		
		if (!guid) {
			return false;
		}
		else {
			Player* target= sObjectMgr.GetPlayer(guid);
			if (target&&target != player && ((Player *)target)->IsInSameRaidWith(player)) {
				player->CastSpell(target, 698, true);//瞬间召唤
			}
		}
	return false;
	}
};

GameObjectAI* GetAI_go_metting_door(GameObject* pGo)
{
	return new go_Meeting_doorAI(pGo);
}

void AddSC_custom_items()
{
	if (!sPzxConfig.SetSource("pzx.conf")) {
		sLog.outError(u8"未找到pzx.conf");
	}
	sPzxMgr->Reload();
	sPzxMgr->startSynDB();//启动定时任务
	Script* newscript;

	newscript = new Script;
	newscript->Name = "custom_teleport_npc_pzx";//赞助卡AI-60004
	newscript->pGossipHelloPzx = &GossipHello_ItemPzx;
	newscript->pGossipSelectPzx = &GossipSelect_ItemPzx;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "custom_teleport_npc_pzx_manager";//团长工具-49999
	newscript->pGossipHelloPzx = &GossipHello_ItemPzxManager;
	newscript->pGossipSelectPzx = &GossipSelect_ItemPzxManager;
	newscript->RegisterSelf(false);


	newscript = new Script;
	newscript->Name = "custom_axns_item";//50001-埃辛诺斯碎片合成
	newscript->pGossipHelloPzx = &GossipHello_axnsItemManager;
	newscript->pGossipSelectPzx = &GossipSelect_axnsItemPzxManager;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "pzx_gob_meeting";//集合石AI-gameobject
	newscript->GOGetAI = &GetAI_go_metting_door;
	//newscript->pQuestRewardedGO = &QuestRewarded_scarab_gong;
	//newscript->pGOGossipHello = &GOHello_scarab_meeting;
	newscript->RegisterSelf();

}
