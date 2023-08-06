/*
 * Author: Sebastian Wozniak
 * IT IS THE MAIN HEADER. THE ONLY ONE YOU SHOULD INCLUDE.
 * Logger features described below.
 * */

#ifndef MYLOGGER_MYLOGGER_H
#define MYLOGGER_MYLOGGER_H

#include "mylogger-priv.h"
#include <stdio.h>

typedef enum mylogger_init_error_code_t
{
    MYLOGGER_INIT_SUCCESS               = 0,
    MYLOGGER_INIT_ALREADY_RUNNING_ERROR = 1,
    MYLOGGER_INIT_FILE_CREATION_ERROR   = 2,
    MYLOGGER_INIT_OTHER_ERROR           = 3
} mylogger_init_error_code_t;

/**
 * Features can be combined with bitwise operator | (OR).
 *
 * MYLOGGER_FEATURE_* :
 * - STDOUT     - display log messages on stdout as well
 * - STDERR     - display log messages on stderr as well
 * - TIMESTAMPS - add timestamps to your log messages
 * - THREAD_ID  - add thread id to your log messages
 * - NO_FILE    - no logging file will be created
 * - ALL        - use all the features except NO_FILE
 * */
#define MYLOGGER_FEATURE_STDOUT         MYLOGGER_FEATURE_STDOUT_WRAP
#define MYLOGGER_FEATURE_STDERR         MYLOGGER_FEATURE_STDERR_WRAP
#define MYLOGGER_FEATURE_TIMESTAMPS     MYLOGGER_FEATURE_TIMESTAMPS_WRAP
#define MYLOGGER_FEATURE_THREAD_ID      MYLOGGER_FEATURE_THREAD_ID_WRAP
#define MYLOGGER_FEATURE_NO_FILE        MYLOGGER_FEATURE_NO_FILE_WRAP

#define MYLOGGER_FEATURE_ALL            (MYLOGGER_FEATURE_STDOUT | MYLOGGER_FEATURE_STDERR | \
                                        MYLOGGER_FEATURE_TIMESTAMPS | MYLOGGER_FEATURE_THREAD_ID)

/**
 * Logging levels:
 * DEBUG    -   The lowest and most verbose level of logging. Used for detailed debugging
 *              information, such as variable values, function calls, and other specific details that
 *              can help diagnose issues during development and testing.
 *
 * INFO     -   Used to log informative messages about the program's execution. Generally, this level
 *              provides non-error messages that are helpful for understanding the flow of the application
 *              and tracking its progress.
 *
 * WARNING  -   Used to log potential issues or situations that might lead to problems in the future but do
 *              not cause immediate errors or failures. Warnings help identify potential risks or misconfigurations
 *              that developers should address.
 *
 * ERROR    -   Used to log errors that do not prevent the program from continuing its execution but indicate
 *              that something has gone wrong. These messages alert developers about specific issues that need
 *              to be addressed to ensure proper program functionality.
 *
 * CRITICAL -   The highest level of logging used to report critical errors or conditions that lead to the
 *              program's inability to continue running. These messages indicate severe problems that require
 *              immediate attention and intervention.
 *
 * FATAL    -   Reserved for the most severe and unrecoverable errors. A fatal error indicates that the program
 *              cannot proceed safely and will terminate immediately. This level is used for catastrophic failures.
 *
 * */
#define MYLOGGER_LEVEL_DEBUG        MYLOGGER_LEVEL_DEBUG_WRAP
#define MYLOGGER_LEVEL_INFO         MYLOGGER_LEVEL_INFO_WRAP
#define MYLOGGER_LEVEL_WARNING      MYLOGGER_LEVEL_WARNING_WRAP
#define MYLOGGER_LEVEL_ERROR        MYLOGGER_LEVEL_ERROR_WRAP
#define MYLOGGER_LEVEL_CRITICAL     MYLOGGER_LEVEL_CRITICAL_WRAP
#define MYLOGGER_LEVEL_FATAL        MYLOGGER_LEVEL_FATAL_WRAP

/**
 * Initializes the logger instance with the given features.
 * @param[in] log_file - file to which the logs are to be written. Can be NULL if stdout or stderr logging feature added.
 * @param[in] features - chosen logger features. Can be combined with bitwise OR operator
 *
 * @return  MYLOGGER_INIT_SUCCESS = 0 on success \n
 *          else any other error code >0
 * */
mylogger_init_error_code_t mylogger_init(FILE* log_file, mylogger_feature_t features);

#define MYLOGGER_FATAL(...)     MYLOGGER_FATAL_WRAPPER(__VA_ARGS__)
#define MYLOGGER_CRITICAL(...)  MYLOGGER_CRITICAL_WRAPPER(__VA_ARGS__)
#define MYLOGGER_ERROR(...)     MYLOGGER_ERROR_WRAPPER(__VA_ARGS__)
#define MYLOGGER_WARNING(...)   MYLOGGER_WARNING_WRAPPER(__VA_ARGS__)
#define MYLOGGER_INFO(...)      MYLOGGER_INFO_WRAPPER(__VA_ARGS__)
#define MYLOGGER_DEBUG(...)     MYLOGGER_DEBUG_WRAPPER(__VA_ARGS__)

/**
 * Destroys currently running logger instance.
 * */
void mylogger_destroy(void);

#endif //MYLOGGER_MYLOGGER_H
