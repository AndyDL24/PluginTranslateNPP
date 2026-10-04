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

#include "npp/PluginInterface.h"
#include <string>
#include <vector>

struct TextSpan
{
	Sci_Position start = 0;
	Sci_Position end = 0;
	std::string textUtf8;
};

class SelectionService
{
public:
	explicit SelectionService(const NppData& npp) : _npp(npp) {}

	HWND currentScintilla() const;
	int nonEmptySelectionCount() const;
	void mainSelection(Sci_Position& start, Sci_Position& end) const;
	Sci_Position lineCount() const;
	Sci_Position lineIndexFromPos(Sci_Position pos) const;
	bool lineSpan(Sci_Position line, TextSpan& span) const;
	bool replaceRange(Sci_Position start, Sci_Position end, const std::string& textUtf8) const;
	void selectRange(Sci_Position start, Sci_Position end) const;
	std::vector<TextSpan> getSpansToTranslate() const;
	bool replaceSpans(const std::vector<TextSpan>& originalSpans,
		const std::vector<std::string>& replacementsUtf8) const;

private:
	NppData _npp;
};
