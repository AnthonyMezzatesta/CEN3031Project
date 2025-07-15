#include "Task.h"

#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map> // Added for priority mapping

// Constructors
// Updated constructors with priority parameter
Task::Task(const std::string& name, const std::string& description, 
           const std::chrono::system_clock::time_point& deadline,
        Priority priority)
    : name_(name), description_(description), deadline_(deadline), priority_(priority) {}

Task::Task(int id, const std::string& name, const std::string& description, 
           const std::chrono::system_clock::time_point& deadline,
        Priority priority)
    : id_(id), name_(name), description_(description), deadline_(deadline), priority_(priority) {}

// Utility methods
std::string Task::getDeadlineString() const {
    return formatDeadline(deadline_);
}

bool Task::isOverdue() const {
    return std::chrono::system_clock::now() > deadline_;
}
// Updated toString to display priority
std::string Task::toString() const {
    std::ostringstream oss;
    oss << "Task[";
    if (id_.has_value()) {
        oss << "ID: " << id_.value() << ", ";
    }
    oss << "Name: \"" << name_ << "\", "
        << "Description: \"" << description_ << "\", "
        << "Deadline: " << getDeadlineString() << ", "
        << "Priority: " << priorityToString(priority_) << ", " // Added priority 
        << "Overdue: " << (isOverdue() ? "Yes" : "No") << "]";
    return oss.str();
}

// Added helper methods for priority
std::string Task::priorityToString(Priority priority) {
    static const std::map<Priority, std::string> priorityMap = {
        {Priority::None, "None"},
        {Priority::Low, "Low"},
        {Priority::Medium, "Medium"},
        {Priority::High, "High"}
    };
    // return string of priority
    return priorityMap.at(priority);
}

Task::Priority Task::stringToPriority(const std::string& priority_str) {
    static const std::map<std::string, Priority> stringMap = {
        {"None", Priority::None},
        {"Low", Priority::Low},
        {"Medium", Priority::Medium},
        {"High", Priority::High}
    };
    // Return priority if it exists, otherwise priority is None
    auto it = stringMap.find(priority_str);
    if (it != stringMap.end()) {
        return it->second;
    }
    else {
        return Priority::None;
    }
}

// Static utility methods
std::chrono::system_clock::time_point Task::parseDeadline(const std::string& deadline_str) {
    std::tm tm = {};
    std::istringstream ss(deadline_str);
    
    // Try to parse format: YYYY-MM-DD HH:MM:SS
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        // Try to parse format: YYYY-MM-DD
        ss.clear();
        ss.str(deadline_str);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        tm.tm_hour = 23;
        tm.tm_min = 59;
        tm.tm_sec = 59;
    }
    
    if (ss.fail()) {
        // Return current time if parsing fails
        return std::chrono::system_clock::now();
    }
    
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

std::string Task::formatDeadline(const std::chrono::system_clock::time_point& deadline) {
    std::time_t time = std::chrono::system_clock::to_time_t(deadline);
    std::tm* tm = std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void mark_task_completed(Task& someTask, std::chrono::system_clock::time_point currentTime) {
    someTask.set_time_completed(currentTime);
    someTask.setTaskCompleted();
}