# NppTranslate

Plugin per **Notepad++** (x64) che traduce il testo selezionato — anche in **multi-selezione** — con:

- **Google Free** (default, senza API key; endpoint non ufficiale)
- **DeepL** (API Free/Pro con chiave)

## Funzionalità

| Voce menu | Azione |
|-----------|--------|
| Traduci e sostituisci | Traduce e sostituisce in-place (`Ctrl+Shift+T`) |
| Anteprima traduzione… | Mostra originale/tradotto e poi Applica |
| Scambia lingue | Inverte sorgente ↔ destinazione |
| Impostazioni… | Provider, lingue, chiave DeepL |
| About | Info e note su Google Free |

Se non c’è selezione, viene tradotta la **riga corrente**.
Le sostituzioni multi-selezione usano un unico undo.

## Requisiti di build

- Windows 10/11
- Visual Studio 2022 con workload **Desktop development with C++**
- Notepad++ x64 per il test

## Build

1. Apri `NppTranslate.sln` in Visual Studio 2022
2. Seleziona **Release | x64**
3. Build → genera `bin64\NppTranslate.dll`

## Installazione

1. Chiudi Notepad++
2. Crea la cartella plugin, ad esempio:
   - `C:\Program Files\Notepad++\plugins\NppTranslate\`
   - oppure `%LOCALAPPDATA%\Notepad++\plugins\NppTranslate\` (installazioni portabili / senza admin)
3. Copia `NppTranslate.dll` dentro quella cartella
4. Riavvia Notepad++ → menu **Plugins → NppTranslate**

## Configurazione

Il file INI viene creato in:

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

## Note su Google Free

L’endpoint gratuito non è un’API ufficiale Google: può rate-limitare, cambiare o mostrare captcha.
Per uso affidabile preferisci DeepL.

## Licenza

GPL (header Notepad++ / plugin demo). Il codice di NppTranslate è rilasciato sotto GPL compatibile.
