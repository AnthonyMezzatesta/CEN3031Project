#include <SFML/Graphics.hpp>
#include <string>
#include "Utilities.h"
#include "../Board/Board.h"
#include "TaskDetailsPrompt.h"
using namespace std;

TaskDetailsPrompt::TaskDetailsPrompt(const sf::RenderWindow& target)
{
    type_ = WindowPrompt::Type::SettingsPrompt;
    view_ = target.getDefaultView();
    view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
    bg_.setFillColor(bgColor_);

    // sf::Font font;
    // if (!font.loadFromFile(Utilities::fontPath))
    //     throw std::runtime_error("could not load font");
    // text_.setFont(font);
}

void TaskDetailsPrompt::Update()
{
    if (isActive) isVisible = true;
}

void TaskDetailsPrompt::Deactivate()
{
    isActive = false;
    isVisible = false;
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

// Function created to allow clicking of the edit button
void TaskDetailsPrompt::HandleClick(sf::Vector2i mousePos)
{
    // Convert pixel coordinates to view coordinates
    sf::Vector2f mouse = sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    
    std::cout << "Click at: " << mouse.x << "," << mouse.y << std::endl;

    // First check if the click is outside the prompt bg
    if (!bg_.getGlobalBounds().contains(mouse)) {
        std::cout << "Click outside prompt - closing" << std::endl;
        isVisible = false;
        editMode_ = false;
        return;
    }
    
    // Draw the button coordinates for debugging
    if (!editMode_) {
        std::cout << "Edit button at: 100,300 to 180,330" << std::endl;
    } else {
        std::cout << "Save button at: 100,300 to 180,330" << std::endl;
        std::cout << "Cancel button at: 200,300 to 280,330" << std::endl;
    }

    // Check button presses with a little more tolerance
    const float CLICK_TOLERANCE = 10.0f;
    
    if (editMode_) {
        // Save button - use explicit coordinates for better debugging
        if (mouse.x >= 100-CLICK_TOLERANCE && mouse.x <= 180+CLICK_TOLERANCE && 
            mouse.y >= 300-CLICK_TOLERANCE && mouse.y <= 330+CLICK_TOLERANCE) {
            std::cout << "Save button clicked!" << std::endl;
            editMode_ = false;
            return;
        }
        
        // Cancel button
        if (mouse.x >= 200-CLICK_TOLERANCE && mouse.x <= 280+CLICK_TOLERANCE && 
            mouse.y >= 300-CLICK_TOLERANCE && mouse.y <= 330+CLICK_TOLERANCE) {
            std::cout << "Cancel button clicked!" << std::endl;
            isVisible = false;
            editMode_ = false;
            return;
        }
    } 
    else {
        // Edit button - use explicit coordinates for better debugging
        if (mouse.x >= 100-CLICK_TOLERANCE && mouse.x <= 180+CLICK_TOLERANCE && 
            mouse.y >= 300-CLICK_TOLERANCE && mouse.y <= 330+CLICK_TOLERANCE) {
            std::cout << "Edit button clicked!" << std::endl;
            editMode_ = true;
            nameInput_ = task_.getName();
            descriptionInput_ = task_.getDescription();
            deadlineInput_ = task_.getDeadline();
            priorityInput_ = task_.getPriority();
            return;
        }
    }
    
    // Click outside the prompt closes it
    if (!bg_.getGlobalBounds().contains(mouse)) {
        isVisible = false;
        editMode_ = false;
        return;
    }

    if (editMode_) {
        // Name field box
        sf::FloatRect nameBox(100, 100, 300, 30);
        if (nameBox.contains(mouse)) {
            currentField_ = FIELD_NAME;
            return;
        }
        
        // Description field box
        sf::FloatRect descBox(100, 140, 300, 60);
        if (descBox.contains(mouse)) {
            currentField_ = FIELD_DESCRIPTION;
            return;
        }
        
        // Deadline field box
        sf::FloatRect deadlineBox(100, 210, 200, 30);
        if (deadlineBox.contains(mouse)) {
            currentField_ = FIELD_DEADLINE;
            return;
        }
        
        // Priority field box
        sf::FloatRect priorityBox(100, 250, 150, 30);
        if (priorityBox.contains(mouse)) {
            currentField_ = FIELD_PRIORITY;
            return;
        }
    }
}
// Draws the task details prompt
void TaskDetailsPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    // Save current view
    sf::View originalView = target.getView();
    
    // Set to default view for UI elements
    target.setView(target.getDefaultView());

    // Draw semi-transparent background overlay for modal effect
    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    target.draw(overlay);

    // Draw main background
    bg_.setSize(sf::Vector2f(400, 400));
    bg_.setPosition(100, 50);  // Fixed position for easier interaction
    bg_.setFillColor(sf::Color(220, 220, 220)); // Lighter color
    target.draw(bg_);

    sf::Font font;
    if (!font.loadFromFile(Utilities::fontPath)) {
        // Handle font loading error
        return;
    }

    if (editMode_) {
        // Editable fields

        // Name
        sf::RectangleShape nameBox(sf::Vector2f(300, 30));
        nameBox.setPosition(100, 100);
        nameBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(nameBox);

        sf::Text nameInputText(nameInput_, font, 24);
        nameInputText.setPosition(105, 105);
        nameInputText.setFillColor(sf::Color::Black);
        target.draw(nameInputText);

        // Description
        sf::RectangleShape descBox(sf::Vector2f(300, 60));
        descBox.setPosition(100, 140);
        descBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(descBox);

        sf::Text descInputText(descriptionInput_, font, 24);
        descInputText.setPosition(105, 145);
        descInputText.setFillColor(sf::Color::Black);
        target.draw(descInputText);

        // Deadline
        sf::RectangleShape deadlineBox(sf::Vector2f(200, 30));
        deadlineBox.setPosition(100, 210);
        deadlineBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(deadlineBox);

        // Format deadline and show with highlighting
        std::string deadlineStr = Task::formatDeadline(deadlineInput_);
        sf::Text deadlineInputText(deadlineStr, font, 24);
        deadlineInputText.setPosition(105, 215);
        deadlineInputText.setFillColor(sf::Color::Black);
        target.draw(deadlineInputText);

        // If deadline field is selected, highlight the current segment
        if (currentField_ == FIELD_DEADLINE) {
            // Use a monospace font width approach with better approximations
            std::vector<std::pair<int, int>> segmentPositions = {
                {0, 4},    // Year: 0-3
                {5, 2},    // Month: 5-6
                {8, 2},    // Day: 8-9
                {11, 2},   // Hour: 11-12
                {14, 2},   // Minute: 14-15
                {17, 2}    // Second: 17-18
            };
            
            if (currentDateSegment_ < segmentPositions.size()) {
                auto segment = segmentPositions[currentDateSegment_];
                int start = segment.first;
                int length = segment.second;
                
                // Get the actual text metrics for correct positioning
                std::string fullStr = Task::formatDeadline(deadlineInput_);
                std::string segmentStr = fullStr.substr(start, length);
                sf::Text measureText(fullStr.substr(0, start), font, 24);
                float startX = measureText.getLocalBounds().width;
                
                sf::Text segmentText(segmentStr, font, 24);
                float segmentWidth = segmentText.getLocalBounds().width;
                
                // Create highlight box sized to the actual text
                sf::RectangleShape segmentHighlight(
                    sf::Vector2f(segmentWidth + 6, 30)); // Add padding
                segmentHighlight.setPosition(105 + startX, 210);
                segmentHighlight.setFillColor(sf::Color(150, 150, 255, 128));
                target.draw(segmentHighlight);
            }
        }

        // Priority
        sf::RectangleShape priorityBox(sf::Vector2f(150, 30));
        priorityBox.setPosition(100, 250);
        priorityBox.setFillColor(sf::Color(200, 200, 200));
        target.draw(priorityBox);

        std::string priorityStr = Task::priorityToString(priorityInput_);
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

    } else {
        // Static fields

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

    // Highlight currently selected field
    if (currentField_ == FIELD_NAME) {
        sf::RectangleShape highlight(sf::Vector2f(305, 35));
        highlight.setPosition(98, 98);
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color::Blue);
        highlight.setOutlineThickness(2.0f);
        target.draw(highlight);
    } else if (currentField_ == FIELD_DESCRIPTION) {
        sf::RectangleShape highlight(sf::Vector2f(305, 65));
        highlight.setPosition(98, 138);
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color::Blue);
        highlight.setOutlineThickness(2.0f);
        target.draw(highlight);
    } else if (currentField_ == FIELD_DEADLINE) {
        sf::RectangleShape highlight(sf::Vector2f(205, 35));
        highlight.setPosition(98, 208);
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color::Blue);
        highlight.setOutlineThickness(2.0f);
        target.draw(highlight);
    } else if (currentField_ == FIELD_PRIORITY) {
        sf::RectangleShape highlight(sf::Vector2f(155, 35));
        highlight.setPosition(98, 248);
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color::Blue);
        highlight.setOutlineThickness(2.0f);
        target.draw(highlight);
    }

    // For debugging - draw status text
    if (font.loadFromFile(Utilities::fontPath)) {
        sf::Text statusText;
        statusText.setFont(font);
        statusText.setCharacterSize(16);
        statusText.setPosition(105, 55);
        statusText.setString(editMode_ ? "EDIT MODE" : "VIEW MODE");
        statusText.setFillColor(sf::Color::Red);
        target.draw(statusText);
    }
    
    // Restore original view
    target.setView(originalView);
}

 void TaskDetailsPrompt::SaveEdits(TaskManager& taskManager) {
     if (editMode_) {
         std::cout << "Saving task: " << nameInput_ << std::endl;
         task_.setName(nameInput_);
         task_.setDescription(descriptionInput_);
         task_.setDeadline(deadlineInput_);
         task_.setPriority(priorityInput_);
         
         // Debug the task object before saving
         std::cout << "Task ID: " << (task_.getId().has_value() ? std::to_string(task_.getId().value()) : "none") << std::endl;
         
         if (task_.getId().has_value()) {
             bool result = taskManager.updateTask(task_);
             std::cout << "Task update result: " << (result ? "success" : "failed") << std::endl;
         }
     }
 }