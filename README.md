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

## Building and Testing

MyLogger comes with a prepared building system using a Makefile, making it easy to compile, test, and install the library.

### Prerequisites

Before building and testing MyLogger, ensure you have the necessary tools and dependencies installed. You can use the following command to set up your environment:

```sh
make configure

This command checks and installs any required dependencies, ensuring that you have everything necessary to run the tests and build the library.

### Build and Install

To build and install the MyLogger library, you can use the following commands:

'''sh
make
make install PREFIX=/your/installation/

The first command compiles the library, while the second command installs it under the specified installation path (PREFIX). If PREFIX is not provided, the library will be installed under the default path.

### Running Tests

MyLogger has been rigorously tested to ensure reliability and robustness. The library boasts 100% test coverage and has been thoroughly checked for memory leaks.

To run the tests, simply use the following command:

'''sh
make test

This command will execute the test suite, providing you with valuable feedback on the library's functionality and reliability.

### More Information

For additional build options and commands, you can always refer to the built-in help:

'''sh
make help

This will display a list of available commands, their description.