#include <SFML/Graphics.hpp>
#include <string>
#include "Utilities.h"
#include "../Board/Board.h"
#include "TaskDetailsPrompt.h"
#include "TaskEdit.h"
using namespace std;

TaskDetailsPrompt::TaskDetailsPrompt(const sf::RenderWindow& target, TaskManager& taskManager, Kanban::Board& board)
    : taskManager_(&taskManager), board_(&board)
{
    type_ = WindowPrompt::Type::TaskDetailsPrompt;
    view_ = target.getDefaultView();
    // view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
    bg_.setFillColor(bgColor_);
}

// Added new SaveEdits method
bool TaskDetailsPrompt::SaveEdits() {
    if (!taskManager_) {
        return false;
    }
    
    bool result = taskEditor_.SaveChanges(*taskManager_);
    if (result) {
        // Update our local copy with the saved values
        task_.setName(taskEditor_.GetNameInput());
        task_.setDescription(taskEditor_.GetDescriptionInput());
        task_.setDeadline(taskEditor_.GetDeadlineInput());
        task_.setPriority(taskEditor_.GetPriorityInput());
        
        // Refresh all TaskCards to prevent segfaults
        if (board_) {
            board_->RefreshTaskCards();
        }
    }
    return result;
}

void TaskDetailsPrompt::SetTask(const Task& task) {
    task_ = task;
    taskEditor_.SetTask(&task_);
}

void TaskDetailsPrompt::EnterEditMode() {
    taskEditor_.EnterEditMode();
}

void TaskDetailsPrompt::ExitEditMode() {
    taskEditor_.ExitEditMode();
}

void TaskDetailsPrompt::Update()
{
    if (isActive) isVisible = true;
}

void TaskDetailsPrompt::Deactivate()
{
    isActive = false;
    isVisible = false;
    taskEditor_.ExitEditMode();
    RemoveAllObservers();
}

bool TaskDetailsPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point)
{
    auto mousePos = target.mapPixelToCoords(point, view_);

    // exit early if not visible
    if (!isVisible || !bg_.getGlobalBounds().contains(mousePos))
    {
        // if point was outside of bounds...
        if (isActive && isVisible)
            Deactivate();

        return false;
    }

    HandleClick(mousePos);

    return true;
}

void TaskDetailsPrompt::HandleClick(sf::Vector2f mousePos)
{
    const float CLICK_TOLERANCE = 10.0f;
    
    if (IsEditMode()) {
        // Save button
        if (mousePos.x >= 100-CLICK_TOLERANCE && mousePos.x <= 180+CLICK_TOLERANCE &&
            mousePos.y >= 300-CLICK_TOLERANCE && mousePos.y <= 330+CLICK_TOLERANCE) {
            bool saved = SaveEdits();
            if (saved) {
                taskEditor_.ExitEditMode();
            }
            return;
        }
        
        // Cancel button
        if (mousePos.x >= 200-CLICK_TOLERANCE && mousePos.x <= 280+CLICK_TOLERANCE &&
            mousePos.y >= 300-CLICK_TOLERANCE && mousePos.y <= 330+CLICK_TOLERANCE) {
            isVisible = false;
            taskEditor_.ExitEditMode();
            return;
        }

        // Field selection
        sf::FloatRect nameBox(100, 100, 300, 30);
        if (nameBox.contains(mousePos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_NAME);
            return;
        }
        
        sf::FloatRect descBox(100, 140, 300, 60);
        if (descBox.contains(mousePos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_DESCRIPTION);
            return;
        }
        
        sf::FloatRect deadlineBox(100, 210, 200, 30);
        if (deadlineBox.contains(mousePos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_DEADLINE);
            return;
        }
        
        sf::FloatRect priorityBox(100, 250, 150, 30);
        if (priorityBox.contains(mousePos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_PRIORITY);
            return;
        }
    } 
    else {
        // Edit button
        if (mousePos.x >= 100-CLICK_TOLERANCE && mousePos.x <= 180+CLICK_TOLERANCE &&
            mousePos.y >= 300-CLICK_TOLERANCE && mousePos.y <= 330+CLICK_TOLERANCE) {
            EnterEditMode();
        }
    }
}

void TaskDetailsPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    // Set to default view for UI elements
    target.setView(view_);

    // Draw semi-transparent background overlay for modal effect
    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    target.draw(overlay);

    // Draw main background
    bg_.setSize(sf::Vector2f(400, 400));
    bg_.setPosition(100, 50);
    bg_.setFillColor(sf::Color(220, 220, 220));
    target.draw(bg_);

    if (IsEditMode()) {
        // Draw editable fields using TaskEdit data
        
        // Name field
        sf::RectangleShape nameBox(sf::Vector2f(300, 30));
        nameBox.setPosition(100, 100);
        nameBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(nameBox);

        sf::Text nameInputText(GetNameInput(), font_, 24);
        nameInputText.setPosition(105, 105);
        nameInputText.setFillColor(sf::Color::Black);
        target.draw(nameInputText);

        // Description field
        sf::RectangleShape descBox(sf::Vector2f(300, 60));
        descBox.setPosition(100, 140);
        descBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(descBox);

        sf::Text descInputText(GetDescriptionInput(), font_, 24);
        descInputText.setPosition(105, 145);
        descInputText.setFillColor(sf::Color::Black);
        target.draw(descInputText);

        // Deadline field
        sf::RectangleShape deadlineBox(sf::Vector2f(200, 30));
        deadlineBox.setPosition(100, 210);
        deadlineBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(deadlineBox);

        std::string deadlineStr = Task::formatDeadline(GetDeadlineInput());
        sf::Text deadlineInputText(deadlineStr, font_, 24);
        deadlineInputText.setPosition(105, 215);
        deadlineInputText.setFillColor(sf::Color::Black);
        target.draw(deadlineInputText);

        // Priority field
        sf::RectangleShape priorityBox(sf::Vector2f(150, 30));
        priorityBox.setPosition(100, 250);
        priorityBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(priorityBox);

        std::string priorityStr = Task::priorityToString(GetPriorityInput());
        sf::Text priorityInputText(priorityStr, font_, 24);
        priorityInputText.setPosition(105, 255);
        priorityInputText.setFillColor(sf::Color::Black);
        target.draw(priorityInputText);

        // Save button
        sf::RectangleShape saveButton(sf::Vector2f(80, 30));
        saveButton.setPosition(100, 300);
        saveButton.setFillColor(sf::Color(100, 200, 100));
        target.draw(saveButton);

        sf::Text saveText("Save", font_, 20);
        saveText.setPosition(110, 305);
        saveText.setFillColor(sf::Color::Black);
        target.draw(saveText);

        // Cancel button
        sf::RectangleShape cancelButton(sf::Vector2f(80, 30));
        cancelButton.setPosition(200, 300);
        cancelButton.setFillColor(sf::Color(200, 100, 100));
        target.draw(cancelButton);

        sf::Text cancelText("Cancel", font_, 20);
        cancelText.setPosition(210, 305);
        cancelText.setFillColor(sf::Color::Black);
        target.draw(cancelText);

        // Highlight currently selected field
        sf::RectangleShape highlight;
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color::Blue);
        highlight.setOutlineThickness(2.0f);

        switch (GetCurrentField()) {
            case TaskEdit::FIELD_NAME:
                highlight.setSize(sf::Vector2f(305, 35));
                highlight.setPosition(98, 98);
                target.draw(highlight);
                break;
            case TaskEdit::FIELD_DESCRIPTION:
                highlight.setSize(sf::Vector2f(305, 65));
                highlight.setPosition(98, 138);
                target.draw(highlight);
                break;
            case TaskEdit::FIELD_DEADLINE:
                highlight.setSize(sf::Vector2f(205, 35));
                highlight.setPosition(98, 208);
                target.draw(highlight);
                break;
            case TaskEdit::FIELD_PRIORITY:
                highlight.setSize(sf::Vector2f(155, 35));
                highlight.setPosition(98, 248);
                target.draw(highlight);
                break;
            case TaskEdit::FIELD_NONE:
                break;
        }

    } else {
        // Draw static fields
        
        sf::Text nameStaticText(task_.getName(), font_, 24);
        nameStaticText.setPosition(100, 100);
        target.draw(nameStaticText);

        sf::Text descStaticText(task_.getDescription(), font_, 24);
        descStaticText.setPosition(100, 140);
        target.draw(descStaticText);

        sf::Text deadlineStaticText(Task::formatDeadline(task_.getDeadline()), font_, 24);
        deadlineStaticText.setPosition(100, 210);
        target.draw(deadlineStaticText);

        sf::Text priorityStaticText(Task::priorityToString(task_.getPriority()), font_, 24);
        priorityStaticText.setPosition(100, 250);
        target.draw(priorityStaticText);

        // Edit button
        sf::RectangleShape editButton(sf::Vector2f(80, 30));
        editButton.setPosition(100, 300);
        editButton.setFillColor(sf::Color(100, 100, 200));
        target.draw(editButton);

        sf::Text editText("Edit", font_, 20);
        editText.setPosition(110, 305);
        editText.setFillColor(sf::Color::Black);
        target.draw(editText);
    }

    // Status text
    sf::Text statusText;
    statusText.setFont(font_);
    statusText.setCharacterSize(16);
    statusText.setPosition(105, 55);
    statusText.setString(IsEditMode() ? "EDIT MODE" : "VIEW MODE");
    statusText.setFillColor(sf::Color::Red);
    target.draw(statusText);
    
    // Restore original view
    target.setView(target.getDefaultView());
}

void TaskDetailsPrompt::ReadUserInput(char c)
{
    if (!isVisible || !IsEditMode())
        return;

    if (c == '\b')
        HandleBackspace();
    else
        HandleTextInput(c);
}

void TaskDetailsPrompt::ProcessKeyEvent(const sf::Keyboard::Key key)
{
    if (!isVisible)
        return;

    // Enter edit mode with E key
    if (key == sf::Keyboard::E && !IsEditMode())
    {
        EnterEditMode();
        return;
    }

    if (!IsEditMode())
        return;

    // Save changes with Enter key
    if (key == sf::Keyboard::Return)
    {
        if (SaveEdits())
        {
            ExitEditMode();
            // board update ?
        }
    }

    // Handle other key presses in edit mode
    HandleKeyPress(key);
}
