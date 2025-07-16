#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <type_traits>

// Simple test framework
class TestRunner {
private:
    static int tests_run;
    static int tests_passed;
    static int tests_failed;
    static std::vector<std::string> failed_tests;

public:
    static void run_test(const std::string& test_name, std::function<void()> test_func) {
        tests_run++;
        std::cout << "Running: " << test_name << " ... ";
        
        try {
            test_func();
            tests_passed++;
            std::cout << "PASSED" << std::endl;
        } catch (const std::exception& e) {
            tests_failed++;
            failed_tests.push_back(test_name + ": " + e.what());
            std::cout << "FAILED: " << e.what() << std::endl;
        }
    }

    static void print_results() {
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Total tests: " << tests_run << std::endl;
        std::cout << "Passed: " << tests_passed << std::endl;
        std::cout << "Failed: " << tests_failed << std::endl;
        
        if (tests_failed > 0) {
            std::cout << "\nFailed tests:" << std::endl;
            for (const auto& failure : failed_tests) {
                std::cout << "  - " << failure << std::endl;
            }
        }
        
        std::cout << "\nSuccess rate: " << (tests_run > 0 ? (tests_passed * 100 / tests_run) : 100) << "%" << std::endl;
    }

    static int get_exit_code() {
        return tests_failed > 0 ? 1 : 0;
    }
};

// Static members are defined in test_framework.cpp

// Helper function to convert values to strings
template<typename T>
std::string to_string_helper(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Specialization for enums (convert to int)
template<typename T>
std::string to_string_helper(const T& value, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr) {
    return std::to_string(static_cast<int>(value));
}

// Test macros
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Expected true but got false"); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("Expected false but got true"); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        std::ostringstream oss; \
        oss << "Expected " << expected << " but got " << actual; \
        throw std::runtime_error(oss.str()); \
    }

#define ASSERT_ENUM_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        std::ostringstream oss; \
        oss << "Expected enum value " << static_cast<int>(expected) << " but got " << static_cast<int>(actual); \
        throw std::runtime_error(oss.str()); \
    }

#define ASSERT_STR_EQ(expected, actual) \
    if (std::string(expected) != std::string(actual)) { \
        throw std::runtime_error("Expected '" + std::string(expected) + "' but got '" + std::string(actual) + "'"); \
    }

#define RUN_TEST(test_name) \
    TestRunner::run_test(#test_name, test_name) 
