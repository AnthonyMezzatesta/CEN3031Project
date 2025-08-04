#pragma once

#include "Task.h"
#include <sqlite3.h>
#include <vector>
#include <memory>
#include <string>
#include <optional>

class TaskManager {
private:
    sqlite3* db_;
    std::string db_path_;
    int current_user_id_;

    bool createTable();
    bool executeQuery(const std::string& query);
    std::vector<Task> executeSelectQuery(const std::string& query);

public:
    explicit TaskManager(const std::string& db_path = "tasks.db");
    ~TaskManager();

    bool initialize();
    void close();

    // User management
    void setCurrentUser(int user_id) { current_user_id_ = user_id; }
    int getCurrentUser() const { return current_user_id_; }
    bool hasCurrentUser() const { return current_user_id_ > 0; }

    // Task operations (user-specific)
    bool addTask(const Task& task);
    bool removeTask(int task_id);
    bool updateTask(const Task& task);
    std::optional<Task> getTask(int task_id);
    std::vector<Task> getAllTasks();
    std::vector<Task> getTasksByName(const std::string& name);
    std::vector<Task> getOverdueTasks();
    std::vector<Task> getUpcomingTasks(int days = 7);
    std::vector<Task> getTasksByPriority(Task::Priority priority);
    
    // Team-based task operations
    bool addTeamTask(const Task& task, int team_id);
    bool removeTeamTask(int task_id, int team_id);
    bool updateTeamTask(const Task& task, int team_id);
    std::optional<Task> getTeamTask(int task_id, int team_id);
    std::vector<Task> getAllTeamTasks(int team_id);
    std::vector<Task> getTeamTasksByName(const std::string& name, int team_id);
    std::vector<Task> getTeamOverdueTasks(int team_id);
    std::vector<Task> getTeamUpcomingTasks(int team_id, int days = 7);
    std::vector<Task> getTeamTasksByPriority(Task::Priority priority, int team_id);
    std::vector<Task> getAllUserAndTeamTasks(int user_id);
    bool isUserAuthorizedForTeamTask(int user_id, int team_id);

    bool isConnected() const { return db_ != nullptr; }
    int getTaskCount();
    bool clearAllTasks();
    bool clearUserTasks(int user_id);

    static std::string getLastError(sqlite3* db);
}; 