#pragma once

#include <string>
#include <chrono>
#include <optional>

class Task {

// Task priority enum
public:
    enum class Priority {None, Low, Medium, High};

private:
    std::optional<int> id_;
    std::string name_;
    std::string description_;
    std::chrono::system_clock::time_point deadline_;
    std::chrono::system_clock::time_point time_of_completion;
    bool completed_ = false;
    bool archived = false;
    Priority priority_ = Priority::None; // Added priority member

public:
    Task() = default;
    Task(const std::string& name, const std::string& description, 
         const std::chrono::system_clock::time_point& deadline,
        Priority priority = Priority::None); // Added priority parameter
    Task(int id, const std::string& name, const std::string& description, 
         const std::chrono::system_clock::time_point& deadline,
        Priority priority = Priority::None);

    std::optional<int> getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::chrono::system_clock::time_point& getDeadline() const { return deadline_; }
    Priority getPriority() const {return priority_;} // Added priority getter

    void setId(int id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setDeadline(const std::chrono::system_clock::time_point& deadline) { deadline_ = deadline; }
    void setPriority(Priority priority) {priority_ = priority;} // Added priority setter
    void setTaskCompleted() { completed_ = true; archived = true; }
    void set_time_completed(const std::chrono::system_clock::time_point& currentTime) { time_of_completion = currentTime; }

    std::string getDeadlineString() const;
    bool isOverdue() const;
    std::string toString() const;
    static std::string priorityToString(Priority priority); // New priority to string helper
    static Priority stringToPriority(const std::string& priority_str); // New string to priority helper

    static std::chrono::system_clock::time_point parseDeadline(const std::string& deadline_str);
    static std::string formatDeadline(const std::chrono::system_clock::time_point& deadline);

};

void mark_task_completed(Task& someTask, std::chrono::system_clock::time_point currentTime);