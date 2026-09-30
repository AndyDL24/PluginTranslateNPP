#pragma once

#include "ITranslator.h"

class GoogleFreeTranslator : public ITranslator
{
public:
	const wchar_t* name() const override { return L"Google Free"; }
	TranslateResult translate(const TranslateRequest& request) override;
};
