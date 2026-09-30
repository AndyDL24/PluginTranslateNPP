// NppTranslate
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

#pragma once

#include "npp/PluginInterface.h"

const wchar_t NPP_PLUGIN_NAME[] = L"NppTranslate";

const int nbFunc = 7;

void pluginInit(HANDLE hModule);
void pluginCleanUp();
void commandMenuInit();
void commandMenuCleanUp();
void refreshLocalizationAndMenu();
bool setCommand(size_t index, const wchar_t* cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk = nullptr, bool check0nInit = false);

void translateAndReplace();
void translatePreview();
void translateIntoColumn();
void swapLanguages();
void openSettings();
void openAbout();

extern HINSTANCE g_hInstance;
