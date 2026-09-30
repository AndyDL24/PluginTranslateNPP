#include "PluginDefinition.h"
#include "Settings.h"
#include "TranslateEngine.h"
#include "Dialogs.h"
#include "Localization.h"

FuncItem funcItem[nbFunc];
NppData nppData;
HINSTANCE g_hInstance = nullptr;

namespace
{
	ShortcutKey* g_skTranslate = nullptr;

	void showError(const std::wstring& msg)
	{
		::MessageBoxW(nppData._nppHandle, msg.c_str(), L"NppTranslate", MB_OK | MB_ICONWARNING);
	}

	void showInfo(const std::wstring& msg)
	{
		::MessageBoxW(nppData._nppHandle, msg.c_str(), L"NppTranslate", MB_OK | MB_ICONINFORMATION);
	}

	void applyLocalizedMenuLabels()
	{
		lstrcpyW(funcItem[0]._itemName, Localization::get(LocId::MenuTranslateReplace));
		lstrcpyW(funcItem[1]._itemName, Localization::get(LocId::MenuPreview));
		lstrcpyW(funcItem[2]._itemName, L"---");
		lstrcpyW(funcItem[3]._itemName, Localization::get(LocId::MenuSwapLanguages));
		lstrcpyW(funcItem[4]._itemName, Localization::get(LocId::MenuSettings));
		lstrcpyW(funcItem[5]._itemName, Localization::get(LocId::MenuAbout));

		HMENU hMenu = ::GetMenu(nppData._nppHandle);
		if (!hMenu)
			return;

		for (int i = 0; i < nbFunc; ++i)
		{
			if (funcItem[i]._cmdID == 0)
				continue;

			MENUITEMINFOW mii{};
			mii.cbSize = sizeof(mii);
			if (i == 2)
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

	setCommand(0, Localization::get(LocId::MenuTranslateReplace), translateAndReplace, g_skTranslate, false);
	setCommand(1, Localization::get(LocId::MenuPreview), translatePreview, nullptr, false);
	setCommand(2, L"---", nullptr, nullptr, false);
	setCommand(3, Localization::get(LocId::MenuSwapLanguages), swapLanguages, nullptr, false);
	setCommand(4, Localization::get(LocId::MenuSettings), openSettings, nullptr, false);
	setCommand(5, Localization::get(LocId::MenuAbout), openAbout, nullptr, false);
}

void commandMenuCleanUp()
{
	delete g_skTranslate;
	g_skTranslate = nullptr;
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
