#include <cmath>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include "Board.h"
#include "../../include/TaskManager.h"
#include "../../include/Task.h"
#include "../Utilities/Utilities.h"
#include "unordered_map"
#include "WindowPromptManager.h"
#include "WindowPrompt.h"

using namespace std;
using namespace Kanban;

// taken from TaskManager/main.cpp
void CreateTasks(TaskManager& taskManager)
{
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto nextWeek = now + std::chrono::hours(24 * 7);
    auto lastWeek = now - std::chrono::hours(24 * 7);

    Task task1("Complete project proposal",
               "Write and submit the final project proposal for the new initiative",
               tomorrow);

    Task task2("Team meeting preparation",
               "Prepare presentation slides and agenda for next team meeting",
               nextWeek);

    Task task3("Submit expense report",
               "Submit the quarterly expense report to accounting",
               lastWeek);

    auto fewDays = now + std::chrono::hours(24 * 3);
    Task task4("Code review",
               "Review and approve pending code changes in the repository",
               fewDays);

    if (taskManager.addTask(task1)) {
        std::cout << "✓ Added: " << task1.getName() << std::endl;
    }

    if (taskManager.addTask(task2)) {
        std::cout << "✓ Added: " << task2.getName() << std::endl;
    }

    if (taskManager.addTask(task3)) {
        std::cout << "✓ Added: " << task3.getName() << std::endl;
    }

    if (taskManager.addTask(task4)) {
        std::cout << "✓ Added: " << task4.getName() << std::endl;
    }
}

int main() {

    TaskManager taskManager("sfml_tasks.db");
    if (!taskManager.initialize())
        throw runtime_error("Failed to initialize TaskManager!");
    taskManager.clearAllTasks();
    CreateTasks(taskManager);
    auto allTasks = taskManager.getAllTasks();
    queue<Task> q;
    for (Task& task : allTasks)
        q.push(task);

    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML window");
    window.setFramerateLimit(60);

    WindowPromptManager windowPromptManager(window);
    windowPromptManager.AddPrompt(WindowPrompt::PromptType::AddTask);
    sf::FloatRect windowPromptViewPort(0.5f, 0.f, 0.5f, 0.5f);

    Kanban::Board board(window);
    board.AddColumn("todo", window, windowPromptManager);
    board.AddColumn("wip", window, windowPromptManager);
    // board.AddColumn("done", window);
    // board.AddColumn("done", window);


    // https://en.sfml-dev.org/forums/index.php?topic=18190.0
    // use getPosition in real-time method for the mouse

    // a 'view' defines which part of the 2D world is seen in the window
    // a 'viewport' defines where the 'view' is shown
    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event)) {
            sf::Time elapsedTime = clock.restart();

            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    bool a = board.CheckCollision(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), window);
                    // cout << std::boolalpha << a << endl;

                }
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    if (!q.empty())
                    {
                        board.AddTaskToColumn("todo", q.front());
                        q.pop();
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed)
            {
                board.MoveView(event.key.code, elapsedTime.asSeconds());
            }
        }

        window.clear(sf::Color::Black);

        board.DrawBoard(window);

        windowPromptManager.Draw(window, windowPromptViewPort);

        window.display();
    }

    return 0;
}