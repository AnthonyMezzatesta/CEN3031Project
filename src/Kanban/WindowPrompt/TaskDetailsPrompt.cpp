#include <SFML/Graphics.hpp>
#include <string>
#include "Utilities.h"
#include "../Board/Board.h"
#include "TaskDetailsPrompt.h"
#include "TaskEdit.h"
using namespace std;

TaskDetailsPrompt::TaskDetailsPrompt(const sf::RenderWindow& target) 
    : taskManager_(nullptr), board_(nullptr) // Initialize both pointers
{
    type_ = WindowPrompt::Type::TaskDetailsPrompt;
    view_ = target.getDefaultView();
    view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
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

    return true;
}

void TaskDetailsPrompt::HandleClick(sf::Vector2i mousePos)
{
    sf::Vector2f mouse = sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // First check if the click is outside the prompt bg
    if (!bg_.getGlobalBounds().contains(mouse)) {
        isVisible = false;
        taskEditor_.ExitEditMode();
        return;
    }
    
    const float CLICK_TOLERANCE = 10.0f;
    
    if (IsEditMode()) {
        // Save button
        if (mouse.x >= 100-CLICK_TOLERANCE && mouse.x <= 180+CLICK_TOLERANCE && 
            mouse.y >= 300-CLICK_TOLERANCE && mouse.y <= 330+CLICK_TOLERANCE) {
            bool saved = SaveEdits();
            if (saved) {
                taskEditor_.ExitEditMode();
            }
            return;
        }
        
        // Cancel button
        if (mouse.x >= 200-CLICK_TOLERANCE && mouse.x <= 280+CLICK_TOLERANCE && 
            mouse.y >= 300-CLICK_TOLERANCE && mouse.y <= 330+CLICK_TOLERANCE) {
            isVisible = false;
            taskEditor_.ExitEditMode();
            return;
        }

        // Field selection
        sf::FloatRect nameBox(100, 100, 300, 30);
        if (nameBox.contains(mouse)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_NAME);
            return;
        }
        
        sf::FloatRect descBox(100, 140, 300, 60);
        if (descBox.contains(mouse)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_DESCRIPTION);
            return;
        }
        
        sf::FloatRect deadlineBox(100, 210, 200, 30);
        if (deadlineBox.contains(mouse)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_DEADLINE);
            return;
        }
        
        sf::FloatRect priorityBox(100, 250, 150, 30);
        if (priorityBox.contains(mouse)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_PRIORITY);
            return;
        }
    } 
    else {
        // Edit button
        if (mouse.x >= 100-CLICK_TOLERANCE && mouse.x <= 180+CLICK_TOLERANCE && 
            mouse.y >= 300-CLICK_TOLERANCE && mouse.y <= 330+CLICK_TOLERANCE) {
            EnterEditMode();
            return;
        }
    }
}

void TaskDetailsPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    // Save current view
    sf::View originalView = target.getView();
    
    // Set to default view for UI elements
    target.setView(target.getDefaultView());

    // Draw semi-transparent background overlay for modal effect
    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    target.draw(overlay);

    // Draw main background
    bg_.setSize(sf::Vector2f(400, 400));
    bg_.setPosition(100, 50);
    bg_.setFillColor(sf::Color(220, 220, 220));
    target.draw(bg_);

    sf::Font font;
    if (!font.loadFromFile(Utilities::fontPath)) {
        target.setView(originalView);
        return;
    }

    if (IsEditMode()) {
        // Draw editable fields using TaskEdit data
        
        // Name field
        sf::RectangleShape nameBox(sf::Vector2f(300, 30));
        nameBox.setPosition(100, 100);
        nameBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(nameBox);

        sf::Text nameInputText(GetNameInput(), font, 24);
        nameInputText.setPosition(105, 105);
        nameInputText.setFillColor(sf::Color::Black);
        target.draw(nameInputText);

        // Description field
        sf::RectangleShape descBox(sf::Vector2f(300, 60));
        descBox.setPosition(100, 140);
        descBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(descBox);

        sf::Text descInputText(GetDescriptionInput(), font, 24);
        descInputText.setPosition(105, 145);
        descInputText.setFillColor(sf::Color::Black);
        target.draw(descInputText);

        // Deadline field
        sf::RectangleShape deadlineBox(sf::Vector2f(200, 30));
        deadlineBox.setPosition(100, 210);
        deadlineBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(deadlineBox);

        std::string deadlineStr = Task::formatDeadline(GetDeadlineInput());
        sf::Text deadlineInputText(deadlineStr, font, 24);
        deadlineInputText.setPosition(105, 215);
        deadlineInputText.setFillColor(sf::Color::Black);
        target.draw(deadlineInputText);

        // Priority field
        sf::RectangleShape priorityBox(sf::Vector2f(150, 30));
        priorityBox.setPosition(100, 250);
        priorityBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(priorityBox);

        std::string priorityStr = Task::priorityToString(GetPriorityInput());
        sf::Text priorityInputText(priorityStr, font, 24);
        priorityInputText.setPosition(105, 255);
        priorityInputText.setFillColor(sf::Color::Black);
        target.draw(priorityInputText);

        // Save button
        sf::RectangleShape saveButton(sf::Vector2f(80, 30));
        saveButton.setPosition(100, 300);
        saveButton.setFillColor(sf::Color(100, 200, 100));
        target.draw(saveButton);

        sf::Text saveText("Save", font, 20);
        saveText.setPosition(110, 305);
        saveText.setFillColor(sf::Color::Black);
        target.draw(saveText);

        // Cancel button
        sf::RectangleShape cancelButton(sf::Vector2f(80, 30));
        cancelButton.setPosition(200, 300);
        cancelButton.setFillColor(sf::Color(200, 100, 100));
        target.draw(cancelButton);

        sf::Text cancelText("Cancel", font, 20);
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
        
        sf::Text nameStaticText(task_.getName(), font, 24);
        nameStaticText.setPosition(100, 100);
        target.draw(nameStaticText);

        sf::Text descStaticText(task_.getDescription(), font, 24);
        descStaticText.setPosition(100, 140);
        target.draw(descStaticText);

        sf::Text deadlineStaticText(Task::formatDeadline(task_.getDeadline()), font, 24);
        deadlineStaticText.setPosition(100, 210);
        target.draw(deadlineStaticText);

        sf::Text priorityStaticText(Task::priorityToString(task_.getPriority()), font, 24);
        priorityStaticText.setPosition(100, 250);
        target.draw(priorityStaticText);

        // Edit button
        sf::RectangleShape editButton(sf::Vector2f(80, 30));
        editButton.setPosition(100, 300);
        editButton.setFillColor(sf::Color(100, 100, 200));
        target.draw(editButton);

        sf::Text editText("Edit", font, 20);
        editText.setPosition(110, 305);
        editText.setFillColor(sf::Color::Black);
        target.draw(editText);
    }

    // Status text
    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setPosition(105, 55);
    statusText.setString(IsEditMode() ? "EDIT MODE" : "VIEW MODE");
    statusText.setFillColor(sf::Color::Red);
    target.draw(statusText);
    
    // Restore original view
    target.setView(originalView);
}