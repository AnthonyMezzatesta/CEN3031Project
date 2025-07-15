#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "TaskManager.h"
#include "Task.h"
#include "Board/Board.h"
#include "WindowPrompt/WindowPromptManager.h"

using namespace std;
using namespace Kanban;

// taken from TaskManager/main.cpp
void CreateTasks(TaskManager& taskManager)
{
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto nextWeek = now + std::chrono::hours(24 * 7);
    auto lastWeek = now - std::chrono::hours(24 * 7);

    Task task1(1, "Complete project proposal",
               "Write and submit the final project proposal for the new initiative",
               tomorrow);

    Task task2(2, "Team meeting preparation",
               "Prepare presentation slides and agenda for next team meeting",
               nextWeek);

    Task task3(3, "Submit expense report",
               "Submit the quarterly expense report to accounting",
               lastWeek);

    auto fewDays = now + std::chrono::hours(24 * 3);
    Task task4(4, "Code review",
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

    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML window");
    window.setFramerateLimit(60);

    Board board(window, taskManager);
    WindowPromptManager windowPromptManager(window, board);

    board.AddColumn("todo", window, windowPromptManager);
    board.AddColumn("wip", window, windowPromptManager);
    board.AddColumn("done", window, windowPromptManager);

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
                    sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
                    if (!windowPromptManager.CheckCollision(pixelPos, window))
                        board.CheckCollision(pixelPos, window);
                }
            }
            if (event.type == sf::Event::KeyPressed)
            {
                board.MoveView(event.key.code, elapsedTime.asSeconds());
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

        // update
        board.Update();
        windowPromptManager.UpdatePrompts();

        // draw
        board.DrawBoard(window);
        windowPromptManager.Draw(window);

        window.display();
    }

    return 0;
}