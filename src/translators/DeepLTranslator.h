#pragma once

#include "ITranslator.h"
#include <string>

class DeepLTranslator : public ITranslator
{
public:
	DeepLTranslator(std::wstring host, std::string apiKey)
		: _host(std::move(host)), _apiKey(std::move(apiKey)) {}

	const wchar_t* name() const override { return L"DeepL"; }
	TranslateResult translate(const TranslateRequest& request) override;

private:
	std::wstring _host;
	std::string _apiKey;
};
