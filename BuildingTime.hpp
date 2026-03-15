#pragma once
#include "StdAfx.h"

namespace BuildingTime
{
#define MONTH_TO_NUM(mmm) \
    ((mmm[0] == 'J' && mmm[1] == 'a') ? 1 : \
     (mmm[0] == 'F') ? 2 : \
     (mmm[0] == 'M' && mmm[2] == 'r') ? 3 : \
     (mmm[0] == 'A' && mmm[1] == 'p') ? 4 : \
     (mmm[0] == 'M' && mmm[2] == 'y') ? 5 : \
     (mmm[0] == 'J' && mmm[2] == 'n') ? 6 : \
     (mmm[0] == 'J' && mmm[2] == 'l') ? 7 : \
     (mmm[0] == 'A' && mmm[1] == 'u') ? 8 : \
     (mmm[0] == 'S') ? 9 : \
     (mmm[0] == 'O') ? 10 : \
     (mmm[0] == 'N') ? 11 : 12)

#define TO_DIGIT(c) ((c) - '0')

    constexpr wchar_t WDATE[] = {
        __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10],
        L'/',
        (wchar_t)(L'0' + MONTH_TO_NUM(__DATE__) / 10),
        (wchar_t)(L'0' + MONTH_TO_NUM(__DATE__) % 10),
        L'/',
        (wchar_t)((__DATE__[4] == ' ') ? L'0' : __DATE__[4]),
        (wchar_t)__DATE__[5],
        0
    };

    constexpr wchar_t WTIME[] = {
        __TIME__[0], __TIME__[1], __TIME__[2], __TIME__[3],
        __TIME__[4], __TIME__[5], __TIME__[6], __TIME__[7],
        __TIME__[8], 0
    };
}


