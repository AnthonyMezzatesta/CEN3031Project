#include "TaskManager.h"
#include "UserManager.h"
#include "Task.h"
#include "User.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <limits>

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

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool authenticateUser(UserManager& userManager) {
    int choice;
    std::string email, password;
    
    while (true) {
        std::cout << "\n=== Task Manager Authentication ===" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register new account" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter choice (1-3): ";
        
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        
        switch (choice) {
            case 1: // Login
                std::cout << "Enter email: ";
                std::getline(std::cin, email);
                std::cout << "Enter password: ";
                std::getline(std::cin, password);
                
                if (userManager.loginUser(email, password)) {
                    std::cout << "✓ Login successful! Welcome back, " << email << std::endl;
                    return true;
                } else {
                    std::cout << "✗ Login failed. Please check your credentials." << std::endl;
                }
                break;
                
            case 2: // Register
                std::cout << "Enter email: ";
                std::getline(std::cin, email);
                std::cout << "Enter password: ";
                std::getline(std::cin, password);
                
                if (userManager.registerUser(email, password)) {
                    std::cout << "✓ Registration successful!" << std::endl;
                    if (userManager.loginUser(email, password)) {
                        std::cout << "✓ Automatically logged in. Welcome, " << email << std::endl;
                        return true;
                    }
                } else {
                    std::cout << "✗ Registration failed. Email might already exist." << std::endl;
                }
                break;
                
            case 3: // Exit
                std::cout << "Goodbye!" << std::endl;
                return false;
                
            default:
                std::cout << "Invalid choice. Please enter 1, 2, or 3." << std::endl;
                break;
        }
    }
}

int main() {
    std::cout << "=== Task Manager with Account System ===" << std::endl;
    
    // Initialize UserManager
    UserManager userManager("task_users.db");
    if (!userManager.initialize()) {
        std::cerr << "Failed to initialize UserManager!" << std::endl;
        return 1;
    }
    
    // Initialize TaskManager
    TaskManager taskManager("user_tasks.db");
    if (!taskManager.initialize()) {
        std::cerr << "Failed to initialize TaskManager!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ System initialized successfully!" << std::endl;
    
    // Authenticate user
    if (!authenticateUser(userManager)) {
        return 0; // User chose to exit
    }
    
    // Set current user in TaskManager
    taskManager.setCurrentUser(userManager.getCurrentUserId());
    
    auto currentUser = userManager.getCurrentUser();
    std::cout << "\n=== Welcome to your personal task manager, " 
              << currentUser.value().getEmail() << "! ===" << std::endl;
    
    // Check if user has existing tasks
    int existingTaskCount = taskManager.getTaskCount();
    if (existingTaskCount > 0) {
        std::cout << "You have " << existingTaskCount << " existing tasks." << std::endl;
        std::cout << "Would you like to see them? (y/n): ";
        char viewTasks;
        std::cin >> viewTasks;
        clearInputBuffer();
        
        if (viewTasks == 'y' || viewTasks == 'Y') {
            auto allTasks = taskManager.getAllTasks();
            printTasks(allTasks, "Your Existing Tasks");
        }
        
        std::cout << "Would you like to clear all your existing tasks and start with demo data? (y/n): ";
        char clearTasks;
        std::cin >> clearTasks;
        clearInputBuffer();
        
        if (clearTasks == 'y' || clearTasks == 'Y') {
            taskManager.clearAllTasks();
            std::cout << "✓ Existing tasks cleared." << std::endl;
        } else {
            std::cout << "Keeping existing tasks. Demo will add new tasks to your collection." << std::endl;
        }
    } else {
        std::cout << "This is your first time! Let's create some demo tasks." << std::endl;
    }
    
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto nextWeek = now + std::chrono::hours(24 * 7);
    auto lastWeek = now - std::chrono::hours(24 * 7);
    
    // Added priority to demo tasks
    Task task1("Complete project proposal", 
               "Write and submit the final project proposal for the new initiative", 
               tomorrow, Task::Priority::High);
    
    Task task2("Team meeting preparation", 
               "Prepare presentation slides and agenda for next team meeting", 
               nextWeek, Task::Priority::Medium);
    
    Task task3("Submit expense report", 
               "Submit the quarterly expense report to accounting", 
               lastWeek, Task::Priority::Low);
    
    auto fewDays = now + std::chrono::hours(24 * 3);
    Task task4("Code review", 
               "Review and approve pending code changes in the repository", 
               fewDays, Task::Priority::Medium);
    
    std::cout << "\nAdding tasks..." << std::endl;
    
    if (taskManager.addTask(task1)) {
        std::cout << "✓ Added: " << task1.getName() << " (Priority: " 
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }
    
    if (taskManager.addTask(task2)) {
        std::cout << "✓ Added: " << task2.getName() << " (Priority: " 
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }
    
    if (taskManager.addTask(task3)) {
        std::cout << "✓ Added: " << task3.getName() << " (Priority: " 
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }
    
    if (taskManager.addTask(task4)) {
        std::cout << "✓ Added: " << task4.getName() << " (Priority: " 
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }
    
    std::cout << "\nTotal tasks in database: " << taskManager.getTaskCount() << std::endl;
    
    auto allTasks = taskManager.getAllTasks();
    printTasks(allTasks, "All Tasks");
    
    auto overdueTasks = taskManager.getOverdueTasks();
    printTasks(overdueTasks, "Overdue Tasks");
    
    auto upcomingTasks = taskManager.getUpcomingTasks(7);
    printTasks(upcomingTasks, "Upcoming Tasks (Next 7 Days)");

    // Added high priority tasks
    auto highPriorityTasks = taskManager.getTasksByPriority(Task::Priority::High);
    printTasks(highPriorityTasks, "High Priority Tasks");

    auto projectTasks = taskManager.getTasksByName("project");
    printTasks(projectTasks, "Tasks containing 'project'");
    
    if (!allTasks.empty()) {
        auto firstTask = allTasks[0];
        if (firstTask.getId().has_value()) {
            std::cout << "\nUpdating task with ID " << firstTask.getId().value() << "..." << std::endl;
            
            firstTask.setDescription(firstTask.getDescription() + " - UPDATED");
            firstTask.setPriority(Task::Priority::High); // Set first task to high priority
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
    
    std::cout << "\nDemonstrating deadline parsing..." << std::endl;
    
    auto parsedDeadline1 = Task::parseDeadline("2024-12-31 23:59:59");
    auto parsedDeadline2 = Task::parseDeadline("2024-06-15");
    
    Task taskFromString("Parse test task", 
                       "Task created with parsed deadline", 
                       parsedDeadline1);
    
    std::cout << "Task with parsed deadline: " << taskFromString.toString() << std::endl;
    
    std::cout << "\nSession options:" << std::endl;
    std::cout << "1. Keep all tasks for next login" << std::endl;
    std::cout << "2. Clear all your tasks" << std::endl;
    std::cout << "3. Just logout" << std::endl;
    std::cout << "Enter choice (1-3): ";
    
    int choice;
    if (!(std::cin >> choice)) {
        choice = 1;
    }
    
    switch (choice) {
        case 2:
            if (taskManager.clearAllTasks()) {
                std::cout << "✓ All your tasks cleared successfully!" << std::endl;
            } else {
                std::cout << "✗ Failed to clear tasks!" << std::endl;
            }
            break;
        case 3:
            std::cout << "Logging out without changes." << std::endl;
            break;
        default:
            std::cout << "Tasks preserved for next login." << std::endl;
            break;
    }
    
    userManager.logoutUser();
    std::cout << "\n=== Session Complete ===" << std::endl;
    
    return 0;
} 