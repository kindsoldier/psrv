/* $Id$ */

#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <wctype.h>

#include <string.h>
#include <ctype.h>

bool ischar(const char ch, const char* chset) {
    for (int i = 0; i < strlen(chset); i++) {
        if (ch == chset[i]) {
            return true;
        }
    }
    return false;
}

bool iswchar(const wchar_t ch, const wchar_t* chset) {
    for (int i = 0; i < wcslen(chset); i++) {
        if (ch == chset[i]) {
            return true;
        }
    }
    return false;
}

int csplitstr(const char *str, char*** words, const int wmax, const char* chset) {
    int wnum = 0;
    bool isword = false;
    int wbegin = 0;
    int wend = 0;

    *words = malloc(sizeof(char*) * wmax);
    for (int i = 0; i < wmax; i++) { (*words)[i] = NULL; }

    for (int i = 0; i < strlen(str); i++) {
        if (!ischar(str[i], chset)) {
            if (isword == false) {
                isword = true;
                wbegin = i;
            }
            continue;
        }
        if (isword == true) {
            isword = false;
            wend = i;
            int wsize = wend - wbegin;

            (*words)[wnum] = malloc(sizeof(char) * 1024 + 1);
            memset((*words)[wnum], '\0', sizeof(char) * wsize + 1);
            strncpy((*words)[wnum], &str[wbegin], wsize);
            wnum++;
        }
        if (wnum > wmax) {
            break;
        }
    }
    return wnum;
}


int splitstr(const char *str, char*** words, const int wmax) {
    int wnum = 0;
    bool isword = false;
    int wbegin = 0;
    int wend = 0;

    *words = malloc(sizeof(char*) * wmax);
    for (int i = 0; i < wmax; i++) { (*words)[i] = NULL; }

    for (int i = 0; i < strlen(str); i++) {
        if (isprint(str[i]) && !isspace(str[i])) {
            if (isword == false) {
                isword = true;
                wbegin = i;
            }
            continue;
        }
        if (isword == true) {
            isword = false;
            wend = i;
            int wsize = wend - wbegin;

            (*words)[wnum] = malloc(sizeof(char) * 1024 + 1);
            memset((*words)[wnum], '\0', sizeof(char) * wsize + 1);
            strncpy((*words)[wnum], &str[wbegin], wsize);
            wnum++;
        }
        if (wnum > wmax) {
            break;
        }
    }
    return wnum;
}


char* sptrim(char* str) {
    int i = 0;
    for (i = 0; i < strlen(str); i++) {
        if (isspace(str[i])) continue;
        break;
    }
    int begin = i;

    int m = 0;
    for (m = strlen(str); m > 0 ; m--) {
        if (isspace(str[m - 1])) continue;
        break;
    }
    int end = m;

    int memsize = sizeof(char) * (end - begin) + 1;
    char* dest = malloc(sizeof(char) * memsize);
    memset(dest, '\0', memsize);

    strncpy(dest, &str[begin], end - begin);
    return dest;
}


wchar_t* wsptrim(const wchar_t* wstr) {
    int i = 0;
    for (i = 0; i < wcslen(wstr); i++) {
        if (iswspace(wstr[i])) continue;
        break;
    }
    int begin = i;

    int m = 0;
    for (m = wcslen(wstr); m > 0 ; m--) {
        if (iswspace(wstr[m - 1])) continue;
        break;
    }
    int end = m;

    int memsize = sizeof(wchar_t) * (end - begin) + 1;
    wchar_t* wdest = malloc(sizeof(wchar_t) * memsize);
    wmemset (wdest, '\0', memsize);

    wcsncpy(wdest, &wstr[begin], end - begin);
    return wdest;
}

wchar_t* wsrtrim(wchar_t* wstr) {
    int i = 0;
    for (i = wcslen(wstr); i > 0 ; i--) {
        if (iswspace(wstr[i - 1])) continue;
        break;
    }

    int memsize = sizeof(wchar_t) * i + 1;
    wchar_t* wdest = malloc(memsize);
    wmemset (wdest, '\0', memsize);

    wcsncpy(wdest, wstr, i);
    return wdest;
}

wchar_t* wsltrim(wchar_t* wstr) {
    int i = 0;
    for (i = 0; i < wcslen(wstr); i++) {
        if (iswspace(wstr[i])) continue;
        break;
    }

    int memsize = sizeof(wchar_t) * i + 1;
    wchar_t* wdest = malloc(memsize);
    wmemset (wdest, '\0', memsize);

    return wdest;
}


wchar_t* wltrim(wchar_t* wstr, wchar_t* chars) {
    int i = 0;
    for (i = 0; i < wcslen(wstr); i++) {
        bool skip = false;
        for (int n = 0; n < wcslen(chars); n++) {
            if (wstr[i] == chars[n]) {
                skip = true;
                break;
            }
        }
        if (skip == true) continue;
        break;
    }

    int memsize = sizeof(wchar_t) * i + 1;
    wchar_t* wdest = malloc(memsize);
    wmemset (wdest, '\0', memsize);

    return wdest;
}
