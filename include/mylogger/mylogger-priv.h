/*
 * Author: Sebastian Wozniak
 * THIS HEADER SHOULD NOT BE INCLUDED DIRECTLY.
 * */

#ifndef MYLOGGER_MYLOGGER_PRIV_H
#define MYLOGGER_MYLOGGER_PRIV_H

#ifndef MYLOGGER_MYLOGGER_H
#error "You should not include private header directly. Try #include <mylogger/mylogger.h>."
#endif

#ifndef __GNUC__
#error "MyLogger lib requires GNU extension!"
#endif

#if __STDC_VERSION__ < 201710L
#error "MyLogger lib requires C17+!"
#endif

#include <stdint.h>
#include <stddef.h>

typedef enum mylogger_level_t
{
    MYLOGGER_LEVEL_DEBUG_WRAP    = 0,
    MYLOGGER_LEVEL_INFO_WRAP     = 1,
    MYLOGGER_LEVEL_WARNING_WRAP  = 2,
    MYLOGGER_LEVEL_ERROR_WRAP    = 3,
    MYLOGGER_LEVEL_CRITICAL_WRAP = 4,
    MYLOGGER_LEVEL_FATAL_WRAP    = 5
} mylogger_level_t;


typedef uint8_t mylogger_feature_t;
#define MYLOGGER_FEATURE_STDOUT_WRAP        (1 << 0)
#define MYLOGGER_FEATURE_STDERR_WRAP        (1 << 1)
#define MYLOGGER_FEATURE_TIMESTAMPS_WRAP    (1 << 2)
#define MYLOGGER_FEATURE_THREAD_ID_WRAP     (1 << 3)
#define MYLOGGER_FEATURE_NO_FILE_WRAP       (1 << 4)


void __attribute__(( format(printf, 5, 6) )) __mylogger_print(const char* file,
                      const char* func,
                      size_t line,
                      mylogger_level_t level,
                      const char* format,
                      ...);

#define MYLOGGER_GENERAL_WRAPPER(LVL, ...) __mylogger_print(__FILE__, __func__, __LINE__, LVL, __VA_ARGS__)
#define MYLOGGER_DEBUG_WRAPPER(...)         MYLOGGER_GENERAL_WRAPPER(MYLOGGER_LEVEL_DEBUG_WRAP, __VA_ARGS__)
#define MYLOGGER_INFO_WRAPPER(...)          MYLOGGER_GENERAL_WRAPPER(MYLOGGER_LEVEL_INFO_WRAP, __VA_ARGS__)
#define MYLOGGER_WARNING_WRAPPER(...)       MYLOGGER_GENERAL_WRAPPER(MYLOGGER_LEVEL_WARNING_WRAP, __VA_ARGS__)
#define MYLOGGER_ERROR_WRAPPER(...)         MYLOGGER_GENERAL_WRAPPER(MYLOGGER_LEVEL_ERROR_WRAP, __VA_ARGS__)
#define MYLOGGER_CRITICAL_WRAPPER(...)      MYLOGGER_GENERAL_WRAPPER(MYLOGGER_LEVEL_CRITICAL_WRAP, __VA_ARGS__)
#define MYLOGGER_FATAL_WRAPPER(...)         MYLOGGER_GENERAL_WRAPPER(MYLOGGER_LEVEL_FATAL_WRAP, __VA_ARGS__)

#endif //MYLOGGER_MYLOGGER_PRIV_H
