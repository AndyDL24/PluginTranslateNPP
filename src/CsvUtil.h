// NppTranslate
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

#include "Utf8Util.h"
#include <cwctype>
#include <string>
#include <vector>

struct CsvField
{
	size_t start = 0;
	size_t end = 0;
	std::string text;
	bool quoted = false;
};

struct CsvLine
{
	char delimiter = ',';
	std::vector<CsvField> fields;
};

inline std::wstring TrimWide(std::wstring value)
{
	while (!value.empty() && iswspace(value.front()))
		value.erase(value.begin());
	while (!value.empty() && iswspace(value.back()))
		value.pop_back();
	return value;
}

inline std::string TrimAscii(std::string value)
{
	while (!value.empty() && (value.front() == ' ' || value.front() == '\t' || value.front() == '\r' || value.front() == '\n'))
		value.erase(value.begin());
	while (!value.empty() && (value.back() == ' ' || value.back() == '\t' || value.back() == '\r' || value.back() == '\n'))
		value.pop_back();
	return value;
}

inline char DetectCsvDelimiter(const std::string& line)
{
	int commas = 0;
	int semis = 0;
	bool inQuotes = false;
	for (size_t i = 0; i < line.size(); ++i)
	{
		const char c = line[i];
		if (c == '"')
		{
			if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
			{
				++i;
				continue;
			}
			inQuotes = !inQuotes;
		}
		else if (!inQuotes)
		{
			if (c == ',')
				++commas;
			else if (c == ';')
				++semis;
		}
	}
	return semis > commas ? ';' : ',';
}

inline CsvLine ParseCsvLine(const std::string& line, char delimiter)
{
	CsvLine out;
	out.delimiter = delimiter;
	const size_t n = line.size();
	size_t i = 0;
	while (true)
	{
		CsvField field;
		field.start = i;
		if (i < n && line[i] == '"')
		{
			field.quoted = true;
			++i;
			while (i < n)
			{
				if (line[i] == '"')
				{
					if (i + 1 < n && line[i + 1] == '"')
					{
						field.text.push_back('"');
						i += 2;
					}
					else
					{
						++i;
						break;
					}
				}
				else
				{
					field.text.push_back(line[i]);
					++i;
				}
			}
			field.end = i;
		}
		else
		{
			while (i < n && line[i] != delimiter)
			{
				field.text.push_back(line[i]);
				++i;
			}
			field.end = i;
		}
		out.fields.push_back(field);
		if (i < n && line[i] == delimiter)
		{
			++i;
			if (i == n)
			{
				CsvField empty;
				empty.start = empty.end = i;
				out.fields.push_back(empty);
				break;
			}
			continue;
		}
		break;
	}
	return out;
}

inline std::string EncodeCsvField(const std::string& text, char delimiter, bool forceQuote)
{
	bool quote = forceQuote;
	for (unsigned char c : text)
	{
		if (c == static_cast<unsigned char>(delimiter) || c == '"' || c == '\r' || c == '\n')
		{
			quote = true;
			break;
		}
	}
	if (!quote)
		return text;

	std::string out;
	out.push_back('"');
	for (char c : text)
	{
		if (c == '"')
			out += "\"\"";
		else
			out.push_back(c);
	}
	out.push_back('"');
	return out;
}

inline std::string HeaderFieldText(const CsvField& field, bool firstField)
{
	std::string text = field.text;
	if (firstField && text.size() >= 3
		&& static_cast<unsigned char>(text[0]) == 0xEF
		&& static_cast<unsigned char>(text[1]) == 0xBB
		&& static_cast<unsigned char>(text[2]) == 0xBF)
	{
		text.erase(0, 3);
	}
	return TrimAscii(std::move(text));
}

inline bool LangHeaderMatch(const std::wstring& header, const std::wstring& code)
{
	const std::wstring wanted = TrimWide(code);
	if (wanted.empty() || _wcsicmp(wanted.c_str(), L"auto") == 0)
		return false;
	if (_wcsicmp(header.c_str(), wanted.c_str()) == 0)
		return true;
	if (header.size() > wanted.size()
		&& _wcsnicmp(header.c_str(), wanted.c_str(), wanted.size()) == 0)
	{
		const wchar_t next = header[wanted.size()];
		return next == L'-' || next == L'_';
	}
	return false;
}

inline int FindLangColumn(const CsvLine& header, const std::wstring& code)
{
	for (size_t i = 0; i < header.fields.size(); ++i)
	{
		const std::wstring name = Utf8ToWide(HeaderFieldText(header.fields[i], i == 0));
		if (LangHeaderMatch(name, code))
			return static_cast<int>(i);
	}
	return -1;
}

inline int ResolveCsvColumn(const std::wstring& spec, const CsvLine* header)
{
	const std::wstring value = TrimWide(spec);
	if (value.empty())
		return -1;

	bool digits = true;
	for (wchar_t c : value)
	{
		if (c < L'0' || c > L'9')
		{
			digits = false;
			break;
		}
	}
	if (digits)
	{
		const int number = _wtoi(value.c_str());
		return number >= 1 ? number - 1 : -1;
	}
	if (!header)
		return -1;
	return FindLangColumn(*header, value);
}

inline int FieldAt(const CsvLine& line, size_t offset)
{
	if (line.fields.empty())
		return -1;
	for (size_t i = 0; i < line.fields.size(); ++i)
	{
		const CsvField& field = line.fields[i];
		if (field.start == field.end && offset == field.start)
			return static_cast<int>(i);
		if (offset >= field.start && offset < field.end)
			return static_cast<int>(i);
	}
	for (size_t i = 0; i < line.fields.size(); ++i)
	{
		if (line.fields[i].end == offset)
			return static_cast<int>(i);
	}
	return -1;
}
