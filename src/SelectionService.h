#pragma once

#include "npp/PluginInterface.h"
#include <string>
#include <vector>

struct TextSpan
{
	Sci_Position start = 0;
	Sci_Position end = 0;
	std::string textUtf8;
};

class SelectionService
{
public:
	explicit SelectionService(const NppData& npp) : _npp(npp) {}

	HWND currentScintilla() const;
	std::vector<TextSpan> getSpansToTranslate() const;
	bool replaceSpans(const std::vector<TextSpan>& originalSpans,
		const std::vector<std::string>& replacementsUtf8) const;

private:
	NppData _npp;
};
