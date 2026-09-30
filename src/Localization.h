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
