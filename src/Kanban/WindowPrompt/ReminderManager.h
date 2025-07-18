#pragma once
#include "TaskManager.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <vector>

class ReminderManager {
public:
    ReminderManager(TaskManager& taskManager);

    // Call this periodically to check if any tasks are upcoming
    void Update();

    // Draw the reminder to the window
    void Draw(sf::RenderWindow& window);

private:
    TaskManager& taskManager;
    std::vector<const Task*> tasksToRemind;
    std::chrono::system_clock::time_point lastCheck;

    bool ShouldRemind(const Task& task, std::chrono::system_clock::time_point now);
};