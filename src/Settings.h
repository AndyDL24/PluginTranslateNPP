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

#pragma once

#include <string>

enum class TranslateProvider
{
	GoogleFree = 0,
	DeepL = 1
};

enum class DeepLEndpoint
{
	Free = 0,
	Pro = 1
};

struct PluginSettings
{
	TranslateProvider provider = TranslateProvider::GoogleFree;
	std::wstring sourceLang = L"auto";
	std::wstring targetLang = L"it";
	std::wstring deeplApiKey;
	DeepLEndpoint deeplEndpoint = DeepLEndpoint::Free;
	std::wstring csvColumn;

	void load();
	void save() const;
	void swapLanguages();

	std::wstring providerName() const;
	std::string sourceLangUtf8() const;
	std::string targetLangUtf8() const;
	std::string deeplApiKeyUtf8() const;
	std::wstring deeplHost() const;
};

PluginSettings& GetSettings();
