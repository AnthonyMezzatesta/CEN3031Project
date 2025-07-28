#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "../GUIElement/ScrollBar.h"
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
            AddColumnButton() : GUIElement(Utilities::fill1), plusIcon(Icon::Type::plus) {}
        protected:
            void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
            {
                // icon pos is based on top left of icon, excluding icon bg...
                // float offset = size.x / 2.0f - Icon::GetDefaultWidth() / 2.0f;
                plusIcon.Draw(basePos.x, basePos.y, target);
            }
        } addColumnButton_;

        enum TaskStatus { Taken, Available };
        std::unordered_map<int, TaskStatus> taskIds_;

        const static int colPerScreen = 3;
        int colWidth_;
        int colHeight_;
        int colPaddingX_;
        int colPosY_;

        const static int BOARD_SCROLL_SPEED = 1000;
        float scrollMoveDelta_ = 0; // [0, 1]
        bool scrollBarActive = false;
        ScrollBar scrollBar_;

        sf::RenderTexture renderTexture_;
        sf::View boardView;

        WindowPromptManager* windowPromptManager_;
        TaskManager* taskManager_;
        Column* activeColumn;
        vector<Kanban::Column*> columns_;

        void UpdateColumnValues(const sf::RenderWindow& window);
        void UpdateRenderTexture(const sf::RenderWindow& window);
        void UpdateBoardView(const sf::RenderWindow& window, const float deltaTime);
        void UpdateScrollBar(const sf::RenderWindow& window, const float deltaTime);
        void DrawColumns(sf::RenderWindow& window);
        void DrawScrollBar(sf::RenderWindow& window);
    public:
        enum UserInputMode { Default, ColumnName, MoveTask, ScrollBar };

        Board(const sf::RenderWindow& target, TaskManager& taskManager, ReminderManager& reminderManager);
        ~Board();

        void AddColumn(const string& name);
        void RemoveColumn(Column& column);
        void SetActiveColumn(Column* column, UserInputMode mode = Default);
        void SetTaskAsTaken(Task& task);
        void ReturnTask(std::optional<int> id);
        vector<Task> GetAvailableTasks() const;

        void ProcessLeftClickReleased();

        void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target);
        void ProcessKeyEvent(sf::Keyboard::Key key);
        void ReadUserInput(char c);
        void Update(const sf::RenderWindow& target, const float deltaTime);
        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target);
        void Draw(sf::RenderWindow& window);
    private:
        UserInputMode userInputMode;
        string userInputStr;
    };
}