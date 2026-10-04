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

#include "TranslateEngine.h"
#include "Settings.h"
#include "Localization.h"
#include "translators/GoogleFreeTranslator.h"
#include "translators/DeepLTranslator.h"

TranslateEngine::TranslateEngine(const NppData& npp)
	: _npp(npp), _selection(npp)
{
}

std::unique_ptr<ITranslator> TranslateEngine::makeTranslator() const
{
	const PluginSettings& s = GetSettings();
	if (s.provider == TranslateProvider::DeepL)
		return std::make_unique<DeepLTranslator>(s.deeplHost(), s.deeplApiKeyUtf8());
	return std::make_unique<GoogleFreeTranslator>();
}

PreparedTranslation TranslateEngine::translateCurrentSelections()
{
	PreparedTranslation prepared;
	prepared.spans = _selection.getSpansToTranslate();
	if (prepared.spans.empty())
	{
		prepared.error = Localization::get(LocId::MsgNoText);
		return prepared;
	}

	std::vector<std::string> texts;
	texts.reserve(prepared.spans.size());
	for (const auto& span : prepared.spans)
		texts.push_back(span.textUtf8);

	TranslateResult result = translateTexts(texts);
	if (!result.ok)
	{
		prepared.error = result.error;
		return prepared;
	}

	prepared.translations = std::move(result.translations);
	prepared.ok = true;
	return prepared;
}

TranslateResult TranslateEngine::translateTexts(const std::vector<std::string>& texts) const
{
	TranslateResult result;
	if (texts.empty())
	{
		result.error = Localization::get(LocId::MsgNoText);
		return result;
	}

	TranslateRequest req;
	const PluginSettings& s = GetSettings();
	req.sourceLang = s.sourceLangUtf8();
	req.targetLang = s.targetLangUtf8();
	req.texts = texts;

	auto translator = makeTranslator();
	result = translator->translate(req);
	if (!result.ok && result.error.empty())
		result.error = Localization::format(LocId::MsgTranslateFailed, translator->name());
	return result;
}

bool TranslateEngine::apply(const PreparedTranslation& prepared) const
{
	if (!prepared.ok)
		return false;
	return _selection.replaceSpans(prepared.spans, prepared.translations);
}
