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

	TranslateRequest req;
	const PluginSettings& s = GetSettings();
	req.sourceLang = s.sourceLangUtf8();
	req.targetLang = s.targetLangUtf8();
	req.texts.reserve(prepared.spans.size());
	for (const auto& span : prepared.spans)
		req.texts.push_back(span.textUtf8);

	auto translator = makeTranslator();
	TranslateResult result = translator->translate(req);
	if (!result.ok)
	{
		prepared.error = result.error.empty()
			? Localization::format(LocId::MsgTranslateFailed, translator->name())
			: result.error;
		return prepared;
	}

	prepared.translations = std::move(result.translations);
	prepared.ok = true;
	return prepared;
}

bool TranslateEngine::apply(const PreparedTranslation& prepared) const
{
	if (!prepared.ok)
		return false;
	return _selection.replaceSpans(prepared.spans, prepared.translations);
}
