#include "libblastrampoline_internal.h"

/* Utilities to convert from Windows' wchar_t stuff to UTF-8 */
int wchar_to_utf8(const wchar_t * wstr, char *str, size_t maxlen) {
    /* Fast-path empty strings, as WideCharToMultiByte() returns zero for them. */
    if (wstr[0] == L'\0') {
        str[0] = '\0';
        return 1;
    }
    size_t len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (!len)
        return 0;
    if (len > maxlen)
        return 0;
    if (!WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL))
        return 0;
    return 1;
}

int utf8_to_wchar(const char * str, wchar_t * wstr, size_t maxlen) {
    /* Fast-path empty strings, as WideCharToMultiByte() returns zero for them. */
    if (str[0] == '\0') {
        wstr[0] = L'\0';
        return 1;
    }
    size_t len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (!len)
        return 0;
    if (len > maxlen)
        return 0;
    if (!MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len))
        return 0;
    return 1;
}
