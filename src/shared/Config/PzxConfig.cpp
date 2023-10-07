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

#include "PzxConfig.h"

#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(PzxConfig);

// Defined here as it must not be exposed to end-users.
bool PzxConfig::GetValueHelper(char const* name, ACE_TString& result)
{
	GuardType guard(m_configLock);

	if (!mConf)
		return false;

	ACE_TString section_name;
	ACE_Configuration_Section_Key section_key;
	ACE_Configuration_Section_Key const& root_key = mConf->root_section();

	int i = 0;
	while (mConf->enumerate_sections(root_key, i, section_name) == 0)
	{
		mConf->open_section(root_key, section_name.c_str(), 0, section_key);
		if (mConf->get_string_value(section_key, name, result) == 0)
			return true;
		++i;
	}

	return false;
}

PzxConfig::PzxConfig()
	: mConf(nullptr)
{
}

PzxConfig::~PzxConfig()
{
	delete mConf;
}

bool PzxConfig::SetSource(char const* file)
{
	mFilename = file;

	return Reload();
}

bool PzxConfig::Reload()
{
	delete mConf;
	mConf = new ACE_Configuration_Heap;

	if (mConf->open() != -1)
	{
		ACE_Ini_ImpExp config_importer(*mConf);
		if (config_importer.import_config(mFilename.c_str()) != -1)
			return true;
	}

	delete mConf;
	mConf = nullptr;
	return false;
}

std::string PzxConfig::GetStringDefault(char const* name, char const* def)
{
	ACE_TString val;
	return GetValueHelper(name, val) ? val.c_str() : def;
}

bool PzxConfig::GetBoolDefault(char const* name, bool def)
{
	ACE_TString val;
	if (!GetValueHelper(name, val))
		return def;

	char const* str = val.c_str();
	return strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
		strcmp(str, "yes") == 0 || strcmp(str, "YES") == 0 ||
		strcmp(str, "1") == 0;
}


int32 PzxConfig::GetIntDefault(char const* name, int32 def)
{
	ACE_TString val;
	return GetValueHelper(name, val) ? atoi(val.c_str()) : def;
}


float PzxConfig::GetFloatDefault(char const* name, float def)
{
	ACE_TString val;
	return GetValueHelper(name, val) ? (float)atof(val.c_str()) : def;
}
