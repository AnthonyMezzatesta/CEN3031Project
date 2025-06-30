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

    // Helper methods
    bool createTable();
    bool executeQuery(const std::string& query);
    std::vector<Task> executeSelectQuery(const std::string& query);

public:
    // Constructor and destructor
    explicit TaskManager(const std::string& db_path = "tasks.db");
    ~TaskManager();

    // Database operations
    bool initialize();
    void close();

    // Task operations
    bool addTask(const Task& task);
    bool removeTask(int task_id);
    bool updateTask(const Task& task);
    std::optional<Task> getTask(int task_id);
    std::vector<Task> getAllTasks();
    std::vector<Task> getTasksByName(const std::string& name);
    std::vector<Task> getOverdueTasks();
    std::vector<Task> getUpcomingTasks(int days = 7);

    // Utility methods
    bool isConnected() const { return db_ != nullptr; }
    int getTaskCount();
    bool clearAllTasks();

    // Static error handling
    static std::string getLastError(sqlite3* db);
}; 