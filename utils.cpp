// Registry utilities to simplify configuration :)

#include "windows.h"
#include "stdio.h"
#include "utils.h"

char *GetString(char *path, char *key, char *def) {
	HKEY hKey;
	char fullpath[1024];
	char dat[1024];
	DWORD len;

	sprintf(fullpath, "Software\\Tursisoft\\FlipTerm\\%s", path);
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, fullpath, 0, KEY_READ, &hKey)) {
		len=sizeof(dat);
		RegQueryValueEx(hKey, key, NULL, NULL, (unsigned char*)&dat, &len);
		RegCloseKey(hKey);
		return strdup(dat);
	}

	return strdup(def);
}

UINT GetInt(char *path, char *key, int def) {
	HKEY hKey;
	char fullpath[1024];
	UINT dat;
	DWORD len;

	sprintf(fullpath, "Software\\Tursisoft\\FlipTerm\\%s", path);
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, fullpath, 0, KEY_READ, &hKey)) {
		len=sizeof(dat);
		RegQueryValueEx(hKey, key, NULL, NULL, (unsigned char*)&dat, &len);
		RegCloseKey(hKey);
		return dat;
	}

	return def;
}

bool WriteString(char *path, char *key, char *data) {
	HKEY hKey;
	char fullpath[1024];

	sprintf(fullpath, "Software\\Tursisoft\\FlipTerm\\%s", path);
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, fullpath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL)) {
		RegSetValueEx(hKey, key, 0, REG_SZ, (unsigned char*)data, strlen(data));
		RegCloseKey(hKey);
		return true;
	}

	return false;
}

bool WriteInt(char *path, char *key, UINT data) {
	HKEY hKey;
	char fullpath[1024];

	sprintf(fullpath, "Software\\Tursisoft\\FlipTerm\\%s", path);
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, fullpath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL)) {
		RegSetValueEx(hKey, key, 0, REG_DWORD, (unsigned char*)&data, sizeof(data));
		RegCloseKey(hKey);
		return true;
	}

	return false;
}
