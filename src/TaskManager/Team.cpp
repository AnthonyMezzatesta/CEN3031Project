#include "Team.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// Constructors
Team::Team(const std::string& name, const std::string& description, int owner_user_id)
    : name_(name), description_(description), owner_user_id_(owner_user_id), 
      created_at_(std::chrono::system_clock::now()) {}

Team::Team(int id, const std::string& name, const std::string& description, 
           int owner_user_id, const std::chrono::system_clock::time_point& created_at)
    : id_(id), name_(name), description_(description), owner_user_id_(owner_user_id), 
      created_at_(created_at) {}

// Utility methods
std::string Team::toString() const {
    std::ostringstream oss;
    oss << "Team[";
    if (id_.has_value()) {
        oss << "ID: " << id_.value() << ", ";
    }
    oss << "Name: \"" << name_ << "\", "
        << "Description: \"" << description_ << "\", "
        << "Owner ID: " << owner_user_id_ << ", "
        << "Created: " << getCreatedAtString() << "]";
    return oss.str();
}

std::string Team::getCreatedAtString() const {
    return formatTimestamp(created_at_);
}

std::string Team::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm* tm = std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}