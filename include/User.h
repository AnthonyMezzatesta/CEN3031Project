#pragma once

#include <string>
#include <optional>
#include <chrono>

class User {
private:
    std::optional<int> id_;
    std::string email_;
    std::string password_hash_;
    std::chrono::system_clock::time_point created_at_;

public:
    User() = default;
    User(const std::string& email, const std::string& password_hash);
    User(int id, const std::string& email, const std::string& password_hash, 
         const std::chrono::system_clock::time_point& created_at);

    std::optional<int> getId() const { return id_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getPasswordHash() const { return password_hash_; }
    const std::chrono::system_clock::time_point& getCreatedAt() const { return created_at_; }

    void setId(int id) { id_ = id; }
    void setEmail(const std::string& email) { email_ = email; }
    void setPasswordHash(const std::string& password_hash) { password_hash_ = password_hash; }
    void setCreatedAt(const std::chrono::system_clock::time_point& created_at) { created_at_ = created_at; }

    std::string toString() const;
    std::string getCreatedAtString() const;
    
    static std::string hashPassword(const std::string& password);
    static bool verifyPassword(const std::string& password, const std::string& hash);
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
}; 