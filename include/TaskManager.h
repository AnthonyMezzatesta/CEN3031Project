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

    // Task operations (now user-specific)
    bool addTask(const Task& task);
    bool removeTask(int task_id);
    bool updateTask(const Task& task);
    std::optional<Task> getTask(int task_id);
    std::vector<Task> getAllTasks();
    std::vector<Task> getTasksByName(const std::string& name);
    std::vector<Task> getOverdueTasks();
    std::vector<Task> getUpcomingTasks(int days = 7);
    std::vector<Task> getTasksByPriority(Task::Priority priority);

    bool isConnected() const { return db_ != nullptr; }
    int getTaskCount();
    bool clearAllTasks();
    bool clearUserTasks(int user_id);

    static std::string getLastError(sqlite3* db);
}; 