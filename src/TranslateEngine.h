#pragma once

#include "SelectionService.h"
#include "translators/ITranslator.h"
#include <memory>
#include <string>
#include <vector>

struct PreparedTranslation
{
	bool ok = false;
	std::wstring error;
	std::vector<TextSpan> spans;
	std::vector<std::string> translations;
};

class TranslateEngine
{
public:
	explicit TranslateEngine(const NppData& npp);

	PreparedTranslation translateCurrentSelections();
	bool apply(const PreparedTranslation& prepared) const;

private:
	std::unique_ptr<ITranslator> makeTranslator() const;

	NppData _npp;
	SelectionService _selection;
};
