# MyLogger - Sebastian Woźniak
[![MAIN-CI](https://github.com/sebawoz02/MyLogger/actions/workflows/main_ci.yaml/badge.svg?branch=main)](https://github.com/sebawoz02/MyLogger/actions/workflows/main_ci.yaml)
[![MEM-LEAKS](https://github.com/sebawoz02/MyLogger/actions/workflows/mem_leaks.yaml/badge.svg?branch=main)](https://github.com/sebawoz02/MyLogger/actions/workflows/mem_leaks.yaml)
[![TEST-COVERAGE](https://github.com/sebawoz02/MyLogger/actions/workflows/test_coverage.yaml/badge.svg?branch=main)](https://github.com/sebawoz02/MyLogger/actions/workflows/test_coverage.yaml)

MyLogger is a thread-safe logging library written in Gnu17 C language standard, designed to provide easy and efficient logging capabilities for your C applications. It offers various features to enhance your logging experience.

## Features

- **Thread Safety**: Logger is designed to handle multiple threads concurrently, ensuring safe logging in multithreaded applications.

- **Log Levels**: Different log levels are explained in the header file, allowing you to control the verbosity of your logs. Choose from DEBUG, INFO, WARNING, ERROR, and more.

- **Automatic Logfile Creation**: Logger can automatically create log files, which can also be turned off if not needed. This feature helps organize and store log information for future reference.

- **Flexible Output Streams**: You have the option to direct log output to both `stdout` and `stderr`, allowing you to choose the appropriate stream for different scenarios.

- **Timestamps**: Logger includes timestamps in the log messages, making it easier to track when each log entry occurred.

- **Thread ID (TID)**: Each log entry can include the Thread ID (TID), helping you identify which thread generated a particular log message.

