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

#include "PluginDefinition.h"

extern FuncItem funcItem[nbFunc];
extern NppData nppData;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD reasonForCall, LPVOID /*lpReserved*/)
{
	try
	{
		switch (reasonForCall)
		{
		case DLL_PROCESS_ATTACH:
			pluginInit(hModule);
			break;
		case DLL_PROCESS_DETACH:
			pluginCleanUp();
			break;
		default:
			break;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData)
{
	nppData = notepadPlusData;
	commandMenuInit();
}

extern "C" __declspec(dllexport) const wchar_t* getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification* notifyCode)
{
	switch (notifyCode->nmhdr.code)
	{
	case NPPN_READY:
	case NPPN_NATIVELANGCHANGED:
		refreshLocalizationAndMenu();
		break;
	case NPPN_SHUTDOWN:
		commandMenuCleanUp();
		break;
	default:
		break;
	}
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return TRUE;
}

extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}
