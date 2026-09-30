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

#include "SelectionService.h"
#include "translators/ITranslator.h"
#include <memory>
#include <string>
#include <vector>

struct PreparedTranslation
{
	bool ok = false;
	std::wstring error;
	std::vector<TextSpan> spans;
	std::vector<std::string> translations;
};

class TranslateEngine
{
public:
	explicit TranslateEngine(const NppData& npp);

	PreparedTranslation translateCurrentSelections();
	TranslateResult translateTexts(const std::vector<std::string>& texts) const;
	bool apply(const PreparedTranslation& prepared) const;

private:
	std::unique_ptr<ITranslator> makeTranslator() const;

	NppData _npp;
	SelectionService _selection;
};
