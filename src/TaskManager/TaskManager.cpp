#include "TaskManager.h"
#include <iostream>
#include <sstream>
#include <ctime>

// Constructor and destructor
TaskManager::TaskManager(const std::string& db_path) : db_(nullptr), db_path_(db_path), current_user_id_(-1) {}

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
// Added priority column and user_id foreign key to table
bool TaskManager::createTable() {
    const std::string create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            team_id INTEGER NULL,
            name TEXT NOT NULL,
            description TEXT NOT NULL,
            deadline INTEGER NOT NULL,
            priority INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE,
            FOREIGN KEY (team_id) REFERENCES teams (id) ON DELETE CASCADE
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
        // Skip user_id column (index 1) and team_id column (index 2) as they're not needed for Task object
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 5);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);
        
        // Read priority from DB
        int priority_val = sqlite3_column_int(stmt, 6);
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
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot add task." << std::endl;
        return false;
    }
    
    const std::string sql = "INSERT INTO tasks (user_id, team_id, name, description, deadline, priority) VALUES (?, NULL, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, current_user_id_);
    sqlite3_bind_text(stmt, 2, task.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, task.getDescription().c_str(), -1, SQLITE_STATIC);
    
    auto deadline_time_t = std::chrono::system_clock::to_time_t(task.getDeadline());
    sqlite3_bind_int64(stmt, 4, deadline_time_t);

    // Bind priority value
    int priority_val = static_cast<int>(task.getPriority());
    sqlite3_bind_int(stmt, 5, priority_val);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool TaskManager::removeTask(int task_id) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot remove task." << std::endl;
        return false;
    }
    
    const std::string sql = "DELETE FROM tasks WHERE id = ? AND user_id = ? AND team_id IS NULL";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    sqlite3_bind_int(stmt, 2, current_user_id_);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

bool TaskManager::updateTask(const Task& task) {
    if (!task.getId().has_value()) {
        std::cerr << "Cannot update task without ID" << std::endl;
        return false;
    }
    
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot update task." << std::endl;
        return false;
    }
    
    // Updated SQL for priority and user ownership
    const std::string sql = "UPDATE tasks SET name = ?, description = ?, deadline = ?, priority = ? WHERE id = ? AND user_id = ?";
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
    sqlite3_bind_int(stmt, 6, current_user_id_);

    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

// Updated SQL for priority and user filtering
std::optional<Task> TaskManager::getTask(int task_id) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get task." << std::endl;
        return std::nullopt;
    }
    
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE id = ? AND user_id = ? AND team_id IS NULL";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    sqlite3_bind_int(stmt, 2, current_user_id_);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        // Skip user_id column (index 1) and team_id column (index 2)
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 5);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        // Added priority column
        int priority_val = sqlite3_column_int(stmt, 6);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);

        
        sqlite3_finalize(stmt);
        return Task(id, name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

// Updated SQL for priority and user filtering
std::vector<Task> TaskManager::getAllTasks() {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get tasks." << std::endl;
        return {};
    }
    
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE user_id = ? AND team_id IS NULL ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    std::vector<Task> tasks;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return tasks;
    }
    
    sqlite3_bind_int(stmt, 1, current_user_id_);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        // Skip user_id column (index 1) and team_id column (index 2)
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 5);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);
        
        int priority_val = sqlite3_column_int(stmt, 6);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);
        
        tasks.emplace_back(id, name, description, deadline, priority);
    }
    
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL execution error: " << sqlite3_errmsg(db_) << std::endl;
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

// Updated SQL for priority and user filtering
std::vector<Task> TaskManager::getTasksByName(const std::string& name) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get tasks." << std::endl;
        return {};
    }
    
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE name LIKE ? AND user_id = ? AND team_id IS NULL ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    std::string pattern = "%" + name + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, current_user_id_);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        // Skip user_id column (index 1)
        std::string task_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 4);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        // Added priority column
        int priority_val = sqlite3_column_int(stmt, 5);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);

        
        tasks.emplace_back(id, task_name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

std::vector<Task> TaskManager::getOverdueTasks() {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get tasks." << std::endl;
        return {};
    }
    
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    const std::string sql = "SELECT id, user_id, name, description, deadline, priority FROM tasks WHERE deadline < ? AND user_id = ? ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    sqlite3_bind_int64(stmt, 1, now_time_t);
    sqlite3_bind_int(stmt, 2, current_user_id_);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        // Skip user_id column (index 1)
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 4);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        int priority_val = sqlite3_column_int(stmt, 5);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);

        tasks.emplace_back(id, name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

std::vector<Task> TaskManager::getUpcomingTasks(int days) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get tasks." << std::endl;
        return {};
    }
    
    auto now = std::chrono::system_clock::now();
    auto future = now + std::chrono::hours(24 * days);
    
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto future_time_t = std::chrono::system_clock::to_time_t(future);
    
    const std::string sql = "SELECT id, user_id, name, description, deadline, priority FROM tasks WHERE deadline >= ? AND deadline <= ? AND user_id = ? ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    sqlite3_bind_int64(stmt, 1, now_time_t);
    sqlite3_bind_int64(stmt, 2, future_time_t);
    sqlite3_bind_int(stmt, 3, current_user_id_);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        // Skip user_id column (index 1)
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 4);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        int priority_val = sqlite3_column_int(stmt, 5);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);
        
        tasks.emplace_back(id, name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

// Method to get tasks by priority (user-specific)
std::vector<Task> TaskManager::getTasksByPriority(Task::Priority priority) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get tasks." << std::endl;
        return {};
    }
    
    const std::string sql = "SELECT id, user_id, name, description, deadline, priority FROM tasks WHERE priority = ? AND user_id = ? ORDER BY deadline ASC";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    int priority_val = static_cast<int>(priority);
    sqlite3_bind_int(stmt, 1, priority_val);
    sqlite3_bind_int(stmt, 2, current_user_id_);
    
    std::vector<Task> tasks;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        // Skip user_id column (index 1)
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 4);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);
        
        int task_priority_val = sqlite3_column_int(stmt, 5);
        Task::Priority task_priority = static_cast<Task::Priority>(task_priority_val);
        
        tasks.emplace_back(id, name, description, deadline, task_priority);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

// Utility methods
int TaskManager::getTaskCount() {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get task count." << std::endl;
        return -1;
    }
    
    const std::string sql = "SELECT COUNT(*) FROM tasks WHERE user_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, current_user_id_);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

bool TaskManager::clearAllTasks() {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot clear tasks." << std::endl;
        return false;
    }
    
    const std::string sql = "DELETE FROM tasks WHERE user_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, current_user_id_);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool TaskManager::clearUserTasks(int user_id) {
    const std::string sql = "DELETE FROM tasks WHERE user_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

// Team-based task operations
bool TaskManager::addTeamTask(const Task& task, int team_id) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot add team task." << std::endl;
        return false;
    }
    
    const std::string sql = "INSERT INTO tasks (user_id, team_id, name, description, deadline, priority) VALUES (?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, current_user_id_);
    sqlite3_bind_int(stmt, 2, team_id);
    sqlite3_bind_text(stmt, 3, task.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, task.getDescription().c_str(), -1, SQLITE_STATIC);
    
    auto deadline_time_t = std::chrono::system_clock::to_time_t(task.getDeadline());
    sqlite3_bind_int64(stmt, 5, deadline_time_t);

    int priority_val = static_cast<int>(task.getPriority());
    sqlite3_bind_int(stmt, 6, priority_val);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool TaskManager::removeTeamTask(int task_id, int team_id) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot remove team task." << std::endl;
        return false;
    }
    
    const std::string sql = "DELETE FROM tasks WHERE id = ? AND team_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    sqlite3_bind_int(stmt, 2, team_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

bool TaskManager::updateTeamTask(const Task& task, int team_id) {
    if (!task.getId().has_value()) {
        std::cerr << "Cannot update task without ID" << std::endl;
        return false;
    }
    
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot update team task." << std::endl;
        return false;
    }
    
    const std::string sql = "UPDATE tasks SET name = ?, description = ?, deadline = ?, priority = ? WHERE id = ? AND team_id = ?";
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
    
    int priority_val = static_cast<int>(task.getPriority());
    sqlite3_bind_int(stmt, 4, priority_val);
    sqlite3_bind_int(stmt, 5, task.getId().value());
    sqlite3_bind_int(stmt, 6, team_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

std::optional<Task> TaskManager::getTeamTask(int task_id, int team_id) {
    if (!hasCurrentUser()) {
        std::cerr << "No user logged in. Cannot get team task." << std::endl;
        return std::nullopt;
    }
    
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE id = ? AND team_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    sqlite3_bind_int(stmt, 2, team_id);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 5);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);

        int priority_val = sqlite3_column_int(stmt, 6);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);
        
        sqlite3_finalize(stmt);
        return Task(id, name, description, deadline, priority);
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<Task> TaskManager::getAllTeamTasks(int team_id) {
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE team_id = " + std::to_string(team_id) + " ORDER BY deadline ASC";
    return executeSelectQuery(sql);
}

std::vector<Task> TaskManager::getTeamTasksByName(const std::string& name, int team_id) {
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE name LIKE '%" + name + "%' AND team_id = " + std::to_string(team_id) + " ORDER BY deadline ASC";
    return executeSelectQuery(sql);
}

std::vector<Task> TaskManager::getTeamOverdueTasks(int team_id) {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE deadline < " + std::to_string(now_time_t) + " AND team_id = " + std::to_string(team_id) + " ORDER BY deadline ASC";
    return executeSelectQuery(sql);
}

std::vector<Task> TaskManager::getTeamUpcomingTasks(int team_id, int days) {
    auto now = std::chrono::system_clock::now();
    auto future = now + std::chrono::hours(24 * days);
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto future_time_t = std::chrono::system_clock::to_time_t(future);
    
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE deadline >= " + std::to_string(now_time_t) + " AND deadline <= " + std::to_string(future_time_t) + " AND team_id = " + std::to_string(team_id) + " ORDER BY deadline ASC";
    return executeSelectQuery(sql);
}

std::vector<Task> TaskManager::getTeamTasksByPriority(Task::Priority priority, int team_id) {
    int priority_val = static_cast<int>(priority);
    const std::string sql = "SELECT id, user_id, team_id, name, description, deadline, priority FROM tasks WHERE priority = " + std::to_string(priority_val) + " AND team_id = " + std::to_string(team_id) + " ORDER BY deadline ASC";
    return executeSelectQuery(sql);
}

std::vector<Task> TaskManager::getAllUserAndTeamTasks(int user_id) {
    const std::string sql = R"(
        SELECT id, user_id, team_id, name, description, deadline, priority 
        FROM tasks 
        WHERE user_id = ? OR team_id IN (
            SELECT team_id FROM team_memberships WHERE user_id = ?
        ) 
        ORDER BY deadline ASC
    )";
    
    sqlite3_stmt* stmt;
    std::vector<Task> tasks;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return tasks;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, user_id);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        int64_t deadline_timestamp = sqlite3_column_int64(stmt, 5);
        auto deadline = std::chrono::system_clock::from_time_t(deadline_timestamp);
        
        int priority_val = sqlite3_column_int(stmt, 6);
        Task::Priority priority = static_cast<Task::Priority>(priority_val);
        
        tasks.emplace_back(id, name, description, deadline, priority);
    }
    
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL execution error: " << sqlite3_errmsg(db_) << std::endl;
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

bool TaskManager::isUserAuthorizedForTeamTask(int user_id, int team_id) {
    const std::string sql = "SELECT COUNT(*) FROM team_memberships WHERE user_id = ? AND team_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, team_id);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count > 0;
}

// Static error handling
std::string TaskManager::getLastError(sqlite3* db) {
    return std::string(sqlite3_errmsg(db));
} 