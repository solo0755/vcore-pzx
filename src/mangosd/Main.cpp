/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
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

/// \addtogroup mangosd Mangos Daemon
/// @{
/// \file

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"
#include "ProgressBar.h"
#include "Log.h"
#include "Master.h"
#include "SystemConfig.h"
#include "revision.h"
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <stdio.h>


#ifdef WIN32
#include <tchar.h>
#include <dbghelp.h>
#include "ObjectAccessor.h"
#include "WheatyExceptionReport.h"
#pragma comment(linker, "/defaultlib:dbghelp.lib")
#include "ServiceWin32.h"
char serviceName[] = "mangosd";
char serviceLongName[] = "MaNGOS world service";
char serviceDescription[] = "Massive Network Game Object Server";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#else
#include "PosixDaemon.h"
#endif

DatabaseType WorldDatabase;                                 ///< Accessor to the world database
DatabaseType CharacterDatabase;                             ///< Accessor to the character database
DatabaseType LoginDatabase;                                 ///< Accessor to the realm/login database
DatabaseType LogsDatabase;                                  ///< Accessor to the logs database

uint32 realmID;                                             ///< Id of the realm

/// Print out the usage string for this program on the console.
void usage(const char *prog)
{
    sLog.outString("Usage: \n %s [<options>]\n"
        "    -v, --version            print version and exist\n\r"
        "    -c config_file           use config_file as configuration file\n\r"
        #ifdef WIN32
        "    Running as service functions:\n\r"
        "    -s run                   run as service\n\r"
        "    -s install               install service\n\r"
        "    -s uninstall             uninstall service\n\r"
        #else
        "    Running as daemon functions:\n\r"
        "    -s run                   run as daemon\n\r"
        "    -s stop                  stop daemon\n\r"
        #endif
        ,prog);
}
#ifdef WIN32
struct CustomPackage : public SYMBOL_INFO_PACKAGE
{
	CustomPackage()
	{
		si.SizeOfStruct = sizeof(SYMBOL_INFO);
		si.MaxNameLen = sizeof(name);
	}
};

bool GetCustomPathModule(PVOID Addr, PTSTR Module, DWORD Len)
{
	DWORD Section = 0;
	DWORD_PTR Offset = 0;
	MEMORY_BASIC_INFORMATION Mb;

	if (!VirtualQuery(Addr, &Mb, sizeof(Mb)))
		return false;

	DWORD_PTR ModPtr = (DWORD_PTR)Mb.AllocationBase;

	if (!ModPtr)
		return false;

	if (!GetModuleFileName((HMODULE)ModPtr, Module, Len))
		return false;

	PIMAGE_DOS_HEADER DosHdr = (PIMAGE_DOS_HEADER)ModPtr;

	PIMAGE_NT_HEADERS NtHdr = (PIMAGE_NT_HEADERS)(ModPtr + DWORD_PTR(DosHdr->e_lfanew));

	PIMAGE_SECTION_HEADER HSection = IMAGE_FIRST_SECTION(NtHdr);

	DWORD_PTR Rva = (DWORD_PTR)Addr - ModPtr;

	for (unsigned i = 0; i < NtHdr->FileHeader.NumberOfSections; i++, HSection++)
	{
		DWORD_PTR SectionStart = HSection->VirtualAddress;
		DWORD_PTR SectionEnd = SectionStart + DWORD_PTR(std::max(HSection->SizeOfRawData, HSection->Misc.VirtualSize));

		if ((Rva >= SectionStart) && (Rva <= SectionEnd))
		{
			Section = i + 1;
			Offset = Rva - SectionStart;
			return true;
		}
	}

	return false;
}

//回调函数
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *Exception)
{
	//开始保存数据

	//先保存玩家
	sObjectAccessor.SaveAllPlayers();
	//保存自定义-数据库
	//sObjectMgr->SaveCustomAutoDB();

	//开始记录宕机日志 只记录重要部分
	SymSetOptions(SYMOPT_DEFERRED_LOADS);

	if (!SymInitialize(GetCurrentProcess(), 0, TRUE))
		::TerminateProcess(GetCurrentProcess(), 4);

	PCONTEXT ExcContext = Exception->ContextRecord;
	CONTEXT *MainContext = ExcContext;

	STACKFRAME64 StackFrame;
	memset(&StackFrame, 0, sizeof(StackFrame));

	//StackFrame.AddrPC.Offset = MainContext->Rip;
	//StackFrame.AddrPC.Mode = AddrModeFlat;
	//StackFrame.AddrStack.Offset = MainContext->Rsp;
	//StackFrame.AddrStack.Mode = AddrModeFlat;
	//StackFrame.AddrFrame.Offset = MainContext->Rbp;
	//StackFrame.AddrFrame.Mode = AddrModeFlat;

	TCHAR ModuleFolderName[MAX_PATH];
	GetModuleFileName(0, ModuleFolderName, MAX_PATH);
	TCHAR* AppFolder = _tcsrchr(ModuleFolderName, '\\');
	if (!AppFolder)
		::TerminateProcess(GetCurrentProcess(), 4);

	AppFolder[0] = '\0';
	++AppFolder;

	//保存宕机日志的文件夹Crashes-Hk
	TCHAR CrashFolderPath[MAX_PATH];
	sprintf(CrashFolderPath, "%s\\%s", ModuleFolderName, _T("Crashes-pzx"));

	if (!CreateDirectoryA(CrashFolderPath, NULL))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
			::TerminateProcess(GetCurrentProcess(), 4);
	}

	SYSTEMTIME DateSystime;
	GetLocalTime(&DateSystime);

	//格式化文件名 保证文件唯一
	char FileName[MAX_PATH];
	snprintf(FileName, MAX_PATH, _T("%s\\%s-[%u月%u日%u时%u分%u秒].txt"), CrashFolderPath, AppFolder, DateSystime.wMonth, DateSystime.wDay, DateSystime.wHour, DateSystime.wMinute, DateSystime.wSecond);
	FILE *CrashesFile = fopen(FileName, "w");
	fprintf(CrashesFile, _T("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n"));

	//直到记录完毕后退出
	while (1)
	{
		if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), GetCurrentThread(), &StackFrame, ExcContext, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0))
			break;

		if (0 == StackFrame.AddrFrame.Offset)
			break;

		DWORD64 Displacement = 0;
		CustomPackage InfoPackage;

		if (SymFromAddr(GetCurrentProcess(), StackFrame.AddrPC.Offset, &Displacement, &InfoPackage.si))
		{
			fprintf(CrashesFile, _T("关联函数名 %hs "), InfoPackage.si.Name);
		}
		else
		{
			TCHAR ExeModule[MAX_PATH] = _T("");
			GetCustomPathModule((PVOID)StackFrame.AddrPC.Offset, ExeModule, sizeof(ExeModule));
			fprintf(CrashesFile, _T("出错模块程序 %s "), ExeModule);
		}

		IMAGEHLP_LINE64 LineInfo = { sizeof(IMAGEHLP_LINE64) };
		DWORD LineDisplacement;
		if (SymGetLineFromAddr64(GetCurrentProcess(), StackFrame.AddrPC.Offset, &LineDisplacement, &LineInfo))
		{
			fprintf(CrashesFile, _T("出错代码路径 %s 行号 %u"), LineInfo.FileName, LineInfo.LineNumber);
		}

		fprintf(CrashesFile, "\n");
	}

	fprintf(CrashesFile, _T("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"));
	fflush(CrashesFile);
	fclose(CrashesFile);

	//记录宕机日志完成

	//弹框或直接关闭 以此屏蔽系统那个坑爹的框
	if (!sConfig.GetBoolDefault("CustomCrash.Eject", false))
	{
		::TerminateProcess(GetCurrentProcess(), 4);
	}
	else
	{
		TCHAR CrashText[1024];
		sprintf(CrashText, _T("发生致命错误 出错日志文件已记录 请自行查看\n\n发生致命错误 出错日志文件已记录 请自行查看\n\n日志文件路径 %s\n\n点击确定关闭 点击确定关闭"), CrashFolderPath);
		MessageBoxA(NULL, CrashText, _T("程序出错"), MB_OK);
		::TerminateProcess(GetCurrentProcess(), 4);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif
/// Launch the mangos server
extern int main(int argc, char **argv)
{
#ifdef WIN32
	new WheatyExceptionReport();
	//SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif
    ///- Command line parsing
    char const* cfg_file = _MANGOSD_CONFIG;


    char const *options = ":c:s:";

    ACE_Get_Opt cmd_opts(argc, argv, options);
    cmd_opts.long_option("version", 'v');

    char serviceDaemonMode = '\0';

    int option;
    while ((option = cmd_opts()) != EOF)
    {
        switch (option)
        {
            case 'c':
                cfg_file = cmd_opts.opt_arg();
                break;
            case 'v':
                printf("Core revision: %s\n", _FULLVERSION);
                return 0;
            case 's':
            {
                const char *mode = cmd_opts.opt_arg();

                if (!strcmp(mode, "run"))
                    serviceDaemonMode = 'r';
#ifdef WIN32
                else if (!strcmp(mode, "install"))
                    serviceDaemonMode = 'i';
                else if (!strcmp(mode, "uninstall"))
                    serviceDaemonMode = 'u';
#else
                else if (!strcmp(mode, "stop"))
                    serviceDaemonMode = 's';
#endif
                else
                {
                    sLog.outError("Runtime-Error: -%c unsupported argument %s", cmd_opts.opt_opt(), mode);
                    usage(argv[0]);
                    Log::WaitBeforeContinueIfNeed();
                    return 1;
                }
                break;
            }
            case ':':
                sLog.outError("Runtime-Error: -%c option requires an input argument", cmd_opts.opt_opt());
                usage(argv[0]);
                Log::WaitBeforeContinueIfNeed();
                return 1;
            default:
                sLog.outError("Runtime-Error: bad format of commandline arguments");
                usage(argv[0]);
                Log::WaitBeforeContinueIfNeed();
                return 1;
        }
    }

#ifdef WIN32                                                // windows service command need execute before config read
    switch (serviceDaemonMode)
    {
        case 'i':
            if (WinServiceInstall())
                sLog.outString("Installing service");
            return 1;
        case 'u':
            if (WinServiceUninstall())
                sLog.outString("Uninstalling service");
            return 1;
        case 'r':
            WinServiceRun();
            break;
    }
#endif

    if (!sConfig.SetSource(cfg_file))
    {
        sLog.outError("Could not find configuration file %s.", cfg_file);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }
		char hostname[128];
		int ret = gethostname(hostname, sizeof(hostname));
		if (ret == -1) {
			return -1;
		}
		struct hostent *hent;
		hent = gethostbyname(hostname);

#ifdef PZXVERSIONCHECK  
		if (strcmp(hent->h_name, "SKY-20160529EEN")!=0){
			sLog.outError("Could not find PZXKEY %s", (hent->h_name));
			Log::WaitBeforeContinueIfNeed();
			return 1;
		}
#endif		
		

#ifndef WIN32                                               // posix daemon commands need apply after config read
    switch (serviceDaemonMode)
    {
    case 'r':
        startDaemon();
        break;
    case 's':
        stopDaemon();
        break;
    }
#endif

    sLog.outString("Core revision: %s [world-daemon]", _FULLVERSION);
    sLog.outString( "<Ctrl-C> to stop." );
    sLog.outString("\n\n"
        "MM   MM         MM   MM  MMMMM   MMMM   MMMMM\n"
        "MM   MM         MM   MM MMM MMM MM  MM MMM MMM\n"
        "MMM MMM         MMM  MM MMM MMM MM  MM MMM\n"
        "MM M MM         MMMM MM MMM     MM  MM  MMM\n"
        "MM M MM  MMMMM  MM MMMM MMM     MM  MM   MMM\n"
        "MM M MM M   MMM MM  MMM MMMMMMM MM  MM    MMM\n"
        "MM   MM     MMM MM   MM MM  MMM MM  MM     MMM\n"
        "MM   MM MMMMMMM MM   MM MMM MMM MM  MM MMM MMM\n"
        "MM   MM MM  MMM MM   MM  MMMMMM  MMMM   MMMMM\n"
        "        MM  MMM http://getmangos.com\n"
        "        MMMMMM\n\n");
    sLog.outString("vMaNGOS : https://github.com/vmangos");
    sLog.outString("Using configuration file %s.", cfg_file);

#define STR(s) #s
#define XSTR(s) STR(s)

    sLog.outInfo("Alloc library: " MANGOS_ALLOC_LIB "");
    sLog.outInfo("Core Revision: " _FULLVERSION);

    DETAIL_LOG("%s (Library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
    if (SSLeay() < 0x009080bfL )
    {
        DETAIL_LOG("WARNING: Outdated version of OpenSSL lib. Logins to server may not work!");
        DETAIL_LOG("WARNING: Minimal required version [OpenSSL 0.9.8k]");
    }

    DETAIL_LOG("Using ACE: %s", ACE_VERSION);

    ///- Set progress bars show mode
    BarGoLink::SetOutputState(sConfig.GetBoolDefault("ShowProgressBars", true));

    ///- and run the 'Master'
    /// \todo Why do we need this 'Master'? Can't all of this be in the Main as for Realmd?
    return sMaster.Run();

    // at sMaster return function exist with codes
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart mangosd
}

/// @}
