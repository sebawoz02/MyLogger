#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static size_t g_malloc_mock_counter = 0;
// Malloc mock function. Fails only on the first use.
static inline void* mock_malloc(size_t size)
{
    if(g_malloc_mock_counter++ == 0)
        return NULL;
    return malloc(size);
}
#define malloc(size) mock_malloc(size)

static size_t g_fopen_mock_counter = 0;
// fopen mock function. Fails only oon the first use.
static inline FILE *mock_fopen(const char *__filename, const char *__modes)
{
    if(g_fopen_mock_counter++ == 0)
        return NULL;
    return fopen(__filename, __modes);
}
#define fopen(__filename, __modes) mock_fopen(__filename, __modes)

#include <../src/mylogger.c>    // Including .c allows us to mock functions and test statics.

static void test_mylogger_init_destroy(void);
static void test_mylogger_full_log_to_file(void);
static void test_mylogger_log_to_stdout(void);
static void test_mylogger_log_to_stderr(void);

/**
 * Testing mylogger_init and mylogger_destroy functions.
 * */
static void test_mylogger_init_destroy(void)
{
    // First use of malloc should fail. Error code expected.
    {
        assert(mylogger_init(NULL, 0) == MYLOGGER_INIT_OTHER_ERROR);
    }
    // First use of fopen should fail. Error code expected.
    {
        assert(mylogger_init(NULL, 0) == MYLOGGER_INIT_FILE_CREATION_ERROR);
    }
    // File specified - No errors expected
    {
        FILE* f = fopen("test_log_file.txt", "a+"); // create tmp file

        assert(mylogger_init(f, 0) == MYLOGGER_INIT_SUCCESS);
        mylogger_destroy();

        // fclose(f); - file is closed by MyLogger
        remove("test_log_file.txt");    // delete tmp file
    }

    // No file specified - No errors expected
    {
        assert(mylogger_init(NULL, 0) == MYLOGGER_INIT_SUCCESS);
        mylogger_destroy();
    }

    // Creating logger with any feature should not raise an error
    {
        assert(mylogger_init(NULL, MYLOGGER_FEATURE_ALL) == MYLOGGER_INIT_SUCCESS);
        mylogger_destroy();
    }

    // No file specified and MYLOGGER_FEATURE_NO_FILE - error expected
    {
        assert(mylogger_init(NULL, MYLOGGER_FEATURE_NO_FILE) == MYLOGGER_INIT_OTHER_ERROR);
        // mylogger_destroy(); - Logger not initialized
    }

    // You should not be able to creat second logger.
    {
        assert(mylogger_init(NULL, 0) == MYLOGGER_INIT_SUCCESS);
        assert(mylogger_init(NULL, 0) == MYLOGGER_INIT_ALREADY_RUNNING_ERROR);
        mylogger_destroy();

        // Destroying already destroyed logger should only print error
        mylogger_destroy();
    }

}

/**
 * Testing MyLogger logging into file with TID and Timestamps feature.
 * */
static void test_mylogger_full_log_to_file(void)
{
    assert(mylogger_init(NULL, MYLOGGER_FEATURE_TIMESTAMPS | MYLOGGER_FEATURE_THREAD_ID) == MYLOGGER_INIT_SUCCESS);

    // Testing all levels
    MYLOGGER_DEBUG("Test - debug\n");
    MYLOGGER_INFO("Test - info\n");
    MYLOGGER_WARNING("Test - warning\n");
    MYLOGGER_ERROR("Test - error\n");
    MYLOGGER_CRITICAL("Test - critical\n");
    MYLOGGER_FATAL("Test - fatal\n");

    mylogger_destroy();
}

/**
 * Testing MyLogger logging into stdout with TID and Timestamps feature.
 * */
static void test_mylogger_log_to_stdout(void)
{
    assert(mylogger_init(NULL,
                         MYLOGGER_FEATURE_TIMESTAMPS |
                         MYLOGGER_FEATURE_THREAD_ID |
                         MYLOGGER_FEATURE_NO_FILE |
                         MYLOGGER_FEATURE_STDOUT) == MYLOGGER_INIT_SUCCESS);
    // Testing all levels
    MYLOGGER_DEBUG("Test - debug\n");
    MYLOGGER_INFO("Test - info\n");
    MYLOGGER_WARNING("Test - warning\n");
    MYLOGGER_ERROR("Test - error\n");
    MYLOGGER_CRITICAL("Test - critical\n");
    MYLOGGER_FATAL("Test - fatal\n");

    mylogger_destroy();
}

/**
 * Testing MyLogger logging into stderr with TID and Timestamps feature.
 * */
static void test_mylogger_log_to_stderr(void)
{
    assert(mylogger_init(NULL,
                         MYLOGGER_FEATURE_TIMESTAMPS |
                         MYLOGGER_FEATURE_THREAD_ID |
                         MYLOGGER_FEATURE_NO_FILE |
                         MYLOGGER_FEATURE_STDERR) == MYLOGGER_INIT_SUCCESS);
    // Testing all levels
    MYLOGGER_DEBUG("Test - debug\n");
    MYLOGGER_INFO("Test - info\n");
    MYLOGGER_WARNING("Test - warning\n");
    MYLOGGER_ERROR("Test - error\n");
    MYLOGGER_CRITICAL("Test - critical\n");
    MYLOGGER_FATAL("Test - fatal\n");

    mylogger_destroy();
}

// "make clean" after running tests to remove created files.
int main(void)
{
    test_mylogger_init_destroy();
    test_mylogger_full_log_to_file();
    test_mylogger_log_to_stdout();
    test_mylogger_log_to_stderr();
    printf("Tests finished successfully!\n");
    return 0;
}
