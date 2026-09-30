#pragma once

#include <string>
#include <windows.h>

inline std::wstring Utf8ToWide(const std::string& utf8)
{
	if (utf8.empty())
		return {};
	const int needed = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
	if (needed <= 0)
		return {};
	std::wstring out(static_cast<size_t>(needed), L'\0');
	::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), out.data(), needed);
	return out;
}

inline std::string WideToUtf8(const std::wstring& wide)
{
	if (wide.empty())
		return {};
	const int needed = ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
	if (needed <= 0)
		return {};
	std::string out(static_cast<size_t>(needed), '\0');
	::WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), out.data(), needed, nullptr, nullptr);
	return out;
}

inline std::string UrlEncode(const std::string& value)
{
	static const char* hex = "0123456789ABCDEF";
	std::string out;
	out.reserve(value.size() * 3);
	for (unsigned char c : value)
	{
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')
			|| c == '-' || c == '_' || c == '.' || c == '~')
		{
			out.push_back(static_cast<char>(c));
		}
		else if (c == ' ')
		{
			out.push_back('+');
		}
		else
		{
			out.push_back('%');
			out.push_back(hex[c >> 4]);
			out.push_back(hex[c & 0x0F]);
		}
	}
	return out;
}
