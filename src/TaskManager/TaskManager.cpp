#include "TaskManager.h"
#include <iostream>
#include <sstream>
#include <ctime>

// Constructor and destructor
TaskManager::TaskManager(const std::string& db_path) : db_(nullptr), db_path_(db_path) {}

TaskManager::~TaskManager() {
    close();
}

// Database operations
bool TaskManager::initialize() {
    int rc = sqlite3_open(db_path_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db_) << std::endl;
        close();
        return false;
    }
    
    return createTable();
}

void TaskManager::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

// Helper methods
// Added priority column to table
bool TaskManager::createTable() {
    const std::string create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            description TEXT NOT NULL,
            deadline INTEGER NOT NULL,
            priority INTEGER NOT NULL DEFAULT 0
        );
    )";
    
    return executeQuery(create_table_sql);
}

bool TaskManager::executeQuery(const std::string& query) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    
    return true;
}

std::vector<Task> TaskManager::executeSelectQuery(const std::string& query) {
    std::vector<Task> tasks;
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return tasks;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 3);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);
        
        // Read priority from DB
        int priority_val = sqlite3_column_int(stmt, 4);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);
        
        // Added priority to task constructor
        tasks.emplace_back(id, name, description, deadline, priority);

    }
    
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL execution error: " << sqlite3_errmsg(db_) << std::endl;
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

// Task operations
bool TaskManager::addTask(const Task& task) {
    const std::string sql = "INSERT INTO tasks (name, description, deadline, priority) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, task.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, task.getDescription().c_str(), -1, SQLITE_STATIC);
    
    auto deadline_time_t = std::chrono::system_clock::to_time_t(task.getDeadline());
    sqlite3_bind_int64(stmt, 3, deadline_time_t);

    // Bind priority value
    int priority_val = static_cast<int>(task.getPriority());
    sqlite3_bind_int(stmt, 4, priority_val);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool TaskManager::removeTask(int task_id) {
    const std::string sql = "DELETE FROM tasks WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;

    // find a way to remove the actual Task from the database.
}

bool TaskManager::updateTask(const Task& task) {
    if (!task.getId().has_value()) {
        std::cerr << "Cannot update task without ID" << std::endl;
        return false;
    }
    
    // Updated SQL for priority
    const std::string sql = "UPDATE tasks SET name = ?, description = ?, deadline = ?, priority = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, task.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, task.getDescription().c_str(), -1, SQLITE_STATIC);
    
    auto deadline_time_t = std::chrono::system_clock::to_time_t(task.getDeadline());
    sqlite3_bind_int64(stmt, 3, deadline_time_t);

    // Bind priority
    int priority_val = static_cast<int>(task.getPriority());
    sqlite3_bind_int(stmt, 4, priority_val);

    sqlite3_bind_int(stmt, 5, task.getId().value());

    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

// Updated SQL for priority
std::optional<Task> TaskManager::getTask(int task_id) {
    const std::string sql = "SELECT id, name, description, deadline, priority FROM tasks WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 3);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        // Added priority column
        int priority_val = sqlite3_column_int(stmt, 4);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);

        
        sqlite3_finalize(stmt);
        return Task(id, name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

// Updated SQL for priority
std::vector<Task> TaskManager::getAllTasks() {
    const std::string sql = "SELECT id, name, description, deadline, priority FROM tasks ORDER BY deadline ASC";
    return executeSelectQuery(sql);
}

// Updated SQL for priority
std::vector<Task> TaskManager::getTasksByName(const std::string& name) {
    const std::string sql = "SELECT id, name, description, deadline, priority FROM tasks WHERE name LIKE ? ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    std::string pattern = "%" + name + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_STATIC);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string task_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 3);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        // Added priority column
        int priority_val = sqlite3_column_int(stmt, 4);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);

        
        tasks.emplace_back(id, task_name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

std::vector<Task> TaskManager::getOverdueTasks() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    const std::string sql = "SELECT id, name, description, deadline FROM tasks WHERE deadline < ? ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    sqlite3_bind_int64(stmt, 1, now_time_t);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 3);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        int priority_val = sqlite3_column_int(stmt, 4);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);

        tasks.emplace_back(id, name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

std::vector<Task> TaskManager::getUpcomingTasks(int days) {
    auto now = std::chrono::system_clock::now();
    auto future = now + std::chrono::hours(24 * days);
    
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto future_time_t = std::chrono::system_clock::to_time_t(future);
    
    const std::string sql = "SELECT id, name, description, deadline, priority FROM tasks WHERE deadline >= ? AND deadline <= ? ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    sqlite3_bind_int64(stmt, 1, now_time_t);
    sqlite3_bind_int64(stmt, 2, future_time_t);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 3);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        int priority_val = sqlite3_column_int(stmt, 4);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);
        
        tasks.emplace_back(id, name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

// New method to get tasks by priority
std::vector<Task> TaskManager::getTasksByPriority(Task::Priority priority) {
    const std::string sql = "SELECT id, name, description, deadline, priority FROM tasks WHERE priority = ? ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    int priority_val = static_cast<int>(priority);
    sqlite3_bind_int(stmt, 1, priority_val);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 3);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);
        
        int task_priority_val = sqlite3_column_int(stmt, 4);
        Task::Priority task_priority = static_cast<Task::Priority>(task_priority_val);
        
        tasks.emplace_back(id, name, description, deadline, task_priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

// Utility methods
int TaskManager::getTaskCount() {
    const std::string sql = "SELECT COUNT(*) FROM tasks";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return -1;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

bool TaskManager::clearAllTasks() {
    const std::string sql = "DELETE FROM tasks";
    return executeQuery(sql);
}

// Static error handling
std::string TaskManager::getLastError(sqlite3* db) {
    return std::string(sqlite3_errmsg(db));
} 