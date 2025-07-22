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
#include "../../ReminderManager/ReminderManager.h"

class WindowPromptManager;
using std::vector;
using std::string;

namespace Kanban
{
    class Column;

    class Board
    {
        class AddColumnButton final : public GUIElement
        {
            Icon plusIcon;
        public:
            AddColumnButton() : GUIElement(sf::Color(112, 112, 112, 255)), plusIcon(Icon::Type::plus) {}
        protected:
            void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
            {
                // icon pos is based on top left of icon, excluding icon bg...
                // float offset = size.x / 2.0f - Icon::GetDefaultWidth() / 2.0f;
                plusIcon.Draw(basePos.x, basePos.y, target);
            }
        };
        AddColumnButton addColumnButton_;

        const static int colPerScreen = 3;

        enum UserInputMode { Default, ColumnName };
        UserInputMode userInputMode;
        string userInputStr;

        sf::View boardView;

        WindowPromptManager* windowPromptManager_;
        TaskManager* taskManager_;
        Column* activeColumn;
        vector<Kanban::Column*> columns_;

        enum TaskStatus { Taken, Available };
        std::unordered_map<int, TaskStatus> taskIds_;

        void DrawColumns(sf::RenderWindow& window);
        void MoveView(sf::Keyboard::Key key, const float deltaTime);
    public:
        Board(const sf::RenderWindow& target, TaskManager& taskManager, ReminderManager& reminderManager);
        ~Board();
        void AddColumn(const string& name);
        void RemoveColumn(Column& column);
        void SetActiveColumn(Column* column);

        void Update();
        void SetTaskAsTaken(Task& task); // for use by columns
        void ReturnTask(std::optional<int> id);
        vector<Task> GetAvailableTasks() const;

        void ProcessKeyEvent(sf::Keyboard::Key key, const float deltaTime);
        void ReadUserInput(char c);
        void Draw(sf::RenderWindow& window);
        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target);
    };
}