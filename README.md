# NppTranslator

**Language / Lingua:** [English](#english) · [Italiano](#italiano)

Notepad++ plugin (x64) for translating selected text — including **multiple selections** — with Google Free and DeepL.

Created by **AndyD**.

---

## English

[↑ Top](#npptranslate) · [Italiano](#italiano)

### Features

| Menu item | Action |
|-----------|--------|
| Translate and replace | In-place translation (`Ctrl+Shift+T`). One selection or many. |
| Preview translation… | Phrase list + editable translation, then Apply |
| Translate into column | Writes the translation into a CSV column (`Ctrl+Alt+T`). One cell or one line only. |
| Swap languages | Swap source ↔ target |
| Settings… | Provider, languages, DeepL API key, CSV column |
| About | Version, credits, and the Ko-fi button |

- If nothing is selected, the **current line** is translated.
- Multi-selection replacements use a **single undo**.
- Plugin UI follows Notepad++ language (**Italian** / **English**).

### Translate into a CSV column

Use this when the English text must stay in place and the translation goes into another column. It runs on **one cell or one line** at a time. Multiple selections keep using **Translate and replace**.

Example:

```csv
id,en,it
1,Hello,
2,Good morning,
```

1. Set the target language to `it` in Settings (the default).
2. Select the English cell, or the whole line.
3. Press `Ctrl+Alt+T`.

The plugin finds the column whose header matches the target language (`it`) and writes the translation there. The English cell is left unchanged. The same column on the next row is then selected, so you can repeat the shortcut row by row.

If the file has no header with that language name, a dialog asks for the column once: a header name (`it`) or a number starting at 1 (`3` is the third column). The choice is saved as `csv_column`. Leave that setting empty to use the header again.

A line with more semicolons than commas is read as a semicolon-separated file.

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

1. Open `NppTranslate.sln` (the project inside is named NppTranslator)
2. Select **Release | x64**
3. Build → output: `x64\Release\NppTranslator.dll`

### Installation

1. Close Notepad++
2. Create a plugin folder, for example:
   - `C:\Program Files\Notepad++\plugins\NppTranslator\`
   - or `%LOCALAPPDATA%\Notepad++\plugins\NppTranslator\` (portable / no admin)
3. Copy `NppTranslator.dll` into that folder
4. Restart Notepad++ → **Plugins → NppTranslator**

### Configuration

INI path:

`%APPDATA%\Notepad++\plugins\config\NppTranslator.ini`

Example:

```ini
[Translate]
provider=google_free
source_lang=auto
target_lang=it
deepl_api_key=
deepl_endpoint=free
csv_column=
```

For DeepL: set `provider=deepl`, add your API key, and choose `free` or `pro`.

`csv_column` is optional. Use a header name or a 1-based column number. Empty means “use the column named like the target language”.

### Notes about Google Free

The free endpoint is **not** an official Google API. It may rate-limit, change, or show a captcha. Prefer DeepL for reliability.

### License

NppTranslator is released under the [GNU General Public License v3.0](license.txt), or any later version. The Notepad++ headers in `src/npp` stay under their own copyright and the same GPL-3 terms.

### Donation

If you would like to support the project:

[![Support me on Ko-fi](support_me_on_kofi_dark.png)](https://ko-fi.com/andyd24)
---

## Italiano

[↑ Inizio](#npptranslate) · [English](#english)

### Funzionalità

| Voce menu | Azione |
|-----------|--------|
| Traduci e sostituisci | Traduzione in-place (`Ctrl+Shift+T`). Una selezione o più selezioni. |
| Anteprima traduzione… | Lista frasi + traduzione modificabile, poi Applica |
| Traduci nella colonna | Scrive la traduzione in una colonna CSV (`Ctrl+Alt+T`). Solo una cella o una riga. |
| Scambia lingue | Inverte sorgente ↔ destinazione |
| Impostazioni… | Provider, lingue, chiave DeepL, colonna CSV |
| About | Versione, crediti e pulsante Ko-fi |

- Se non c’è selezione, viene tradotta la **riga corrente**.
- Le sostituzioni multi-selezione usano un **unico undo**.
- L’interfaccia del plugin segue la lingua di Notepad++ (**italiano** / **inglese**).

### Tradurre in una colonna CSV

Serve quando il testo inglese deve restare dov’è e la traduzione va in un’altra colonna. Funziona su **una cella o una riga** per volta. Con più selezioni si continua a usare **Traduci e sostituisci**.

Esempio:

```csv
id,en,it
1,Hello,
2,Good morning,
```

1. Nelle Impostazioni lascia la lingua di destinazione su `it`.
2. Seleziona la cella inglese, oppure l’intera riga.
3. Premi `Ctrl+Alt+T`.

Il plugin cerca la colonna la cui intestazione coincide con la lingua di destinazione (`it`) e ci scrive la traduzione. La cella inglese non viene modificata. Poi seleziona la stessa colonna nella riga sotto, così puoi ripetere lo shortcut riga per riga.

Se il file non ha un’intestazione con quel nome, una finestra chiede la colonna una sola volta: il nome (`it`) oppure un numero da 1 (`3` è la terza colonna). La scelta resta in `csv_column`. Lascia il campo vuoto per tornare a usare l’intestazione.

Una riga con più punti e virgola che virgole viene letta come file separato da `;`.

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

1. Apri `NppTranslate.sln` (il progetto dentro si chiama NppTranslator)
2. Seleziona **Release | x64**
3. Build → output: `x64\Release\NppTranslator.dll`

### Installazione

1. Chiudi Notepad++
2. Crea la cartella plugin, ad esempio:
   - `C:\Program Files\Notepad++\plugins\NppTranslator\`
   - oppure `%LOCALAPPDATA%\Notepad++\plugins\NppTranslator\` (portabile / senza admin)
3. Copia `NppTranslator.dll` dentro quella cartella
4. Riavvia Notepad++ → **Plugins → NppTranslator**

### Configurazione

Percorso INI:

`%APPDATA%\Notepad++\plugins\config\NppTranslator.ini`

Esempio:

```ini
[Translate]
provider=google_free
source_lang=auto
target_lang=it
deepl_api_key=
deepl_endpoint=free
csv_column=
```

Per DeepL: `provider=deepl`, inserisci la chiave e scegli `free` o `pro`.

`csv_column` è facoltativo. Puoi mettere il nome dell’intestazione o il numero di colonna partendo da 1. Vuoto significa «usa la colonna che ha il nome della lingua di destinazione».

### Note su Google Free

L’endpoint gratuito **non** è un’API ufficiale Google: può rate-limitare, cambiare o mostrare captcha. Per uso affidabile preferisci DeepL.

### Licenza

NppTranslator è rilasciato sotto la [GNU General Public License v3.0](license.txt), o qualsiasi versione successiva. Gli header di Notepad++ in `src/npp` restano sotto il loro copyright e gli stessi termini GPL-3.

### Donazione

Se desideri supportare il progetto:

[![Support me on Ko-fi](support_me_on_kofi_dark.png)](https://ko-fi.com/andyd24)
