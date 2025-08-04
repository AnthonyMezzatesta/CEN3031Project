#pragma once
#include <SFML/Graphics.hpp>
#include "../Utilities/EventSystem/Subject.h"
#include "WindowPrompt.h"
#include "../Board/Board.h"
#include "TaskEdit.h"

class TaskDetailsPrompt final : public WindowPrompt, public EventSystem::TaskSubject
{
    sf::Color bgColor_ = sf::Color(128, 128, 128, 255);
    Task task_;
    TaskEdit taskEditor_; // Use TaskEdit for editing functionality
    TaskManager* taskManager_; // Add TaskManager reference

    sf::RectangleShape bg_;
    sf::Transform transform_;

    sf::RectangleShape nameBox;
    sf::RectangleShape descBox;
    sf::RectangleShape deadlineBox;
    sf::RectangleShape priorityBox;

    sf::RectangleShape saveButton;
    sf::RectangleShape cancelButton;
    sf::RectangleShape editButton;

public:
    TaskDetailsPrompt(const sf::RenderWindow& target, TaskManager& taskManager);
    ~TaskDetailsPrompt() {}

    void SetTask(const Task& task);
    void Update() override;
    void Deactivate();
    void SetTaskManager(TaskManager* taskManager) { taskManager_ = taskManager; } // Add setter

    void HandleClick(sf::Vector2f localPos);

    // Expose TaskEdit functionality
    void EnterEditMode();
    void ExitEditMode();
    bool IsEditMode() const { return taskEditor_.IsEditMode(); }
    
    void HandleTextInput(char inputChar) { taskEditor_.HandleTextInput(inputChar); }
    void HandleKeyPress(sf::Keyboard::Key key) { taskEditor_.HandleKeyPress(key); }
    void HandleBackspace() { taskEditor_.HandleBackspace(); }
    bool SaveEdits(); // Remove TaskManager parameter

    // Getters for drawing
    TaskEdit::EditField GetCurrentField() const { return taskEditor_.GetCurrentField(); }
    TaskEdit::DateSegment GetCurrentDateSegment() const { return taskEditor_.GetCurrentDateSegment(); }
    
    const std::string& GetNameInput() const { return taskEditor_.GetNameInput(); }
    const std::string& GetDescriptionInput() const { return taskEditor_.GetDescriptionInput(); }
    const std::chrono::system_clock::time_point& GetDeadlineInput() const { return taskEditor_.GetDeadlineInput(); }
    Task::Priority GetPriorityInput() const { return taskEditor_.GetPriorityInput(); }

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;
    void Draw(sf::RenderTarget& target) override;
    void ReadUserInput(char c) override;
    void ProcessKeyEvent(const sf::Keyboard::Key key) override;
};