# NppTranslate

**Language / Lingua:** [English](#english) · [Italiano](#italiano)

Notepad++ plugin (x64) for translating selected text — including **multiple selections** — with Google Free and DeepL.

Created by **AndyD**.

---

## English

[↑ Top](#npptranslate) · [Italiano](#italiano)

### Features

| Menu item | Action |
|-----------|--------|
| Translate and replace | In-place translation (`Ctrl+Shift+T`) |
| Preview translation… | Phrase list + editable translation, then Apply |
| Swap languages | Swap source ↔ target |
| Settings… | Provider, languages, DeepL API key |
| About | Version and credits |

- If nothing is selected, the **current line** is translated.
- Multi-selection replacements use a **single undo**.
- Plugin UI follows Notepad++ language (**Italian** / **English**).

### Preview editor

In **Preview translation…**:

1. Pick a phrase from the left list
2. Read the original (read-only)
3. Edit the translation if needed
4. Click **Apply** to write everything into the document

### Build requirements

- Windows 10/11
- Visual Studio 2022 / 2026 **Build Tools** (or full IDE) with **Desktop development with C++**
- Notepad++ x64 for testing

### Build

1. Open `NppTranslate.sln`
2. Select **Release | x64**
3. Build → output: `bin64\NppTranslate.dll`

### Installation

1. Close Notepad++
2. Create a plugin folder, for example:
   - `C:\Program Files\Notepad++\plugins\NppTranslate\`
   - or `%LOCALAPPDATA%\Notepad++\plugins\NppTranslate\` (portable / no admin)
3. Copy `NppTranslate.dll` into that folder
4. Restart Notepad++ → **Plugins → NppTranslate**

### Configuration

INI path:

`%APPDATA%\Notepad++\plugins\config\NppTranslate.ini`

Example:

```ini
[Translate]
provider=google_free
source_lang=auto
target_lang=it
deepl_api_key=
deepl_endpoint=free
```

For DeepL: set `provider=deepl`, add your API key, and choose `free` or `pro`.

### Notes about Google Free

The free endpoint is **not** an official Google API. It may rate-limit, change, or show a captcha. Prefer DeepL for reliability.

### License

GPL-compatible (Notepad++ / plugin demo headers). NppTranslate is released under a GPL-compatible license.

---

## Italiano

[↑ Inizio](#npptranslate) · [English](#english)

### Funzionalità

| Voce menu | Azione |
|-----------|--------|
| Traduci e sostituisci | Traduzione in-place (`Ctrl+Shift+T`) |
| Anteprima traduzione… | Lista frasi + traduzione modificabile, poi Applica |
| Scambia lingue | Inverte sorgente ↔ destinazione |
| Impostazioni… | Provider, lingue, chiave DeepL |
| About | Versione e crediti |

- Se non c’è selezione, viene tradotta la **riga corrente**.
- Le sostituzioni multi-selezione usano un **unico undo**.
- L’interfaccia del plugin segue la lingua di Notepad++ (**italiano** / **inglese**).

### Editor in anteprima

In **Anteprima traduzione…**:

1. Scegli una frase dalla lista a sinistra
2. Leggi l’originale (sola lettura)
3. Modifica la traduzione se serve
4. Clicca **Applica** per scrivere tutto nel documento

### Requisiti di build

- Windows 10/11
- Visual Studio 2022 / 2026 **Build Tools** (o IDE completo) con **Sviluppo di applicazioni Desktop con C++**
- Notepad++ x64 per il test

### Build

1. Apri `NppTranslate.sln`
2. Seleziona **Release | x64**
3. Build → output: `bin64\NppTranslate.dll`

### Installazione

1. Chiudi Notepad++
2. Crea la cartella plugin, ad esempio:
   - `C:\Program Files\Notepad++\plugins\NppTranslate\`
   - oppure `%LOCALAPPDATA%\Notepad++\plugins\NppTranslate\` (portabile / senza admin)
3. Copia `NppTranslate.dll` dentro quella cartella
4. Riavvia Notepad++ → **Plugins → NppTranslate**

### Configurazione

Percorso INI:

`%APPDATA%\Notepad++\plugins\config\NppTranslate.ini`

Esempio:

```ini
[Translate]
provider=google_free
source_lang=auto
target_lang=it
deepl_api_key=
deepl_endpoint=free
```

Per DeepL: `provider=deepl`, inserisci la chiave e scegli `free` o `pro`.

### Note su Google Free

L’endpoint gratuito **non** è un’API ufficiale Google: può rate-limitare, cambiare o mostrare captcha. Per uso affidabile preferisci DeepL.

### Licenza

GPL compatibile (header Notepad++ / plugin demo). Il codice di NppTranslate è rilasciato sotto licenza GPL compatibile.
