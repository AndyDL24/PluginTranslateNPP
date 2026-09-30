#pragma once

#include "SelectionService.h"
#include <string>
#include <vector>
#include <windows.h>

INT_PTR ShowSettingsDialog(HINSTANCE hInst, HWND parent);
INT_PTR ShowPreviewDialog(HINSTANCE hInst, HWND parent,
	const std::vector<TextSpan>& spans,
	std::vector<std::string>& translations,
	bool& applyRequested);
void ShowAboutDialog(HINSTANCE hInst, HWND parent);
