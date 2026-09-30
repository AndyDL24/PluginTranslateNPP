#include "Dialogs.h"
#include "Settings.h"
#include "Utf8Util.h"
#include "Localization.h"
#include "resource.h"

namespace
{
	struct PreviewDlgData
	{
		const std::vector<TextSpan>* spans = nullptr;
		const std::vector<std::string>* translations = nullptr;
		bool* applyRequested = nullptr;
	};

	void FillProviderCombo(HWND hCombo)
	{
		::SendMessageW(hCombo, CB_RESETCONTENT, 0, 0);
		::SendMessageW(hCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Localization::get(LocId::ProviderGoogleFree)));
		::SendMessageW(hCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Localization::get(LocId::ProviderDeepL)));
	}

	void FillEndpointCombo(HWND hCombo)
	{
		::SendMessageW(hCombo, CB_RESETCONTENT, 0, 0);
		::SendMessageW(hCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Localization::get(LocId::EndpointFree)));
		::SendMessageW(hCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Localization::get(LocId::EndpointPro)));
	}

	void LocalizeSettingsDialog(HWND hwnd)
	{
		::SetWindowTextW(hwnd, Localization::get(LocId::DlgSettingsTitle));
		::SetDlgItemTextW(hwnd, IDC_STATIC_PROVIDER, Localization::get(LocId::LabelProvider));
		::SetDlgItemTextW(hwnd, IDC_STATIC_SOURCE, Localization::get(LocId::LabelSourceLang));
		::SetDlgItemTextW(hwnd, IDC_STATIC_TARGET, Localization::get(LocId::LabelTargetLang));
		::SetDlgItemTextW(hwnd, IDC_STATIC_DEEPL_KEY, Localization::get(LocId::LabelDeeplKey));
		::SetDlgItemTextW(hwnd, IDC_STATIC_DEEPL_EP, Localization::get(LocId::LabelDeeplEndpoint));
		::SetDlgItemTextW(hwnd, IDC_STATIC_LANG_HINT, Localization::get(LocId::LabelLangCodesHint));
		::SetDlgItemTextW(hwnd, IDOK, Localization::get(LocId::BtnOk));
		::SetDlgItemTextW(hwnd, IDCANCEL, Localization::get(LocId::BtnCancel));
	}

	void LocalizePreviewDialog(HWND hwnd)
	{
		::SetWindowTextW(hwnd, Localization::get(LocId::DlgPreviewTitle));
		::SetDlgItemTextW(hwnd, IDC_STATIC_ORIGINAL, Localization::get(LocId::LabelOriginal));
		::SetDlgItemTextW(hwnd, IDC_STATIC_TRANSLATED, Localization::get(LocId::LabelTranslated));
		::SetDlgItemTextW(hwnd, IDC_BTN_APPLY, Localization::get(LocId::BtnApply));
		::SetDlgItemTextW(hwnd, IDC_BTN_CANCEL, Localization::get(LocId::BtnCancel));
	}

	INT_PTR CALLBACK SettingsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM /*lParam*/)
	{
		switch (msg)
		{
		case WM_INITDIALOG:
		{
			LocalizeSettingsDialog(hwnd);
			PluginSettings& s = GetSettings();
			FillProviderCombo(::GetDlgItem(hwnd, IDC_PROVIDER));
			FillEndpointCombo(::GetDlgItem(hwnd, IDC_DEEPL_ENDPOINT));
			::SendDlgItemMessageW(hwnd, IDC_PROVIDER, CB_SETCURSEL,
				s.provider == TranslateProvider::DeepL ? 1 : 0, 0);
			::SendDlgItemMessageW(hwnd, IDC_DEEPL_ENDPOINT, CB_SETCURSEL,
				s.deeplEndpoint == DeepLEndpoint::Pro ? 1 : 0, 0);
			::SetDlgItemTextW(hwnd, IDC_SOURCE_LANG, s.sourceLang.c_str());
			::SetDlgItemTextW(hwnd, IDC_TARGET_LANG, s.targetLang.c_str());
			::SetDlgItemTextW(hwnd, IDC_DEEPL_KEY, s.deeplApiKey.c_str());
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
			{
				PluginSettings& s = GetSettings();
				const LRESULT providerSel = ::SendDlgItemMessageW(hwnd, IDC_PROVIDER, CB_GETCURSEL, 0, 0);
				s.provider = (providerSel == 1) ? TranslateProvider::DeepL : TranslateProvider::GoogleFree;
				const LRESULT epSel = ::SendDlgItemMessageW(hwnd, IDC_DEEPL_ENDPOINT, CB_GETCURSEL, 0, 0);
				s.deeplEndpoint = (epSel == 1) ? DeepLEndpoint::Pro : DeepLEndpoint::Free;

				wchar_t buf[512] = {};
				::GetDlgItemTextW(hwnd, IDC_SOURCE_LANG, buf, 512);
				s.sourceLang = buf;
				::GetDlgItemTextW(hwnd, IDC_TARGET_LANG, buf, 512);
				s.targetLang = buf;
				::GetDlgItemTextW(hwnd, IDC_DEEPL_KEY, buf, 512);
				s.deeplApiKey = buf;
				s.save();
				::EndDialog(hwnd, IDOK);
				return TRUE;
			}
			case IDCANCEL:
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}

	INT_PTR CALLBACK PreviewDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		PreviewDlgData* data = reinterpret_cast<PreviewDlgData*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		switch (msg)
		{
		case WM_INITDIALOG:
		{
			data = reinterpret_cast<PreviewDlgData*>(lParam);
			::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
			LocalizePreviewDialog(hwnd);

			std::wstring original;
			std::wstring translated;
			for (size_t i = 0; i < data->spans->size(); ++i)
			{
				if (i > 0)
				{
					original += L"\r\n----------\r\n";
					translated += L"\r\n----------\r\n";
				}
				original += Utf8ToWide((*data->spans)[i].textUtf8);
				if (i < data->translations->size())
					translated += Utf8ToWide((*data->translations)[i]);
			}
			::SetDlgItemTextW(hwnd, IDC_PREVIEW_ORIGINAL, original.c_str());
			::SetDlgItemTextW(hwnd, IDC_PREVIEW_TRANSLATED, translated.c_str());
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_BTN_APPLY:
			case IDOK:
				if (data && data->applyRequested)
					*data->applyRequested = true;
				::EndDialog(hwnd, IDOK);
				return TRUE;
			case IDC_BTN_CANCEL:
			case IDCANCEL:
				if (data && data->applyRequested)
					*data->applyRequested = false;
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}

	INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM /*lParam*/)
	{
		switch (msg)
		{
		case WM_INITDIALOG:
			::SetWindowTextW(hwnd, Localization::get(LocId::DlgAboutTitle));
			::SetDlgItemTextW(hwnd, IDC_ABOUT_TEXT, Localization::get(LocId::AboutText));
			::SetDlgItemTextW(hwnd, IDOK, Localization::get(LocId::BtnOk));
			return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				::EndDialog(hwnd, IDOK);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}

INT_PTR ShowSettingsDialog(HINSTANCE hInst, HWND parent)
{
	return ::DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_SETTINGS_DLG), parent, SettingsDlgProc, 0);
}

INT_PTR ShowPreviewDialog(HINSTANCE hInst, HWND parent,
	const std::vector<TextSpan>& spans,
	const std::vector<std::string>& translations,
	bool& applyRequested)
{
	applyRequested = false;
	PreviewDlgData data{ &spans, &translations, &applyRequested };
	return ::DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_PREVIEW_DLG), parent, PreviewDlgProc,
		reinterpret_cast<LPARAM>(&data));
}

void ShowAboutDialog(HINSTANCE hInst, HWND parent)
{
	::DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_ABOUT_DLG), parent, AboutDlgProc, 0);
}
