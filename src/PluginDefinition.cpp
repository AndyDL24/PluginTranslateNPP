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

#include "PluginDefinition.h"
#include "Settings.h"
#include "TranslateEngine.h"
#include "Dialogs.h"
#include "Localization.h"
#include "CsvUtil.h"

FuncItem funcItem[nbFunc];
NppData nppData;
HINSTANCE g_hInstance = nullptr;

namespace
{
	ShortcutKey* g_skTranslate = nullptr;
	ShortcutKey* g_skColumn = nullptr;
	const int kSeparatorIndex = 3;

	void showError(const std::wstring& msg)
	{
		::MessageBoxW(nppData._nppHandle, msg.c_str(), L"NppTranslator", MB_OK | MB_ICONWARNING);
	}

	void showInfo(const std::wstring& msg)
	{
		::MessageBoxW(nppData._nppHandle, msg.c_str(), L"NppTranslator", MB_OK | MB_ICONINFORMATION);
	}

	void applyLocalizedMenuLabels()
	{
		lstrcpyW(funcItem[0]._itemName, Localization::get(LocId::MenuTranslateReplace));
		lstrcpyW(funcItem[1]._itemName, Localization::get(LocId::MenuPreview));
		lstrcpyW(funcItem[2]._itemName, Localization::get(LocId::MenuTranslateColumn));
		lstrcpyW(funcItem[3]._itemName, L"---");
		lstrcpyW(funcItem[4]._itemName, Localization::get(LocId::MenuSwapLanguages));
		lstrcpyW(funcItem[5]._itemName, Localization::get(LocId::MenuSettings));
		lstrcpyW(funcItem[6]._itemName, Localization::get(LocId::MenuAbout));

		HMENU hMenu = ::GetMenu(nppData._nppHandle);
		if (!hMenu)
			return;

		for (int i = 0; i < nbFunc; ++i)
		{
			if (funcItem[i]._cmdID == 0)
				continue;

			MENUITEMINFOW mii{};
			mii.cbSize = sizeof(mii);
			if (i == kSeparatorIndex)
			{
				mii.fMask = MIIM_FTYPE;
				mii.fType = MFT_SEPARATOR;
			}
			else
			{
				mii.fMask = MIIM_STRING | MIIM_FTYPE;
				mii.fType = MFT_STRING;
				mii.dwTypeData = funcItem[i]._itemName;
			}
			::SetMenuItemInfoW(hMenu, static_cast<UINT>(funcItem[i]._cmdID), FALSE, &mii);
		}
		::DrawMenuBar(nppData._nppHandle);
	}
}

void pluginInit(HANDLE hModule)
{
	g_hInstance = static_cast<HINSTANCE>(hModule);
}

void pluginCleanUp()
{
}

void commandMenuInit()
{
	GetSettings().load();
	Localization::refresh(nppData._nppHandle);

	g_skTranslate = new ShortcutKey();
	g_skTranslate->_isCtrl = true;
	g_skTranslate->_isAlt = false;
	g_skTranslate->_isShift = true;
	g_skTranslate->_key = 0x54; // 'T'

	g_skColumn = new ShortcutKey();
	g_skColumn->_isCtrl = true;
	g_skColumn->_isAlt = true;
	g_skColumn->_isShift = false;
	g_skColumn->_key = 0x54; // 'T'

	setCommand(0, Localization::get(LocId::MenuTranslateReplace), translateAndReplace, g_skTranslate, false);
	setCommand(1, Localization::get(LocId::MenuPreview), translatePreview, nullptr, false);
	setCommand(2, Localization::get(LocId::MenuTranslateColumn), translateIntoColumn, g_skColumn, false);
	setCommand(3, L"---", nullptr, nullptr, false);
	setCommand(4, Localization::get(LocId::MenuSwapLanguages), swapLanguages, nullptr, false);
	setCommand(5, Localization::get(LocId::MenuSettings), openSettings, nullptr, false);
	setCommand(6, Localization::get(LocId::MenuAbout), openAbout, nullptr, false);
}

void commandMenuCleanUp()
{
	delete g_skTranslate;
	g_skTranslate = nullptr;
	delete g_skColumn;
	g_skColumn = nullptr;
}

void refreshLocalizationAndMenu()
{
	Localization::refresh(nppData._nppHandle);
	applyLocalizedMenuLabels();
}

bool setCommand(size_t index, const wchar_t* cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk, bool check0nInit)
{
	if (index >= static_cast<size_t>(nbFunc))
		return false;
	if (!pFunc && (cmdName == nullptr || cmdName[0] == L'-'))
	{
		lstrcpyW(funcItem[index]._itemName, cmdName ? cmdName : L"---");
		funcItem[index]._pFunc = nullptr;
		funcItem[index]._init2Check = false;
		funcItem[index]._pShKey = nullptr;
		return true;
	}
	if (!cmdName || !pFunc)
		return false;

	lstrcpyW(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;
	return true;
}

void translateAndReplace()
{
	TranslateEngine engine(nppData);
	PreparedTranslation prepared = engine.translateCurrentSelections();
	if (!prepared.ok)
	{
		showError(prepared.error);
		return;
	}
	if (!engine.apply(prepared))
		showError(Localization::get(LocId::MsgApplyFailed));
}

void translatePreview()
{
	TranslateEngine engine(nppData);
	PreparedTranslation prepared = engine.translateCurrentSelections();
	if (!prepared.ok)
	{
		showError(prepared.error);
		return;
	}

	bool apply = false;
	ShowPreviewDialog(g_hInstance, nppData._nppHandle, prepared.spans, prepared.translations, apply);
	if (apply)
	{
		if (!engine.apply(prepared))
			showError(Localization::get(LocId::MsgApplyFailed));
	}
}

void translateIntoColumn()
{
	SelectionService selection(nppData);
	if (selection.nonEmptySelectionCount() > 1)
	{
		showError(Localization::get(LocId::MsgCsvSingleOnly));
		return;
	}

	Sci_Position selStart = 0;
	Sci_Position selEnd = 0;
	selection.mainSelection(selStart, selEnd);
	const Sci_Position lineIndex = selection.lineIndexFromPos(selStart);

	TextSpan line;
	TextSpan firstLine;
	if (!selection.lineSpan(lineIndex, line) || !selection.lineSpan(0, firstLine))
	{
		showError(Localization::get(LocId::MsgNoText));
		return;
	}

	PluginSettings& settings = GetSettings();
	const char headerDelim = DetectCsvDelimiter(firstLine.textUtf8);
	const CsvLine headerParsed = ParseCsvLine(firstLine.textUtf8, headerDelim);
	const bool hasHeader = FindLangColumn(headerParsed, settings.targetLang) >= 0
		|| FindLangColumn(headerParsed, settings.sourceLang) >= 0
		|| FindLangColumn(headerParsed, L"en") >= 0;
	const CsvLine* header = hasHeader ? &headerParsed : nullptr;
	if (hasHeader && lineIndex == 0)
	{
		showError(Localization::get(LocId::MsgCsvHeaderRow));
		return;
	}

	const char delimiter = hasHeader ? headerDelim : DetectCsvDelimiter(line.textUtf8);
	const CsvLine csv = ParseCsvLine(line.textUtf8, delimiter);

	const bool wholeLine = selEnd > selStart && selStart <= line.start && selEnd >= line.end;
	int sourceField = -1;
	if (wholeLine && csv.fields.size() == 1)
	{
		sourceField = 0;
	}
	else if (wholeLine)
	{
		if (header)
		{
			if (_wcsicmp(settings.sourceLang.c_str(), L"auto") != 0)
				sourceField = FindLangColumn(*header, settings.sourceLang);
			else
				sourceField = FindLangColumn(*header, L"en");
		}
		if (sourceField < 0)
		{
			showError(Localization::get(LocId::MsgCsvOneCell));
			return;
		}
	}
	else
	{
		const size_t relStart = selStart >= line.start ? static_cast<size_t>(selStart - line.start) : 0;
		const size_t relEnd = (selEnd > line.start) ? static_cast<size_t>(selEnd - line.start) : relStart;
		const int startField = FieldAt(csv, relStart);
		const size_t endProbe = relEnd > relStart ? relEnd - 1 : relStart;
		const int endField = FieldAt(csv, endProbe);
		if (startField < 0 || startField != endField)
		{
			showError(Localization::get(LocId::MsgCsvOneCell));
			return;
		}
		sourceField = startField;
	}

	if (sourceField < 0 || static_cast<size_t>(sourceField) >= csv.fields.size())
	{
		showError(Localization::get(LocId::MsgCsvOneCell));
		return;
	}

	const std::string sourceText = TrimAscii(csv.fields[static_cast<size_t>(sourceField)].text);
	if (sourceText.empty())
	{
		showError(Localization::get(LocId::MsgCsvEmptySource));
		return;
	}

	int targetField = ResolveCsvColumn(settings.csvColumn, header);
	if (targetField < 0 && header)
		targetField = FindLangColumn(*header, settings.targetLang);
	if (targetField < 0)
	{
		std::wstring spec = std::to_wstring(sourceField + 2);
		if (ShowCsvColumnDialog(g_hInstance, nppData._nppHandle, spec) != IDOK)
			return;
		targetField = ResolveCsvColumn(spec, header);
		if (targetField < 0)
		{
			showError(Localization::get(LocId::MsgCsvBadColumn));
			return;
		}
		settings.csvColumn = TrimWide(spec);
		settings.save();
	}

	if (targetField == sourceField)
	{
		showError(Localization::get(LocId::MsgCsvSameColumn));
		return;
	}

	TranslateEngine engine(nppData);
	const TranslateResult result = engine.translateTexts({ sourceText });
	if (!result.ok || result.translations.empty())
	{
		showError(result.error.empty()
			? Localization::format(LocId::MsgTranslateFailed, L"NppTranslator")
			: result.error);
		return;
	}

	const std::string& translated = result.translations[0];
	if (static_cast<size_t>(targetField) < csv.fields.size())
	{
		const CsvField& field = csv.fields[static_cast<size_t>(targetField)];
		const std::string encoded = EncodeCsvField(translated, delimiter, field.quoted);
		selection.replaceRange(
			line.start + static_cast<Sci_Position>(field.start),
			line.start + static_cast<Sci_Position>(field.end),
			encoded);
	}
	else
	{
		std::string extra;
		for (int i = static_cast<int>(csv.fields.size()); i < targetField; ++i)
			extra.push_back(delimiter);
		extra += EncodeCsvField(translated, delimiter, false);
		selection.replaceRange(line.end, line.end, extra);
	}

	const Sci_Position next = lineIndex + 1;
	if (next >= selection.lineCount())
		return;

	TextSpan nextLine;
	if (!selection.lineSpan(next, nextLine))
		return;

	const CsvLine nextCsv = ParseCsvLine(nextLine.textUtf8, delimiter);
	if (static_cast<size_t>(sourceField) < nextCsv.fields.size())
	{
		const CsvField& field = nextCsv.fields[static_cast<size_t>(sourceField)];
		selection.selectRange(
			nextLine.start + static_cast<Sci_Position>(field.start),
			nextLine.start + static_cast<Sci_Position>(field.end));
		return;
	}
	selection.selectRange(nextLine.start, nextLine.start);
}

void swapLanguages()
{
	PluginSettings& s = GetSettings();
	s.swapLanguages();
	s.save();
	showInfo(Localization::format(LocId::MsgLanguagesUpdated, s.sourceLang, s.targetLang));
}

void openSettings()
{
	ShowSettingsDialog(g_hInstance, nppData._nppHandle);
}

void openAbout()
{
	ShowAboutDialog(g_hInstance, nppData._nppHandle);
}
