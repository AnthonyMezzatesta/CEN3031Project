#include "test_framework.h"

// Test function declarations
void run_task_tests();
void run_user_tests();
void run_task_manager_tests();
void run_user_manager_tests();

int main() {
    std::cout << "=== TaskManager Test Suite ===" << std::endl;
    std::cout << "Running basic test cases..." << std::endl;
    
    // Run all test suites
    run_task_tests();
    run_user_tests();
    run_task_manager_tests();
    run_user_manager_tests();
    
    // Print final results
    TestRunner::print_results();
    
    return TestRunner::get_exit_code();
} 