#include "test_framework.h"
#include "../include/TaskManager.h"
#include <chrono>
#include <filesystem>

void test_task_manager_creation() {
    TaskManager tm("test_tasks.db");
    
    // Should be able to create a TaskManager
    ASSERT_TRUE(tm.initialize());
    ASSERT_TRUE(tm.isConnected());
    
    tm.close();
}

void test_task_manager_user_management() {
    TaskManager tm("test_tasks.db");
    tm.initialize();
    
    // Test user management
    tm.setCurrentUser(1);
    ASSERT_EQ(1, tm.getCurrentUser());
    ASSERT_TRUE(tm.hasCurrentUser());
    
    tm.close();
}

void test_add_and_retrieve_task() {
    TaskManager tm("test_tasks.db");
    tm.initialize();
    tm.setCurrentUser(1);
    
    // Clear any existing tasks
    tm.clearAllTasks();
    
    auto deadline = std::chrono::system_clock::now() + std::chrono::hours(24);
    Task task("Test Task", "Test Description", deadline, Task::Priority::High);
    
    ASSERT_TRUE(tm.addTask(task));
    
    auto tasks = tm.getAllTasks();
    ASSERT_EQ(1, static_cast<int>(tasks.size()));
    ASSERT_STR_EQ("Test Task", tasks[0].getName());
    ASSERT_STR_EQ("Test Description", tasks[0].getDescription());
    ASSERT_ENUM_EQ(Task::Priority::High, tasks[0].getPriority());
    
    tm.close();
}

void test_task_count() {
    TaskManager tm("test_tasks.db");
    tm.initialize();
    tm.setCurrentUser(1);
    
    tm.clearAllTasks();
    ASSERT_EQ(0, tm.getTaskCount());
    
    auto deadline = std::chrono::system_clock::now() + std::chrono::hours(24);
    Task task("Test Task", "Test Description", deadline);
    tm.addTask(task);
    
    ASSERT_EQ(1, tm.getTaskCount());
    
    tm.close();
}

void test_get_tasks_by_priority() {
    TaskManager tm("test_tasks.db");
    tm.initialize();
    tm.setCurrentUser(1);
    
    tm.clearAllTasks();
    
    auto deadline = std::chrono::system_clock::now() + std::chrono::hours(24);
    Task high_task("High Task", "High priority", deadline, Task::Priority::High);
    Task low_task("Low Task", "Low priority", deadline, Task::Priority::Low);
    
    tm.addTask(high_task);
    tm.addTask(low_task);
    
    auto high_tasks = tm.getTasksByPriority(Task::Priority::High);
    auto low_tasks = tm.getTasksByPriority(Task::Priority::Low);
    
    ASSERT_EQ(1, static_cast<int>(high_tasks.size()));
    ASSERT_EQ(1, static_cast<int>(low_tasks.size()));
    ASSERT_STR_EQ("High Task", high_tasks[0].getName());
    ASSERT_STR_EQ("Low Task", low_tasks[0].getName());
    
    tm.close();
}

void test_get_overdue_tasks() {
    TaskManager tm("test_tasks.db");
    tm.initialize();
    tm.setCurrentUser(1);
    
    tm.clearAllTasks();
    
    auto past_deadline = std::chrono::system_clock::now() - std::chrono::hours(1);
    auto future_deadline = std::chrono::system_clock::now() + std::chrono::hours(1);
    
    Task overdue_task("Overdue Task", "Past deadline", past_deadline);
    Task future_task("Future Task", "Future deadline", future_deadline);
    
    tm.addTask(overdue_task);
    tm.addTask(future_task);
    
    auto overdue_tasks = tm.getOverdueTasks();
    ASSERT_EQ(1, static_cast<int>(overdue_tasks.size()));
    ASSERT_STR_EQ("Overdue Task", overdue_tasks[0].getName());
    
    tm.close();
}

void cleanup_task_test_files() {
    // Clean up test database files
    std::filesystem::remove("test_tasks.db");
}

void run_task_manager_tests() {
    std::cout << "\n=== Running TaskManager Tests ===" << std::endl;
    
    RUN_TEST(test_task_manager_creation);
    RUN_TEST(test_task_manager_user_management);
    RUN_TEST(test_add_and_retrieve_task);
    RUN_TEST(test_task_count);
    RUN_TEST(test_get_tasks_by_priority);
    RUN_TEST(test_get_overdue_tasks);
    
    cleanup_task_test_files();
} 