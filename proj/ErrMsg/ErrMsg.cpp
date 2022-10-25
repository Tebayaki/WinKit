#include <Windows.h>
#include <locale.h>
#include <stdio.h>

bool isint (TCHAR * str) {
    int i = 0;
    if (str[0] == TEXT('-'))
        i = 1;
    for (;; i++) {
        if (str[i] >= '0' && str[i] <= '9')
            continue;
        else if (str[i] == 0)
            return i;
        break;
    }
    return 0;
}

int wmain(int argc, wchar_t *argv[]) {
    setlocale(LC_ALL, "");
    HLOCAL hlocal;

    for (int i = 1; i < argc; i++) {
        if (isint(argv[i])) {
            if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                NULL, _wtoi(argv[i]), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPWSTR)&hlocal, 0, NULL)) {
                printf("%ls: %ls", argv[i], (LPCWSTR)LocalLock(hlocal));
                LocalFree(hlocal);
            }
            else {
                printf("%ls: Error code not found!\n", argv[i]);
            }
        }
        else {
            printf("%ls: Invalid error code\n", argv[i]);
        }
    }
    return 0;
}