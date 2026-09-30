#pragma once

#include "npp/PluginInterface.h"

const wchar_t NPP_PLUGIN_NAME[] = L"NppTranslate";

const int nbFunc = 6;

void pluginInit(HANDLE hModule);
void pluginCleanUp();
void commandMenuInit();
void commandMenuCleanUp();
void refreshLocalizationAndMenu();
bool setCommand(size_t index, const wchar_t* cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk = nullptr, bool check0nInit = false);

void translateAndReplace();
void translatePreview();
void swapLanguages();
void openSettings();
void openAbout();

extern HINSTANCE g_hInstance;
