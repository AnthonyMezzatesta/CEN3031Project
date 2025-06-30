#include "TaskManager.h"
#include "Task.h"
#include <iostream>
#include <chrono>
#include <vector>

void printSeparator() {
    std::cout << "========================================" << std::endl;
}

void printTasks(const std::vector<Task>& tasks, const std::string& title) {
    std::cout << "\n" << title << ":" << std::endl;
    printSeparator();
    
    if (tasks.empty()) {
        std::cout << "No tasks found." << std::endl;
    } else {
        for (const auto& task : tasks) {
            std::cout << task.toString() << std::endl;
        }
    }
    printSeparator();
}

int main() {
    std::cout << "=== Task Manager Demo ===" << std::endl;
    
    // Initialize TaskManager
    TaskManager taskManager("demo_tasks.db");
    
    if (!taskManager.initialize()) {
        std::cerr << "Failed to initialize TaskManager!" << std::endl;
        return 1;
    }
    
    std::cout << "TaskManager initialized successfully!" << std::endl;
    
    // Clear existing tasks for demo
    taskManager.clearAllTasks();
    
    // Create some sample tasks
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto nextWeek = now + std::chrono::hours(24 * 7);
    auto lastWeek = now - std::chrono::hours(24 * 7);
    
    // Task 1: Due tomorrow
    Task task1("Complete project proposal", 
               "Write and submit the final project proposal for the new initiative", 
               tomorrow);
    
    // Task 2: Due next week
    Task task2("Team meeting preparation", 
               "Prepare presentation slides and agenda for next team meeting", 
               nextWeek);
    
    // Task 3: Overdue task
    Task task3("Submit expense report", 
               "Submit the quarterly expense report to accounting", 
               lastWeek);
    
    // Task 4: Due in a few days
    auto fewDays = now + std::chrono::hours(24 * 3);
    Task task4("Code review", 
               "Review and approve pending code changes in the repository", 
               fewDays);
    
    // Add tasks
    std::cout << "\nAdding tasks..." << std::endl;
    
    if (taskManager.addTask(task1)) {
        std::cout << "✓ Added: " << task1.getName() << std::endl;
    }
    
    if (taskManager.addTask(task2)) {
        std::cout << "✓ Added: " << task2.getName() << std::endl;
    }
    
    if (taskManager.addTask(task3)) {
        std::cout << "✓ Added: " << task3.getName() << std::endl;
    }
    
    if (taskManager.addTask(task4)) {
        std::cout << "✓ Added: " << task4.getName() << std::endl;
    }
    
    std::cout << "\nTotal tasks in database: " << taskManager.getTaskCount() << std::endl;
    
    // Display all tasks
    auto allTasks = taskManager.getAllTasks();
    printTasks(allTasks, "All Tasks");
    
    // Display overdue tasks
    auto overdueTasks = taskManager.getOverdueTasks();
    printTasks(overdueTasks, "Overdue Tasks");
    
    // Display upcoming tasks (next 7 days)
    auto upcomingTasks = taskManager.getUpcomingTasks(7);
    printTasks(upcomingTasks, "Upcoming Tasks (Next 7 Days)");
    
    // Search for tasks by name
    auto projectTasks = taskManager.getTasksByName("project");
    printTasks(projectTasks, "Tasks containing 'project'");
    
    // Get a specific task and update it
    if (!allTasks.empty()) {
        auto firstTask = allTasks[0];
        if (firstTask.getId().has_value()) {
            std::cout << "\nUpdating task with ID " << firstTask.getId().value() << "..." << std::endl;
            
            firstTask.setDescription(firstTask.getDescription() + " - UPDATED");
            firstTask.setDeadline(now + std::chrono::hours(48)); // Push deadline to day after tomorrow
            
            if (taskManager.updateTask(firstTask)) {
                std::cout << "✓ Task updated successfully!" << std::endl;
                
                // Fetch and display the updated task
                auto updatedTask = taskManager.getTask(firstTask.getId().value());
                if (updatedTask.has_value()) {
                    std::cout << "Updated task: " << updatedTask.value().toString() << std::endl;
                }
            } else {
                std::cout << "✗ Failed to update task!" << std::endl;
            }
        }
    }
    
    // Demonstrate task parsing from string
    std::cout << "\nDemonstrating deadline parsing..." << std::endl;
    
    try {
        auto parsedDeadline1 = Task::parseDeadline("2024-12-31 23:59:59");
        auto parsedDeadline2 = Task::parseDeadline("2024-06-15");
        
        Task taskFromString("Parse test task", 
                           "Task created with parsed deadline", 
                           parsedDeadline1);
        
        std::cout << "Task with parsed deadline: " << taskFromString.toString() << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error parsing deadline: " << e.what() << std::endl;
    }
    
    // Clean up demonstration
    std::cout << "\nDemo cleanup options:" << std::endl;
    std::cout << "1. Keep demo data" << std::endl;
    std::cout << "2. Clear all tasks" << std::endl;
    std::cout << "Enter choice (1 or 2): ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 2) {
        if (taskManager.clearAllTasks()) {
            std::cout << "✓ All tasks cleared successfully!" << std::endl;
        } else {
            std::cout << "✗ Failed to clear tasks!" << std::endl;
        }
    } else {
        std::cout << "Demo data preserved. Database file: demo_tasks.db" << std::endl;
    }
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    
    return 0;
} 