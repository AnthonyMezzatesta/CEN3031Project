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

    taskDetailsPrompt->SetTaskManager(&taskManager);
    taskDetailsPrompt->SetBoard(&board);

    Task* selectedTask = nullptr;

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
                            taskDetailsPrompt->ExitEditMode();
                            taskDetailsPrompt->isActive = false;
                        } else {
                            // Click inside - handle normally
                            taskDetailsPrompt->HandleClick(pixelPos);
                        }

                        auto clickedTask = board.GetTaskAtPosition(pixelPos, window);
                        if (clickedTask.has_value()) {
                            selectedTask = new Task(clickedTask.value());
                            taskDetailsPrompt->SetTask(clickedTask.value());
                            taskDetailsPrompt->isVisible = true;
                        } else {
                            selectedTask = nullptr;
                            board.CheckCollision(pixelPos, window);
                        }
                    } else {
                        // Try to get a task at the clicked position
                        auto clickedTask = board.GetTaskAtPosition(pixelPos, window);
                        if (clickedTask.has_value()) {
                            selectedTask = new Task(clickedTask.value()); // Store selected task
                            taskDetailsPrompt->SetTask(clickedTask.value());
                            taskDetailsPrompt->isVisible = true;
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
                        taskDetailsPrompt->EnterEditMode();
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                // Enter edit mode with E key
                if (taskDetailsPrompt->isVisible && !taskDetailsPrompt->IsEditMode() && 
                    event.key.code == sf::Keyboard::E) {
                    taskDetailsPrompt->EnterEditMode();
                }
                // Save changes with Enter key
                else if (taskDetailsPrompt->isVisible && taskDetailsPrompt->IsEditMode() && 
                         event.key.code == sf::Keyboard::Return) {
                    bool saved = taskDetailsPrompt->SaveEdits();
                    if (saved) {
                        taskDetailsPrompt->ExitEditMode();
                        board.Update();
                    }
                }
                // Handle other key presses in edit mode
                else if (taskDetailsPrompt->isVisible && taskDetailsPrompt->IsEditMode()) {
                    taskDetailsPrompt->HandleKeyPress(event.key.code);
                }
                else {
                    board.ProcessKeyEvent(event.key.code, elapsedTime.asSeconds());
                }
            }

            if (event.type == sf::Event::TextEntered) {
                if (taskDetailsPrompt->isVisible && taskDetailsPrompt->IsEditMode()) {
                    if (event.text.unicode == 8) { // Backspace
                        taskDetailsPrompt->HandleBackspace();
                    } else if (event.text.unicode < 128) { // ASCII characters only
                        char inputChar = static_cast<char>(event.text.unicode);
                        taskDetailsPrompt->HandleTextInput(inputChar);
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

        window.clear(sf::Color::Black);
        board.DrawBoard(window);
        window.setView(window.getDefaultView());
        board.windowPromptManager_->Draw(window);
        taskDetailsPrompt->Draw(window);
        window.display();
    }

    return 0;
}

// TODO:
// Task Metrics w/ GUI

//show general statistics of tasks. E.g. task completion rate, percentage of tasks submitted on time, etc.
//and a way to filter which tasks are shown based on given criteria