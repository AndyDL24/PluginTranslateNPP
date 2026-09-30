// NppTranslate
// Copyright (C) 2026 AndyD
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "Settings.h"
#include "Utf8Util.h"
#include "npp/PluginInterface.h"
#include <shlwapi.h>
#include <utility>

#pragma comment(lib, "shlwapi.lib")

extern NppData nppData;

namespace
{
	std::wstring ConfigIniPath()
	{
		wchar_t path[MAX_PATH] = {};
		::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(path));
		::PathAppendW(path, L"NppTranslate.ini");
		return path;
	}

	std::wstring ReadIni(const std::wstring& file, const wchar_t* key, const wchar_t* def)
	{
		wchar_t buf[2048] = {};
		::GetPrivateProfileStringW(L"Translate", key, def, buf, 2048, file.c_str());
		return buf;
	}
}

PluginSettings& GetSettings()
{
	static PluginSettings s;
	return s;
}

void PluginSettings::load()
{
	const std::wstring ini = ConfigIniPath();
	const std::wstring providerStr = ReadIni(ini, L"provider", L"google_free");
	if (providerStr == L"deepl")
		provider = TranslateProvider::DeepL;
	else
		provider = TranslateProvider::GoogleFree;

	sourceLang = ReadIni(ini, L"source_lang", L"auto");
	targetLang = ReadIni(ini, L"target_lang", L"it");
	deeplApiKey = ReadIni(ini, L"deepl_api_key", L"");

	const std::wstring ep = ReadIni(ini, L"deepl_endpoint", L"free");
	deeplEndpoint = (ep == L"pro") ? DeepLEndpoint::Pro : DeepLEndpoint::Free;
	csvColumn = ReadIni(ini, L"csv_column", L"");
}

void PluginSettings::save() const
{
	const std::wstring ini = ConfigIniPath();
	::WritePrivateProfileStringW(L"Translate", L"provider",
		provider == TranslateProvider::DeepL ? L"deepl" : L"google_free", ini.c_str());
	::WritePrivateProfileStringW(L"Translate", L"source_lang", sourceLang.c_str(), ini.c_str());
	::WritePrivateProfileStringW(L"Translate", L"target_lang", targetLang.c_str(), ini.c_str());
	::WritePrivateProfileStringW(L"Translate", L"deepl_api_key", deeplApiKey.c_str(), ini.c_str());
	::WritePrivateProfileStringW(L"Translate", L"deepl_endpoint",
		deeplEndpoint == DeepLEndpoint::Pro ? L"pro" : L"free", ini.c_str());
	::WritePrivateProfileStringW(L"Translate", L"csv_column", csvColumn.c_str(), ini.c_str());
}

void PluginSettings::swapLanguages()
{
	if (_wcsicmp(sourceLang.c_str(), L"auto") == 0)
	{
		sourceLang = targetLang;
		targetLang = L"en";
		if (_wcsicmp(sourceLang.c_str(), L"en") == 0)
			targetLang = L"it";
		return;
	}
	std::swap(sourceLang, targetLang);
}

std::wstring PluginSettings::providerName() const
{
	return provider == TranslateProvider::DeepL ? L"DeepL" : L"Google Free";
}

std::string PluginSettings::sourceLangUtf8() const
{
	return WideToUtf8(sourceLang);
}

std::string PluginSettings::targetLangUtf8() const
{
	return WideToUtf8(targetLang);
}

std::string PluginSettings::deeplApiKeyUtf8() const
{
	return WideToUtf8(deeplApiKey);
}

std::wstring PluginSettings::deeplHost() const
{
	return deeplEndpoint == DeepLEndpoint::Pro
		? L"api.deepl.com"
		: L"api-free.deepl.com";
}
