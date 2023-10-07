#include "PzxMgr.h"
#include "Policies/SingletonImp.h"
#include "ace/Thread_Manager.h"
//#include "ace/OS_NS_time.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "Config/PzxConfig.h"
#include "ace/Reactor.h"
#include "ace/Event_Handler.h"
//#include "MangosSocketMgrImpl.h"

//template class PzxMgr<WorldSocket>;
INSTANTIATE_SINGLETON_1(PzxMgr);

class MyTimerHandler : public ACE_Event_Handler
{
public:
	PzxMgr * pzxMgr;
	MyTimerHandler(PzxMgr * pzxmgr) {
		pzxMgr = pzxmgr;
	}
	int handle_timeout(const ACE_Time_Value &current_time,
		const void * = 0)
	{
		time_t epoch = ((timespec_t)current_time).tv_sec;
		ACE_DEBUG((LM_INFO, ACE_TEXT("handle_timeout: %s\n"), ctime(&epoch)));
		pzxMgr->loadAllPonits();
		return 0;
	}
};

void PzxMgr::thread_start(PzxMgr* arg)
{
	MyTimerHandler * timer = new MyTimerHandler(arg);
	ACE_Time_Value initialDelay(20);  //��ʼ��ʱʱ��
	ACE_Time_Value interval(10);      //���ʱ��
	ACE_Reactor::instance()->schedule_timer(timer,
		0,
		initialDelay,
		interval);
	ACE_Reactor::instance()->run_reactor_event_loop();


}
PzxMgr* PzxMgr::Instance()
{

	return ACE_Singleton<PzxMgr, ACE_Thread_Mutex>::instance();
	//ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)PzxMgr::thread_start,PPP);
	//return PPP;
}


void PzxMgr::startSynDB() {
	ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)PzxMgr::thread_start, sPzxMgr);
}
bool PzxMgr::Reload()
{
	treeMenu.clear();
	allMenu.clear();
	accountPMap.clear();

	tfTimeMap.clear();
	acctTypeMap.clear();

	loadAllPonits();
	loadAllTimes();
	//treeMenu = loadAllMenu(0);
	return true;
}


void PzxMgr::loadAllTimes()
{

	uint32 count = 0;

	QueryResult* result = CharacterDatabase.PQuery("SELECT guid,type,value,accid FROM _character_extra ");
	if (!result)
	{
		BarGoLink bar(1);
		bar.step();

		sLog.outString();
		sLog.outString(">> Loaded 0 creature respawn time.");
	}

	BarGoLink bar(result->GetRowCount());
	//CharaPointMap unMap;
	do
	{
		bar.step();
		Field* fields = result->Fetch();

		uint32 guid = fields[0].GetUInt32();
		uint32 type = fields[1].GetUInt32();
		uint64 val = fields[2].GetUInt64();
		uint64 accid = fields[3].GetUInt64();
		if (accid > 0 && acctTypeMap.find(accid) != acctTypeMap.end()) {//��ȡ����ID��ֵ
			acctTypeMap[accid].insert(type);
		}
		else {
			std::set<uint32> tps;
			tps.insert(type);
			acctTypeMap.insert(std::pair<uint32, std::set<uint32>>(accid, tps));
		}

		if (tfTimeMap.find(guid) != tfTimeMap.end()) {//�Ѿ�����

			std::map<uint32, uint64>::iterator got = tfTimeMap[guid].find(type);
			if (got != tfTimeMap[guid].end()) {
				got->second = val;
			}
			else {
				tfTimeMap[guid].insert(std::pair<uint32, uint64>(type, val));
			}
		}
		else {//������
			std::map<uint32, uint64> newVVV;
			newVVV.insert(std::pair<uint32, uint64>(type, val));
			tfTimeMap.insert(std::pair<uint32, std::map<uint32, uint64>>(guid, newVVV));
		}
		++count;

	} while (result->NextRow());

	delete result;

	sLog.outString();
	sLog.outString(">> Loaded %u AccountPoint", count);

}
bool PzxMgr::updateTime(uint32 type, uint32 guid, uint32 accid, uint64 time) {
	QueryResult* result = CharacterDatabase.PQuery("SELECT guid,type,value FROM _character_extra where guid='%u' and type='%u'", guid, type);
	if (!result)
	{
		BarGoLink bar(1);
		bar.step();
		if (CharacterDatabase.PExecute("insert into _character_extra values ('%u','%u','%u','%u')", guid, type, time, accid)) {
			if (accid > 0 && acctTypeMap.find(accid) != acctTypeMap.end()) {//��ȡ����ID��ֵ
				acctTypeMap[accid].insert(type);
			}
			else {
				std::set<uint32> tps;
				tps.insert(type);
				acctTypeMap.insert(std::pair<uint32, std::set<uint32>>(accid, tps));
			}
			//tfTimeMap ��Ӧ����ʱ��
			std::map<uint32, uint64>::iterator got = tfTimeMap[guid].find(type);
			if (got != tfTimeMap[guid].end()) {
				got->second = time;//����ֵ
			}
			else {
				tfTimeMap[guid].insert(std::pair<uint32, uint64>(type, time));
			}
			return true;
		}
		else {
			sLog.outString(">> [PZX] system error 1", time);
			return false;
		}


	}
	if (CharacterDatabase.DirectPExecute("update _character_extra set value='%u' ,accid='%u' where guid='%u' and type='%u'", time, accid, guid, type)) {//���ٳɹ�

		if (accid > 0 && acctTypeMap.find(accid) != acctTypeMap.end()) {//��ȡ����ID��ֵ
			acctTypeMap[accid].insert(type);
		}
		else {
			std::set<uint32> tps;
			tps.insert(type);
			acctTypeMap.insert(std::pair<uint32, std::set<uint32>>(accid, tps));
		}

		std::map<uint32, uint64>::iterator got = tfTimeMap[guid].find(type);
		if (got != tfTimeMap[guid].end()) {
			got->second = time;//����ֵ
		}
		else {
			tfTimeMap[guid].insert(std::pair<uint32, uint64>(type, time));
		}
		return true;

	}
	else {
		sLog.outString(">> [PZX]system db error 2", time);
	}
	return false;
}

uint32 PzxMgr::getMyPoint(uint32 accountID) {
	AccountPointMap::const_iterator got = accountPMap.find(accountID);

	if (got != accountPMap.end()) {
		if (got->second.val < 0)
			return 0;
		else
			return got->second.val;
	}
	return 0;
}
MenuTree  PzxMgr::getTreeByID(uint32 id) {
	CharaMenuMap::const_iterator got = allMenu.find(id);

	if (got != allMenu.end()) {
		return got->second;
	}
	return{};
}
uint64 PzxMgr::getTimeLength(uint32 guid, uint32 type) {
	std::map<uint32, std::map<uint32, uint64>> ::iterator it = tfTimeMap.find(guid);

	if (it != tfTimeMap.end()) {
		//�ж�˫�츳�Ƿ�����Ҫ���¿���
		std::map<uint32, uint64> v = it->second;
		std::map<uint32, uint64>::const_iterator timeIT = v.find(type);
		if (timeIT != v.end()) {
			return timeIT->second;
		}
		else
			return 0;
	}
	else
		return 0;
}

bool PzxMgr::getAcctType(uint32 accid, uint32 type) {
	if (accid > 0 && acctTypeMap.find(accid) != acctTypeMap.end()) {//��ȡ����ID��ֵ
		std::set<uint32> itTypes = acctTypeMap[accid];
		//std::set<uint32> ::iterator iter;
		return itTypes.find(type) != itTypes.end();
	}
	else {
		return false;
	}
}
bool PzxMgr::isTimeOut(uint32 guid, uint32 type) {
	std::map<uint32, std::map<uint32, uint64>> ::iterator it = tfTimeMap.find(guid);

	if (it != tfTimeMap.end()) {
		//�ж�˫�츳�Ƿ�����Ҫ���¿���
		std::map<uint32, uint64> v = it->second;
		std::map<uint32, uint64>::const_iterator timeIT = v.find(type);
		if (timeIT != v.end()) {
			MenuTree getTree = getTreeByID(type);
			return sizeof(getTree) > 1 ? time(nullptr) > (getTree.itemid + timeIT->second) : true;
		}
		else
			return true;
	}
	else
		return true;
}

bool PzxMgr::addPoints(uint32 cutPoint, uint32 accountID) {
	bool ret = LoginDatabase.DirectPExecute("update account set zanzu=zanzu+'%u' where id='%u'", cutPoint, accountID);

	if (ret) {//���ӳɹ�

		AccountPointMap::iterator got = accountPMap.find(accountID);
		if (got != accountPMap.end()) {
			uint32 newVel = got->second.val + cutPoint;
			accountPMap[got->first].val = newVel;//����ֵ
			sLog.outString(u8">> [PZX]1id:%d + %d point", accountID, cutPoint);

		}
		else {
			sLog.outString(u8">> [PZX ERROR]2id:%d + %d point", accountID, cutPoint);
		}
		return true;
	}
	else {
		sLog.outString(u8">> [PZX ERROR]3id:%d + %d point", accountID, cutPoint);
	}
	return false;
}
bool PzxMgr::cutPoints(int cutPoint, uint32 accountID) {
	if (cutPoint == 0) {//��ѵ�
		return true;
	}
	bool ret = false;
	if (cutPoint > 0) {
		ret = LoginDatabase.DirectPExecute("update _account_extra set val=val-'%u' where id='%u'", cutPoint, accountID);
	}
	else {
		ret = LoginDatabase.DirectPExecute("update _account_extra set val=val+'%u' where id='%u'", -cutPoint, accountID);
	}

	if (ret) {//���ٳɹ�
		AccountPointMap::iterator got = accountPMap.find(accountID);
		if (got != accountPMap.end()) {
			uint32 newVel = got->second.val - cutPoint;
			accountPMap[got->first].val = newVel;//����ֵ
			sLog.outString(u8">> [PZX]%d �ۼ� %d", accountID, cutPoint);
			return true;
		}
		else {
			sLog.outString(u8">> [PZX]%d ϵͳ�쳣a %d", accountID, cutPoint);
		}
	}
	else {
		sLog.outString(u8">> [PZX]%d ϵͳ�쳣b %d", accountID, cutPoint);
	}
	return false;
}

void PzxMgr::loadAllPonits()
{
	try
	{


		//startLienster = true;
		uint32 count = 0;
		QueryResult* result = LoginDatabase.PQuery("SELECT id,idx,val FROM _account_extra ");
		if (!result)
		{
			BarGoLink bar(1);
			bar.step();

			sLog.outString();
			sLog.outString(">> Loaded 0 Player Ponits.");
			//return;//����return�е�BUG����Ҫ������һ�����ݲŴ����������϶��У���ȥ��
		}

		BarGoLink bar(result->GetRowCount());
		//CharaPointMap unMap;
		do
		{
			bar.step();
			Field* fields = result->Fetch();

			uint32 accountID = fields[0].GetUInt32();
			uint32 idx = fields[1].GetUInt32();
			int32 val = fields[2].GetUInt32();


			AccountPointMap::iterator got = accountPMap.find(accountID);
			if (got != accountPMap.end()) {
				got->second.val = val;//����ֵ,��ʱ����ͬ��
			}
			else {
				AccountPoint pt = { accountID, idx, val };
				accountPMap.insert(std::pair<uint32, AccountPoint>(accountID, pt));
			}

			++count;

		} while (result->NextRow());

		delete result;

		//ͬ������ ����ֻ����һ����������
		if (sPzxConfig.GetIntDefault("server.computerPoint", 0)) {

			_AccountPointMap _accountPMap;
			try
			{
				//��ѯ
				QueryResult* result2 = LoginDatabase.PQuery("SELECT id,zanzu FROM account  where zanzu>1");
				if (!result2)
				{
					BarGoLink bar(1);
					bar.step();
					return;
				}

				BarGoLink bar2(result2->GetRowCount());
				do
				{
					bar2.step();
					Field* fields = result2->Fetch();
					uint32 accountID = fields[0].GetUInt32();
					int32 val = fields[1].GetInt32();
					_AccountPointMap::iterator got = _accountPMap.find(accountID);
					if (got != _accountPMap.end()) {
						got->second = val;//����ֵ,��ʱ����ͬ��
					}
					else {
						_accountPMap.insert(std::pair<uint32, int32>(accountID, val));
					}
				} while (result2->NextRow());
				delete result2;
			}
			catch (...)
			{
				sLog.outError("[pzx-exception-SEH-loadAllPonits1]");

			}
			_AccountPointMap::iterator sysnIt = _accountPMap.begin();
			while (sysnIt != _accountPMap.end()) {
				uint32 accID = sysnIt->first;
				int32 synVal = sysnIt->second - 1;
				AccountPointMap::iterator got = accountPMap.find(accID);
				if (got != accountPMap.end()) {//��ǰ�����ݣ�����account������_account_extra
					sLog.outString(">> Syn Player point ==> accid=%d,point=%d", accID, synVal);
					//1 ��update
					LoginDatabase.PExecute("update account set zanzu=1 where id='%u'", accID);//����
					LoginDatabase.PExecute("update _account_extra set val=val+%u where id='%u'", synVal, accID);//����

					//2����map
					got->second.val = got->second.val + synVal;//����ֵ,��ʱ����ͬ��
				}
				else {//��ǰû��ֵ�����������ݲ�������
					sLog.outString(">> Create Player point ==> accid=%d,point=%d", accID, synVal);
					//1 ���������
					LoginDatabase.PExecute("INSERT INTO _account_extra (`id`, `idx`, `val`) VALUES('%u','1','%u')", accID, synVal);//����
					AccountPoint pt = { accID, 1, synVal };
					accountPMap.insert(std::pair<uint32, AccountPoint>(accID, pt));
					LoginDatabase.PExecute("update account set 1=1 where id='%u'", accID);//����
				}
				sysnIt++;
			}
			sLog.outString();
			sLog.outString(">> Loaded %u AccountPoint", count);
		}
	}
	catch (...)
	{
		sLog.outError("[pzx-exception-SEH-loadAllPonits2]");

	}
	//startLienster = false;
}
CharaMenuMap PzxMgr::loadAllMenu(uint32 pid)
{
	uint32 count = 0;
	QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM _pzx_cardTree where pid='%u'", pid);
	if (!result)
	{
		BarGoLink bar(1);
		bar.step();

		sLog.outString();
		sLog.outString(">> Loaded 0 _zzcard.");
		return{};
	}
	BarGoLink bar(result->GetRowCount());
	CharaMenuMap menuMap;
	do
	{
		bar.step();
		Field* fields = result->Fetch();
		uint32 id = fields[0].GetUInt32();
		uint32 pid = fields[1].GetUInt32();
		std::string name = fields[2].GetString();
		uint32 unionID = fields[3].GetUInt32();
		uint32 needval = fields[4].GetUInt32();
		uint32 type = fields[5].GetUInt32();
		uint32 raceMask = fields[6].GetUInt32();
		uint32 itemid = fields[7].GetUInt32();
		uint32 itemNum = fields[8].GetUInt32();
		std::string telexyz = fields[9].GetString();
		//uint32 spellid = fields[10].GetUInt32();
		//uint32 time_length = fields[11].GetUInt32();
		uint32 unioncheck = fields[10].GetUInt32();
		uint32 popMenu = fields[11].GetUInt32();
		uint32 iconID = fields[12].GetUInt32();
		//menu�ֲ�
		//1.���ҳ�pidΪ0�Ĳ˵���Ŀ
		MenuTree pt = { id, pid, name,unionID,needval,type,raceMask,itemid ,itemNum,telexyz ,unioncheck,popMenu,iconID,loadAllMenu(id) };
		//sLog.outString("loadAllMenu4");
		std::pair<uint32, MenuTree> menu(id, pt);
		menuMap.insert(menu);
		allMenu.insert(menu);
		++count;
	} while (result->NextRow());
	delete result;
	sLog.outString();
	sLog.outString(">> Loaded %u pzx zanzhu menu", count);

	return menuMap;

}