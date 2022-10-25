#pragma warning(disable : 6385 6386)

#include <Windows.h>
#include <stdio.h>
#include <locale.h>
#include <tlhelp32.h>

BOOL FindProcessByNameW(const WCHAR *name, PROCESSENTRY32W *entry);
HANDLE StealProcessToken(DWORD pid);
BOOL IsRunasAdmin();

int wmain(int argc, wchar_t *argv[]) {
    if (argc < 2) return 0;

    wchar_t **paths = (wchar_t **)malloc(sizeof(argv) * ((unsigned long long)argc - 1));
    if (paths == NULL) return 0;

    setlocale(LC_ALL, "");

    int cnt = 0;
    STARTUPINFOW si = {sizeof(STARTUPINFOW), 0};
    PROCESS_INFORMATION pi;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == L'-' && argv[i][1] == L'n') {
            if (argv[i][2] == L'\0') {
                if (++i < argc) {
                    paths[cnt++] = argv[i];
                }
                else {
                    break;
                }
            }
            else {
                paths[cnt++] = argv[i] + 2;
            }
            continue;
        }
        else {
            if (CreateProcessW(NULL, argv[i], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            else {
                printf("Failed(%d): %ls\n", GetLastError(), argv[i]);
            }
        }
    }

    if (cnt > 0) {
        if (IsRunasAdmin()) {
            PROCESSENTRY32W entry = {sizeof(PROCESSENTRY32W)};
            if (FindProcessByNameW(L"explorer.exe", &entry)) {
                HANDLE hToken = StealProcessToken(entry.th32ProcessID);
                if (hToken) {
                    for (int i = 0; i < cnt; i++) {
                        if (CreateProcessWithTokenW(hToken, 0, NULL, paths[i], 0, NULL, NULL, &si, &pi)) {
                            CloseHandle(pi.hProcess);
                            CloseHandle(pi.hThread);
                        }
                        else {
                            printf("Failed(%d): %ls\n", GetLastError(), paths[i]);
                            system("pause");
                        }
                    }
                    CloseHandle(hToken);
                }
                else {
                    printf("Failed to get token from explorer(%d)\n", GetLastError());
                    system("pause");
                }
            }
            else {
                printf("Failed to find explorer.exe\n");
            }
        }
        else {
            for (int i = 0; i < cnt; i++) {
                if (CreateProcessW(NULL, paths[i], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }
                else {
                    printf("Failed(%d): %ls\n", GetLastError(), paths[i]);
                }
            }
        }
    }

    free(paths);
    return 0;
}

BOOL FindProcessByNameW(const WCHAR *name, PROCESSENTRY32W *entry) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        if (Process32FirstW(hSnapshot, entry)) {
            do {
                if (wcscmp(entry->szExeFile, name) == 0) {
                    CloseHandle(hSnapshot);
                    return TRUE;
                }
            }
            while (Process32NextW(hSnapshot, entry));
        }
        CloseHandle(hSnapshot);
    }
    return FALSE;
}

HANDLE StealProcessToken(DWORD pid) {
    HANDLE hProcess;
    HANDLE hShellToken, hToken;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProcess) {
        return 0;
    }

    if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hShellToken)) {
        DWORD err = GetLastError();
        CloseHandle(hProcess);
        SetLastError(err);
        return 0;
    }

    if (!DuplicateTokenEx(hShellToken, TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID, NULL, SecurityImpersonation, TokenPrimary, &hToken)) {
        DWORD err = GetLastError();
        CloseHandle(hShellToken);
        CloseHandle(hProcess);
        SetLastError(err);
        return 0;
    }

    CloseHandle(hShellToken);
    CloseHandle(hProcess);
    return hToken;
}

BOOL IsRunasAdmin() {
    BOOL bElevated = FALSE;
    HANDLE hToken = NULL;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return FALSE;

    TOKEN_ELEVATION tokenEle;
    DWORD dwRetLen = 0;

    if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen)) {
        if (dwRetLen == sizeof(tokenEle)) {
            bElevated = tokenEle.TokenIsElevated;
        }
    }

    CloseHandle(hToken);
    return bElevated;
}