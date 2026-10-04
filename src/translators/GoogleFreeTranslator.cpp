// NppTranslator
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

#include "GoogleFreeTranslator.h"
#include "../HttpClient.h"
#include "../Utf8Util.h"
#include "../Localization.h"

namespace
{
	// Response shape: [[["translated","original",...],...], ...]
	// Concatenate first string of each inner segment group.
	std::string ParseGoogleGtxResponse(const std::string& body)
	{
		std::string out;
		size_t i = 0;
		// Find the first array of segments: after "[[["
		const size_t start = body.find("[[[");
		if (start == std::string::npos)
			return {};
		i = start + 1; // at first '[' of segment list-ish; walk quotations at depth for first strings

		// Simpler approach: repeatedly find pattern ["translated"
		// Walk through top-level segment arrays.
		size_t pos = start;
		while (pos < body.size())
		{
			// Each segment looks like: ["translated text","src",...
			const size_t seg = body.find("[\"", pos);
			if (seg == std::string::npos)
				break;

			// Ensure we are still inside the first big array (before "],null," pattern ends)
			if (body.find("]],", start) != std::string::npos && seg > body.find("]],", start))
				break;

			size_t p = seg + 2;
			std::string piece;
			while (p < body.size())
			{
				const char c = body[p++];
				if (c == '\\' && p < body.size())
				{
					const char n = body[p++];
					switch (n)
					{
					case 'n': piece.push_back('\n'); break;
					case 'r': piece.push_back('\r'); break;
					case 't': piece.push_back('\t'); break;
					case '"': piece.push_back('"'); break;
					case '\\': piece.push_back('\\'); break;
					case '/': piece.push_back('/'); break;
					case 'u':
						if (p + 3 < body.size())
						{
							wchar_t code = 0;
							for (int k = 0; k < 4; ++k)
							{
								const char h = body[p++];
								code <<= 4;
								if (h >= '0' && h <= '9') code |= static_cast<wchar_t>(h - '0');
								else if (h >= 'a' && h <= 'f') code |= static_cast<wchar_t>(h - 'a' + 10);
								else if (h >= 'A' && h <= 'F') code |= static_cast<wchar_t>(h - 'A' + 10);
							}
							std::wstring w(1, code);
							piece += WideToUtf8(w);
						}
						break;
					default:
						piece.push_back(n);
						break;
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
			out += piece;

			// Advance to next segment after this one's closing ]
			const size_t close = body.find(']', p);
			if (close == std::string::npos)
				break;
			pos = close + 1;

			// Stop when we leave the segments array ("]],")
			if (pos + 1 < body.size() && body[pos] == ']' )
				break;
		}
		return out;
	}

	TranslateResult TranslateOne(const std::string& text, const std::string& sl, const std::string& tl)
	{
		TranslateResult r;
		const std::wstring path = L"/translate_a/single?client=gtx&sl=" + Utf8ToWide(UrlEncode(sl))
			+ L"&tl=" + Utf8ToWide(UrlEncode(tl))
			+ L"&dt=t&q=" + Utf8ToWide(UrlEncode(text));

		const HttpResponse resp = HttpClient::get(L"translate.googleapis.com", path);
		if (!resp.error.empty())
		{
			r.error = resp.error;
			return r;
		}
		if (resp.statusCode == 429 || resp.statusCode == 503)
		{
			r.error = Localization::format(LocId::ErrGoogleLimited, std::to_wstring(resp.statusCode));
			return r;
		}
		if (resp.statusCode != 200)
		{
			r.error = Localization::format(LocId::ErrGoogleHttp, std::to_wstring(resp.statusCode));
			return r;
		}
		if (resp.body.find("sorry") != std::string::npos || resp.body.find("/sorry/") != std::string::npos)
		{
			r.error = Localization::get(LocId::ErrGoogleCaptcha);
			return r;
		}

		const std::string translated = ParseGoogleGtxResponse(resp.body);
		if (translated.empty())
		{
			r.error = Localization::get(LocId::ErrGoogleInvalid);
			return r;
		}
		r.ok = true;
		r.translations.push_back(translated);
		return r;
	}
}

TranslateResult GoogleFreeTranslator::translate(const TranslateRequest& request)
{
	TranslateResult all;
	all.translations.reserve(request.texts.size());
	const std::string sl = request.sourceLang.empty() ? "auto" : request.sourceLang;
	const std::string tl = request.targetLang.empty() ? "it" : request.targetLang;

	for (const auto& text : request.texts)
	{
		if (text.empty())
		{
			all.translations.emplace_back();
			continue;
		}
		TranslateResult one = TranslateOne(text, sl, tl);
		if (!one.ok)
		{
			all.ok = false;
			all.error = one.error;
			return all;
		}
		all.translations.push_back(one.translations[0]);
	}
	all.ok = true;
	return all;
}
