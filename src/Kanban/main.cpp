#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "UserManager.h"
#include "TaskManager.h"
#include "Task.h"
#include "WindowResizeHandler.h"
#include "Board/Board.h"
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

    Task task4(4, "test4",
               "Review and approve pending code changes in the repository",
               fewDays, Task::Priority::Medium);

    Task task5(5, "test5",
               "Review and approve pending code changes in the repository",
               tomorrow, Task::Priority::Medium);

    Task task6(6, "test6",
               "Review and approve pending code changes in the repository",
               fewDays, Task::Priority::Medium);

    Task task7(7, "test7",
               "Review and approve pending code changes in the repository",
               fewDays, Task::Priority::Low);

    Task task8(8, "test8",
               "Review and approve pending code changes in the repository",
               tomorrow, Task::Priority::High);

    if (taskManager.addTask(task1)) {
        std::cout << "✓ Added: " << task1.getName() << " (Priority: "
        << Task::priorityToString(task1.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task2)) {
        std::cout << "✓ Added: " << task2.getName() << " (Priority: "
        << Task::priorityToString(task2.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task3)) {
        std::cout << "✓ Added: " << task3.getName() << " (Priority: "
        << Task::priorityToString(task3.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task4)) {
        std::cout << "✓ Added: " << task4.getName() << " (Priority: "
        << Task::priorityToString(task4.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task5)) {
        std::cout << "✓ Added: " << task5.getName() << " (Priority: "
        << Task::priorityToString(task5.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task6)) {
        std::cout << "✓ Added: " << task6.getName() << " (Priority: "
        << Task::priorityToString(task6.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task7)) {
        std::cout << "✓ Added: " << task7.getName() << " (Priority: "
        << Task::priorityToString(task7.getPriority()) << ")" << std::endl;
    }

    if (taskManager.addTask(task8)) {
        std::cout << "✓ Added: " << task8.getName() << " (Priority: "
        << Task::priorityToString(task8.getPriority()) << ")" << std::endl;
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

int main()
{
    UserManager userManager("sfml_users.db");
    if (!userManager.initialize())
        throw runtime_error("Failed to initialize UserManager!");

    TaskManager taskManager("sfml_tasks.db");
    if (!taskManager.initialize())
        throw runtime_error("Failed to initialize TaskManager!");

    User currentUser = SetupUser(taskManager, userManager);

    sf::Clock clock;
    auto defaultWindowSize = Utilities::defaultWindowSize;
    sf::RenderWindow window(sf::VideoMode(defaultWindowSize.x, defaultWindowSize.y), Utilities::windowTitle, Utilities::windowStyle);
    window.setFramerateLimit(60);

    WindowResizeHandler windowResizeHandler;
    ReminderManager reminderManager(taskManager);
    Board board(window, taskManager, reminderManager, windowResizeHandler);

    board.AddColumn("todo");
    board.AddColumn("wip");
    board.AddColumn("done");

    while (window.isOpen())
    {
        sf::Event event{};
        sf::Time elapsedTime = clock.restart();

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
                    board.CheckCollision(pixelPos, window);
                }
            }
            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    board.ProcessLeftClickReleased();
                }
            }
            if (event.type == sf::Event::MouseMoved)
            {
                board.ProcessMouseMove(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), window);
            }
            if (event.type == sf::Event::TextEntered)
            {
                // 0-126 so that we skip DEL char
                if (event.text.unicode < 127)
                {
                    char c = static_cast<char>(event.text.unicode);
                    board.ReadUserInput(c);
                }
            }
        }

        window.clear(sf::Color::Black);

        // update systems
        reminderManager.Update();

        // update UI/GUI
        board.Update(window, elapsedTime.asSeconds());

        // draw
        board.Draw(window);

        window.display();
    }

    userManager.clearAllUsers();

    return 0;
}