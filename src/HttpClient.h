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
