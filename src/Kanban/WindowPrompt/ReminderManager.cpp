#include "ReminderManager.h"
#include "Task.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// Initialize task manager and the last check time
ReminderManager::ReminderManager(TaskManager& tm)
    : taskManager(tm), lastCheck(std::chrono::system_clock::now()) {}

bool ReminderManager::ShouldRemind(const Task& task, std::chrono::system_clock::time_point now) {
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

void ReminderManager::Update() {
    auto now = std::chrono::system_clock::now();
    // Check every minute
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() < 60)
        return;
    lastCheck = now;

    // Clear the old reminders
    tasksToRemind.clear();
    for (const auto& task : taskManager.getAllTasks()) {
        if (ShouldRemind(task, now)) {
            tasksToRemind.push_back(&task);
        }
    }
}

// Simple window for drawing any reminders
void ReminderManager::Draw(sf::RenderWindow& window) {
    sf::Font font;
    font.loadFromFile("AovelSansRounded-rdDL.tff"); 
    int y = 10;
    for (const Task* task : tasksToRemind) {
        sf::Text text;
        text.setFont(font);
        text.setString("Reminder: " + task->getName());
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Yellow);
        text.setPosition(10, y);
        window.draw(text);
        y += 30;
    }
}