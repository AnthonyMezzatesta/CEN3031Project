#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "UserManager.h"
#include "TaskManager.h"
#include "Task.h"
#include "Board/Board.h"
#include "WindowPrompt/WindowPromptManager.h"
#include "../ReminderManager/ReminderManager.h"

using namespace std;
using namespace Kanban;

// taken from TaskManager/main.cpp
void CreateTasks(TaskManager& taskManager)
{
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto fewDays = now + std::chrono::hours(24 * 3);
    auto nextWeek = now + std::chrono::hours(24 * 7);
    auto lastWeek = now - std::chrono::hours(24 * 7);

    Task task1(1, "Complete project proposal",
               "Write and submit the final project proposal for the new initiative",
               tomorrow, Task::Priority::High);

    Task task2(2, "Team meeting preparation",
               "Prepare presentation slides and agenda for next team meeting",
               nextWeek, Task::Priority::Medium);

    Task task3(3, "Submit expense report",
               "Submit the quarterly expense report to accounting",
               lastWeek, Task::Priority::Low);

    Task task4(4, "Code review",
               "Review and approve pending code changes in the repository",
               fewDays, Task::Priority::Medium);

    if (taskManager.addTask(task1)) {
        std::cout << "✓ Added: " << task1.getName() << " (Priority: "
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task2)) {
        std::cout << "✓ Added: " << task2.getName() << " (Priority: "
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task3)) {
        std::cout << "✓ Added: " << task3.getName() << " (Priority: "
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task4)) {
        std::cout << "✓ Added: " << task4.getName() << " (Priority: "
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }
}

User SetupUser(TaskManager& taskManager, UserManager& userManager)
{
    string usr = "SFML_Test_User";
    string pass = "password";

    // Authenticate user
    bool newRegister = userManager.registerUser(usr, pass);

    userManager.loginUser(usr, pass);

    auto currentUser = userManager.getCurrentUser();
    if (!currentUser.has_value())
        throw runtime_error("Failed to login user");

    taskManager.setCurrentUser(userManager.getCurrentUserId());

    // create tasks for current user
    if (newRegister)
        CreateTasks(taskManager);

    return currentUser.value();
}

int main() {
    UserManager userManager("sfml_users.db");
    if (!userManager.initialize())
        throw runtime_error("Failed to initialize UserManager!");

    TaskManager taskManager("sfml_tasks.db");
    if (!taskManager.initialize())
        throw runtime_error("Failed to initialize TaskManager!");

    User currentUser = SetupUser(taskManager, userManager);

    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML window");
    window.setFramerateLimit(60);

    ReminderManager reminderManager(taskManager);
    Board board(window, taskManager, reminderManager);

    board.AddColumn("todo");
    board.AddColumn("wip");
    board.AddColumn("done");

    // TaskDetailsPrompt taskDetailsPrompt(window);

    TaskDetailsPrompt* taskDetailsPrompt = dynamic_cast<TaskDetailsPrompt*>(
        board.windowPromptManager_->GetPrompt(WindowPrompt::Type::TaskDetailsPrompt));

    Task* selectedTask = nullptr; // Add this variable at the top of main()

    // Add this immediately after creating the board (for testing)
    board.Update(); // Make sure prompts are updated once before first draw

    while (window.isOpen()) {
        sf::Event event{};
        sf::Time elapsedTime = clock.restart(); // Move this outside event polling

        // Process all pending events
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);

                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);

                    if (taskDetailsPrompt->isVisible) {
                        // Check if click is inside the prompt
                        sf::Vector2f mousePos = sf::Vector2f(static_cast<float>(pixelPos.x), 
                                                            static_cast<float>(pixelPos.y));
                        
                        // Convert bounds to screen coordinates for proper check
                        sf::FloatRect bounds = taskDetailsPrompt->bg_.getGlobalBounds();
                        
                        if (!bounds.contains(mousePos)) {
                            // Click outside - close prompt
                            taskDetailsPrompt->isVisible = false;
                            taskDetailsPrompt->editMode_ = false;
                            taskDetailsPrompt->isActive = false;
                            std::cout << "Clicked outside, closing prompt" << std::endl;
                        } else {
                            // Click inside - handle normally
                            taskDetailsPrompt->HandleClick(pixelPos);
                        }
                    } else {
                        // Try to get a task at the clicked position
                        auto clickedTask = board.GetTaskAtPosition(pixelPos, window);
                        if (clickedTask.has_value()) {
                            selectedTask = new Task(clickedTask.value()); // Store selected task
                            taskDetailsPrompt->SetTask(clickedTask.value());
                            taskDetailsPrompt->isVisible = true;
                            std::cout << "Task selected: " << clickedTask.value().getName() << std::endl;
                        } else {
                            selectedTask = nullptr; // Clear selection
                            board.CheckCollision(pixelPos, window);
                        }
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Right) {
                    auto clickedTask = board.GetTaskAtPosition(pixelPos, window);
                    if (clickedTask.has_value()) {
                        selectedTask = new Task(clickedTask.value());
                        taskDetailsPrompt->SetTask(clickedTask.value());
                        taskDetailsPrompt->isVisible = true;
                        taskDetailsPrompt->editMode_ = true;
                        
                        // Initialize input fields
                        taskDetailsPrompt->nameInput_ = clickedTask.value().getName();
                        taskDetailsPrompt->descriptionInput_ = clickedTask.value().getDescription();
                        taskDetailsPrompt->deadlineInput_ = clickedTask.value().getDeadline();
                        taskDetailsPrompt->priorityInput_ = clickedTask.value().getPriority();
                        taskDetailsPrompt->deadlineInputStr_ = "";
                        taskDetailsPrompt->currentDateSegment_ = TaskDetailsPrompt::DATE_YEAR; // Reset to year
                        
                        std::cout << "Right-clicked task - entering edit mode" << std::endl;
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                // Add detailed debug info but remove from final version
                std::cout << "Key pressed: " << event.key.code << std::endl;
                
                // Enter edit mode with E key (when task details visible and not in edit mode)
                if (taskDetailsPrompt->isVisible && !taskDetailsPrompt->editMode_ && 
                    event.key.code == sf::Keyboard::E) {
                    std::cout << "E key pressed - entering edit mode" << std::endl;
                    taskDetailsPrompt->editMode_ = true;
                    taskDetailsPrompt->nameInput_ = taskDetailsPrompt->task_.getName();
                    taskDetailsPrompt->descriptionInput_ = taskDetailsPrompt->task_.getDescription();
                    taskDetailsPrompt->deadlineInput_ = taskDetailsPrompt->task_.getDeadline();
                    taskDetailsPrompt->priorityInput_ = taskDetailsPrompt->task_.getPriority();
                    taskDetailsPrompt->deadlineInputStr_ = "";
                    taskDetailsPrompt->currentDateSegment_ = TaskDetailsPrompt::DATE_YEAR; // Reset to year
                }
                // Save changes with Enter key (when in edit mode)
                else if (taskDetailsPrompt->isVisible && taskDetailsPrompt->editMode_ && 
                         event.key.code == sf::Keyboard::Return) {  // Use Return instead of Enter
                    std::cout << "Enter pressed - saving changes" << std::endl;
                    taskDetailsPrompt->SaveEdits(taskManager);
                    taskDetailsPrompt->editMode_ = false;
                    
                    // Force update the board to reflect changes
                    board.Update();
                }
                // Priority field arrow key handling
                else if (taskDetailsPrompt->isVisible && taskDetailsPrompt->editMode_ && 
                         taskDetailsPrompt->currentField_ == TaskDetailsPrompt::FIELD_PRIORITY) {
                    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Right) {
                        // Cycle up through priorities
                        switch (taskDetailsPrompt->priorityInput_) {
                            case Task::Priority::None:
                                taskDetailsPrompt->priorityInput_ = Task::Priority::Low;
                                break;
                            case Task::Priority::Low:
                                taskDetailsPrompt->priorityInput_ = Task::Priority::Medium;
                                break;
                            case Task::Priority::Medium:
                                taskDetailsPrompt->priorityInput_ = Task::Priority::High;
                                break;
                            case Task::Priority::High: // Wrap around
                                taskDetailsPrompt->priorityInput_ = Task::Priority::None;
                                break;
                        }
                        std::cout << "Priority changed to: " << 
                            Task::priorityToString(taskDetailsPrompt->priorityInput_) << std::endl;
                    } 
                    else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::Left) {
                        // Cycle down through priorities
                        switch (taskDetailsPrompt->priorityInput_) {
                            case Task::Priority::None:
                                taskDetailsPrompt->priorityInput_ = Task::Priority::High;
                                break;
                            case Task::Priority::Low:
                                taskDetailsPrompt->priorityInput_ = Task::Priority::None;
                                break;
                            case Task::Priority::Medium:
                                taskDetailsPrompt->priorityInput_ = Task::Priority::Low;
                                break;
                            case Task::Priority::High:
                                taskDetailsPrompt->priorityInput_ = Task::Priority::Medium;
                                break;
                        }
                        std::cout << "Priority changed to: " << 
                            Task::priorityToString(taskDetailsPrompt->priorityInput_) << std::endl;
                    }
                }
                // Date field arrow key handling
                else if (taskDetailsPrompt->isVisible && taskDetailsPrompt->editMode_ && 
                         taskDetailsPrompt->currentField_ == TaskDetailsPrompt::FIELD_DEADLINE) {
                    
                    // Use left/right arrows to select date segment
                    if (event.key.code == sf::Keyboard::Left) {
                        // Move to previous segment (with wrap-around)
                        taskDetailsPrompt->currentDateSegment_ = static_cast<TaskDetailsPrompt::DateSegment>(
                            (taskDetailsPrompt->currentDateSegment_ + TaskDetailsPrompt::DATE_SEGMENTS_COUNT - 1) 
                            % TaskDetailsPrompt::DATE_SEGMENTS_COUNT
                        );
                        std::cout << "Selected date segment: " << taskDetailsPrompt->currentDateSegment_ << std::endl;
                    }
                    else if (event.key.code == sf::Keyboard::Right) {
                        // Move to next segment (with wrap-around)
                        taskDetailsPrompt->currentDateSegment_ = static_cast<TaskDetailsPrompt::DateSegment>(
                            (taskDetailsPrompt->currentDateSegment_ + 1) % TaskDetailsPrompt::DATE_SEGMENTS_COUNT
                        );
                        std::cout << "Selected date segment: " << taskDetailsPrompt->currentDateSegment_ << std::endl;
                    }
                    
                    // Use up/down arrows to change selected segment value
                    else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
                        // Get current time point
                        auto timePoint = taskDetailsPrompt->deadlineInput_;
                        std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
                        std::tm* tm = std::localtime(&time);
                        
                        // Determine increment direction
                        int direction = (event.key.code == sf::Keyboard::Up) ? 1 : -1;
                        
                        // Modify the appropriate field
                        switch (taskDetailsPrompt->currentDateSegment_) {
                            case TaskDetailsPrompt::DATE_YEAR:
                                tm->tm_year += direction; // Years since 1900
                                break;
                            case TaskDetailsPrompt::DATE_MONTH:
                                tm->tm_mon = (tm->tm_mon + direction + 12) % 12; // 0-11
                                break;
                            case TaskDetailsPrompt::DATE_DAY:
                                // Simple day increment, actual month length not checked
                                tm->tm_mday = ((tm->tm_mday - 1 + direction + 31) % 31) + 1; // 1-31
                                break;
                            case TaskDetailsPrompt::DATE_HOUR:
                                tm->tm_hour = (tm->tm_hour + direction + 24) % 24; // 0-23
                                break;
                            case TaskDetailsPrompt::DATE_MINUTE:
                                tm->tm_min = (tm->tm_min + direction + 60) % 60; // 0-59
                                break;
                            case TaskDetailsPrompt::DATE_SECOND:
                                tm->tm_sec = (tm->tm_sec + direction + 60) % 60; // 0-59
                                break;
                        }
                        
                        // Convert back to time_point
                        std::time_t newTime = std::mktime(tm);
                        taskDetailsPrompt->deadlineInput_ = std::chrono::system_clock::from_time_t(newTime);
                        
                        // Update the string representation
                        taskDetailsPrompt->deadlineInputStr_ = Task::formatDeadline(taskDetailsPrompt->deadlineInput_);
                        std::cout << "Updated date: " << taskDetailsPrompt->deadlineInputStr_ << std::endl;
                    }
                }
                // Tab navigation between fields
                else if (taskDetailsPrompt->isVisible && taskDetailsPrompt->editMode_ && event.key.code == sf::Keyboard::Tab) {
                    // Cycle through fields
                    switch (taskDetailsPrompt->currentField_) {
                        case TaskDetailsPrompt::FIELD_NONE:
                            taskDetailsPrompt->currentField_ = TaskDetailsPrompt::FIELD_NAME;
                            break;
                        case TaskDetailsPrompt::FIELD_NAME:
                            taskDetailsPrompt->currentField_ = TaskDetailsPrompt::FIELD_DESCRIPTION;
                            break;
                        case TaskDetailsPrompt::FIELD_DESCRIPTION:
                            taskDetailsPrompt->currentField_ = TaskDetailsPrompt::FIELD_DEADLINE;
                            break;
                        case TaskDetailsPrompt::FIELD_DEADLINE:
                            taskDetailsPrompt->currentField_ = TaskDetailsPrompt::FIELD_PRIORITY;
                            break;
                        case TaskDetailsPrompt::FIELD_PRIORITY:
                            taskDetailsPrompt->currentField_ = TaskDetailsPrompt::FIELD_NAME; // Back to first field
                            break;
                    }
                    std::cout << "Tabbed to field: " << taskDetailsPrompt->currentField_ << std::endl;
                }
                else {
                    board.ProcessKeyEvent(event.key.code, elapsedTime.asSeconds());
                }
            }

            if (event.type == sf::Event::TextEntered) {
                // Handle text input for task editing
                if (taskDetailsPrompt->isVisible && taskDetailsPrompt->editMode_) {
                    if (event.text.unicode < 128) { // ASCII characters only
                        char inputChar = static_cast<char>(event.text.unicode);
                        
                        // Process based on which field is selected
                        switch (taskDetailsPrompt->currentField_) {
                            case TaskDetailsPrompt::FIELD_NAME:
                                if (event.text.unicode == 8) { // Backspace
                                    if (!taskDetailsPrompt->nameInput_.empty())
                                        taskDetailsPrompt->nameInput_.pop_back();
                                } else if (inputChar >= 32) { // Printable characters
                                    taskDetailsPrompt->nameInput_ += inputChar;
                                }
                                break;
                                
                            case TaskDetailsPrompt::FIELD_DESCRIPTION:
                                if (event.text.unicode == 8) { // Backspace
                                    if (!taskDetailsPrompt->descriptionInput_.empty())
                                        taskDetailsPrompt->descriptionInput_.pop_back();
                                } else if (inputChar >= 32) { // Printable characters
                                    taskDetailsPrompt->descriptionInput_ += inputChar;
                                }
                                break;
                
                            // Handle deadline field
                            case TaskDetailsPrompt::FIELD_DEADLINE:
                            {
                                // Initialize deadlineStr from the current value if it's empty
                                if (taskDetailsPrompt->deadlineInputStr_.empty()) {
                                    taskDetailsPrompt->deadlineInputStr_ = Task::formatDeadline(taskDetailsPrompt->deadlineInput_);
                                }
                                
                                if (event.text.unicode == 8) { // Backspace
                                    if (!taskDetailsPrompt->deadlineInputStr_.empty())
                                        taskDetailsPrompt->deadlineInputStr_.pop_back();
                                } else if ((inputChar >= '0' && inputChar <= '9') || 
                                          inputChar == '-' || inputChar == ' ' || inputChar == ':') {
                                    taskDetailsPrompt->deadlineInputStr_ += inputChar;
                                }
                                
                                // Try to parse back to a time_point
                                try {
                                    taskDetailsPrompt->deadlineInput_ = Task::parseDeadline(taskDetailsPrompt->deadlineInputStr_);
                                } catch (...) {
                                    // Keep the old value if parsing fails
                                }
                                break;
                            }
                
                            // We'll handle FIELD_PRIORITY with arrow keys, not text input
                        }
                    }
                } else {
                    // Existing text entry logic for column names, etc.
                    if (event.text.unicode < 127) {
                        char c = static_cast<char>(event.text.unicode);
                        board.ReadUserInput(c);
                    }
                }
            }
        }

        // Update and draw outside the event loop
        board.Update();

        // Add this outside the event loop, right before the drawing code
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5) && taskDetailsPrompt->isVisible) {
            std::cout << "F5 - TOGGLE EDIT MODE" << std::endl;
            taskDetailsPrompt->editMode_ = !taskDetailsPrompt->editMode_;
            if (taskDetailsPrompt->editMode_) {
                taskDetailsPrompt->nameInput_ = taskDetailsPrompt->task_.getName();
                taskDetailsPrompt->descriptionInput_ = taskDetailsPrompt->task_.getDescription();
                taskDetailsPrompt->deadlineInput_ = taskDetailsPrompt->task_.getDeadline();
                taskDetailsPrompt->priorityInput_ = taskDetailsPrompt->task_.getPriority();
            }
        }

        window.clear(sf::Color::Black);
        board.DrawBoard(window);
        window.setView(window.getDefaultView());
        board.windowPromptManager_->Draw(window);
        taskDetailsPrompt->Draw(window);
        window.display();
    }

    return 0;
}