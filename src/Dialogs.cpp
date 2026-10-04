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

#include "Dialogs.h"
#include "Settings.h"
#include "Utf8Util.h"
#include "Localization.h"
#include "resource.h"
#include <objbase.h>
#include <shellapi.h>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

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
		::SetDlgItemTextW(hwnd, IDC_STATIC_CSV_COLUMN, Localization::get(LocId::LabelCsvColumn));
		::SetDlgItemTextW(hwnd, IDC_STATIC_CSV_HINT, Localization::get(LocId::LabelCsvColumnHint));
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
			::SetDlgItemTextW(hwnd, IDC_CSV_COLUMN, s.csvColumn.c_str());
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
				::GetDlgItemTextW(hwnd, IDC_CSV_COLUMN, buf, 512);
				s.csvColumn = buf;
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

	HBITMAP LoadScaledPng(HINSTANCE inst, int resId, int maxWidth, int& outW, int& outH)
	{
		outW = 0;
		outH = 0;
		HRSRC res = ::FindResourceW(inst, MAKEINTRESOURCEW(resId), RT_RCDATA);
		if (!res)
			return nullptr;
		HGLOBAL loaded = ::LoadResource(inst, res);
		const void* bytes = ::LockResource(loaded);
		const DWORD size = ::SizeofResource(inst, res);
		if (!bytes || size == 0)
			return nullptr;

		HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE, size);
		if (!mem)
			return nullptr;
		void* dest = ::GlobalLock(mem);
		if (!dest)
		{
			::GlobalFree(mem);
			return nullptr;
		}
		memcpy(dest, bytes, size);
		::GlobalUnlock(mem);

		IStream* stream = nullptr;
		if (FAILED(::CreateStreamOnHGlobal(mem, TRUE, &stream)))
		{
			::GlobalFree(mem);
			return nullptr;
		}

		const HRESULT co = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		const bool uninitCom = SUCCEEDED(co);

		IWICImagingFactory* factory = nullptr;
		IWICBitmapDecoder* decoder = nullptr;
		IWICBitmapFrameDecode* frame = nullptr;
		IWICBitmapScaler* scaler = nullptr;
		IWICFormatConverter* conv = nullptr;
		HBITMAP bmp = nullptr;

		if (SUCCEEDED(::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))
			&& SUCCEEDED(factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder))
			&& SUCCEEDED(decoder->GetFrame(0, &frame)))
		{
			UINT w = 0;
			UINT h = 0;
			frame->GetSize(&w, &h);
			UINT dw = w;
			UINT dh = h;
			if (w > static_cast<UINT>(maxWidth) && w > 0)
			{
				dw = static_cast<UINT>(maxWidth);
				dh = static_cast<UINT>((static_cast<unsigned long long>(h) * static_cast<unsigned>(maxWidth)) / w);
			}
			if (dh == 0)
				dh = 1;

			if (SUCCEEDED(factory->CreateBitmapScaler(&scaler))
				&& SUCCEEDED(scaler->Initialize(frame, dw, dh, WICBitmapInterpolationModeFant))
				&& SUCCEEDED(factory->CreateFormatConverter(&conv))
				&& SUCCEEDED(conv->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom)))
			{
				BITMAPINFO bi{};
				bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bi.bmiHeader.biWidth = static_cast<LONG>(dw);
				bi.bmiHeader.biHeight = -static_cast<LONG>(dh);
				bi.bmiHeader.biPlanes = 1;
				bi.bmiHeader.biBitCount = 32;
				bi.bmiHeader.biCompression = BI_RGB;
				void* bits = nullptr;
				HDC dc = ::GetDC(nullptr);
				bmp = ::CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
				::ReleaseDC(nullptr, dc);
				if (bmp && bits)
				{
					const UINT stride = dw * 4;
					if (FAILED(conv->CopyPixels(nullptr, stride, stride * dh, static_cast<BYTE*>(bits))))
					{
						::DeleteObject(bmp);
						bmp = nullptr;
					}
					else
					{
						outW = static_cast<int>(dw);
						outH = static_cast<int>(dh);
					}
				}
			}
		}

		if (conv)
			conv->Release();
		if (scaler)
			scaler->Release();
		if (frame)
			frame->Release();
		if (decoder)
			decoder->Release();
		if (factory)
			factory->Release();
		stream->Release();
		if (uninitCom)
			::CoUninitialize();
		return bmp;
	}

	void LayoutAboutDialog(HWND hwnd, int imgW, int imgH)
	{
		RECT client{};
		::GetClientRect(hwnd, &client);
		RECT margin{ 12, 10, 0, 0 };
		::MapDialogRect(hwnd, &margin);

		HWND text = ::GetDlgItem(hwnd, IDC_ABOUT_TEXT);
		RECT textRc{};
		::GetWindowRect(text, &textRc);
		::MapWindowPoints(nullptr, hwnd, reinterpret_cast<POINT*>(&textRc), 2);

		HWND kofi = ::GetDlgItem(hwnd, IDC_KOFI_BTN);
		const int x = (client.right - imgW) / 2;
		const int y = textRc.bottom + margin.top;
		::MoveWindow(kofi, x, y, imgW, imgH, TRUE);

		HWND ok = ::GetDlgItem(hwnd, IDOK);
		RECT okRc{};
		::GetWindowRect(ok, &okRc);
		const int okW = okRc.right - okRc.left;
		const int okH = okRc.bottom - okRc.top;
		const int okY = y + imgH + margin.top;
		::MoveWindow(ok, (client.right - okW) / 2, okY, okW, okH, TRUE);

		RECT dlg{};
		::GetWindowRect(hwnd, &dlg);
		const int extra = (okY + okH + margin.top) - client.bottom;
		if (extra > 0)
		{
			::SetWindowPos(hwnd, nullptr, 0, 0,
				dlg.right - dlg.left, dlg.bottom - dlg.top + extra,
				SWP_NOMOVE | SWP_NOZORDER);
		}
	}

	INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM /*lParam*/)
	{
		switch (msg)
		{
		case WM_INITDIALOG:
		{
			::SetWindowTextW(hwnd, Localization::get(LocId::DlgAboutTitle));
			::SetDlgItemTextW(hwnd, IDC_ABOUT_TEXT, Localization::get(LocId::AboutText));
			::SetDlgItemTextW(hwnd, IDOK, Localization::get(LocId::BtnOk));

			const HINSTANCE inst = reinterpret_cast<HINSTANCE>(::GetWindowLongPtrW(hwnd, GWLP_HINSTANCE));
			int imgW = 0;
			int imgH = 0;
			HBITMAP bmp = LoadScaledPng(inst, IDR_KOFI_PNG, 300, imgW, imgH);
			if (bmp)
			{
				::SendDlgItemMessageW(hwnd, IDC_KOFI_BTN, STM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(bmp));
				::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(bmp));
				LayoutAboutDialog(hwnd, imgW, imgH);
			}
			else
			{
				::ShowWindow(::GetDlgItem(hwnd, IDC_KOFI_BTN), SW_HIDE);
			}
			return TRUE;
		}
		case WM_SETCURSOR:
			if (reinterpret_cast<HWND>(wParam) == ::GetDlgItem(hwnd, IDC_KOFI_BTN))
			{
				::SetCursor(::LoadCursorW(nullptr, IDC_HAND));
				return TRUE;
			}
			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_KOFI_BTN)
			{
				::ShellExecuteW(hwnd, L"open", L"https://ko-fi.com/andyd24", nullptr, nullptr, SW_SHOWNORMAL);
				return TRUE;
			}
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				::EndDialog(hwnd, IDOK);
				return TRUE;
			}
			break;
		case WM_DESTROY:
		{
			HBITMAP bmp = reinterpret_cast<HBITMAP>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
			if (bmp)
				::DeleteObject(bmp);
			break;
		}
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

namespace
{
	INT_PTR CALLBACK CsvColumnDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		std::wstring* spec = reinterpret_cast<std::wstring*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		switch (msg)
		{
		case WM_INITDIALOG:
			spec = reinterpret_cast<std::wstring*>(lParam);
			::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(spec));
			::SetWindowTextW(hwnd, Localization::get(LocId::DlgCsvColumnTitle));
			::SetDlgItemTextW(hwnd, IDC_STATIC_CSV_COLUMN, Localization::get(LocId::LabelCsvColumn));
			::SetDlgItemTextW(hwnd, IDC_STATIC_CSV_HINT, Localization::get(LocId::LabelCsvColumnHint));
			::SetDlgItemTextW(hwnd, IDOK, Localization::get(LocId::BtnOk));
			::SetDlgItemTextW(hwnd, IDCANCEL, Localization::get(LocId::BtnCancel));
			if (spec)
				::SetDlgItemTextW(hwnd, IDC_CSV_COLUMN, spec->c_str());
			return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK)
			{
				wchar_t buf[256] = {};
				::GetDlgItemTextW(hwnd, IDC_CSV_COLUMN, buf, 256);
				if (spec)
					*spec = buf;
				::EndDialog(hwnd, IDOK);
				return TRUE;
			}
			if (LOWORD(wParam) == IDCANCEL)
			{
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}

INT_PTR ShowCsvColumnDialog(HINSTANCE hInst, HWND parent, std::wstring& columnSpec)
{
	return ::DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_CSV_COLUMN_DLG), parent, CsvColumnDlgProc,
		reinterpret_cast<LPARAM>(&columnSpec));
}
