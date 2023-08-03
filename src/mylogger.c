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
    bool feat_no_file:1;        // MYLOGGER_FEATURE_NO_FILE
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
      .feat_tid =           features & MYLOGGER_FEATURE_THREAD_ID,
      .feat_no_file =       features & MYLOGGER_FEATURE_NO_FILE
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

