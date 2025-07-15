#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "../../include/Task.h"
#include "../WindowPrompt/WindowPromptManager.h"
#include "TaskManager.h"

struct WindowPromptManager;
using std::vector;
using std::string;

namespace Kanban
{
    class Column;

    class Board
    {
        sf::View boardView;
        const static int colPerScreen = 3;
        vector<Kanban::Column*> columns_;
        TaskManager* taskManager_;
        enum TaskStatus { Taken, Available };
        std::unordered_map<int, TaskStatus> taskIds_;
    public:
        Board(const sf::RenderTarget& target, TaskManager& taskManager);
        ~Board();
        bool AddColumn(const string& name, const sf::RenderTarget& target, WindowPromptManager& windowPromptManager);
        void RemoveColumn(Column& column);
        // bool AddTaskToColumn(string colName, Task& task);
        void MoveView(sf::Keyboard::Key key, const float deltaTime);

        void Update();
        // Task GetTask(int id);
        void SetTaskAsTaken(Task& task); // for use by columns
        void ReturnTask(std::optional<int> id);
        // void ReturnTasks(vector<Task>& tasks); // for use by columns
        vector<Task> GetAvailableTasks() const;

        void DrawBoard(sf::RenderWindow& window);
        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target);
    };
}