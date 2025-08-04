#include "test_framework.h"
#include "../include/TaskManager.h"
#include "../include/TeamManager.h"
#include "../include/UserManager.h"
#include <filesystem>

void test_team_task_operations() {
    // Setup databases - use same database for teams and tasks
    UserManager um("test_integration.db");
    TeamManager tm("test_integration.db");
    TaskManager task_mgr("test_integration.db");
    
    um.initialize();
    tm.initialize();
    task_mgr.initialize();
    
    // Clear existing data
    um.clearAllUsers();
    tm.clearAllTeams();
    task_mgr.clearAllTasks();
    
    // Setup users
    um.registerUser("user1@test.com", "password");
    um.registerUser("user2@test.com", "password");
    um.loginUser("user1@test.com", "password");
    task_mgr.setCurrentUser(um.getCurrentUserId());
    
    // Setup team
    tm.createTeam("Dev Team", "Development team", um.getCurrentUserId());
    auto team = tm.getTeamByName("Dev Team");
    ASSERT_TRUE(team.has_value());
    int team_id = team.value().getId().value();
    
    // Add user2 to team
    auto user2 = um.getUserByEmail("user2@test.com");
    ASSERT_TRUE(user2.has_value());
    tm.addUserToTeam(user2.value().getId().value(), team_id);
    
    // Create a team task
    auto deadline = std::chrono::system_clock::now() + std::chrono::hours(48);
    Task team_task("Team Feature", "Implement new team feature", deadline, Task::Priority::High);
    
    ASSERT_TRUE(task_mgr.addTeamTask(team_task, team_id));
    
    // Get team tasks
    auto team_tasks = task_mgr.getAllTeamTasks(team_id);
    ASSERT_EQ(1, team_tasks.size());
    ASSERT_STR_EQ("Team Feature", team_tasks[0].getName());
    
    // Get personal tasks (should be empty)
    auto personal_tasks = task_mgr.getAllTasks();
    ASSERT_EQ(0, personal_tasks.size());
    
    // Get combined tasks for user
    auto all_user_tasks = task_mgr.getAllUserAndTeamTasks(um.getCurrentUserId());
    ASSERT_EQ(1, all_user_tasks.size());
    
    um.close();
    tm.close();
    task_mgr.close();
}

void test_team_task_authorization() {
    UserManager um("test_integration.db");
    TeamManager tm("test_integration.db");
    TaskManager task_mgr("test_integration.db");
    
    um.initialize();
    tm.initialize();
    task_mgr.initialize();
    
    um.clearAllUsers();
    tm.clearAllTeams();
    task_mgr.clearAllTasks();
    
    // Setup users
    um.registerUser("owner@test.com", "password");
    um.registerUser("member@test.com", "password");
    um.registerUser("outsider@test.com", "password");
    
    auto owner = um.getUserByEmail("owner@test.com");
    auto member = um.getUserByEmail("member@test.com");
    auto outsider = um.getUserByEmail("outsider@test.com");
    
    ASSERT_TRUE(owner.has_value());
    ASSERT_TRUE(member.has_value());
    ASSERT_TRUE(outsider.has_value());
    
    // Setup team
    tm.createTeam("Private Team", "Private team", owner.value().getId().value());
    auto team = tm.getTeamByName("Private Team");
    ASSERT_TRUE(team.has_value());
    int team_id = team.value().getId().value();
    
    // Add member to team
    tm.addUserToTeam(member.value().getId().value(), team_id);
    
    // Test authorization
    ASSERT_TRUE(task_mgr.isUserAuthorizedForTeamTask(owner.value().getId().value(), team_id));
    ASSERT_TRUE(task_mgr.isUserAuthorizedForTeamTask(member.value().getId().value(), team_id));
    ASSERT_FALSE(task_mgr.isUserAuthorizedForTeamTask(outsider.value().getId().value(), team_id));
    
    um.close();
    tm.close();
    task_mgr.close();
}

void test_team_task_filtering() {
    UserManager um("test_integration.db");
    TeamManager tm("test_integration.db");
    TaskManager task_mgr("test_integration.db");
    
    um.initialize();
    tm.initialize();
    task_mgr.initialize();
    
    um.clearAllUsers();
    tm.clearAllTeams();
    task_mgr.clearAllTasks();
    
    // Setup user and team
    um.registerUser("test@test.com", "password");
    um.loginUser("test@test.com", "password");
    task_mgr.setCurrentUser(um.getCurrentUserId());
    
    tm.createTeam("Filter Team", "Team for filtering tests", um.getCurrentUserId());
    auto team = tm.getTeamByName("Filter Team");
    ASSERT_TRUE(team.has_value());
    int team_id = team.value().getId().value();
    
    // Create tasks with different priorities and deadlines
    auto now = std::chrono::system_clock::now();
    auto overdue = now - std::chrono::hours(24);
    auto upcoming = now + std::chrono::hours(48);
    auto future = now + std::chrono::hours(24 * 10);
    
    Task high_task("High Priority", "High priority task", upcoming, Task::Priority::High);
    Task medium_task("Medium Priority", "Medium priority task", upcoming, Task::Priority::Medium);
    Task overdue_task("Overdue Task", "Overdue task", overdue, Task::Priority::Low);
    Task future_task("Future Task", "Future task", future, Task::Priority::Medium);
    
    // Add team tasks
    ASSERT_TRUE(task_mgr.addTeamTask(high_task, team_id));
    ASSERT_TRUE(task_mgr.addTeamTask(medium_task, team_id));
    ASSERT_TRUE(task_mgr.addTeamTask(overdue_task, team_id));
    ASSERT_TRUE(task_mgr.addTeamTask(future_task, team_id));
    
    // Test filtering by priority
    auto high_priority_tasks = task_mgr.getTeamTasksByPriority(Task::Priority::High, team_id);
    ASSERT_EQ(1, high_priority_tasks.size());
    ASSERT_STR_EQ("High Priority", high_priority_tasks[0].getName());
    
    auto medium_priority_tasks = task_mgr.getTeamTasksByPriority(Task::Priority::Medium, team_id);
    ASSERT_EQ(2, medium_priority_tasks.size());
    
    // Test filtering by name
    auto named_tasks = task_mgr.getTeamTasksByName("Priority", team_id);
    ASSERT_EQ(2, named_tasks.size());
    
    // Test overdue tasks
    auto overdue_tasks = task_mgr.getTeamOverdueTasks(team_id);
    ASSERT_EQ(1, overdue_tasks.size());
    ASSERT_STR_EQ("Overdue Task", overdue_tasks[0].getName());
    
    // Test upcoming tasks (next 7 days)
    auto upcoming_tasks = task_mgr.getTeamUpcomingTasks(team_id, 7);
    ASSERT_EQ(2, upcoming_tasks.size()); // high_task and medium_task
    
    um.close();
    tm.close();
    task_mgr.close();
}

void test_mixed_personal_and_team_tasks() {
    UserManager um("test_integration.db");
    TeamManager tm("test_integration.db");
    TaskManager task_mgr("test_integration.db");
    
    um.initialize();
    tm.initialize();
    task_mgr.initialize();
    
    um.clearAllUsers();
    tm.clearAllTeams();
    task_mgr.clearAllTasks();
    
    // Setup user and team
    um.registerUser("mixed@test.com", "password");
    um.loginUser("mixed@test.com", "password");
    task_mgr.setCurrentUser(um.getCurrentUserId());
    
    tm.createTeam("Mixed Team", "Team for mixed task tests", um.getCurrentUserId());
    auto team = tm.getTeamByName("Mixed Team");
    ASSERT_TRUE(team.has_value());
    int team_id = team.value().getId().value();
    
    auto deadline = std::chrono::system_clock::now() + std::chrono::hours(24);
    
    // Create personal tasks
    Task personal_task1("Personal Task 1", "First personal task", deadline);
    Task personal_task2("Personal Task 2", "Second personal task", deadline);
    
    // Create team tasks
    Task team_task1("Team Task 1", "First team task", deadline);
    Task team_task2("Team Task 2", "Second team task", deadline);
    
    // Add tasks
    ASSERT_TRUE(task_mgr.addTask(personal_task1));
    ASSERT_TRUE(task_mgr.addTask(personal_task2));
    ASSERT_TRUE(task_mgr.addTeamTask(team_task1, team_id));
    ASSERT_TRUE(task_mgr.addTeamTask(team_task2, team_id));
    
    // Test separation
    auto personal_tasks = task_mgr.getAllTasks();
    ASSERT_EQ(2, personal_tasks.size());
    
    auto team_tasks = task_mgr.getAllTeamTasks(team_id);
    ASSERT_EQ(2, team_tasks.size());
    
    // Test combined view
    auto all_tasks = task_mgr.getAllUserAndTeamTasks(um.getCurrentUserId());
    ASSERT_EQ(4, all_tasks.size());
    
    um.close();
    tm.close();
    task_mgr.close();
}

void run_team_task_tests() {
    std::cout << "Running Team Task tests..." << std::endl;
    
    // Clean up test database
    if (std::filesystem::exists("test_integration.db")) {
        std::filesystem::remove("test_integration.db");
    }
    
    test_team_task_operations();
    test_team_task_authorization();
    test_team_task_filtering();
    test_mixed_personal_and_team_tasks();
    
    // Clean up
    if (std::filesystem::exists("test_integration.db")) {
        std::filesystem::remove("test_integration.db");
    }
    
    std::cout << "All Team Task tests passed!" << std::endl;
}