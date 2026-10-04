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
