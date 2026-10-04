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
#include <windows.h>

enum class UiLang
{
	English = 0,
	Italian = 1
};

enum class LocId
{
	MenuTranslateReplace = 0,
	MenuPreview,
	MenuSwapLanguages,
	MenuSettings,
	MenuAbout,

	DlgSettingsTitle,
	DlgPreviewTitle,
	DlgAboutTitle,

	LabelProvider,
	LabelSourceLang,
	LabelTargetLang,
	LabelDeeplKey,
	LabelDeeplEndpoint,
	LabelLangCodesHint,
	LabelPhrases,
	LabelOriginal,
	LabelTranslated,

	BtnOk,
	BtnCancel,
	BtnApply,

	ProviderGoogleFree,
	ProviderDeepL,
	EndpointFree,
	EndpointPro,

	AboutText,

	MsgNoText,
	MsgTranslateFailed,
	MsgApplyFailed,
	MsgLanguagesUpdated,
	MsgSource,
	MsgTarget,

	ErrWinHttpSession,
	ErrWinHttpConnect,
	ErrWinHttpRequest,
	ErrNetwork,

	ErrGoogleLimited,
	ErrGoogleHttp,
	ErrGoogleCaptcha,
	ErrGoogleInvalid,

	ErrDeeplMissingKey,
	ErrDeeplForbidden,
	ErrDeeplQuota,
	ErrDeeplHttp,
	ErrDeeplInvalid,

	MenuTranslateColumn,
	LabelCsvColumn,
	LabelCsvColumnHint,
	DlgCsvColumnTitle,
	MsgCsvSingleOnly,
	MsgCsvOneCell,
	MsgCsvHeaderRow,
	MsgCsvEmptySource,
	MsgCsvSameColumn,
	MsgCsvBadColumn,

	Count
};

class Localization
{
public:
	static void refresh(HWND nppHandle);
	static UiLang language();
	static bool isItalian();
	static const wchar_t* get(LocId id);
	static std::wstring format(LocId id, const std::wstring& a);
	static std::wstring format(LocId id, const std::wstring& a, const std::wstring& b);
};
