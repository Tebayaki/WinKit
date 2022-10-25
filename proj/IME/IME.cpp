#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "imm32")

#define IMC_GETOPENSTATUS 0x0005
#define IMC_SETOPENSTATUS 0x0006

int main(int argc, char *argv[]) {
    HWND hwnd = ImmGetDefaultIMEWnd(GetForegroundWindow());
    if (hwnd) {
		if (argc < 2) {
			DWORD_PTR status;
			if (SendMessageTimeoutW(hwnd, WM_IME_CONTROL, IMC_GETOPENSTATUS, 0, 0, 200, &status)) {
				printf("%lld", status);
			}
		}
		else if (argv[1][0] == '1') {
			SendMessageTimeoutW(hwnd, WM_IME_CONTROL, IMC_SETOPENSTATUS, TRUE, 0, 200, NULL);
		}
		else {
			SendMessageTimeoutW(hwnd, WM_IME_CONTROL, IMC_SETOPENSTATUS, FALSE, 0, 200, NULL);
		}
    }
    return 0;
}