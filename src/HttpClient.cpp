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

#include "HttpClient.h"
#include "Localization.h"
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

namespace
{
	HttpResponse request(const wchar_t* method, const std::wstring& host, const std::wstring& path,
		const std::string* body, const std::wstring& contentType,
		const std::vector<std::pair<std::wstring, std::wstring>>& headers)
	{
		HttpResponse result;

		HINTERNET hSession = ::WinHttpOpen(L"NppTranslator/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (!hSession)
		{
			result.error = Localization::get(LocId::ErrWinHttpSession);
			return result;
		}

		::WinHttpSetTimeouts(hSession, 10000, 10000, 30000, 30000);

		HINTERNET hConnect = ::WinHttpConnect(hSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (!hConnect)
		{
			result.error = Localization::format(LocId::ErrWinHttpConnect, host);
			::WinHttpCloseHandle(hSession);
			return result;
		}

		HINTERNET hRequest = ::WinHttpOpenRequest(hConnect, method, path.c_str(), nullptr,
			WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
		if (!hRequest)
		{
			result.error = Localization::get(LocId::ErrWinHttpRequest);
			::WinHttpCloseHandle(hConnect);
			::WinHttpCloseHandle(hSession);
			return result;
		}

		std::wstring extraHeaders;
		if (!contentType.empty())
			extraHeaders += L"Content-Type: " + contentType + L"\r\n";
		for (const auto& h : headers)
			extraHeaders += h.first + L": " + h.second + L"\r\n";

		LPCWSTR headersPtr = extraHeaders.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : extraHeaders.c_str();
		DWORD headersLen = extraHeaders.empty() ? 0 : static_cast<DWORD>(-1);

		BOOL ok = ::WinHttpSendRequest(hRequest, headersPtr, headersLen,
			body ? const_cast<char*>(body->data()) : WINHTTP_NO_REQUEST_DATA,
			body ? static_cast<DWORD>(body->size()) : 0,
			body ? static_cast<DWORD>(body->size()) : 0, 0);

		if (!ok || !::WinHttpReceiveResponse(hRequest, nullptr))
		{
			result.error = Localization::format(LocId::ErrNetwork, host);
			::WinHttpCloseHandle(hRequest);
			::WinHttpCloseHandle(hConnect);
			::WinHttpCloseHandle(hSession);
			return result;
		}

		DWORD status = 0;
		DWORD statusSize = sizeof(status);
		::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX);
		result.statusCode = static_cast<int>(status);

		std::string responseBody;
		DWORD available = 0;
		while (::WinHttpQueryDataAvailable(hRequest, &available) && available > 0)
		{
			std::string chunk(available, '\0');
			DWORD read = 0;
			if (!::WinHttpReadData(hRequest, chunk.data(), available, &read))
				break;
			chunk.resize(read);
			responseBody += chunk;
		}
		result.body = std::move(responseBody);

		::WinHttpCloseHandle(hRequest);
		::WinHttpCloseHandle(hConnect);
		::WinHttpCloseHandle(hSession);
		return result;
	}
}

HttpResponse HttpClient::get(const std::wstring& host, const std::wstring& pathAndQuery,
	const std::vector<std::pair<std::wstring, std::wstring>>& headers)
{
	return request(L"GET", host, pathAndQuery, nullptr, L"", headers);
}

HttpResponse HttpClient::post(const std::wstring& host, const std::wstring& path,
	const std::string& bodyUtf8, const std::wstring& contentType,
	const std::vector<std::pair<std::wstring, std::wstring>>& headers)
{
	return request(L"POST", host, path, &bodyUtf8, contentType, headers);
}
