#pragma warning(disable : 6386)

#include <Windows.h>
#include <strsafe.h>
#include <stdio.h>
#include <locale.h>

int wmain(int argc, wchar_t *argv[]) {
    setlocale(LC_ALL, "");
	if (argc < 2) {
		return 0;
	}
	WCHAR *paths = (WCHAR *)malloc(sizeof(WCHAR) * MAX_PATH * ((size_t)argc - 1));
	if (paths == NULL) {
		return 0;
	}
	WCHAR *path = paths;
	for (int i = 1; i < argc; i++) {
		size_t cch = GetFullPathNameW(argv[i], MAX_PATH, path, NULL);
		if (cch != 0) {
			path += cch + 1;
		}
	}
	*path = L'\0';

	SHFILEOPSTRUCT op = {0};
	op.wFunc = FO_DELETE;
	op.pFrom = paths;
	op.fFlags = FOF_ALLOWUNDO;
	int err = SHFileOperationW(&op);
	if (err != 0){
		printf("Error(%d)\n", err);
	}
	free(paths);
	return err;
}