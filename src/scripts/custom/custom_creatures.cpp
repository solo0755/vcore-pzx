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




bool GossipHello_TeleportNPC(Player *player, Creature *_Creature)
{
	player->PrepareGossipMenu(_Creature, 60002);
	player->ADD_GOSSIP_ITEM(0, u8"提升到40级-学习骑术", GOSSIP_SENDER_MAIN, 121);
	MainMenu(player, _Creature->GetGUID());
	if (sPzxConfig.GetIntDefault("openBuff", 1)) {
		player->ADD_GOSSIP_ITEM(0, u8"给我加满|cff6247c8世界BUFF|h|r", GOSSIP_SENDER_MAIN, 102);
	}
	//sPzxConfig.GetIntDefault("initItemSet", 1);
	player->ADD_GOSSIP_ITEM(0, u8"免费提升到60级+初始套装", GOSSIP_SENDER_MAIN, 122);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _Creature->GetGUID());
	return true;
}

bool GossipHello_xiaoyizi(Player *player, Creature *_Creature)
{
	if (_Creature->GetOwnerGuid() != player->GetObjectGuid()) {
		PSendSysMessage(player, u8"[系统消息]:|cffff0000这个%s不属于您哦~v~|h|r", _Creature->GetName());
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	player->PrepareGossipMenu(_Creature, 60001);
	player->ADD_GOSSIP_ITEM(0, u8"传送到->|cff6247c8黑翼之巢|h|r", GOSSIP_SENDER_MAIN, 101);
	player->ADD_GOSSIP_ITEM(0, u8"传送到->|cff6247c8安其拉神殿|h|r", GOSSIP_SENDER_MAIN, 102);
	player->ADD_GOSSIP_ITEM(0, u8"传送到->|cff6247c8纳克萨玛斯|h|r", GOSSIP_SENDER_MAIN, 103);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _Creature->GetGUID());
	return true;
}

bool GossipHello_huanhua(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, u8"神器样品-属性展示", GOSSIP_SENDER_MAIN, 62002);
	player->ADD_GOSSIP_ITEM(0, u8"幻化模型->|cff6247c8高阶督军武器库|h|r", GOSSIP_SENDER_MAIN, 62003);
	player->ADD_GOSSIP_ITEM(0, u8"幻化模型->|cff6247c8大元帅武器库|h|r", GOSSIP_SENDER_MAIN, 62004);
	player->ADD_GOSSIP_ITEM(0, u8"幻化模型->|cff6247c8极品装备库|h|r", GOSSIP_SENDER_MAIN, 62005);
	player->ADD_GOSSIP_ITEM(0, u8"幻化模型->|cff6247c8 R14套装库|h|r", GOSSIP_SENDER_MAIN, 62006);
	player->ADD_GOSSIP_ITEM(0, u8"幻化模型->|cff6247c8 光荣、巨神、翡翠、海盗套装|h|r", GOSSIP_SENDER_MAIN, 62007);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _Creature->GetGUID());
	return true;
}



void LearnSkillRecipesHelper(Player *player, uint32 skill_id)
{
	uint32 classmask = player->GetClassMask();

	for (uint32 j = 0; j < sObjectMgr.GetMaxSkillLineAbilityId(); ++j)
	{
		SkillLineAbilityEntry const *skillLine = sObjectMgr.GetSkillLineAbility(j);
		if (!skillLine)
			continue;

		// wrong skill
		if (skillLine->skillId != skill_id)
			continue;

		// not high rank
		if (skillLine->forward_spellid)
			continue;

		// skip racial skills
		if (skillLine->racemask != 0)
			continue;

		// skip wrong class skills
		if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
			continue;

		SpellEntry const* spellEntry = sSpellMgr.GetSpellEntry(skillLine->spellId);
		if (!spellEntry || !SpellMgr::IsSpellValid(spellEntry, player, false))
			continue;

		player->LearnSpell(skillLine->spellId, false);
	}
}
bool LearnAllRecipesInProfession(Player *pPlayer, SkillType skill)
{
	ChatHandler handler(pPlayer->GetSession());
	char* skill_name;

	SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
	skill_name = SkillInfo->name[sWorld.GetDefaultDbcLocale()];

	if (!SkillInfo)
	{
		sLog.outError("Profession NPC: received non-valid skill ID");
		return false;
	}
	pPlayer->SetSkill(SkillInfo->id, 300, 300);
	LearnSkillRecipesHelper(pPlayer, SkillInfo->id);
	PSendSysMessage(pPlayer, u8"所有 %s 配方已经学习完成", skill_name);
	return true;
}

bool GossipHello_ProfessionNPC(Player* player, Creature* creature)
{

	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"炼金术", GOSSIP_SENDER_MAIN, 1);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"锻造", GOSSIP_SENDER_MAIN, 2);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"制皮", GOSSIP_SENDER_MAIN, 3);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"裁缝", GOSSIP_SENDER_MAIN, 4);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"工程学", GOSSIP_SENDER_MAIN, 5);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"附魔", GOSSIP_SENDER_MAIN, 6);
	//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Jewelcrafting",      GOSSIP_SENDER_MAIN, 7);
	//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Inscription",        GOSSIP_SENDER_MAIN, 8);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"草药", GOSSIP_SENDER_MAIN, 9);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"剥皮", GOSSIP_SENDER_MAIN, 10);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"采矿", GOSSIP_SENDER_MAIN, 11);

	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"急救", GOSSIP_SENDER_MAIN, 12);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"钓鱼", GOSSIP_SENDER_MAIN, 13);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, u8"烹饪", GOSSIP_SENDER_MAIN, 14);

	player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
	return true;

}
void CompleteLearnProfession(Player *pPlayer, Creature *pCreature, SkillType skill)
{
	if (pPlayer->GetFreePrimaryProfessionPoints() == 0 && !(skill == SKILL_COOKING || skill == SKILL_FIRST_AID))
	{
		PSendSysMessage(pPlayer, u8"[系统消息]:你已经学习了2项专业技能了.");
	}
	else
	{
		if (!LearnAllRecipesInProfession(pPlayer, skill))
			PSendSysMessage(pPlayer, u8"系统错误.");
	}
}
bool GossipSelect_ProfessionNPC(Player* player, Creature* creature, uint32 sender, const uint32 action)
{
	switch (action)
	{
	case 1:
		if (!player->HasSkill(SKILL_ALCHEMY))
			CompleteLearnProfession(player, creature, SKILL_ALCHEMY);
		break;
	case 2:
		if (!player->HasSkill(SKILL_BLACKSMITHING))
			CompleteLearnProfession(player, creature, SKILL_BLACKSMITHING);
		break;
	case 3:
		if (!player->HasSkill(SKILL_LEATHERWORKING))
			CompleteLearnProfession(player, creature, SKILL_LEATHERWORKING);
		break;
	case 4:
		if (!player->HasSkill(SKILL_TAILORING))
			CompleteLearnProfession(player, creature, SKILL_TAILORING);
		break;
	case 5:
		if (!player->HasSkill(SKILL_ENGINEERING))
			CompleteLearnProfession(player, creature, SKILL_ENGINEERING);
		break;
	case 6:
		if (!player->HasSkill(SKILL_ENCHANTING))
			CompleteLearnProfession(player, creature, SKILL_ENCHANTING);
		break;
	case 7:
	case 8:
		break;
	case 9:
		if (!player->HasSkill(SKILL_HERBALISM))
			CompleteLearnProfession(player, creature, SKILL_HERBALISM);
		break;
	case 10:
		if (!player->HasSkill(SKILL_SKINNING))
			CompleteLearnProfession(player, creature, SKILL_SKINNING);
		break;
	case 11:
		if (!player->HasSkill(SKILL_MINING))
			CompleteLearnProfession(player, creature, SKILL_MINING);
		break;
	case 12:
		if (!player->HasSkill(SKILL_FIRST_AID))
			CompleteLearnProfession(player, creature, SKILL_FIRST_AID);
		break;
	case 13:
		if (!player->HasSkill(SKILL_FISHING))
			CompleteLearnProfession(player, creature, SKILL_FISHING);
		break;
	case 14:
		if (!player->HasSkill(SKILL_COOKING))
			CompleteLearnProfession(player, creature, SKILL_COOKING);
		break;
	}

	player->CLOSE_GOSSIP_MENU();
	return true;
}

void SendListInventory(Player * _player,Creature *_Creature, uint32 vendorID)
{
	/*DEBUG_LOG("WORLD: Sent SMSG_LIST_INVENTORY");

	Creature* pCreature = GetPlayer()->GetNPCIfCanInteractWith(vendorguid, UNIT_NPC_FLAG_VENDOR);

	if (!pCreature)
	{
		DEBUG_LOG("WORLD: SendListInventory - %s not found or you can't interact with him.", vendorguid.GetString().c_str());
		_player->SendSellError(SELL_ERR_CANT_FIND_VENDOR, nullptr, ObjectGuid(), 0);
		return;
	}*/


	ObjectGuid vendorguid = _Creature->GetObjectGuid();
	VendorItemData const* vItems = sObjectMgr.GetNpcVendorItemList(vendorID);
	VendorItemData const* tItems = sObjectMgr.GetNpcVendorTemplateItemList(vendorID);

	if (!vItems && !tItems)
	{
		WorldPacket data(SMSG_LIST_INVENTORY, (8 + 1 + 1));
		data << ObjectGuid(vendorguid);
		data << uint8(0);                                   // count==0, next will be error code
		data << uint8(0);                                   // "Vendor has no inventory"
		_player->GetSession()->SendPacket(&data);
		return;
	}

	uint8 customitems = vItems ? vItems->GetItemCount() : 0;
	uint8 numitems = customitems + (tItems ? tItems->GetItemCount() : 0);

	uint8 count = 0;

	WorldPacket data(SMSG_LIST_INVENTORY, (8 + 1 + numitems * 7 * 4));
	data << ObjectGuid(vendorguid);

	size_t count_pos = data.wpos();
	data << uint8(count);

	//float discountMod = _player->GetReputationPriceDiscount(pCreature);
	float discountMod = 0.1f;//强制最低折扣

	for (int i = 0; i < numitems; ++i)
	{
		VendorItem const* crItem = i < customitems ? vItems->GetItem(i) : tItems->GetItem(i - customitems);

		if (crItem)
		{
			if (ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(crItem->item))
			{
				if (!_player->IsGameMaster())
				{
					// class wrong item skip only for bindable case
					if ((pProto->AllowableClass & _player->GetClassMask()) == 0 && pProto->Bonding == BIND_WHEN_PICKED_UP)
						continue;

					// race wrong item skip always
					if ((pProto->AllowableRace & _player->GetRaceMask()) == 0)
						continue;

					// when no faction required but rank > 0 will be used faction id from the vendor faction template to compare the rank
					//if (!pProto->RequiredReputationFaction && pProto->RequiredReputationRank > 0 &&
						//ReputationRank(pProto->RequiredReputationRank) > _player->GetReputationRank(pCreature->getFactionTemplateEntry()->faction))
						//continue;
				}

				++count;

				// reputation discount
				uint32 price = uint32(floor(pProto->BuyPrice * discountMod));

				data << uint32(count);
				data << uint32(crItem->item);
				data << uint32(pProto->DisplayInfoID);
				data << uint32(0xFFFFFFFF);
				data << uint32(price);
				data << uint32(pProto->MaxDurability);
				data << uint32(pProto->BuyCount);
			}
		}
	}

	if (count == 0)
	{
		data << uint8(0);                                   // "Vendor has no inventory"
		_player->GetSession()->SendPacket(&data);
		return;
	}

	data.put<uint8>(count_pos, count);
	_player->GetSession()->SendPacket(&data);
}


bool GossipSelect_huanhua(Player *player, Creature *_Creature, uint32 sender, uint32 action) {
	SendListInventory(player, _Creature, action);
	return true;
}

bool GossipSelect_xiaoyizi(Player *player, Creature *_Creature, uint32 sender, uint32 action) {
	if (action < 20) {//系统菜单
		player->OnGossipSelect(_Creature, 0);
		switch (action)
		{
		case GOSSIP_OPTION_BANKER:
			player->GetSession()->SendShowBank(_Creature->GetGUID());
			break;
		case GOSSIP_OPTION_STABLEPET:
			player->GetSession()->SendStablePet(_Creature->GetGUID());
			break;
		case GOSSIP_OPTION_VENDOR:
		case GOSSIP_OPTION_ARMORER:
			player->GetSession()->SendListInventory(_Creature->GetGUID(), VENDOR_MENU_ALL);
			break;
		case GOSSIP_OPTION_AUCTIONEER:
			player->GetSession()->SendAuctionHello(_Creature);
			break;
		case GOSSIP_OPTION_TRAINER:
			player->GetSession()->SendTrainerList(_Creature->GetGUID());
			break;
		}
		return true;

	}
	if (action == 101) {//传送黑翼之巢
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(469, -7665.55f, -1102.49f, 400.679f, 0.0f);
	}

	if (action == 102)// Temple of Ahn'Qiraj
	{
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(1, -8245.837891f, 1983.736206f, 129.071686f, 0.936195f);
	}
	if (action == 103)// Naxxramas
	{
		player->CLOSE_GOSSIP_MENU();
		player->TeleportTo(0, 3121.061768f, -3689.973389f, 133.458786f, 0.0f);
	}
	return true;
}
bool GossipSelect_TeleportNPC(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
	if (action < 20) {//系统菜单
		player->OnGossipSelect(_Creature, 0);
		switch (action)
		{
		case GOSSIP_OPTION_VENDOR:
		case GOSSIP_OPTION_ARMORER:
			player->GetSession()->SendListInventory(_Creature->GetGUID(), VENDOR_MENU_ALL);
			break;
		case GOSSIP_OPTION_TRAINER:
			player->GetSession()->SendTrainerList(_Creature->GetGUID());
			break;
		}
		return true;

	}
	if (action >= 20000) {//附魔的菜单，20000~30000+
		GossipSelect_Enchant(player, _Creature->GetGUID(), sender, action);
	}
	if (action == 100) {//主菜单
		GossipHello_TeleportNPC(player, _Creature); //再次显示,不关闭
		return true;
	}
	else
	{
		if (sender == GOSSIP_SENDER_MAIN) {
			Menu_Teleport(player, _Creature->GetGUID(), action);
		}
	}

	return true;
}
/*
* Custom premade gear and spec scripts
*/

#define SPELL_LIGHTNING_VISUAL 24240

bool GossipHello_PremadeGearNPC(Player* player, Creature* creature)
{
    for (auto itr : sObjectMgr.GetPlayerPremadeGearTemplates())
    {
        if (itr.second.requiredClass == player->GetClass())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, itr.second.name.c_str(), GOSSIP_SENDER_MAIN, itr.first);
        }
    }

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
    return true;
}

bool GossipSelect_PremadeGearNPC(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    player->SendSpellGo(player, SPELL_LIGHTNING_VISUAL);
    sObjectMgr.ApplyPremadeGearTemplateToPlayer(action, player);
    player->CLOSE_GOSSIP_MENU();
    return true;
}

bool GossipHello_PremadeSpecNPC(Player* player, Creature* creature)
{
    for (auto itr : sObjectMgr.GetPlayerPremadeSpecTemplates())
    {
        if (itr.second.requiredClass == player->GetClass())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, itr.second.name.c_str(), GOSSIP_SENDER_MAIN, itr.first);
        }
    }

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
    return true;
}

bool GossipSelect_PremadeSpecNPC(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    player->SendSpellGo(player, SPELL_LIGHTNING_VISUAL);
    sObjectMgr.ApplyPremadeSpecTemplateToPlayer(action, player);
    player->CLOSE_GOSSIP_MENU();
    return true;
}

/*
* Custom training dummy script
*/

struct npc_training_dummyAI : ScriptedAI
{
	explicit npc_training_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		npc_training_dummyAI::Reset();
	}

	uint32 m_uiCombatTimer;
	std::unordered_map<ObjectGuid, time_t> attackers;

	void Reset() override
	{
		m_uiCombatTimer = 10000;
		attackers.clear();
	}

	void AttackStart(Unit* /*pWho*/) override {}

	void Aggro(Unit* pWho) override
	{
		SetCombatMovement(false);
	}

	void AddAttackerToList(Unit* pWho)
	{
		auto itr = attackers.find(pWho->GetObjectGuid());
		if (itr != attackers.end())
		{
			itr->second = std::time(nullptr);
		}
		else
		{
			attackers.emplace(pWho->GetObjectGuid(), std::time(nullptr));
		}
	}

	void DamageTaken(Unit* pWho, uint32& /*uiDamage*/) override
	{
		if (pWho)
			AddAttackerToList(pWho);
	}

	void SpellHit(Unit* pWho, SpellEntry const* /*pSpell*/) override
	{
		if (pWho)
			AddAttackerToList(pWho);
	}

	void UpdateAI(uint32 const diff) override
	{
		if (m_creature->IsInCombat())
		{
			if (m_uiCombatTimer <= diff)
			{
				for (auto itr = attackers.begin(); itr != attackers.end();)
				{
					Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first);

					if (!pAttacker || !pAttacker->IsInWorld())//TODO 或者距离大于50码
					{
						itr = attackers.erase(itr);
						continue;
					}
					if (pAttacker->GetDistance(m_creature) > 48.0f) {//如果目标大于50码
						m_creature->_removeAttacker(pAttacker);
						m_creature->GetThreatManager().modifyThreatPercent(pAttacker, -101.0f);
						itr = attackers.erase(itr);
						continue;
					}

					if (itr->second + 9 < std::time(nullptr))//自己9秒钟未攻击，但是别人还在继续攻击，就清空自己的仇恨值为负数，脱离战斗
					{
						m_creature->_removeAttacker(pAttacker);
						m_creature->GetThreatManager().modifyThreatPercent(pAttacker, -101.0f);
						itr = attackers.erase(itr);
						continue;
					}
					++itr;
				}

				if (m_creature->GetThreatManager().isThreatListEmpty())
					EnterEvadeMode();

				m_uiCombatTimer = 10000;
			}
			else
				m_uiCombatTimer -= diff;
		}
	}
};

CreatureAI* GetAI_npc_training_dummy(Creature* pCreature)
{
	return new npc_training_dummyAI(pCreature);
}

struct npc_summon_debugAI : ScriptedAI
{
	uint32 m_maxSummonCount = 200;
	uint32 m_summonCount = 0;
	Creature *m_summons[200];

	npc_summon_debugAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		m_summonCount = 0;
		for (uint32 i = 0; i < m_maxSummonCount; ++i)
			m_summons[i] = nullptr;

		Reset();
	}

	void Reset() override
	{
		m_summonCount = 0;
		for (uint32 i = 0; i < m_maxSummonCount; ++i)
		{
			if (m_summons[i])
				((TemporarySummon*)m_summons[i])->UnSummon();

			m_summons[i] = nullptr;
		}
	}


	void JustDied(Unit* /* killer */) override
	{
		Reset();
	}

	void UpdateAI(uint32 const diff) override
	{
		if (!m_creature->GetVictim())
			return;

		if (m_summonCount >= m_maxSummonCount)
			return;

		m_summons[m_summonCount++] = m_creature->SummonCreature(12458, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0);
	}

};

CreatureAI* GetAI_custom_summon_debug(Creature *creature)
{
	return new npc_summon_debugAI(creature);
}

void AddSC_custom_creatures()
{

	Script* newscript;

	newscript = new Script;//新手接待功能
	newscript->Name = "custom_teleport_npc";
	newscript->pGossipHello = &GossipHello_TeleportNPC;
	newscript->pGossipSelect = &GossipSelect_TeleportNPC;
	newscript->RegisterSelf(false);

	newscript = new Script;//小姨子功能
	newscript->Name = "custom_xiaoyizi_npc";
	newscript->pGossipHello = &GossipHello_xiaoyizi;
	newscript->pGossipSelect = &GossipSelect_xiaoyizi;
	newscript->RegisterSelf(false);


	newscript = new Script;//幻化展示大师
	newscript->Name = "custom_huanhua_npc";
	newscript->pGossipHello = &GossipHello_huanhua;
	newscript->pGossipSelect = &GossipSelect_huanhua;
	newscript->RegisterSelf(false);
	//newscript = new Script;
	//newscript->Name = "custom_enchant_npc";
	//newscript->pGossipHello = &GossipHello_EnchantNPC;
	//newscript->pGossipSelect = &GossipSelect_EnchantNPC;
	//newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "custom_professions_npc";
	newscript->pGossipHello = &GossipHello_ProfessionNPC;
	newscript->pGossipSelect = &GossipSelect_ProfessionNPC;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "custom_premade_gear_npc";
	newscript->pGossipHello = &GossipHello_PremadeGearNPC;
	newscript->pGossipSelect = &GossipSelect_PremadeGearNPC;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "custom_premade_spec_npc";
	newscript->pGossipHello = &GossipHello_PremadeSpecNPC;
	newscript->pGossipSelect = &GossipSelect_PremadeSpecNPC;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "custom_npc_training_dummy";
	newscript->GetAI = &GetAI_npc_training_dummy;
	newscript->RegisterSelf(false);

	newscript = new Script;
	newscript->Name = "custom_npc_summon_debugAI";
	newscript->GetAI = &GetAI_custom_summon_debug;
	newscript->RegisterSelf(false);


}
