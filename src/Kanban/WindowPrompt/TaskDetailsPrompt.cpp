#include <SFML/Graphics.hpp>
#include <string>
#include "Utilities.h"
#include "../Board/Board.h"
#include "TaskDetailsPrompt.h"
#include "TaskEdit.h"
using namespace std;

TaskDetailsPrompt::TaskDetailsPrompt(const sf::RenderWindow& target, TaskManager& taskManager)
    : taskManager_(&taskManager)
{
    type_ = WindowPrompt::Type::TaskDetailsPrompt;
    view_ = target.getDefaultView();
    bg_.setFillColor(bgColor_);

    // view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
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
    auto localPos = transform_.getInverse().transformPoint(mousePos);

    // exit early if not visible
    if (!isVisible || !bg_.getGlobalBounds().contains(localPos))
    {
        // if point was outside of bounds...
        if (isActive && isVisible)
            Deactivate();

        return false;
    }

    HandleClick(localPos);

    return true;
}

void TaskDetailsPrompt::HandleClick(sf::Vector2f localPos)
{
    // const float CLICK_TOLERANCE = 10.0f;
    
    if (IsEditMode()) {
        // Save button
        if (saveButton.getGlobalBounds().contains(localPos))
        {
            bool saved = SaveEdits();
            if (saved) {
                taskEditor_.ExitEditMode();
            }
            return;
        }
        
        // Cancel button
        if (cancelButton.getGlobalBounds().contains(localPos))
        {
            Deactivate();
            return;
        }

        // Field selection
        if (nameBox.getGlobalBounds().contains(localPos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_NAME);
            return;
        }
        
        if (descBox.getGlobalBounds().contains(localPos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_DESCRIPTION);
            return;
        }

        if (deadlineBox.getGlobalBounds().contains(localPos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_DEADLINE);
            return;
        }
        
        if (priorityBox.getGlobalBounds().contains(localPos)) {
            taskEditor_.SetCurrentField(TaskEdit::FIELD_PRIORITY);
            return;
        }
    } 
    else {
        // Edit button
        if (editButton.getGlobalBounds().contains(localPos))
            EnterEditMode();
        }
}

void TaskDetailsPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    const auto size = static_cast<sf::Vector2f>(target.getSize());

    // Draw semi-transparent background overlay for modal effect
    sf::RectangleShape overlay(size);
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    target.draw(overlay);

    // Set to default view for UI elements
    target.setView(view_);

    sf::Vector2f bgSize(size.x * 0.6f, size.y * 0.9f);

    transform_ = sf::Transform::Identity; // reset
    transform_.translate((size.x - bgSize.x)/2.f, (size.y - bgSize.y)/2.f);

    // Draw main background
    bg_.setSize(bgSize);
    bg_.setFillColor(sf::Color(220, 220, 220));
    target.draw(bg_, transform_);

    // ========= remapping old values to new output range so that they scale with new window resolutions =========

    // original settings
    // size: 400 x 400
    // pos offset: (100, 50)

    sf::Text text24("", font_, bgSize.y / 24);
    sf::Text text20("", font_, bgSize.y / 32);

    if (IsEditMode()) {
        // Draw editable fields using TaskEdit data

        // Name field
        nameBox.setSize(sf::Vector2f(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 300),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 30))
        );
        nameBox.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 100-50));
        nameBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(nameBox, transform_);

        text24.setString(GetNameInput());
        text24.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 105-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 105-50)
        );
        target.draw(text24, transform_);

        // Description field
        descBox.setSize(sf::Vector2f(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 300),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 60))
        );
        descBox.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 140-50));
        descBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(descBox, transform_);

        text24.setString(GetDescriptionInput());
        text24.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 105-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 145-50)
        );
        target.draw(text24, transform_);

        // Deadline field
        deadlineBox.setSize(sf::Vector2f(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 200),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 30)
        ));
        deadlineBox.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 210-50));
        deadlineBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(deadlineBox, transform_);

        std::string deadlineStr = Task::formatDeadline(GetDeadlineInput());
        text24.setString(deadlineStr);
        text24.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 105-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 215-50)
        );
        target.draw(text24, transform_);

        // Priority field
        priorityBox.setSize(sf::Vector2f(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 150),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 30))
        );
        priorityBox.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 250-50));
        priorityBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(priorityBox, transform_);

        std::string priorityStr = Task::priorityToString(GetPriorityInput());
        text24.setString(priorityStr);
        text24.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 105-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 255-50)
        );
        target.draw(text24, transform_);

        // Save button
        saveButton.setSize(sf::Vector2f(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 80),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 30)
        ));
        saveButton.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 300-50));
        saveButton.setFillColor(sf::Color(100, 200, 100));
        target.draw(saveButton, transform_);

        text20.setString("Save");
        text20.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 110-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 305-50)
        );
        target.draw(text20, transform_);

        // Cancel button
        cancelButton.setSize(sf::Vector2f(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 80),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 30)
        ));
        cancelButton.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 200-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 300-50)
        );
        cancelButton.setFillColor(sf::Color(200, 100, 100));
        target.draw(cancelButton, transform_);

        text20.setString("Cancel");
        text20.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 210-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 305-50)
        );
        target.draw(text20, transform_);

        // Highlight currently selected field
        sf::RectangleShape highlight;
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color::Blue);

        switch (GetCurrentField()) {
            case TaskEdit::FIELD_NAME:
                highlight.setSize(sf::Vector2f(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 305),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 35))
                );
                highlight.setPosition(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 98-100),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 98-50)
                );
                highlight.setOutlineThickness(nameBox.getSize().y / 16);
                break;
            case TaskEdit::FIELD_DESCRIPTION:
                highlight.setSize(sf::Vector2f(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 305),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 65))
                );
                highlight.setPosition(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 98-100),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 138-50)
                );
                highlight.setOutlineThickness(descBox.getSize().y / 16);
                break;
            case TaskEdit::FIELD_DEADLINE:
                highlight.setSize(sf::Vector2f(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 205),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 35))
                );
                highlight.setPosition(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 98-100),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 208-50)
                );
                highlight.setOutlineThickness(deadlineBox.getSize().y / 16);
                break;
            case TaskEdit::FIELD_PRIORITY:
                highlight.setSize(sf::Vector2f(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 155),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 35))
                );
                highlight.setPosition(
                    Utilities::RemapClamped(0, 400, 0, bgSize.x, 98-100),
                    Utilities::RemapClamped(0, 400, 0, bgSize.y, 248-50)
                );
                highlight.setOutlineThickness(priorityBox.getSize().y / 16);
                break;
            case TaskEdit::FIELD_NONE:
                break;
        }
        target.draw(highlight, transform_);

    } else {
        // Draw static fields
        
        text24.setString(task_.getName());
        text24.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 100-50));
        target.draw(text24, transform_);

        text24.setString(task_.getDescription());
        text24.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 140-50));
        target.draw(text24, transform_);

        text24.setString(task_.getDeadlineString());
        text24.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 210-50));
        target.draw(text24, transform_);

        text24.setString(Task::priorityToString(task_.getPriority()));
        text24.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 250-50));
        target.draw(text24, transform_);

        // Edit button
        editButton.setSize(sf::Vector2f(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 80),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 30))
        );
        editButton.setPosition(0, Utilities::RemapClamped(0, 400, 0, bgSize.y, 300-50));
        editButton.setFillColor(sf::Color(100, 100, 200));
        target.draw(editButton, transform_);

        text20.setString("Edit");
        text20.setPosition(
            Utilities::RemapClamped(0, 400, 0, bgSize.x, 110-100),
            Utilities::RemapClamped(0, 400, 0, bgSize.y, 305-50)
        );
        target.draw(text20, transform_);
    }

    // Status text
    sf::Text statusText("", font_, bgSize.y / 48);
    statusText.setPosition(
        Utilities::RemapClamped(0, 400, 0, bgSize.x, 105-100),
        Utilities::RemapClamped(0, 400, 0, bgSize.y, 55-50)
    );
    statusText.setString(IsEditMode() ? "EDIT MODE" : "VIEW MODE");
    statusText.setFillColor(sf::Color::Red);
    target.draw(statusText, transform_);
    
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
            ExitEditMode();
    }

    // Handle other key presses in edit mode
    HandleKeyPress(key);
}
