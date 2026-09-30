#include "DeepLTranslator.h"
#include "../HttpClient.h"
#include "../Utf8Util.h"
#include "../Localization.h"
#include <vector>
#include <string>

namespace
{
	std::string ToDeepLLang(std::string code)
	{
		for (char& c : code)
		{
			if (c >= 'a' && c <= 'z')
				c = static_cast<char>(c - 'a' + 'A');
		}
		if (code == "AUTO")
			return {};
		// DeepL uses EN, IT, etc. Keep regional variants like EN-US if provided.
		return code;
	}

	// Extract "text":"..." values inside translations array, in order.
	std::vector<std::string> ParseDeepLTexts(const std::string& body)
	{
		std::vector<std::string> out;
		const std::string key = "\"text\":\"";
		size_t pos = 0;
		while ((pos = body.find(key, pos)) != std::string::npos)
		{
			pos += key.size();
			std::string piece;
			while (pos < body.size())
			{
				const char c = body[pos++];
				if (c == '\\' && pos < body.size())
				{
					const char n = body[pos++];
					switch (n)
					{
					case 'n': piece.push_back('\n'); break;
					case 'r': piece.push_back('\r'); break;
					case 't': piece.push_back('\t'); break;
					case '"': piece.push_back('"'); break;
					case '\\': piece.push_back('\\'); break;
					case '/': piece.push_back('/'); break;
					case 'u':
						if (pos + 3 < body.size())
						{
							wchar_t code = 0;
							for (int k = 0; k < 4; ++k)
							{
								const char h = body[pos++];
								code <<= 4;
								if (h >= '0' && h <= '9') code |= static_cast<wchar_t>(h - '0');
								else if (h >= 'a' && h <= 'f') code |= static_cast<wchar_t>(h - 'a' + 10);
								else if (h >= 'A' && h <= 'F') code |= static_cast<wchar_t>(h - 'A' + 10);
							}
							piece += WideToUtf8(std::wstring(1, code));
						}
						break;
					default: piece.push_back(n); break;
					}
				}
				else if (c == '"')
				{
					break;
				}
				else
				{
					piece.push_back(c);
				}
			}
			out.push_back(std::move(piece));
		}
		return out;
	}
}

TranslateResult DeepLTranslator::translate(const TranslateRequest& request)
{
	TranslateResult result;
	if (_apiKey.empty())
	{
		result.error = Localization::get(LocId::ErrDeeplMissingKey);
		return result;
	}

	std::string form;
	form.reserve(256);
	for (const auto& t : request.texts)
	{
		if (!form.empty())
			form.push_back('&');
		form += "text=";
		form += UrlEncode(t);
	}

	const std::string target = ToDeepLLang(request.targetLang.empty() ? "IT" : request.targetLang);
	form += "&target_lang=";
	form += UrlEncode(target);

	const std::string source = ToDeepLLang(request.sourceLang);
	if (!source.empty() && source != "AUTO")
	{
		form += "&source_lang=";
		form += UrlEncode(source);
	}

	std::wstring auth = L"DeepL-Auth-Key ";
	auth += Utf8ToWide(_apiKey);

	const HttpResponse resp = HttpClient::post(_host, L"/v2/translate", form,
		L"application/x-www-form-urlencoded",
		{ { L"Authorization", auth } });

	if (!resp.error.empty())
	{
		result.error = resp.error;
		return result;
	}
	if (resp.statusCode == 403)
	{
		result.error = Localization::get(LocId::ErrDeeplForbidden);
		return result;
	}
	if (resp.statusCode == 456)
	{
		result.error = Localization::get(LocId::ErrDeeplQuota);
		return result;
	}
	if (resp.statusCode != 200)
	{
		result.error = Localization::format(LocId::ErrDeeplHttp, std::to_wstring(resp.statusCode));
		return result;
	}

	result.translations = ParseDeepLTexts(resp.body);
	if (result.translations.size() != request.texts.size())
	{
		result.error = Localization::get(LocId::ErrDeeplInvalid);
		result.translations.clear();
		return result;
	}
	result.ok = true;
	return result;
}
