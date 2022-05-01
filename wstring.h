
#ifndef WSTRING_H_HYTUY
#define WSTRING_H_HYTUY

#include <wchar.h>
#include <wctype.h>

bool iswchar(const wchar_t ch, const wchar_t* chset);
wchar_t* wltrim(const wchar_t* wstr, const wchar_t* chars);
wchar_t* wsltrim(const wchar_t* wstr);
wchar_t* wsrtrim(const wchar_t* wstr);
wchar_t* wsptrim(const wchar_t* wstr);

bool ischar(const char ch, const char* chset);
char* sptrim(const char* str);
int splitstr(const char *str, char*** words, const int wmax);
int csplitstr(const char *str, char*** words, const int wmax, char* chset);

#endif
