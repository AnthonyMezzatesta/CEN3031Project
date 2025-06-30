#pragma once

#include <string>
#include <chrono>
#include <optional>

class Task {
private:
    std::optional<int> id_;
    std::string name_;
    std::string description_;
    std::chrono::system_clock::time_point deadline_;

public:
    // Constructors
    Task() = default;
    Task(const std::string& name, const std::string& description, 
         const std::chrono::system_clock::time_point& deadline);
    Task(int id, const std::string& name, const std::string& description, 
         const std::chrono::system_clock::time_point& deadline);

    // Getters
    std::optional<int> getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::chrono::system_clock::time_point& getDeadline() const { return deadline_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setDeadline(const std::chrono::system_clock::time_point& deadline) { deadline_ = deadline; }

    // Utility methods
    std::string getDeadlineString() const;
    bool isOverdue() const;
    std::string toString() const;

    // Static utility methods
    static std::chrono::system_clock::time_point parseDeadline(const std::string& deadline_str);
    static std::string formatDeadline(const std::chrono::system_clock::time_point& deadline);
}; 