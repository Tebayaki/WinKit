#include <Windows.h>

BOOL LockTaskBar(HWND hwnd, DWORD bMovable);

int wmain(int argc, wchar_t *argv[]) {
    HWND hwnd = 0;
    for (int i = 0;; i++) {
		hwnd = FindWindowW (L"Shell_TrayWnd", NULL);
        if (hwnd) {
            break;
        }
        else if (i > 50) {
            return 1;
        }
        else {
            Sleep(100);
        }
    }

    APPBARDATA barData = {sizeof(APPBARDATA)};
    SHAppBarMessage(ABM_GETTASKBARPOS, &barData);
    if (barData.uEdge != ABE_LEFT) {
        return 2;
    }

    DWORD dwMovable = 0;
    DWORD cbMovable = sizeof(dwMovable);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"TaskbarSizeMove", RRF_RT_REG_DWORD, NULL, &dwMovable, &cbMovable);

    if (!dwMovable && !LockTaskBar(hwnd, 1)) {
		return 4;
    }

    barData.rc.left = -2000;
    barData.rc.right = argc > 1 ? _wtoi(argv[1]) : 30;
    SendMessageW(hwnd, WM_SIZING, WMSZ_RIGHT, (LPARAM)&barData.rc);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    if (!dwMovable && !LockTaskBar(hwnd, 0)) {
		return 4;
    }

    return 0;
}

BOOL LockTaskBar(HWND hwnd, DWORD dwMovable) {
	if (ERROR_SUCCESS != RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
		L"TaskbarSizeMove", REG_DWORD, &dwMovable, sizeof(dwMovable))){
		return FALSE;
	}
	SendMessageW(hwnd, WM_COMMAND, 0x1A065, 0);
    return TRUE;
}

