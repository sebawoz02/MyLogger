#include <assert.h>
#include <stdio.h>

#include <mylogger/mylogger.h>

static void test_mylogger_init_destroy(void);

/**
 * Testing mylogger_init and mylogger_destroy functions.
 * */
static void test_mylogger_init_destroy(void)
{
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
    }

}

// "make clean" after running tests to remove created files.
int main(void)
{
    test_mylogger_init_destroy();
    return 0;
}
