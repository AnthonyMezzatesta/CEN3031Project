#include "test_framework.h"
#include "../include/Task.h"
#include <chrono>

void test_task_creation() {
    auto now = std::chrono::system_clock::now();
    Task task("Test Task", "Test Description", now, Task::Priority::High);
    
    ASSERT_STR_EQ("Test Task", task.getName());
    ASSERT_STR_EQ("Test Description", task.getDescription());
    ASSERT_ENUM_EQ(Task::Priority::High, task.getPriority());
    ASSERT_FALSE(task.getId().has_value());
}

void test_task_with_id() {
    auto now = std::chrono::system_clock::now();
    Task task(1, "Test Task", "Test Description", now, Task::Priority::Medium);
    
    ASSERT_TRUE(task.getId().has_value());
    ASSERT_EQ(1, task.getId().value());
    ASSERT_ENUM_EQ(Task::Priority::Medium, task.getPriority());
}

void test_task_setters() {
    Task task;
    task.setName("New Name");
    task.setDescription("New Description");
    task.setPriority(Task::Priority::Low);
    task.setId(42);
    
    ASSERT_STR_EQ("New Name", task.getName());
    ASSERT_STR_EQ("New Description", task.getDescription());
    ASSERT_ENUM_EQ(Task::Priority::Low, task.getPriority());
    ASSERT_EQ(42, task.getId().value());
}

void test_priority_string_conversion() {
    ASSERT_STR_EQ("None", Task::priorityToString(Task::Priority::None));
    ASSERT_STR_EQ("Low", Task::priorityToString(Task::Priority::Low));
    ASSERT_STR_EQ("Medium", Task::priorityToString(Task::Priority::Medium));
    ASSERT_STR_EQ("High", Task::priorityToString(Task::Priority::High));
    
    ASSERT_ENUM_EQ(Task::Priority::None, Task::stringToPriority("None"));
    ASSERT_ENUM_EQ(Task::Priority::Low, Task::stringToPriority("Low"));
    ASSERT_ENUM_EQ(Task::Priority::Medium, Task::stringToPriority("Medium"));
    ASSERT_ENUM_EQ(Task::Priority::High, Task::stringToPriority("High"));
}

void test_task_overdue() {
    auto past = std::chrono::system_clock::now() - std::chrono::hours(1);
    auto future = std::chrono::system_clock::now() + std::chrono::hours(1);
    
    Task overdue_task("Overdue", "Past deadline", past);
    Task future_task("Future", "Future deadline", future);
    
    ASSERT_TRUE(overdue_task.isOverdue());
    ASSERT_FALSE(future_task.isOverdue());
}

void test_task_completion() {
    Task task("Test", "Description", std::chrono::system_clock::now());
    auto completion_time = std::chrono::system_clock::now();
    
    mark_task_completed(task, completion_time);
    
    // This tests the mark_task_completed function
    // The actual test would depend on the implementation
}

void run_task_tests() {
    std::cout << "\n=== Running Task Tests ===" << std::endl;
    
    RUN_TEST(test_task_creation);
    RUN_TEST(test_task_with_id);
    RUN_TEST(test_task_setters);
    RUN_TEST(test_priority_string_conversion);
    RUN_TEST(test_task_overdue);
    RUN_TEST(test_task_completion);
} 