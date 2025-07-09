#include "UserManager.h"
#include <iostream>
#include <ctime>

// Constructor and destructor
UserManager::UserManager(const std::string& db_path) : db_(nullptr), db_path_(db_path) {}

UserManager::~UserManager() {
    close();
}

// Database operations
bool UserManager::initialize() {
    int rc = sqlite3_open(db_path_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open user database: " << sqlite3_errmsg(db_) << std::endl;
        close();
        return false;
    }
    
    return createUsersTable();
}

void UserManager::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool UserManager::createUsersTable() {
    const std::string create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            created_at INTEGER NOT NULL
        );
    )";
    
    return executeQuery(create_table_sql);
}

bool UserManager::executeQuery(const std::string& query) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    
    return true;
}

// Authentication methods
bool UserManager::registerUser(const std::string& email, const std::string& password) {
    if (emailExists(email)) {
        std::cerr << "Email already exists: " << email << std::endl;
        return false;
    }
    
    std::string password_hash = User::hashPassword(password);
    User new_user(email, password_hash);
    
    return addUser(new_user);
}

bool UserManager::loginUser(const std::string& email, const std::string& password) {
    auto user = getUserByEmail(email);
    if (!user.has_value()) {
        std::cerr << "User not found: " << email << std::endl;
        return false;
    }
    
    if (!User::verifyPassword(password, user.value().getPasswordHash())) {
        std::cerr << "Invalid password for user: " << email << std::endl;
        return false;
    }
    
    current_user_ = user.value();
    std::cout << "User logged in successfully: " << email << std::endl;
    return true;
}

void UserManager::logoutUser() {
    if (current_user_.has_value()) {
        std::cout << "User logged out: " << current_user_.value().getEmail() << std::endl;
        current_user_.reset();
    }
}

// User management methods
bool UserManager::addUser(const User& user) {
    const std::string sql = "INSERT INTO users (email, password_hash, created_at) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, user.getEmail().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.getPasswordHash().c_str(), -1, SQLITE_STATIC);
    
    auto created_time_t = std::chrono::system_clock::to_time_t(user.getCreatedAt());
    sqlite3_bind_int64(stmt, 3, created_time_t);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::optional<User> UserManager::getUserByEmail(const std::string& email) {
    const std::string sql = "SELECT id, email, password_hash, created_at FROM users WHERE email = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string user_email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 3);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        sqlite3_finalize(stmt);
        return User(id, user_email, password_hash, created_at);
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::optional<User> UserManager::getUserById(int user_id) {
    const std::string sql = "SELECT id, email, password_hash, created_at FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 3);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        sqlite3_finalize(stmt);
        return User(id, email, password_hash, created_at);
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<User> UserManager::getAllUsers() {
    const std::string sql = "SELECT id, email, password_hash, created_at FROM users ORDER BY created_at ASC";
    sqlite3_stmt* stmt;
    std::vector<User> users;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return users;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        int64_t created_timestamp = sqlite3_column_int64(stmt, 3);
        auto created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        
        users.emplace_back(id, email, password_hash, created_at);
    }
    
    sqlite3_finalize(stmt);
    return users;
}

bool UserManager::updateUser(const User& user) {
    if (!user.getId().has_value()) {
        std::cerr << "Cannot update user without ID" << std::endl;
        return false;
    }
    
    const std::string sql = "UPDATE users SET email = ?, password_hash = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, user.getEmail().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.getPasswordHash().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, user.getId().value());
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

bool UserManager::removeUser(int user_id) {
    const std::string sql = "DELETE FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

// Session management
int UserManager::getCurrentUserId() const {
    if (current_user_.has_value() && current_user_.value().getId().has_value()) {
        return current_user_.value().getId().value();
    }
    return -1;
}

// Utility methods
int UserManager::getUserCount() {
    const std::string sql = "SELECT COUNT(*) FROM users";
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

bool UserManager::clearAllUsers() {
    const std::string sql = "DELETE FROM users";
    current_user_.reset(); // Logout current user if clearing all users
    return executeQuery(sql);
}

bool UserManager::emailExists(const std::string& email) {
    const std::string sql = "SELECT COUNT(*) FROM users WHERE email = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count > 0;
}

// Static error handling
std::string UserManager::getLastError(sqlite3* db) {
    return std::string(sqlite3_errmsg(db));
} 