#include "TaskManager.h"
#include "UserManager.h"
#include "TeamManager.h"
#include "Task.h"
#include "User.h"
#include "Team.h"
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

void printTeams(const std::vector<Team>& teams, const std::string& title) {
    std::cout << "\n" << title << ":" << std::endl;
    printSeparator();
    
    if (teams.empty()) {
        std::cout << "No teams found." << std::endl;
    } else {
        for (const auto& team : teams) {
            std::cout << team.toString() << std::endl;
        }
    }
    printSeparator();
}

bool manageTeams(UserManager& userManager, TaskManager& taskManager) {
    TeamManager teamManager("teams.db");
    if (!teamManager.initialize()) {
        std::cerr << "Failed to initialize TeamManager!" << std::endl;
        return false;
    }
    
    int userId = userManager.getCurrentUserId();
    
    while (true) {
        std::cout << "\n=== Team Management ===" << std::endl;
        std::cout << "1. View my teams" << std::endl;
        std::cout << "2. Create new team" << std::endl;
        std::cout << "3. Join existing team" << std::endl;
        std::cout << "4. View all teams" << std::endl;
        std::cout << "5. View team tasks" << std::endl;
        std::cout << "6. Add task to team" << std::endl;
        std::cout << "7. View all my tasks (personal + team)" << std::endl;
        std::cout << "8. Back to main menu" << std::endl;
        std::cout << "Enter choice (1-8): ";
        
        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        
        switch (choice) {
            case 1: { // View my teams
                auto myTeams = teamManager.getTeamsForUser(userId);
                printTeams(myTeams, "My Teams");
                break;
            }
            
            case 2: { // Create new team
                std::string name, description;
                std::cout << "Enter team name: ";
                std::getline(std::cin, name);
                std::cout << "Enter team description: ";
                std::getline(std::cin, description);
                
                if (teamManager.createTeam(name, description, userId)) {
                    std::cout << "✓ Team '" << name << "' created successfully!" << std::endl;
                } else {
                    std::cout << "✗ Failed to create team. Name might already exist." << std::endl;
                }
                break;
            }
            
            case 3: { // Join existing team
                auto allTeams = teamManager.getAllTeams();
                printTeams(allTeams, "Available Teams");
                
                if (!allTeams.empty()) {
                    std::cout << "Enter team ID to join: ";
                    int teamId;
                    if (std::cin >> teamId) {
                        clearInputBuffer();
                        if (teamManager.addUserToTeam(userId, teamId)) {
                            std::cout << "✓ Successfully joined team!" << std::endl;
                        } else {
                            std::cout << "✗ Failed to join team. You might already be a member." << std::endl;
                        }
                    } else {
                        std::cout << "Invalid team ID." << std::endl;
                        clearInputBuffer();
                    }
                }
                break;
            }
            
            case 4: { // View all teams
                auto allTeams = teamManager.getAllTeams();
                printTeams(allTeams, "All Teams");
                break;
            }
            
            case 5: { // View team tasks
                auto myTeams = teamManager.getTeamsForUser(userId);
                if (myTeams.empty()) {
                    std::cout << "You're not a member of any teams." << std::endl;
                    break;
                }
                
                printTeams(myTeams, "Select a team to view tasks");
                std::cout << "Enter team ID: ";
                int teamId;
                if (std::cin >> teamId) {
                    clearInputBuffer();
                    if (taskManager.isUserAuthorizedForTeamTask(userId, teamId)) {
                        auto teamTasks = taskManager.getAllTeamTasks(teamId);
                        auto team = teamManager.getTeamById(teamId);
                        std::string title = team.has_value() ? 
                            "Tasks for team: " + team.value().getName() : 
                            "Team Tasks";
                        printTasks(teamTasks, title);
                    } else {
                        std::cout << "✗ You don't have access to this team's tasks." << std::endl;
                    }
                } else {
                    std::cout << "Invalid team ID." << std::endl;
                    clearInputBuffer();
                }
                break;
            }
            
            case 6: { // Add task to team
                auto myTeams = teamManager.getTeamsForUser(userId);
                if (myTeams.empty()) {
                    std::cout << "You're not a member of any teams." << std::endl;
                    break;
                }
                
                printTeams(myTeams, "Select a team to add task to");
                std::cout << "Enter team ID: ";
                int teamId;
                if (!(std::cin >> teamId)) {
                    std::cout << "Invalid team ID." << std::endl;
                    clearInputBuffer();
                    break;
                }
                clearInputBuffer();
                
                if (!taskManager.isUserAuthorizedForTeamTask(userId, teamId)) {
                    std::cout << "✗ You don't have access to add tasks to this team." << std::endl;
                    break;
                }
                
                std::string name, description, deadlineStr;
                int priority;
                
                std::cout << "Enter task name: ";
                std::getline(std::cin, name);
                std::cout << "Enter task description: ";
                std::getline(std::cin, description);
                std::cout << "Enter deadline (YYYY-MM-DD HH:MM:SS or YYYY-MM-DD): ";
                std::getline(std::cin, deadlineStr);
                std::cout << "Enter priority (0=Low, 1=Medium, 2=High): ";
                
                if (!(std::cin >> priority) || priority < 0 || priority > 2) {
                    std::cout << "Invalid priority. Using Medium as default." << std::endl;
                    priority = 1;
                }
                clearInputBuffer();
                
                auto deadline = Task::parseDeadline(deadlineStr);
                Task task(name, description, deadline, static_cast<Task::Priority>(priority));
                
                if (taskManager.addTeamTask(task, teamId)) {
                    std::cout << "✓ Task added to team successfully!" << std::endl;
                } else {
                    std::cout << "✗ Failed to add task to team." << std::endl;
                }
                break;
            }
            
            case 7: { // View all my tasks (personal + team)
                auto allTasks = taskManager.getAllUserAndTeamTasks(userId);
                printTasks(allTasks, "All My Tasks (Personal + Team)");
                break;
            }
            
            case 8: // Back to main menu
                return true;
                
            default:
                std::cout << "Invalid choice. Please enter 1-8." << std::endl;
                break;
        }
    }
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
    
    // Main menu
    while (true) {
        std::cout << "\n=== Main Menu ===" << std::endl;
        std::cout << "1. Manage Teams" << std::endl;
        std::cout << "2. Personal Task Manager (Demo)" << std::endl;
        std::cout << "3. Logout" << std::endl;
        std::cout << "Enter choice (1-3): ";
        
        int mainChoice;
        if (!(std::cin >> mainChoice)) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        
        switch (mainChoice) {
            case 1:
                if (!manageTeams(userManager, taskManager)) {
                    std::cout << "Error accessing team management." << std::endl;
                }
                break;
                
            case 2:
                // Continue with existing personal task demo
                goto personal_tasks_demo;
                
            case 3:
                userManager.logoutUser();
                std::cout << "\n=== Session Complete ===" << std::endl;
                return 0;
                
            default:
                std::cout << "Invalid choice. Please enter 1-3." << std::endl;
                break;
        }
    }
    
    personal_tasks_demo:
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
    
    // This should not be reached due to the main menu loop
    return 0;
} 