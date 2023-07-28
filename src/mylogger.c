//
// Created by sebastian on 7/26/23.
//
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

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

static MyLogger_features_S __mylogger_parse_features( mylogger_feature_t features);
static void __mylogger_create_file_name(char* fileName);

static MyLogger_instance_S* g_mylogger_instance;
static atomic_flag g_logger_is_initilized;

/**
 * Function that parses MyLogger features from uint8_t to structure.
 * @param[in] features - features for initialized logger as mylogger_feature_t.
 *
 * @return features for initialized logger as MyLogger_features_S.
 * */
static MyLogger_features_S __mylogger_parse_features( mylogger_feature_t features)
{
    return (MyLogger_features_S)
    {
      .feat_stdout =        features & MYLOGGER_FEATURE_STDOUT,
      .feat_stderr =        features & MYLOGGER_FEATURE_STDERR,
      .feat_timestamps =    features & MYLOGGER_FEATURE_TIMESTAMPS,
      .feat_tid =           features &MYLOGGER_FEATURE_THREAD_ID
    };
}

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

    strftime(fileName, 256, "log_%Y%m%d_%H%M%S.txt", timeInfo);
    free(timeInfo);
}


mylogger_init_error_code_t mylogger_init(FILE* log_file, mylogger_feature_t features)
{
    if(atomic_flag_test_and_set(&g_logger_is_initilized) == 0)
    {
        g_mylogger_instance = malloc(sizeof(MyLogger_instance_S));
        if(g_mylogger_instance == NULL)
        {
            atomic_flag_clear(&g_logger_is_initilized);
            perror("MyLogger malloc error!");
            return MYLOGGER_INIT_OTHER_ERROR;
        }

        (*g_mylogger_instance) = (MyLogger_instance_S) {
          .file_fd = log_file,
          .mutex = PTHREAD_MUTEX_INITIALIZER,
          .features = __mylogger_parse_features(features)
        };

        if(g_mylogger_instance->file_fd == NULL)
        {
            char filename[256];
            __mylogger_create_file_name(filename);
            g_mylogger_instance->file_fd = fopen(filename, "a+");

            if(g_mylogger_instance->file_fd == NULL)
            {
                perror("MyLogger file creation error!");
                atomic_flag_clear(&g_logger_is_initilized);
                return MYLOGGER_INIT_FILE_CREATION_ERROR;
            }
        }

        return MYLOGGER_INIT_SUCCESS;
    }
    perror("MyLogger is already initialized!");
    return MYLOGGER_INIT_ALREADY_RUNNING_ERROR;
}
