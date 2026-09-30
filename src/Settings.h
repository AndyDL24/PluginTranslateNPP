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
