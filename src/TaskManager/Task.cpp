#include "Task.h"

#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>

// Constructors
Task::Task(const std::string& name, const std::string& description, 
           const std::chrono::system_clock::time_point& deadline)
    : name_(name), description_(description), deadline_(deadline) {}

Task::Task(int id, const std::string& name, const std::string& description, 
           const std::chrono::system_clock::time_point& deadline)
    : id_(id), name_(name), description_(description), deadline_(deadline) {}

// Utility methods
std::string Task::getDeadlineString() const {
    return formatDeadline(deadline_);
}

bool Task::isOverdue() const {
    return std::chrono::system_clock::now() > deadline_;
}

std::string Task::toString() const {
    std::ostringstream oss;
    oss << "Task[";
    if (id_.has_value()) {
        oss << "ID: " << id_.value() << ", ";
    }
    oss << "Name: \"" << name_ << "\", "
        << "Description: \"" << description_ << "\", "
        << "Deadline: " << getDeadlineString() << ", "
        << "Overdue: " << (isOverdue() ? "Yes" : "No") << "]";
    return oss.str();
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
    someTask.set_Task_Completed();
}