#pragma once

#include <string>
#include <vector>

struct TranslateRequest
{
	std::string sourceLang; // "auto" or code
	std::string targetLang;
	std::vector<std::string> texts; // UTF-8
};

struct TranslateResult
{
	bool ok = false;
	std::wstring error;
	std::vector<std::string> translations; // UTF-8, same order as request
};

class ITranslator
{
public:
	virtual ~ITranslator() = default;
	virtual const wchar_t* name() const = 0;
	virtual TranslateResult translate(const TranslateRequest& request) = 0;
};
