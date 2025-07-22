#pragma once
#include <iostream>
#include <TaskManager.h>
#include "Task.h"

class ReminderManager
{
    const static int INTERVAL_SEC = 5;
    TaskManager taskManager_;
    std::vector<Task> tasksToRemind_;
    std::chrono::time_point<std::chrono::system_clock> lastCheck_;
public:
    // Initialize task manager and the last check time
    ReminderManager(TaskManager& tm)
        : taskManager_(tm), lastCheck_(std::chrono::system_clock::now())
    {
        // fill task container upon construction
        Reset();
    }

    std::vector<Task> GetTasks() { return tasksToRemind_; }

    static bool ShouldRemind(const Task& task, std::chrono::system_clock::time_point now) {
        auto deadline = task.getDeadline();
        auto priority = task.getPriority();
        auto hoursLeft = std::chrono::duration_cast<std::chrono::hours>(deadline - now).count();

        if (hoursLeft < 0) return false; // already past

        // Higher priority gets reminded sooner
        switch (priority) {
            case Task::Priority::High:
                return hoursLeft <= 48; // Remind if within 2 days
            case Task::Priority::Medium:
                return hoursLeft <= 24; // Remind if within 1 day
            case Task::Priority::Low:
                return hoursLeft <= 8;  // Remind if within 8 hours
        }
        return false;
    }

    void Reset()
    {
        std::cout << "Refreshing ReminderManager" << std::endl;

        // Clear old and refresh with new reminders
        tasksToRemind_.clear();
        const auto allTasks = taskManager_.getAllTasks();
        for (const auto& task : allTasks) {
            if (ShouldRemind(task, lastCheck_)) {
                tasksToRemind_.push_back(task);
            }
        }
    }

    void Update() {
        // Check every minute
        auto now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck_).count() < INTERVAL_SEC)
            return;

        lastCheck_ = now;
        Reset();
    }
};
