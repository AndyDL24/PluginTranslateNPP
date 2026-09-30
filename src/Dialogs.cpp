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
		std::vector<std::string>* translations = nullptr;
		bool* applyRequested = nullptr;
		int currentIndex = -1;
	};

	std::wstring PreviewListLabel(size_t index, const std::string& textUtf8)
	{
		std::wstring text = Utf8ToWide(textUtf8);
		for (wchar_t& ch : text)
		{
			if (ch == L'\r' || ch == L'\n' || ch == L'\t')
				ch = L' ';
		}
		while (!text.empty() && text.front() == L' ')
			text.erase(text.begin());

		constexpr size_t maxLen = 42;
		if (text.size() > maxLen)
			text = text.substr(0, maxLen - 1) + L"…";

		return L"#" + std::to_wstring(index + 1) + L"  " + text;
	}

	std::wstring GetDlgItemString(HWND hwnd, int controlId)
	{
		const int len = ::GetWindowTextLengthW(::GetDlgItem(hwnd, controlId));
		if (len <= 0)
			return {};
		std::wstring text(static_cast<size_t>(len) + 1, L'\0');
		::GetDlgItemTextW(hwnd, controlId, text.data(), len + 1);
		text.resize(static_cast<size_t>(::wcslen(text.c_str())));
		return text;
	}

	void SaveCurrentTranslation(HWND hwnd, PreviewDlgData* data)
	{
		if (!data || !data->translations || data->currentIndex < 0)
			return;
		if (static_cast<size_t>(data->currentIndex) >= data->translations->size())
			return;
		(*data->translations)[static_cast<size_t>(data->currentIndex)] =
			WideToUtf8(GetDlgItemString(hwnd, IDC_PREVIEW_TRANSLATED));
	}

	void ShowPhrase(HWND hwnd, PreviewDlgData* data, int index)
	{
		if (!data || !data->spans || !data->translations)
			return;
		if (index < 0 || static_cast<size_t>(index) >= data->spans->size())
			return;

		data->currentIndex = index;
		::SetDlgItemTextW(hwnd, IDC_PREVIEW_ORIGINAL,
			Utf8ToWide((*data->spans)[static_cast<size_t>(index)].textUtf8).c_str());

		std::wstring translated;
		if (static_cast<size_t>(index) < data->translations->size())
			translated = Utf8ToWide((*data->translations)[static_cast<size_t>(index)]);
		::SetDlgItemTextW(hwnd, IDC_PREVIEW_TRANSLATED, translated.c_str());
	}

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
		::SetDlgItemTextW(hwnd, IDC_STATIC_PHRASES, Localization::get(LocId::LabelPhrases));
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

			HWND hList = ::GetDlgItem(hwnd, IDC_PREVIEW_LIST);
			::SendMessageW(hList, LB_RESETCONTENT, 0, 0);
			for (size_t i = 0; i < data->spans->size(); ++i)
			{
				const std::wstring label = PreviewListLabel(i, (*data->spans)[i].textUtf8);
				::SendMessageW(hList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
			}
			if (!data->spans->empty())
			{
				::SendMessageW(hList, LB_SETCURSEL, 0, 0);
				ShowPhrase(hwnd, data, 0);
			}
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_PREVIEW_LIST:
				if (HIWORD(wParam) == LBN_SELCHANGE && data)
				{
					const int sel = static_cast<int>(::SendDlgItemMessageW(hwnd, IDC_PREVIEW_LIST, LB_GETCURSEL, 0, 0));
					if (sel != data->currentIndex)
					{
						SaveCurrentTranslation(hwnd, data);
						ShowPhrase(hwnd, data, sel);
					}
				}
				return TRUE;
			case IDC_BTN_APPLY:
			case IDOK:
				SaveCurrentTranslation(hwnd, data);
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
	std::vector<std::string>& translations,
	bool& applyRequested)
{
	applyRequested = false;
	PreviewDlgData data{ &spans, &translations, &applyRequested, -1 };
	return ::DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_PREVIEW_DLG), parent, PreviewDlgProc,
		reinterpret_cast<LPARAM>(&data));
}

void ShowAboutDialog(HINSTANCE hInst, HWND parent)
{
	::DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_ABOUT_DLG), parent, AboutDlgProc, 0);
}
