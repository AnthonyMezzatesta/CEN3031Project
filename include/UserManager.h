#pragma once

#include "User.h"
#include <sqlite3.h>
#include <vector>
#include <memory>
#include <string>
#include <optional>

class UserManager {
private:
    sqlite3* db_;
    std::string db_path_;
    std::optional<User> current_user_;

    bool createUsersTable();
    bool executeQuery(const std::string& query);

public:
    explicit UserManager(const std::string& db_path = "users.db");
    ~UserManager();

    bool initialize();
    void close();

    bool registerUser(const std::string& email, const std::string& password);
    bool loginUser(const std::string& email, const std::string& password);
    void logoutUser();
    
    bool addUser(const User& user);
    std::optional<User> getUserByEmail(const std::string& email);
    std::optional<User> getUserById(int user_id);
    std::vector<User> getAllUsers();
    bool updateUser(const User& user);
    bool removeUser(int user_id);
    
    bool isLoggedIn() const { return current_user_.has_value(); }
    std::optional<User> getCurrentUser() const { return current_user_; }
    int getCurrentUserId() const;
    
    bool isConnected() const { return db_ != nullptr; }
    int getUserCount();
    bool clearAllUsers();
    bool emailExists(const std::string& email);
    
    static std::string getLastError(sqlite3* db);
}; 