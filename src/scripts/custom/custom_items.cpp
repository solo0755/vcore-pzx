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

		//�����������ƣ������滻������츳����
		//�л���

	}
	const talentST pt = { player->GetGUIDLow(), cost_0, cost_1, cost_2,spells.str() };
	//PSendSysMessage(player, u8"��ǰ�츳����Ϣ:%d,%d,%d,", cost_0, cost_1, cost_2);
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
			player->CastSpell(player, 14867, true);//��������
			ChatHandler(player).SendSysMessage(LANG_RESET_TALENTS);
			//����Լ����츳Ϊ���츳
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


	player->ADD_GOSSIP_ITEM(0, u8"��������", GOSSIP_SENDER_MAIN, 404);
	if (sPzxConfig.GetIntDefault("open.fullbuff", 1) || player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"����BUFF", GOSSIP_SENDER_MAIN, 102);
	}
	if (sPzxConfig.GetIntDefault("RiadTooldelbuff", 1)|| player->IsGameMaster()){
		if (sPzxConfig.GetIntDefault("show.allHiddenBuff", 1)) {//�ų���ȫ������
			player->ADD_GOSSIP_ITEM(0, u8"ȫ������", GOSSIP_SENDER_MAIN, 410);
			player->ADD_GOSSIP_ITEM(0, u8"�Ƴ�����", GOSSIP_SENDER_MAIN, 407);
		}
		if (sPzxConfig.GetIntDefault("show.cthunPostion", 0)) {//�ų���ȫ������
			player->ADD_GOSSIP_ITEM(0, u8"����ȫ�ŵ�|cff6247c8���ն�վλ��|h|r--������վλר�á�", GOSSIP_SENDER_MAIN, 412);
		}
		if (sPzxConfig.GetIntDefault("show.allKillBuff", 1)) {//�ų���ȫ�����¿�ʼ
			player->ADD_GOSSIP_ITEM(0, u8"��ɱȫ��", GOSSIP_SENDER_MAIN, 411);
		}
		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_cooldown", 0))
			player->ADD_GOSSIP_ITEM(0, u8"����ȫ�ż���CD", GOSSIP_SENDER_MAIN, 413);

		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_15_add", 0)) 
		player->ADD_GOSSIP_ITEM(0, u8"���˹⻷ |cff6247c8����15%|h|r", GOSSIP_SENDER_MAIN, 405);

		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_50_add", 0))
		player->ADD_GOSSIP_ITEM(0, u8"���˹⻷ |cff6247c8����35%|h|r", GOSSIP_SENDER_MAIN, 406);

		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_15_del", 0))
		player->ADD_GOSSIP_ITEM(0, u8"Ϊ|cff6247c8Ŀ��|h|r���ӻ��ܣ�ʹ�����˺�|cff6247c8����15%|h|r", GOSSIP_SENDER_MAIN, 408);
		if (sPzxConfig.GetIntDefault("RiadTooldelbuff_50_del", 0))
		player->ADD_GOSSIP_ITEM(0, u8"Ϊ|cff6247c8Ŀ��|h|r���ӻ��ܣ�ʹ�����˺�|cff6247c8����35%|h|r", GOSSIP_SENDER_MAIN, 409);

	}
	player->ADD_GOSSIP_ITEM(0, u8"ԭ�ظ��� ���󹤻�Ƶ������up�س�", GOSSIP_SENDER_MAIN, 600);

	
	uint32 accID = player->GetSession()->GetAccountId();
	if (sPzxConfig.GetIntDefault("show.theendboss", 0) || player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8����|h|rһ����������¡�Ľ���", GOSSIP_SENDER_MAIN, 997);
		player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8����|h|rһ���˶��ռӵµĽ���", GOSSIP_SENDER_MAIN, 998);
		if(sPzxConfig.GetIntDefault("show.theendboss2", 0) || player->IsGameMaster()) {
			player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8����|h|rһ���ع�BOSS(����ʿ,����,Ģ����,����˹��)�Ľ���", GOSSIP_SENDER_MAIN, 996);
		}
	}

	if (sPzxConfig.GetIntDefault("RiadTooldExtMenu", 0)|| player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"|cff6247c8����|h|rһ�����������½���", GOSSIP_SENDER_MAIN, 500);
		player->ADD_GOSSIP_ITEM(7, u8"-->|cff6247c8����|h|r��Ŀ�ĵ�", GOSSIP_SENDER_MAIN, 104);
		//player->ADD_GOSSIP_ITEM(0, u8"��������û�", GOSSIP_SENDER_MAIN, 888);
	}


	if (sPzxConfig.GetIntDefault("show.additem", 0)) {
		player->ADD_GOSSIP_ITEM_EXTENDED(6, u8"����Ҫһ����Ʒ<|cffff0000Σ�ղ���|h|r>|n|cff00f700   **ʹ��˵��**|h|r:�ڵ�����������|cff6247c8��ƷID|h|r", GOSSIP_SENDER_MAIN, 777, "reload config ����abc", true);
	}


	if (player->IsGameMaster()) {
		player->ADD_GOSSIP_ITEM(0, u8"���¼��ط��������Ի�����", GOSSIP_SENDER_MAIN, 999);
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
				PSendSysMessage(player, u8"��� |cffff0000[%s]|h|r ���䵽��%d�Ŵ���ˢ�µ�", player->GetName(), indexid);
		
		
}

bool GossipHello_axnsItemManager(Player *player, Item *_item) {
	if (player->HasItemCount(50001, 40, false)) {//��40��
												 //WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
												 //data << player->GetObjectGuid();
												 //player->GetSession()->SendPacket(&data);
		player->ADD_GOSSIP_ITEM(0, u8"����40����|cffff8000|Hitem:50001:0:0:0|h[����ŵ˹����Ƭ]|h|r�����ɶһ�������|cffe6cc80|Hitem:18582:0:0:0|h[����ŵ˹˫��]|h|r��", GOSSIP_SENDER_MAIN, 601);
	}
	else {//����40��
		player->ADD_GOSSIP_ITEM(0, u8"��Ƭ����40�������һ���", GOSSIP_SENDER_MAIN, 602);
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
				//�������Ƿ�
					uint32 accID = player->GetSession()->GetAccountId();
					Item* item = player->StoreNewItem(dest, toadditems, true);
					sLog.out(LOG_LOOTS, u8"Account %u guid %u : create ����˫��  %u", accID, player->GetGUIDLow(), toadditems);
					player->SendNewItem(item, 1, true, false);
					ItemPrototype const* pProto = item->GetProto();
					ostringstream outs;
					//outputItemColorFormat(player, outs, toadditems, );
					ItemPrototype const* topProto = ObjectMgr::GetItemPrototype(toadditems);
					if (topProto) {
						const char * colorItemPre = ItemQualityColorsStr[topProto->Quality];
						outs << colorItemPre << topProto->Name1 << "|h|r";
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:%s�Ѿ���ӵ��������", outs.str().c_str());
					}
					player->SaveToDB(true);//������Ʒ
			}
			else
			{
				player->SendEquipError(msg, nullptr, nullptr, 50001);
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��������ռ�");
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
		//�ж��Ƿ���

		//�鿴���ݿ����Ƿ��м�¼��ʱ��
		//uint64 timeDur = sPzxMgr->getTimeLength(player->GetGUIDLow(), 49999);
		//if (timeDur == 0|| (timeDur + 60 * 60 * 24 * 30) < uint64(time(nullptr))){
		//	  //�жϳ���,�ݻ���Ʒ��
		//		player->DestroyItemCount(49999, 1, true, false, true);
		//		sLog.out(LOG_LOOTS, u8"GUID %d �����Զ��ݻ�%d", player->GetGUIDLow(), 49999);
		//		PSendSysMessage(player, u8"�����ų������Ѿ�����,�����¹���");
		//		player->CLOSE_GOSSIP_MENU();
		//		return false;
		//}


		if (action == 777) {
		/*	if (!player->HasItemCount(sPzxConfig.GetIntDefault("vipItemID", 40003), 1, true)) {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��ҪVIP��֤���ſ���ʹ�ñ����ܣ�����ϵGM��ȡ");
				player->CLOSE_GOSSIP_MENU();

				return false;
			}*/
			sLog.outString("[pzx] get Input str =%s", reStr);
			uint32 getItemID;
			try {
				getItemID = std::stoi(reStr);
				if (getItemID <= 0) {
					PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��������ȷ����ƷID");
					player->CLOSE_GOSSIP_MENU();

					return false;
				}
			}
			catch (...) {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��������ȷ����ƷID");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}


			ItemPrototype const *pProto = sItemStorage.LookupEntry<ItemPrototype>(getItemID);
			if (pProto) {

				if (pProto->Quality < sPzxConfig.GetIntDefault("item.quality", ITEM_QUALITY_LEGENDARY)&& pProto->ItemLevel < sPzxConfig.GetIntDefault("item.level", 80))
				{
					if (player->HasItemCount(pProto->ItemId, 1, true)) {//�Ѿ���һ����
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:����ƷΨһ");
					}
					ItemPosCountVec dest;
					InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, pProto->ItemId, 1);
					if (msg == EQUIP_ERR_OK)
					{
						Item* item = player->StoreNewItem(dest, pProto->ItemId, true);
						player->SendNewItem(item, 1, true, false);
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:%s �Ѿ���ӵ������", item->GetProto()->Name1);
					}
					else
					{
						player->SendEquipError(msg, nullptr, nullptr, pProto->ItemId);
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�뱣�ְ������㹻�ռ�");
					}
				}
				else {
					PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��ȡ��Ʒ�ȼ����ߣ�����ϵGM");
				}
			}
			else
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��Ʒδ�ҵ�");

			player->CLOSE_GOSSIP_MENU();
			return false;

		}
	
	sLog.outString("[pzx](%d)-[%s] action %d", player->GetGUIDLow(), player->GetName(), action);
	switch (action)
	{

	case 404://�ų�����
	case 405:
	case 406:
	case 407:
	case 408:
	case 409://+����buff
	case 410:
	case 411:
	case 412:
	case 413:
	{
		if (player->IsInCombat()&&action!= 411) {//ɱ��ȫ�ſ�����ս����ʹ��
			PSendSysMessage(player, u8"|cffff0000[ϵͳ��Ϣ]:|h|r��������ս������ʹ�ô˹���");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		if (!(player->GetGroup() && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER))) {
			PSendSysMessage(player, u8"|cffff0000[ϵͳ��Ϣ]:|h|rֻ���Ŷ��������ʹ�ô˹���");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		if (action == 410&&sPzxConfig.GetIntDefault("forbidden.area", 0) && sPzxConfig.GetIntDefault("forbidden.area", 0) == player->GetMapId()) {
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�������ڽ�ֹ��������⻷");
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
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000���Ѿ��ڶ�����߸�����|h|r���������������뿪��������.");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		uint32 id = action == 508 ? 4 : action - 501;//˫�Ӳ���5������ID=4
		//Լ��һЩ����״̬
		uint32 ins_satus = 0;
		if (action == 508) {
			ins_satus = 4;
		}
		resetIntance(player, 531, true);
		saveInstance(player, id, TAQINSTANCE_DATA[id], 531, ins_satus);
		
		PSendSysMessage(player, u8"����TAQ�½��ȣ�%s���Ѿ������ɹ�,���Խ��븱���ˡ�", TAQINSTANCE_Names[id]);
		player->CLOSE_GOSSIP_MENU();
		return true;
	}
	switch (action)
	{
	case 600:
		return GossipHello_ItemPzxManager(player, item);
	case 500:
	{
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8��������|h|r����", GOSSIP_SENDER_MAIN, 501);
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8ɳ��ͼ��|h|r����", GOSSIP_SENDER_MAIN, 502);
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8������˹|h|r����", GOSSIP_SENDER_MAIN, 503);
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8������|h|r����", GOSSIP_SENDER_MAIN, 504);
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8˫�ӻʵ�|h|r����", GOSSIP_SENDER_MAIN, 505);
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8˫�ӻʵ�(�����幷)|h|r����", GOSSIP_SENDER_MAIN, 508);
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8����|h|r����", GOSSIP_SENDER_MAIN, 506);
		player->ADD_GOSSIP_ITEM(7, u8"����|cff6247c8���ն�|h|r����", GOSSIP_SENDER_MAIN, 507);
		player->ADD_GOSSIP_ITEM(7, u8"-->|cff6247c8����|h|r", GOSSIP_SENDER_MAIN, 100);
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
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000���Ѿ��ڶ�����߸�����|h|r���������������뿪��������.");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		resetIntance(player, 533, true);

		uint32 id = sMapMgr.GenerateInstanceId();
		uint32 count = 0;
		while (!CharacterDatabase.DirectPExecute("INSERT INTO instance VALUES ('%u', '%u', '%u', '%s')", id, 533, 0, action== 997?"3 3 3 3 3 3 3 3 3 3 3 3 3 0 0":(action == 998 ? "3 3 3 3 3 3 3 3 3 3 3 3 3 3 0":"3 3 0 3 3 0 3 3 0 3 3 3 0 0 0 "))) {
			count++;
			if (count > 100) {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000ϵͳ��æ���Ժ�����|h|r");//��ֹ��ѭ��
				player->CLOSE_GOSSIP_MENU();
				return false;
			}
			id = sMapMgr.GenerateInstanceId();																		 
			continue;//����ɹ�Ϊֹ
		}
		time_t mutetime = time(nullptr) + 60 * 48 * MINUTE;//2�����ˢ�£�һ�㹫�᲻����������2��
		if (action == 997 || action == 998) {
			for (size_t i = 0; i < 16; i++)
			{
				CharacterDatabase.DirectPExecute("INSERT INTO creature_respawn VALUES ('%u', '%u', '%u','%u')", naxxbossIndex[i], uint64(mutetime), id, 533);
			}
		}else {//�����ع�BOSS
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
		PSendSysMessage(player, action == 997?(u8"���ı��������Ѿ������ɹ�,���Խ��븱���ˡ�"):(u8"�����Ͽ˽����Ѿ������ɹ�,���Խ��븱���ˡ�"));
		player->CLOSE_GOSSIP_MENU();
	}
		break;
	case 888://�û�����Ϊ�罣
	{

		if (player->IsTwoHandUsed()) {
			player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + EQUIPMENT_SLOT_MAINHAND * MAX_VISIBLE_ITEM_OFFSET, WeaponDisplay_id2[urand(0, 3)]);// ����ͷ�
		}
		else {
			if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
				player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + EQUIPMENT_SLOT_MAINHAND * MAX_VISIBLE_ITEM_OFFSET, WeaponDisplay_id1[urand(0, 3)]);// ����ͷ�
			if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
				player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + EQUIPMENT_SLOT_OFFHAND * MAX_VISIBLE_ITEM_OFFSET, WeaponDisplay_id1[urand(0, 3)]);// ����ͷ�
		}
		PSendSysMessage(player, u8"���������û����");
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 405:
	case 406:
	case 407:
	case 413:
	{
		if (action == 405 || action == 406) { //NAXX��ֹ���˺�����
			//uint32 zone, area;
			//player->GetZoneAndAreaId(zone, area);
			if (sPzxConfig.GetIntDefault("forbidden.area", 0)&&sPzxConfig.GetIntDefault("forbidden.area", 0) == player->GetMapId()) {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�������ڽ�ֹ��������⻷");
				player->CLOSE_GOSSIP_MENU();
				break;
			}
		}
		if (action == 413&&!player->CustomPlayerActionTimeCheck(sPzxConfig.GetIntDefault("timecool.coolspell", 30), PLAYED_TIME_COOL)) {
			//PSendSysMessage(player, u8"[ϵͳ��Ϣ]:������ȴ��.");
			player->CLOSE_GOSSIP_MENU();
			break;
		}
		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
			{
				Player* pl = itr->getSource();

				if (!pl || !pl->GetSession())
					continue;
				if (pl->IsAlive()) {
					if (action == 405 || action == 406) {//�������⻷
						uint32 tocheckSpell = action == 405 ? 8299 : 21751;
						if(!pl->HasAura(tocheckSpell))
							pl->CastSpell(pl, tocheckSpell, true);//21751  ����50%
					}
					else if (action == 413) {//��ȴ���м���
						if (pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl)) {
							pl->RemoveAllSpellCooldown();
							pl->CastSpell(pl, 14867, true);//��������
						}else {
							//PSendSysMessage(player,  u8"�����Ŷӳ�Ա|cffff0000%s|h|r������Ұ��Χ��",pl->GetName());
						}
					}else{//�Ƴ����� action=407

						if (pl->HasAura(21079))
							pl->RemoveAurasDueToSpell(21079);
					}
				}
			}

		PSendSysMessage(player, action == 407 ? u8"[|cffff0000ϵͳ��Ϣ|h|r]:����ȫ�ų�Ա�Ѿ�|cffff0000�Ƴ�����|h|r" : (action == 413 ? u8"[|cffff0000ϵͳ��Ϣ|h|r]::����ȫ�ų�Ա�Ѿ� |cff0000ff�����꼼����ȴʱ�� |h|r": u8"[|cffff0000ϵͳ��Ϣ|h|r]::����ȫ�ų�Ա�Ѿ�|cff0000ff���ӻ���|h|r"));
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 408:
	case 409://ָ����Ա���Ӽ���BUFF
	{
		if (player->GetSelectedPlayer()) {

				for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
				{
					Player* pl = itr->getSource();

					if (!pl || !pl->GetSession())
						continue;
					if (pl->IsAlive() && player->GetSelectedPlayer() == pl) {
						pl->CastSpell(pl, action == 408 ? 8299 : 21751, true);//21751  ����50%
						PSendSysMessage(player, u8"���Ѿ�Ϊ��Աcff00ff00%sh|r���ӻ���%s--", pl->GetName(), (action == 408) ? u8"����15%�˺�" : u8"����35%�˺�");
						break;
					}
				}
			}
			else {
				PSendSysMessage(player, u8"[|cffff0000ϵͳ��Ϣ|h|r]:����ѡ��һ���Ŷӳ�Ա");
			}

		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 410://����վλ
	{

		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
		{
			Player* pl = itr->getSource();

		
			if (!pl || !pl->GetSession()|| pl->IsInCombat()|| !pl->IsAlive()) {
				continue;
			}
			
			if (pl == player) {
				if(!pl->HasAura(21079))
					pl->CastSpell(pl, 21079, false);//�����ɵµĻ���
				continue;
			}
			if (pl->IsAlive() && pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl)) {
				//С��50�������ͬһ��ͼ�����ٻ�
				if (!pl->HasAura(21079))
					pl->CastSpell(pl, 21079, false);//�����ɵµĻ���
			}
			else {
				//PSendSysMessage(player, u8"�Ŷӳ�Ա[|cffff0000%s|h|r] ������Ұ��", pl->GetName());
			}
		}
		PSendSysMessage(player, u8"[|cffff0000ϵͳ��Ϣ|h|r]:����ȫ�ų�Ա�Ѿ�|cff00ff00��������⻷|h|r");
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 412://������ն���վλ
	{
		//-8577.26 1986.93 100.4
		uint32 zone, area;
		player->GetZoneAndAreaId(zone, area);
		if (zone != 3428) {
			PSendSysMessage(player, u8"��Ҫ�ڰ����������ڲ���ʹ�ô˹���");
			player->CLOSE_GOSSIP_MENU();
			break;
		}
		if(Creature* pCreature = player->FindNearestCreature(15727, 90.0f, true)) {

		}
		else {
			PSendSysMessage(player, u8"��Ҫ�ڿ��ն����丽������ʹ�ô˹���");
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
				PSendSysMessage(player, u8"�Ŷӳ�Ա[|cffff0000%s|h|r]����ս����", pl->GetName());
				continue;
			}
			if (!pl->IsAlive()) {
				continue;
			}
			uint8 memberId=	player->GetGroup()->GetMemberGroup(pl->GetGUID());
			if (pl == player) {
				pl->CastSpell(pl, 21079, false);//�����ɵµĻ���
				telePlaytoPosition(pl, memberId);
				continue;
			}

			if (pl->IsAlive() && pl->GetDistance2d(player) < sPzxConfig.GetIntDefault("raidTool.instance", 50) && player->IsWithinLOSInMap(pl)) {
				//С��50�������ͬһ��ͼ�����ٻ�
				pl->CastSpell(pl, 21079, false);//�����ɵµĻ���
				telePlaytoPosition(pl, memberId);
			}
			else {
				PSendSysMessage(player, u8"�Ŷӳ�Ա[|cffff0000%s|h|r] ������Ұ��", pl->GetName());
			}
		}
		PSendSysMessage(player, u8"����ȫ�ų�Ա�Ѿ�������|cff00ff00���ն�վλ|h|r");
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 411://ɱ�����
	{
		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
		{
			Player* pl = itr->getSource();


			if (!pl || !pl->GetSession())
				continue;
			/*if (pl->IsInCombat()) {
				PSendSysMessage(player, u8"�Ŷӳ�Ա[|cffff0000%s|h|r]����ս����", pl->GetName());
				continue;
			}*/
			if (pl->IsAlive() &&pl->GetGroup()->IsMember(player->GetGUIDLow())&& pl->GetMap()&&pl->GetMap()->IsDungeon()) {
				//ɱ���������������
				player->DealDamage(pl, pl->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
				//PSendSysMessage(player, u8"�Ŷӳ�Ա[|cff00ff00%s|h|r]�Ѿ���ɱ��",pl->GetName());
			}
		}
		//
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case 404://�ӳ�����+����
	{
		
		if (!player->CustomPlayerActionTimeCheck(sPzxConfig.GetIntDefault("timecool.pullplayer", 15), PLAYED_TIME_PULL)) {
			//PSendSysMessage(player, u8"[ϵͳ��Ϣ]:������ȴ��.");
			player->CLOSE_GOSSIP_MENU();
			break;
		}
		InstancePlayerBind *playBind=nullptr;
		DungeonPersistentState* save=nullptr;
		if (player->GetMap() && player->GetMap()->IsDungeon()) {
			playBind = player->GetBoundInstance(player->GetMapId());//�ų��Ľ�����Ϣ
			save = ((DungeonMap*)player->GetMap())->GetPersistanceState();//�ų��ĸ�����Ϣ
		}
		//std::list<Player*> reivePlayers;
		for (GroupReference* itr = player->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
		{
			Player* pl = itr->getSource();
			

			if (!pl || !pl->GetSession() )
				continue;

			if (pl == player) {//��ֹ�ų����޸����˺����ϻ�������⻷
				if (player->IsAlive()&&player->HasAura(21079))
					player->RemoveAurasDueToSpell(21079);
				continue;
			}

			// check online security
			if ( pl->IsInCombat() || pl->IsBeingTeleported()|| pl->IsTaxiFlying()) {//ս�������ڴ����в����ٴ�ʹ��
				PSendSysMessage(player, u8"�Ŷӳ�Ա[|cffff0000%s|h|r]����ս����", pl->GetName());
				continue;
			}

			uint32 teleFlags = TELE_TO_GM_MODE;

			if (save) {//�ų��ڸ�����
				uint32 instanceId = save->GetInstanceId();
				DungeonPersistentState* pl_save = nullptr;//���ѵĸ�����Ϣ
				if(pl->GetMap() && pl->GetMap()->IsDungeon())
					pl_save = ((DungeonMap*)pl->GetMap())->GetPersistanceState();//���ѵĸ�����Ϣ
				InstancePlayerBind *pl_bind = pl->GetBoundInstance(player->GetMapId());//���Ѵ浵
				//ǿ�ƺ��Դ浵��һ�µ����



				if (playBind) {//�ų��д浵
					if (!pl_bind) {//û�б�����ȣ�ǿ���л���ͼ
						teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
					}else {//����������Ľ���
						  //1.�ж��Ƿ���ͬһ����
						if (pl_bind->state->GetInstanceId() != instanceId) {//����ID��ͬ
							pl->UnbindInstance(player->GetMapId());
							pl->BindToInstance(save, true, false);
							teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
							PSendSysMessage(pl, u8"[|cffff0000ϵͳ��Ϣ|h|r]:�ų������ٻ��㡢�뾡���������CD");//ǿ�Ƴ����������ٻ�
							pl->RemoveAllSpellCooldown();
							continue;
						}
					}
				}else {//�ų��޴浵(�½���)���и���ID��û������Ϣ
					
					if (!pl_save) {//��ҵ�ǰ��ͼ�޴浵
						if (!pl_bind|| pl_bind->state->GetInstanceId() != instanceId) {//���ų��󶨵�ID��һ��
							teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
						}
					}else {//��ҵ�ǰ��ͼ�д浵���д浵��һ���и���bind�����ܴ浵����������
							if (pl_bind&&pl_bind->state->GetInstanceId() != instanceId) {//����ID��ͬ
								pl->UnbindInstance(player->GetMapId());
								teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
								PSendSysMessage(pl, u8"[|cffff0000ϵͳ��Ϣ|h|r]:�ų������ٻ��㡢�뾡���������CD");//ǿ�Ƴ����������ٻ�
								pl->RemoveAllSpellCooldown();
								continue;
							}
					}


				}
			

			}else {
				if (pl->GetMap()->IsDungeon()) {//����ڸ�����ֱ�Ӻ���
					PSendSysMessage(pl, u8"[|cffff0000ϵͳ��Ϣ|h|r]:�ų����ٻ��㡢�뾡���������CD");
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
				//С��50�������ͬһ��ͼ�����ٻ�
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
						if (!pcor) {//û�ͷţ���û��ʬ��
							if (player->IsWithinLOSInMap(pl)) {
								player->CastSpell(pl, 20770, true);
							}
							else {//������Ұ��ֱ��������
								pl->TeleportTo(player->GetMapId(), x, y, z + 2.0f, pl->GetAngle(player));
							}
							continue;
						}
				//�ͷ���,ֱ�Ӹ���������
			
						pl->ResurrectPlayer(0.99f);
						pl->SpawnCorpseBones();
						if (pl->IsAlive()) {
							player->CastSpell(pl, 7720, true);
						}
				}
				catch (const std::exception&)
				{
					//PSendSysMessage(player, u8"�ٻ���� |cffff0000[%s]|h|r ʧ��", pl->GetName());
					sLog.outError("[pzx-exception-call] (%d)-[%s]", player->GetGUIDLow(), player->GetName());
				}


			}

			//PSendSysMessage(player, u8"�ٻ���� |cff00ff00[%s]|h|r �ɹ�", pl->GetName());
		}

			PSendSysMessage(player, u8"[|cffff0000ϵͳ��Ϣ|h|r]:���Ķ����Ѿ�|cff00ff00�������|h|r");
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
	//$60001|t  �˵�������

	smatch result;
	//regex pattern("\\d{4}");	//ƥ���ĸ�����

								//����������
	string::const_iterator iterStart = name.begin();
	string::const_iterator iterEnd = name.end();
	string temp;
	while (regex_search(iterStart, iterEnd, result, pattern))
	{
		temp = result[0];
		iterStart = result[0].second;	//����������ʼλ��,����ʣ�µ��ַ���
	}
	if (temp.length() > 0) {
		string id = temp.substr(1);//��ȡ$��ȡ�ַ���
		int i_id = stoi(id);
		MenuTree _tree = sPzxMgr->getTreeByID(i_id);
		uint32 ty = _tree.type;
		if (ty == 3) {
			uint64 dbtime = sPzxMgr->getTimeLength(player->GetGUIDLow(), i_id);
			uint32 dist;
			if ((_tree.itemid + dbtime) < time(nullptr)) {//������
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
	//��Ʒ����ʱ��
	iterStart = name.begin();
	iterEnd = name.end();
	temp="";
	regex ItemIDpattern("[#]{1}\\d{2,}");
	while (regex_search(iterStart, iterEnd, result, ItemIDpattern))
	{
		temp = result[0];
		iterStart = result[0].second;	//����������ʼλ��,����ʣ�µ��ַ���
	}
	if (temp.length() > 0) {
		string id = temp.substr(1);//��ȡ$��ȡ�ַ���
		int item_id = stoi(id);

		if (!player->HasItemCount(item_id, 1, true))
			return "";//û����Ʒ����
		
		//�鿴���ݿ����Ƿ��м�¼��ʱ��
		uint64 timeDur =sPzxMgr->getTimeLength(player->GetGUIDLow(), item_id);
		if (timeDur == 0) {//û�в������ݵ�
			sPzxMgr->updateTime(item_id, player->GetGUIDLow(), accID,uint64(time(nullptr)));//���µ�ǰʱ��
			timeDur = 60 * 60 * 24 * 30;//Ĭ��30��
		}
		else {//������
			//�жϳ���
			int32 leftLen = (timeDur+ 60 * 60 * 24 * 30)- uint64(time(nullptr));
			if (leftLen<=0) {
				//�ݻ���Ʒ��
				player->DestroyItemCount(item_id, 1, true, false, true);
				sLog.out(LOG_LOOTS, u8"GUID %d �����Զ��ݻ�%d", player->GetGUIDLow(), item_id);
				PSendSysMessage(player, u8"[|cffff0000ϵͳ��Ϣ|h|r]:�����ų������Ѿ�����,�����¹���");
				name = u8"�����ų����ߵ����Ѿ�����,�����¹���";
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
		uint32 unionid = tree.unionID;//ʱ���ж�
		uint32 unioncheck = tree.unioncheck;
		uint32 raceMask = tree.racemask;
		if (raceMask && ((player->GetRaceMask()&raceMask) == 0)) {
			continue;
		}
		//sLog.outString(">>menuInfo %d %s", id, tree.name);
		string name = changeName(tree.name, player, &tree);//�˵�����
		if (name.length() > 0) {//һ�㶼�ǹ��ڻ���δ��ͨ
			if (unionid) {//�й�������
				if (unionid == 4001) {
					MenuTree unionTree = sPzxMgr->getTreeByID(unionid);
						uint64 oldVel = sPzxMgr->getTimeLength(guid, unionid);
						if (oldVel == 0) {

							if (!unioncheck) {//��һ�ο�ͨ��û��ͨ���������쳣���
								player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
							}
						}
						else {
							if (time(nullptr) > oldVel + unionTree.itemid) {//�����ˣ����¿�ͨ
								if (!unioncheck) {//�����飬�쳣���
									player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
								}
							}
							else {//û�й���
								if (unioncheck) {//�����飬�������
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
		player->ADD_GOSSIP_ITEM_EXTENDED(0, u8"���¼��ط��������Ի�����", GOSSIP_SENDER_MAIN, 999, "reload config", false);
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
	case 100://�������˵�
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
	case 1005://action=1005 ˫�츳���л�������룬Ӳ����
	{


		const talentST  readPlay = readTalents(player);
		ostringstream oss2;
		oss2 << u8"->��|cff6247c8Ŀǰ|h|rʹ���츳��";
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
		oss2 << u8"��";
		player->ADD_GOSSIP_ITEM(7, oss2.str().c_str(), GOSSIP_SENDER_MAIN, 401);

		const talentST  readdb = readFromDB(player, false);
		ostringstream oss;
		oss << u8"�л���|cff6247c8�ڶ���|h|r�츳 ��";
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
		oss << u8"��";
		player->ADD_GOSSIP_ITEM(3, oss.str().c_str(), GOSSIP_SENDER_MAIN, 402);
		player->ADD_GOSSIP_ITEM(0, u8"����|cff6247c8��ǰ�츳|h|r", GOSSIP_SENDER_MAIN, 403);
		player->ADD_GOSSIP_ITEM(0, u8"�������˵�", GOSSIP_SENDER_MAIN, 100);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
		fit = true;
		break;
	}
	case 401://�Ӳ˵�
		GossipSelect_ItemPzx(player, _item, sender, 1005, nullptr);
		fit = true;
		break;
	case 403://�����츳
		player->ResetTalents(true);
		PSendSysMessage(player, u8"[|cffff0000ϵͳ��Ϣ|h|r]:���ĵ�ǰ�츳��|cffff0000�Ѿ�����|h|r�����ٴ��츳����޸��츳����");
		player->CLOSE_GOSSIP_MENU();
		fit = true;
		break;
	case 402://�л��츳
	{
		talentST  readPlay = readFromDB(player, true);
		ostringstream oss;
		oss << u8"[|cffff0000ϵͳ��Ϣ|h|r]:|h|r�츳���л��ɹ�.��ǰ�츳�Ѿ����Ϊ";
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
		oss << u8"��";
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
		if (mmap.size() > 0) {//�����Ӳ˵�
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
			CharaMenuMap::iterator it;
			for (it = mmap.begin(); it != mmap.end(); it++) {

				uint32 id = it->first;
				MenuTree tree = it->second;
				uint32 raceMask = tree.racemask;
				if (raceMask && ((player->GetRaceMask()&raceMask) == 0)) {
					continue;
				}
				//TODO �˴������union��
				string name = changeName(tree.name, player, &tree);//�˵�����
				if (tree.popMenu) {
					player->ADD_GOSSIP_ITEM_EXTENDED(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id, "abcdefg", true);
				}
				else {
					if (tree.unionID) {
						if(sPzxMgr->getAcctType(player->GetSession()->GetAccountId(), tree.unionID) == 0)//��һ�ο�ͨ��û��ͨ�������г齱
							player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
					}
					else {
						player->ADD_GOSSIP_ITEM(tree.iconID, name.c_str(), GOSSIP_SENDER_MAIN, id);
					}
				}
			}
			player->ADD_GOSSIP_ITEM(0, u8"�������˵�", GOSSIP_SENDER_MAIN, 100);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _item->GetGUID());
			return true;
		}
		else {//�����˵�
			//0�˵�, 1���ؽڵ㣬2������Ʒ��3����ʱ����4ѧϰ����, 5���͵ص�
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
				resetIntance(player, getTree.itemid, true);//���ø���
				break;
			case 7:
			case 8:
			case 13://����������ֶ�
			case 14://����60��+T0
				simpleAction(player, getTree, action, reStr);//��㹦�ܺͽ�ɫ����
				break;
			case 15://������ֵ
			{
				if (addHoner(player, (float)(getTree.itemid), false)) {
					PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000���Ѿ�������������ȼ��ˣ�ϵͳ���Ժ��˻���������|h|r]");
				}
				else {
					simpleAction(player, getTree, action, reStr);//��㹦�ܺͽ�ɫ����
				}
			}
			break;
			case 9:
			case 10:
			case 11:
			case 12:
			case 16://ת������
			case 17://ת������

				if (simpleAction(player, getTree, action, reStr))//��㹦�ܺͽ�ɫ����
				{
					return true; //��ΪҪ���ص������б�
				}
				break;
			default:
				break;
			}

			player->CLOSE_GOSSIP_MENU();
		}
	}
	else {//û���Ӳ˵�
		player->CLOSE_GOSSIP_MENU();
	}

	return true;
}
bool simpleAction(Player *player, MenuTree & tree, uint32 action, char const* reStr) {
	uint32 accID = player->GetSession()->GetAccountId();
	uint32 mypoint = sPzxMgr->getMyPoint(accID);
	uint32 needval = tree.needval;

	if (tree.type == 16&&player->GetSession()->GetSecurity() == SEC_PLAYER&&sPzxMgr->m_GMONOFFOPTONS[GMOPTIONS_O1]) {//�������ã�1Ϊ����
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�ù���GM�ڲ�����...");
		return false;
	}

	if (player->GetLevel()<60 || player->GetTotalPlayedTime()<3600&&player->GetSession()->GetSecurity() == SEC_PLAYER) {
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�ù���ֻ������1Сʱ���ϵ�60��ҿ���");
		return false;
	}
	
	if (tree.type == 16 && reStr&&strlen(reStr) > 0 && strcmp(player->GetName(), reStr) != 0) {
		
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��������ȷ��������ƣ���ȷ��ת����Ϣ.");
		return false;
	}
	if (mypoint < needval) {//��������
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�㵱ǰ����[|cff00ff00%d|h|r]����֧��", mypoint);
		return false;
	}
	//TODO �޸����ݿⲢ������ʱ��
	if (needval == 0 || sPzxMgr->cutPoints(needval, accID)) {//�������ѵ�
		if (needval > 0)
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:ϵͳ�ۼ�[|cffff0000%d|h|r]��ʣ��[|cff00ff00%d|h|r]�����㡣", needval, sPzxMgr->getMyPoint(accID));
		switch (tree.type)
		{
		case 7:
			player->SetTaxiCheater(true);
			player->SaveToDB(true);
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��������Ѿ���ʱȫ����");
			break;
		case 8:
		{
			for (uint8 i = 0; i < PLAYER_EXPLORED_ZONES_SIZE; ++i)
			{
				player->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF); //��
				//player->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0); //��
			}
			player->SaveToDB(true);
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:���ĵ�ͼ�Ѿ�ȫ��̽�����");
		}
		break;
		case 9://������ɫ��
			CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '1' WHERE guid = '%u'", player->GetGUIDLow());
			player->SetAtLoginFlag(AT_LOGIN_RENAME);
			player->SaveToDB();
			player->GetSession()->LogoutPlayer(false);
			break;
		case 11://�����Ա�
			{
				uint8 gender = player->GetGender();
				CharacterDatabase.PExecute("UPDATE characters SET gender = '%u' WHERE guid = '%u'", gender == 0 ? 1 : 0, player->GetGUIDLow());
				//player->SaveToDB(false);
				player->GetSession()->LogoutPlayer(false);
			}
		break;
		case 16://��ɫת��
		{
				uint32 guid = player->GetGUIDLow();
				string pdump_path = sPzxConfig.GetStringDefault("pdump.path", "pdump");
				string file = pdump_path+"/" + to_string(guid)+"_"+ player->GetSession()->GetUsername();
				if (PlayerDumpWriter().WriteDump(file, guid) == DUMP_SUCCESS)
				{
					//дһ�������ļ�
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
		case 17://GM��������ת�ƽ�ɫ
		{


			if (player->GetSession()->GetSecurity() == SEC_PLAYER) {//����GMʹ��
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
				//��ʼת��
				//uint32 guid = player->GetGUIDLow();
				string file = pdump_path + "/" + to_string(guid) + "_" + name;
				if (PlayerDumpWriter().WriteDump(file, guid) == DUMP_SUCCESS)
				{
					//дһ�������ļ�
					string endfile = file + ".end";
					FILE* fout = fopen(endfile.c_str(), "w");
					if (!fout) {
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:[%u-%s] �쳣", guid, name);
						sLog.outString(u8"[ϵͳ��Ϣ]:[%u-%s] �쳣", guid, name);
						continue;
					}
					fprintf(fout, "%s\n", "ok");
					fclose(fout);

					//player->GetSession()->LogoutPlayer(false);
					player->DeleteFromDB(guid, accID, true, true);
					PSendSysMessage(player, u8"[ϵͳ��Ϣ]:[%u-%s] �ɹ�", guid, name);
					sLog.outString(u8"[ϵͳ��Ϣ]:[%u-%s]�Ѿ�ת�Ƴɹ�", guid, name);
					

				}
				else {
					sLog.outString(u8"[ϵͳ��Ϣ]:[%u-%s]�Ѿ�ת��ʧ��", guid, name);
					continue;
				}


			} while (result->NextRow());

			delete result;

			sLog.outString(">> [pzx char trancf]   %u  success", count);

	

		}
		break;
		case 12://��������
		{
			if (reStr&&strlen(reStr) > 0) {
				uint32 toCopyGuid = 0;
				Player* pl = sObjectMgr.GetPlayer(reStr);
				if (!pl) {
					PlayerCacheData *pdata = sObjectMgr.GetPlayerDataByName(reStr);
					if (pdata) {
						if (pdata->uiGender != player->GetGender() || pdata->uiRace != player->GetRace()) {
							sPzxMgr->cutPoints(0 - needval, accID);//����
							PSendSysMessage(player, u8"[ϵͳ��Ϣ]:Ŀ���ɫ�뵱ǰ��ɫ|cffff0000%s����|h|r", pdata->uiGender != player->GetGender() ? u8"�Ա�" : u8"����");
						}
						else {
							//��������
							toCopyGuid = pdata->uiGuid;

						}
					}
					else {
						sPzxMgr->cutPoints(0 - needval, accID);//����
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:δ�ҵ���Ӧ�Ľ�ɫ��");
					}
				}
				else {
					toCopyGuid = pl->GetGUIDLow();
				}
				if (toCopyGuid > 0) {

					bool change = CharacterDatabase.PExecute("update characters a join characters b on a.guid='%u' and b.guid='%u' set a.playerBytes = b.playerBytes,a.playerBytes2=b.playerBytes2", player->GetGUIDLow(), toCopyGuid);
					if (change) {
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:���ͱ���ɹ�");
						player->GetSession()->LogoutPlayer(false);
						return true;
					}
					else {
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:���ͱ��ʧ��222");
						return false;
					}
				}
				else {
					PSendSysMessage(player, u8"[ϵͳ��Ϣ]:���ͱ��ʧ��");
					return false;
				}
			}
			else {
				sPzxMgr->cutPoints(0 - needval, accID);//����
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�������ɫ����");
				return false;
			}
		}
		break;
		case 10://��������
		{
			if (reStr&&strlen(reStr) > 0) {
				uint32 race = 0;
				Player* pl = sObjectMgr.GetPlayer(reStr);
				if (!pl) {
					PlayerCacheData *pdata = sObjectMgr.GetPlayerDataByName(reStr);
					if (pdata) {
						if (pdata->uiGender != player->GetGender()) {
							sPzxMgr->cutPoints(0 - needval, accID);//����
							PSendSysMessage(player, u8"[ϵͳ��Ϣ]:Ŀ���ɫ�뵱ǰ��ɫ�Ա𲻷�");
						}
						else
							race = (pdata)->uiRace;
					}
					else {
						sPzxMgr->cutPoints(0 - needval, accID);//����
						PSendSysMessage(player, u8"[ϵͳ��Ϣ]:δ�ҵ���Ӧ�Ľ�ɫ��");
					}
				}
				else {
					race = pl->GetRace();
				}
				if (race&&player->ChangeRace(race))
				{
					return true;
					//PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�������ɹ�");
				}
				else {
					PSendSysMessage(player, u8"[ϵͳ��Ϣ]:������ʧ��");
					return false;

				}
			}
			else {
				sPzxMgr->cutPoints(0 - needval, accID);//����
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�������ɫ����");
				return false;
			}
		}
		break;
		case 13://��������ȼ�
		{
			if (player->GetPet() && player->GetPet()->getPetType() == HUNTER_PET) {
				uint32 maxlevel = std::min(sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL), player->GetLevel());
				Pet* HunterPet = player->GetPet();
				if (HunterPet->GetLevel() < maxlevel || HunterPet->GetLoyaltyLevel() < LoyaltyLevel(BEST_FRIEND)) {

					//player->ADD_GOSSIP_ITEM(3, u8"���� �ҵĳ����ҳ϶Ⱥ͵ȼ� ", GOSSIP_SENDER_MAIN, 205);
					player->GetPet()->GivePetXP(99999999);
					player->GetPet()->ModifyLoyalty(1000000.0);
				}
				else {
					PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cff0000ff ���ĳ����ҳ϶��Ѿ��������!|h|r");
				}
			}
			else {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:����|cffff0000 ѱ�������ٻ���|h|rһֻҪǿ���ĳ���");
			}
		}

		break;
		case 14://���-������60��+T0��װ
		{
			//player->LearnSpell(33389, false);
			ObjectGuid target_guid;
			if (player->GetLevel() < 60) {
				player->GiveLevel(60);
				player->InitTalentForLevel();
			}
			player->SetUInt32Value(PLAYER_XP, 0);
			player->UpdateSkillsToMaxSkillsForLevel();
			addItemSet(player, 0);//����T1��װ
		}

		break;
		case 15://���������ȼ�
		{
			if (!addHoner(player, (float)(tree.itemid), true)) {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000������|h|r]");
				sPzxMgr->cutPoints(0 - needval, accID);//����
			}
			else {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000���������ȼ��Ѿ�����,�뵽�������鿴�ȼ�|h|r");
			}
		}
		default:
			break;
		}

	}
	else {//�ۼ��쳣
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000��������ϵͳ�쳣������ϵGM-1008|h|r]");
	}

	return true;

}

bool modifyTimeLength(Player *player, uint32 timelength, uint32 needval, uint32 action) {
	const char* txt;
	
	switch (action)
	{
	case 4001:
		txt = u8"˫�츳����";
		break;
		
	default:
		txt = u8"˲�ɹ���";
		break;
	}
	uint32 accID = player->GetSession()->GetAccountId();
	uint32 guid = player->GetGUIDLow();
	uint64 oldVel = sPzxMgr->getTimeLength(guid, action);

	if (oldVel) {
		//�޸����ݿ⣬���ĳ���
		if (time(nullptr) > oldVel + timelength) {//�����ˣ����¿�ͨ

			uint32 mypoint = sPzxMgr->getMyPoint(accID);

			if (mypoint < needval) {//��������
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�㵱ǰ����[|cffff0000%d|h|r]����֧��", mypoint);
				return false;
			}
			//TODO �޸����ݿⲢ������ʱ��
			if (sPzxMgr->cutPoints(needval, accID) && sPzxMgr->updateTime(action, guid, accID,  uint64(time(nullptr)))) {
	
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:ϵͳ�ۼ�[|cffff0000%d|h|r]��ʣ��[|cff00ff00%d|h|r]������", needval, sPzxMgr->getMyPoint(accID));
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:����%s����ɹ�", txt);
			}
			else {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000��������ϵͳ�쳣������ϵGM-1004|h|r");
			}

		}
		else {//û����
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:����%sʱ�仹����Ч��", txt);
		}
		time_t mutetime = time(nullptr) + timelength;//2�����ˢ�£�һ�㹫�᲻����������2��
	}
	else {//��û��ͨ������һ�ο�ͨ
		uint32 mypoint = sPzxMgr->getMyPoint(accID);

		if (mypoint < needval) {//��������
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�㵱ǰ����[|cffff0000%d|h|r]����֧��", mypoint);
			return false;
		}
		//TODO �޸����ݿⲢ������ʱ��
		if (sPzxMgr->cutPoints(needval, accID) && sPzxMgr->updateTime(action, guid, accID, uint64(time(nullptr)))) {

			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:ϵͳ�ۼ�[|cffff0000%d|h|r]��ʣ��[|cff00ff00%d|h|r]������", needval, sPzxMgr->getMyPoint(accID));
			if (action==4001) {
				talentST  readPlay = readFromDB(player, true);
				//PSendSysMessage(player, u8"|cffff0000[ϵͳ��Ϣ]:|h|r���ĵ�2���츳�����Ѿ���ͨ�����ڿ���ͨ��[|cffeb8920ų�����|h|r]�����µ��츳��");
				//player->CLOSE_GOSSIP_MENU();
			}
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:����%s��ͨ�ɹ�", txt);
		}
		else {
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:|cffff0000��������ϵͳ�쳣������ϵGM-1005|h|r");
		}
	}
	return true;
}

bool teleToPoint(Player *player, string xyz, uint32 needval) {
	uint32 accID = player->GetSession()->GetAccountId();
	uint32 mypoint = sPzxMgr->getMyPoint(accID);



	if (mypoint < needval) {//�����㹻
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�㵱ǰ����[|cff00ff00%d|h|r]����֧��", mypoint);
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
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:ϵͳ�ۼ�[|cffff0000%d|h|r]��ʣ��[|cff00ff00%d|h|r]������", needval, sPzxMgr->getMyPoint(accID));
		return player->TeleportTo(mapid, x, y, z, 0.0f);
	}
	else
	{
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�㵱ǰ����[|cff00ff00%d|h|r]����֧��", mypoint);
		return false;
	}
}

bool cutPointAndlearnSpell(Player *pPlayer, uint32 skill, uint32 needval, uint32 action) {
	if (pPlayer->GetFreePrimaryProfessionPoints() == 0 && !(skill == 185 || skill == 129))
	{//��ħ�ͼ���
		PSendSysMessage(pPlayer, u8"[ϵͳ��Ϣ]:���Ѿ�ѧϰ��2��רҵ������.");
		return false;
	}

	if (!pPlayer->HasSkill(skill) || pPlayer->GetSkillValue(skill) < 300) {
		//ChatHandler handler(pPlayer->GetSession());
		//�������Ƿ�
		uint32 accID = pPlayer->GetSession()->GetAccountId();
		uint32 mypoint = sPzxMgr->getMyPoint(accID);

		if (mypoint >= needval) {//�����㹻
			SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
			if (!SkillInfo)
			{
				PSendSysMessage(pPlayer, u8"[ϵͳ��Ϣ]:��������ϵͳ�쳣������ϵGM-1001");
				sLog.outError("Profession NPC: received non-valid skill ID %d" + skill);
				return false;
			}
			if (sPzxMgr->cutPoints(needval, accID)) {
				PSendSysMessage(pPlayer, u8"[ϵͳ��Ϣ]:ϵͳ�ۼ�[|cffff0000%d|h|r]��ʣ��[|cff00ff00%d|h|r]������", needval, sPzxMgr->getMyPoint(accID));
				pPlayer->SetSkill(SkillInfo->id, 300, 300);
				LearnSkillRecipesHelper(pPlayer, SkillInfo->id);
				char* skill_name = SkillInfo->name[sWorld.GetDefaultDbcLocale()];
				PSendSysMessage(pPlayer, u8"���� %s �䷽�Ѿ�ѧϰ���", skill_name);
				return true;
			}
			else {
				PSendSysMessage(pPlayer, u8"[ϵͳ��Ϣ]:2��������ϵͳ�쳣������ϵGM-1002");

			}
		}
		else {
			PSendSysMessage(pPlayer, u8"[ϵͳ��Ϣ]:�㵱ǰ����[|cff00ff00%d|h|r]����֧��", mypoint);
		}
		return false;
	}
	else {
		PSendSysMessage(pPlayer, u8"[ϵͳ��Ϣ]:���Ѿ�������רҵ");
		return false;
	}
	//PSendSysMessage(pPlayer, u8"ϵͳ����.");


}
void outputItemColorFormat(Player *player, ostringstream &itemret, uint32 itemID, const char *format) {
	ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemID);
	if (pProto) {
		const char * colorItemPre = ItemQualityColorsStr[pProto->Quality];
		itemret << colorItemPre << pProto->Name1 << "|h|r";
		PSendSysMessage(player, format, itemret.str().c_str());
	}
	else {
		PSendSysMessage(player, u8"δ�ҵ���ƷID-%d", itemID);
	}
	itemret.str(std::string());
}

bool cutPointAndAddItem(Player *player, uint32 items, uint32 needval, uint32 count) {
	ostringstream outs;
	ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(items);
	if (player->HasItemCount(items, 1, true) && pProto->MaxCount == 1) {//Ψһ��Ʒ���
		outputItemColorFormat(player, outs, items, u8"[ϵͳ��Ϣ]:%s�Ѿ�����Ʒ�嵥��");
		return true;
	}
	ItemPosCountVec dest;
	InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, items, items == 49999?1:count);
	if (msg == EQUIP_ERR_OK)
	{
		//�������Ƿ�
		uint32 accID = player->GetSession()->GetAccountId();
		uint32 mypoint = sPzxMgr->getMyPoint(accID);

		if (mypoint >= needval) {//�����㹻
			Item* item = player->StoreNewItem(dest, items, true);
			if (sPzxMgr->cutPoints(needval, accID)) {
				sLog.out(LOG_LOOTS, "Account %u guid %u : getitem from zz %u", accID , player->GetGUIDLow(),items);
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:ϵͳ�ۼ�[|cffff0000%d|h|r]��ʣ��[|cff00ff00%d|h|r]������", needval, sPzxMgr->getMyPoint(accID));
				player->SendNewItem(item, 1, true, false);
				ItemPrototype const* pProto = item->GetProto();
				if (pProto&&pProto->RequiredSkill == 762) {//��Ʒ�������Զ�ѧϰ����
					player->LearnSpell(33389, false);
					player->LearnSpell(33392, false);
				}
				if (items == 49999) {//�ų�������Ҫ����ʱ��
					uint64 leftTime = uint64(time(nullptr)) - (30 - count) * 60 * 60 * 24;
					sPzxMgr->updateTime(49999, player->GetGUIDLow(), accID, leftTime);
					//����contʱ������,���contenС��30��������ݿ��¼ʱ���Ƴ�
					
					//ʱ�䳤��
				}
				if (items == 60003) {//������飬����һ����¼��֤�������
					sPzxMgr->updateTime(items, player->GetGUIDLow(), accID,uint64(time(nullptr)));
				}
				outputItemColorFormat(player, outs, items, u8"[ϵͳ��Ϣ]:%s�Ѿ���ӵ��������");
				player->SaveToDB(true);//������Ʒ
			}
			else {
				PSendSysMessage(player, u8"[ϵͳ��Ϣ]:��������ϵͳ�쳣������ϵGM-1003");
			}

		}
		else {
			PSendSysMessage(player, u8"[ϵͳ��Ϣ]:�㵱ǰ����[|cff00ff00%d|h|r]����֧��", mypoint);
		}

	}
	else
	{
		player->SendEquipError(msg, nullptr, nullptr, items);
		PSendSysMessage(player, u8"[ϵͳ��Ϣ]:���޷�ӵ�и���Ʒ,��������ռ��װ��Ψһ��");
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
				player->CastSpell(target, 698, true);//˲���ٻ�
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
		sLog.outError(u8"δ�ҵ�pzx.conf");
	}
	sPzxMgr->Reload();
	sPzxMgr->startSynDB();//������ʱ����
	Script* newscript;

	newscript = new Script;
	newscript->Name = "custom_teleport_npc_pzx";//������AI-60004
	newscript->pGossipHelloPzx = &GossipHello_ItemPzx;
	newscript->pGossipSelectPzx = &GossipSelect_ItemPzx;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "custom_teleport_npc_pzx_manager";//�ų�����-49999
	newscript->pGossipHelloPzx = &GossipHello_ItemPzxManager;
	newscript->pGossipSelectPzx = &GossipSelect_ItemPzxManager;
	newscript->RegisterSelf(false);


	newscript = new Script;
	newscript->Name = "custom_axns_item";//50001-����ŵ˹��Ƭ�ϳ�
	newscript->pGossipHelloPzx = &GossipHello_axnsItemManager;
	newscript->pGossipSelectPzx = &GossipSelect_axnsItemPzxManager;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "pzx_gob_meeting";//����ʯAI-gameobject
	newscript->GOGetAI = &GetAI_go_metting_door;
	//newscript->pQuestRewardedGO = &QuestRewarded_scarab_gong;
	//newscript->pGOGossipHello = &GOHello_scarab_meeting;
	newscript->RegisterSelf();

}
