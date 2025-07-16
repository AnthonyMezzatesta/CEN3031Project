#include "test_framework.h"

// Initialize static members
int TestRunner::tests_run = 0;
int TestRunner::tests_passed = 0;
int TestRunner::tests_failed = 0;
std::vector<std::string> TestRunner::failed_tests; 