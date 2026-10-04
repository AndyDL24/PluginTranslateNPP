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
#include <vector>
#include <utility>

struct HttpResponse
{
	int statusCode = 0;
	std::string body;
	std::wstring error;
};

class HttpClient
{
public:
	static HttpResponse get(const std::wstring& host, const std::wstring& pathAndQuery,
		const std::vector<std::pair<std::wstring, std::wstring>>& headers = {});

	static HttpResponse post(const std::wstring& host, const std::wstring& path,
		const std::string& bodyUtf8, const std::wstring& contentType,
		const std::vector<std::pair<std::wstring, std::wstring>>& headers = {});
};
