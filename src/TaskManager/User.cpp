#include "User.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <functional>

// Constructors
User::User(const std::string& email, const std::string& password_hash)
    : email_(email), password_hash_(password_hash), created_at_(std::chrono::system_clock::now()) {}

User::User(int id, const std::string& email, const std::string& password_hash, 
           const std::chrono::system_clock::time_point& created_at)
    : id_(id), email_(email), password_hash_(password_hash), created_at_(created_at) {}

// Utility methods
std::string User::toString() const {
    std::ostringstream oss;
    oss << "User[";
    if (id_.has_value()) {
        oss << "ID: " << id_.value() << ", ";
    }
    oss << "Email: \"" << email_ << "\", "
        << "Created: " << getCreatedAtString() << "]";
    return oss.str();
}

std::string User::getCreatedAtString() const {
    return formatTimestamp(created_at_);
}

// Static utility methods
std::string User::hashPassword(const std::string& password) {
    // Simple hash implementation - NOT suitable for production!
    // In production, use bcrypt, scrypt, or Argon2
    std::hash<std::string> hasher;
    size_t hash_value = hasher(password + "salt_cen_pro_2024"); // Add salt
    
    std::ostringstream oss;
    oss << std::hex << hash_value;
    return oss.str();
}

bool User::verifyPassword(const std::string& password, const std::string& hash) {
    return hashPassword(password) == hash;
}

std::string User::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm* tm = std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
} 