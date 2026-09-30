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
