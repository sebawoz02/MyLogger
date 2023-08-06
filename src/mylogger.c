//
// Created by sebastian on 7/26/23.
//
#include <stdbool.h>
#include <stdatomic.h>      /* atomic flag */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>       /* Functions used to get timestamp */
#include <sys/syscall.h>    /* Definition of SYS_* constants */
#include <unistd.h>         /* syscall() */
#include <execinfo.h>       /* Stack trace */
#include <stdarg.h>         /* va_start() */

#include <mylogger/mylogger.h>

/**
 * Structure that tells which options where selected during initialization.
 * */
typedef struct MyLogger_features
{
    bool feat_stdout:1;         // MYLOGGER_FEATURE_STDOUT
    bool feat_stderr:1;         // MYLOGGER_FEATURE_STDERR
    bool feat_timestamps:1;     // MYLOGGER_FEATURE_TIMESTAMPS
    bool feat_tid:1;            // MYLOGGER_FEATURE_THREAD_ID
    bool feat_no_file:1;        // MYLOGGER_FEATURE_NO_FILE
    bool feat_ansi_logs:1;      // MYLOGGER_FEATURE_STDOUT/STDERR | MYLOGGER_FEATURE_NO_FILE
} MyLogger_features_S;


/**
 * Structure that contains the most important information about logger.
 */
typedef struct MyLogger_instance
{
    FILE* file_fd;
    pthread_mutex_t mutex;
    MyLogger_features_S features;
}MyLogger_instance_S;

static MyLogger_features_S __mylogger_parse_features(const mylogger_feature_t features);
static void __mylogger_create_file_name(char* fileName);
static size_t __mylogger_add_timestamp(char* log_buffer, const size_t buf_size);
static size_t __mylogger_add_trace(char* log_buffer, const size_t buf_size);
static size_t __mylogger_add_tid(char* log_buffer, const size_t buf_size);

static MyLogger_instance_S* g_mylogger_instance;
static atomic_flag g_logger_is_initilized;
static const char* mylogger_level_print[] = {"DEBUG",
                                             "INFO",
                                             "WARNING",
                                             "ERROR",
                                             "CRITICAL",
                                             "FATAL"
                                            };
static const char* mylogger_level_print_ansi[] = {"DEBUG\033[0m",
                                                  "\033[0;32mINFO\033[0m",
                                                  "\033[0;33mWARNING\033[0m",
                                                  "\033[0;31mERROR\033[0m",
                                                  "\033[0;31mCRITICAL\033[0m",
                                                  "\033[0;31mFATAL\033[0m"
                                                };

/**
 * Function that parses MyLogger features from uint8_t to structure.
 * @param[in] features - features for initialized logger as mylogger_feature_t.
 *
 * @return features for initialized logger as MyLogger_features_S.
 * */
static MyLogger_features_S __mylogger_parse_features(const mylogger_feature_t features)
{
    return (MyLogger_features_S)
    {
      .feat_stdout =        features & MYLOGGER_FEATURE_STDOUT,
      .feat_stderr =        features & MYLOGGER_FEATURE_STDERR,
      .feat_timestamps =    features & MYLOGGER_FEATURE_TIMESTAMPS,
      .feat_tid =           features & MYLOGGER_FEATURE_THREAD_ID,
      .feat_no_file =       features & MYLOGGER_FEATURE_NO_FILE,
      .feat_ansi_logs =     features & MYLOGGER_FEATURE_NO_FILE
    };
}

#define MYLOGGER_FILE_NAME_MAX_SIZE 256
/**
 * Creates name for the new log file with a timestamp.
 * @param[out] fileName - pointer where to save new log file name
 */
static void __mylogger_create_file_name(char* fileName)
{
    time_t rawTime;
    struct tm* timeInfo = malloc(sizeof(*timeInfo));

    time(&rawTime);
    localtime_r(&rawTime, timeInfo);

    strftime(fileName, MYLOGGER_FILE_NAME_MAX_SIZE, "log_%Y%m%d_%H%M%S.txt", timeInfo);
    free(timeInfo);
}

/**
 * Creates timestamp and adds it to the message buffer with snprinf function.
 *
 * @param[in] log_buffer message buffer
 * @param[in] buf_size current buffer size
 * @return The number of characters that would have been written on the buffer.
 * */
static size_t __mylogger_add_timestamp(char* log_buffer, const size_t buf_size)
{
    size_t new_buf_size = 0;

    struct timeval timeval_str;
    gettimeofday(&timeval_str, NULL);

    const struct tm tm_time = *localtime(&(time_t){timeval_str.tv_sec});
    new_buf_size = strftime(&log_buffer[new_buf_size], buf_size - new_buf_size, "[%H:%M:%S", &tm_time);
    new_buf_size += (size_t)snprintf(&log_buffer[new_buf_size], buf_size - new_buf_size, ".%06ld] ", (long)timeval_str.tv_usec);

    return  new_buf_size;
}

#define MYLOGGER_CALLSTACK_MAX_SIZE 256
/**
 * Gets stack trace and adds it to the message buffer with snprinf function.
 *
 * @param[in] log_buffer message buffer
 * @param[in] buf_size current buffer size
 * @return The number of characters that would have been written on the buffer.
 * */
static size_t __mylogger_add_trace(char* log_buffer, const size_t buf_size)
{
    size_t new_buf_size = 0;
    void* stack[MYLOGGER_CALLSTACK_MAX_SIZE];
    int frames = backtrace(stack, MYLOGGER_CALLSTACK_MAX_SIZE);
    if(frames < 0)
        return 0;

    char** symbols = backtrace_symbols(stack, frames);
    if(symbols == NULL)
        return 0;
    new_buf_size = (size_t) snprintf(&log_buffer[new_buf_size],
                                     buf_size - new_buf_size,
                                     "TRACE:\n");
    for(int i = 0; i < frames; i++)
    {
        new_buf_size += (size_t) snprintf(&log_buffer[new_buf_size],
                                          buf_size - new_buf_size,
                                          "%s\n", symbols[i]);
    }

    free(symbols);
    return new_buf_size;
}

/**
 * Gets Thread ID and adds it to the message buffer with snprinf function.
 *
 * @param[in] log_buffer message buffer
 * @param[in] buf_size current buffer size
 * @return The number of characters that would have been written on the buffer.
 * */
static size_t __mylogger_add_tid(char* log_buffer, const size_t buf_size)
{
    return (size_t) snprintf(log_buffer, buf_size, "[TID: %ld]", (long) syscall(__NR_gettid));
}

mylogger_init_error_code_t mylogger_init(FILE* log_file, mylogger_feature_t features)
{
    // check if logger is not already initialized
    if(atomic_flag_test_and_set(&g_logger_is_initilized) == 0)
    {
        g_mylogger_instance = malloc(sizeof(MyLogger_instance_S));
        if(g_mylogger_instance == NULL)
        {
            atomic_flag_clear(&g_logger_is_initilized);
            fprintf(stderr,"MyLogger malloc error!\n");
            return MYLOGGER_INIT_OTHER_ERROR;
        }

        (*g_mylogger_instance) = (MyLogger_instance_S) {
          .file_fd = log_file,
          .mutex = PTHREAD_MUTEX_INITIALIZER,
          .features = __mylogger_parse_features(features)
        };

        // if no logging file specified create new file
        if(g_mylogger_instance->file_fd == NULL && !g_mylogger_instance->features.feat_no_file)
        {
            char filename[MYLOGGER_FILE_NAME_MAX_SIZE];
            __mylogger_create_file_name(filename);
            g_mylogger_instance->file_fd = fopen(filename, "a+");

            if(g_mylogger_instance->file_fd == NULL)
            {
                fprintf(stderr,"MyLogger file creation error!\n");

                pthread_mutex_destroy(&g_mylogger_instance->mutex);
                atomic_flag_clear(&g_logger_is_initilized);
                free(g_mylogger_instance);

                return MYLOGGER_INIT_FILE_CREATION_ERROR;
            }
        }
        else if(g_mylogger_instance->file_fd == NULL && \
                !g_mylogger_instance->features.feat_stdout && \
                !g_mylogger_instance->features.feat_stderr )
        {
            fprintf(stderr,"MyLogger no file descriptors specified!\n");

            pthread_mutex_destroy(&g_mylogger_instance->mutex);
            atomic_flag_clear(&g_logger_is_initilized);
            free(g_mylogger_instance);

            return MYLOGGER_INIT_OTHER_ERROR;
        }

        return MYLOGGER_INIT_SUCCESS;
    }
    fprintf(stderr,"MyLogger is already initialized!\n");
    return MYLOGGER_INIT_ALREADY_RUNNING_ERROR;
}

void mylogger_destroy(void)
{
    if(atomic_flag_test_and_set(&g_logger_is_initilized) == 1)
    {
        // destroy the mutex
        pthread_mutex_destroy(&g_mylogger_instance->mutex);
        // close file
        if(!g_mylogger_instance->features.feat_no_file)
            fclose(g_mylogger_instance->file_fd);
        free(g_mylogger_instance);
        atomic_flag_clear(&g_logger_is_initilized);
    }
    else
    {
        atomic_flag_clear(&g_logger_is_initilized);
        fprintf(stderr,"Cannot destroy MyLogger because its not initialized!\n");
    }
}

void __attribute__(( format(printf, 5, 6) )) __mylogger_print(const char* file,
                                                              const char* func,
                                                              size_t line,
                                                              mylogger_level_t level,
                                                              const char* format,
                                                              ...)
{
    if(atomic_flag_test_and_set(&g_logger_is_initilized) == 0)
    {
        fprintf(stderr, "You need to initialize MyLogger before using it!\n");
        atomic_flag_clear(&g_logger_is_initilized);
        return;
    }
    pthread_mutex_lock(&g_mylogger_instance->mutex);

    static char buffer[1 << 20] = {0};
    size_t buffer_idx = 0;

    // ADD LOG LEVEL
    const char* level_print = g_mylogger_instance->features.feat_ansi_logs ? mylogger_level_print_ansi[level] : mylogger_level_print[level];
    buffer_idx = (size_t)snprintf(&buffer[0],
                                  sizeof(buffer) - buffer_idx,
                                  "[%s] ", level_print);
    // ADD TIMESTAMP
    if(g_mylogger_instance->features.feat_timestamps == 1)
        buffer_idx += __mylogger_add_timestamp(&buffer[buffer_idx], sizeof(buffer) - buffer_idx);

    // ADD TID
    if(g_mylogger_instance->features.feat_tid == 1)
        buffer_idx += __mylogger_add_tid(&buffer[buffer_idx], sizeof(buffer) - buffer_idx);

    buffer_idx += (size_t) snprintf(&buffer[buffer_idx],
                                    sizeof(buffer) - buffer_idx,
                                    "%s:%zu %s: ", file, line, func);
    // ADD LOG MESSAGE
    va_list args;
    va_start(args, format);
    buffer_idx += (size_t)vsnprintf(&buffer[buffer_idx],
                                      sizeof(buffer) - buffer_idx,
                                      format, args);

    // ADD STACKTRACE
    if(level == MYLOGGER_LEVEL_FATAL)
        buffer_idx += __mylogger_add_trace(&buffer[buffer_idx], sizeof(buffer) - buffer_idx);

    // WRITE LOG MESSAGE
    if(g_mylogger_instance->file_fd != NULL)
        fprintf(g_mylogger_instance->file_fd, "%s", buffer);
    if(g_mylogger_instance->features.feat_stdout)
        fprintf(stdout, "%s", buffer);
    if(g_mylogger_instance->features.feat_stderr)
        fprintf(stderr, "%s", buffer);

    pthread_mutex_unlock(&g_mylogger_instance->mutex);
}
