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

#include "SelectionService.h"
#include <algorithm>
#include <windows.h>

HWND SelectionService::currentScintilla() const
{
	int which = 0;
	::SendMessage(_npp._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&which));
	return (which == 0) ? _npp._scintillaMainHandle : _npp._scintillaSecondHandle;
}

int SelectionService::nonEmptySelectionCount() const
{
	HWND sci = currentScintilla();
	const auto nSel = static_cast<int>(::SendMessage(sci, SCI_GETSELECTIONS, 0, 0));
	int count = 0;
	for (int i = 0; i < nSel; ++i)
	{
		const Sci_Position start = ::SendMessage(sci, SCI_GETSELECTIONNSTART, i, 0);
		const Sci_Position end = ::SendMessage(sci, SCI_GETSELECTIONNEND, i, 0);
		if (end > start)
			++count;
	}
	return count;
}

void SelectionService::mainSelection(Sci_Position& start, Sci_Position& end) const
{
	HWND sci = currentScintilla();
	start = ::SendMessage(sci, SCI_GETSELECTIONSTART, 0, 0);
	end = ::SendMessage(sci, SCI_GETSELECTIONEND, 0, 0);
}

Sci_Position SelectionService::lineCount() const
{
	return ::SendMessage(currentScintilla(), SCI_GETLINECOUNT, 0, 0);
}

Sci_Position SelectionService::lineIndexFromPos(Sci_Position pos) const
{
	return ::SendMessage(currentScintilla(), SCI_LINEFROMPOSITION, pos, 0);
}

bool SelectionService::lineSpan(Sci_Position line, TextSpan& span) const
{
	HWND sci = currentScintilla();
	span.start = ::SendMessage(sci, SCI_POSITIONFROMLINE, line, 0);
	span.end = ::SendMessage(sci, SCI_GETLINEENDPOSITION, line, 0);
	if (span.end < span.start)
		return false;

	const Sci_Position len = span.end - span.start;
	span.textUtf8.assign(static_cast<size_t>(len) + 1, '\0');
	if (len > 0)
	{
		Sci_TextRangeFull tr{};
		tr.chrg.cpMin = span.start;
		tr.chrg.cpMax = span.end;
		tr.lpstrText = span.textUtf8.data();
		::SendMessage(sci, SCI_GETTEXTRANGEFULL, 0, reinterpret_cast<LPARAM>(&tr));
	}
	span.textUtf8.resize(static_cast<size_t>(len));
	return true;
}

bool SelectionService::replaceRange(Sci_Position start, Sci_Position end, const std::string& textUtf8) const
{
	HWND sci = currentScintilla();
	::SendMessage(sci, SCI_BEGINUNDOACTION, 0, 0);
	::SendMessage(sci, SCI_SETTARGETRANGE, start, end);
	::SendMessage(sci, SCI_REPLACETARGET, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(textUtf8.c_str()));
	::SendMessage(sci, SCI_ENDUNDOACTION, 0, 0);
	return true;
}

void SelectionService::selectRange(Sci_Position start, Sci_Position end) const
{
	::SendMessage(currentScintilla(), SCI_SETSEL, start, end);
}

std::vector<TextSpan> SelectionService::getSpansToTranslate() const
{
	HWND sci = currentScintilla();
	std::vector<TextSpan> spans;

	const auto nSel = static_cast<int>(::SendMessage(sci, SCI_GETSELECTIONS, 0, 0));
	bool anyNonEmpty = false;
	for (int i = 0; i < nSel; ++i)
	{
		const Sci_Position start = ::SendMessage(sci, SCI_GETSELECTIONNSTART, i, 0);
		const Sci_Position end = ::SendMessage(sci, SCI_GETSELECTIONNEND, i, 0);
		if (end > start)
			anyNonEmpty = true;
	}

	if (anyNonEmpty)
	{
		spans.reserve(static_cast<size_t>(nSel));
		for (int i = 0; i < nSel; ++i)
		{
			TextSpan span;
			span.start = ::SendMessage(sci, SCI_GETSELECTIONNSTART, i, 0);
			span.end = ::SendMessage(sci, SCI_GETSELECTIONNEND, i, 0);
			if (span.end <= span.start)
				continue;

			const Sci_Position len = span.end - span.start;
			span.textUtf8.assign(static_cast<size_t>(len) + 1, '\0');
			Sci_TextRangeFull tr{};
			tr.chrg.cpMin = span.start;
			tr.chrg.cpMax = span.end;
			tr.lpstrText = span.textUtf8.data();
			::SendMessage(sci, SCI_GETTEXTRANGEFULL, 0, reinterpret_cast<LPARAM>(&tr));
			span.textUtf8.resize(static_cast<size_t>(len));
			spans.push_back(std::move(span));
		}
	}
	else
	{
		// Current line
		const Sci_Position caret = ::SendMessage(sci, SCI_GETCURRENTPOS, 0, 0);
		const Sci_Position line = ::SendMessage(sci, SCI_LINEFROMPOSITION, caret, 0);
		TextSpan span;
		span.start = ::SendMessage(sci, SCI_POSITIONFROMLINE, line, 0);
		span.end = ::SendMessage(sci, SCI_GETLINEENDPOSITION, line, 0);
		if (span.end > span.start)
		{
			const Sci_Position len = span.end - span.start;
			span.textUtf8.assign(static_cast<size_t>(len) + 1, '\0');
			Sci_TextRangeFull tr{};
			tr.chrg.cpMin = span.start;
			tr.chrg.cpMax = span.end;
			tr.lpstrText = span.textUtf8.data();
			::SendMessage(sci, SCI_GETTEXTRANGEFULL, 0, reinterpret_cast<LPARAM>(&tr));
			span.textUtf8.resize(static_cast<size_t>(len));
			spans.push_back(std::move(span));
		}
	}

	return spans;
}

bool SelectionService::replaceSpans(const std::vector<TextSpan>& originalSpans,
	const std::vector<std::string>& replacementsUtf8) const
{
	if (originalSpans.size() != replacementsUtf8.size() || originalSpans.empty())
		return false;

	HWND sci = currentScintilla();

	struct Indexed
	{
		size_t index;
		Sci_Position start;
	};
	std::vector<Indexed> order;
	order.reserve(originalSpans.size());
	for (size_t i = 0; i < originalSpans.size(); ++i)
		order.push_back({ i, originalSpans[i].start });

	std::sort(order.begin(), order.end(), [](const Indexed& a, const Indexed& b) {
		return a.start > b.start; // right-to-left
	});

	::SendMessage(sci, SCI_BEGINUNDOACTION, 0, 0);
	for (const auto& item : order)
	{
		const TextSpan& span = originalSpans[item.index];
		const std::string& repl = replacementsUtf8[item.index];
		::SendMessage(sci, SCI_SETTARGETRANGE, span.start, span.end);
		::SendMessage(sci, SCI_REPLACETARGET, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(repl.c_str()));
	}
	::SendMessage(sci, SCI_ENDUNDOACTION, 0, 0);
	return true;
}
