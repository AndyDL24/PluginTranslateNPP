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

#include "Localization.h"
#include "npp/PluginInterface.h"
#include <cctype>
#include <cstring>

namespace
{
	UiLang g_lang = UiLang::English;

	bool looksLikeItalian(const char* fileName)
	{
		if (!fileName || !*fileName)
			return false;
		char lower[256] = {};
		size_t n = 0;
		for (; fileName[n] && n + 1 < sizeof(lower); ++n)
			lower[n] = static_cast<char>(std::tolower(static_cast<unsigned char>(fileName[n])));
		lower[n] = '\0';
		return std::strstr(lower, "italian") != nullptr
			|| std::strcmp(lower, "it.xml") == 0
			|| std::strncmp(lower, "it-", 3) == 0
			|| std::strncmp(lower, "it_", 3) == 0;
	}

	// Order must match LocId
	const wchar_t* const kEn[] = {
		L"Translate and replace",
		L"Preview translation...",
		L"Swap languages",
		L"Settings...",
		L"About",

		L"NppTranslator - Settings",
		L"NppTranslator - Preview",
		L"NppTranslator - About",

		L"Provider:",
		L"Source language:",
		L"Target language:",
		L"DeepL API key:",
		L"DeepL endpoint:",
		L"Language codes: auto, it, en, de, fr, es, ...",
		L"Phrases:",
		L"Original:",
		L"Translated (editable):",

		L"OK",
		L"Cancel",
		L"Apply",

		L"Google Free (default)",
		L"DeepL",
		L"Free (api-free.deepl.com)",
		L"Pro (api.deepl.com)",

		L"NppTranslator 1.0\r\n"
		L"Created by AndyD\r\n\r\n"
		L"Translates multiple selections in Notepad++.\r\n"
		L"Default provider: Google Free (unofficial endpoint, no API key).\r\n"
		L"DeepL is available with an API key in Settings.\r\n\r\n"
		L"Shortcuts:\r\n"
		L"Ctrl+Shift+T = Translate and replace\r\n"
		L"Ctrl+Alt+T = Translate into column\r\n\r\n"
		L"Donations: click the Ko-fi button below.",

		L"Nothing to translate. Select some text or place the caret on a line.",
		L"Translation failed (%1).",
		L"Could not apply the translation in the editor.",
		L"Languages updated:\r\nSource: %1\r\nTarget: %2",
		L"Source",
		L"Target",

		L"Could not open a WinHTTP session.",
		L"Could not connect to %1",
		L"Could not create the HTTP request.",
		L"Network error while contacting %1",

		L"Google Free rate-limited the request (HTTP %1). Try again later or use DeepL in Settings.",
		L"Google Free returned HTTP %1. The free endpoint may be blocked: try DeepL.",
		L"Google requested a captcha / blocked this IP. Use DeepL in Settings.",
		L"Invalid or empty Google Free response.",

		L"DeepL API key is missing. Set it under Plugins → NppTranslator → Settings.",
		L"DeepL rejected the API key (HTTP 403). Check the key and Free/Pro endpoint.",
		L"DeepL quota exceeded (HTTP 456).",
		L"DeepL returned HTTP %1",
		L"Incomplete or invalid DeepL response.",

		L"Translate into column",
		L"CSV column:",
		L"Name (it) or number (3). Empty uses the target-language header.",
		L"NppTranslator - CSV column",
		L"Select one cell or one line. Multiple selections stay on Translate and replace.",
		L"Select a single cell, or the whole line when the header names the source column.",
		L"The first line is the header. Move to a data row.",
		L"That cell is empty.",
		L"Source and destination column are the same.",
		L"Column not found. Use a header name (it) or a number starting at 1.",
	};

	const wchar_t* const kIt[] = {
		L"Traduci e sostituisci",
		L"Anteprima traduzione...",
		L"Scambia lingue",
		L"Impostazioni...",
		L"About",

		L"NppTranslator - Impostazioni",
		L"NppTranslator - Anteprima",
		L"NppTranslator - About",

		L"Provider:",
		L"Lingua sorgente:",
		L"Lingua destinazione:",
		L"DeepL API key:",
		L"DeepL endpoint:",
		L"Codici lingua: auto, it, en, de, fr, es, ...",
		L"Frasi:",
		L"Originale:",
		L"Tradotto (modificabile):",

		L"OK",
		L"Annulla",
		L"Applica",

		L"Google Free (default)",
		L"DeepL",
		L"Free (api-free.deepl.com)",
		L"Pro (api.deepl.com)",

		L"NppTranslator 1.0\r\n"
		L"Creato da AndyD\r\n\r\n"
		L"Traduce selezioni multiple in Notepad++.\r\n"
		L"Provider default: Google Free (endpoint non ufficiale, senza chiave).\r\n"
		L"DeepL disponibile con API key nelle Impostazioni.\r\n\r\n"
		L"Scorciatoie:\r\n"
		L"Ctrl+Shift+T = Traduci e sostituisci\r\n"
		L"Ctrl+Alt+T = Traduci nella colonna\r\n\r\n"
		L"Donazioni: clicca il pulsante Ko-fi qui sotto.",

		L"Nessun testo da tradurre. Seleziona del testo oppure posiziona il cursore su una riga.",
		L"Traduzione fallita (%1).",
		L"Impossibile applicare la traduzione nell'editor.",
		L"Lingue aggiornate:\r\nSorgente: %1\r\nDestinazione: %2",
		L"Sorgente",
		L"Destinazione",

		L"Impossibile aprire sessione WinHTTP.",
		L"Impossibile connettersi a %1",
		L"Impossibile creare la richiesta HTTP.",
		L"Errore di rete durante la richiesta a %1",

		L"Google Free ha limitato le richieste (HTTP %1). Riprova più tardi o usa DeepL nelle Impostazioni.",
		L"Google Free ha restituito HTTP %1. L'endpoint gratuito può essere bloccato: prova DeepL.",
		L"Google ha richiesto un captcha / ha bloccato l'IP. Usa DeepL nelle Impostazioni.",
		L"Risposta Google Free non valida o vuota.",

		L"Chiave API DeepL mancante. Impostala da Plugins → NppTranslator → Impostazioni.",
		L"DeepL ha rifiutato la chiave API (HTTP 403). Controlla chiave ed endpoint Free/Pro.",
		L"Quota DeepL esaurita (HTTP 456).",
		L"DeepL ha restituito HTTP %1",
		L"Risposta DeepL incompleta o non valida.",

		L"Traduci nella colonna",
		L"Colonna CSV:",
		L"Nome (it) o numero (3). Vuoto = intestazione della lingua di destinazione.",
		L"NppTranslator - Colonna CSV",
		L"Seleziona una sola cella o una sola riga. Con più selezioni usa Traduci e sostituisci.",
		L"Seleziona una sola cella, oppure l'intera riga se l'intestazione indica la colonna sorgente.",
		L"La prima riga è l'intestazione. Vai su una riga di dati.",
		L"Quella cella è vuota.",
		L"La colonna sorgente e quella di destinazione coincidono.",
		L"Colonna non trovata. Usa il nome dell'intestazione (it) o un numero da 1.",
	};

	static_assert(sizeof(kEn) / sizeof(kEn[0]) == static_cast<size_t>(LocId::Count), "EN strings mismatch");
	static_assert(sizeof(kIt) / sizeof(kIt[0]) == static_cast<size_t>(LocId::Count), "IT strings mismatch");

	std::wstring replaceOne(const wchar_t* tmpl, const wchar_t* token, const std::wstring& value)
	{
		std::wstring out = tmpl;
		const size_t pos = out.find(token);
		if (pos != std::wstring::npos)
			out.replace(pos, std::wcslen(token), value);
		return out;
	}
}

void Localization::refresh(HWND nppHandle)
{
	g_lang = UiLang::English;
	if (!nppHandle)
		return;

	const int len = static_cast<int>(::SendMessage(nppHandle, NPPM_GETNATIVELANGFILENAME, 0, 0));
	if (len <= 0)
		return;

	std::string fileName(static_cast<size_t>(len) + 1, '\0');
	::SendMessage(nppHandle, NPPM_GETNATIVELANGFILENAME,
		static_cast<WPARAM>(fileName.size()), reinterpret_cast<LPARAM>(fileName.data()));
	fileName.resize(std::strlen(fileName.c_str()));

	if (looksLikeItalian(fileName.c_str()))
		g_lang = UiLang::Italian;
}

UiLang Localization::language()
{
	return g_lang;
}

bool Localization::isItalian()
{
	return g_lang == UiLang::Italian;
}

const wchar_t* Localization::get(LocId id)
{
	const size_t index = static_cast<size_t>(id);
	if (index >= static_cast<size_t>(LocId::Count))
		return L"";
	return isItalian() ? kIt[index] : kEn[index];
}

std::wstring Localization::format(LocId id, const std::wstring& a)
{
	return replaceOne(get(id), L"%1", a);
}

std::wstring Localization::format(LocId id, const std::wstring& a, const std::wstring& b)
{
	std::wstring out = replaceOne(get(id), L"%1", a);
	const size_t pos = out.find(L"%2");
	if (pos != std::wstring::npos)
		out.replace(pos, 2, b);
	return out;
}
