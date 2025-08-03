#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "../../include/Task.h"
#include "../WindowPrompt/WindowPromptManager.h"
#include "TaskManager.h"
#include "WindowResizeHandler.h"
#include "../../ReminderManager/ReminderManager.h"
#include "../GUIElement/ScrollableTexture.h"

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
            AddColumnButton() : GUIElement(Utilities::fill2), plusIcon(Icon::Type::plus, 2.0f) {}
        protected:
            void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
            {
                // icon pos is based on top left of icon
                size /= 2.0f;
                float iconCenter = plusIcon.GetWidth() / 2.0f;
                plusIcon.Draw(basePos.x + size.x - iconCenter, basePos.y + size.y - iconCenter, target);
            }
        } addColumnButton_;

        enum TaskStatus { Taken, Available };
        std::unordered_map<int, TaskStatus> taskIds_;

        const static int colPerScreen = 3;
        int colWidth_;
        int colHeight_;
        int colPaddingX_;
        int colPosY_;

        ScrollableTexture scrollTexture_;
        sf::View boardView;
        EventSystem::WindowResizeObserver windowResizeObserver_;

        WindowPromptManager* windowPromptManager_;
        TaskManager* taskManager_;
        Column* activeColumn;
        vector<Kanban::Column*> columns_;
        vector<Icon*> icons_;

        Icon::Type iconArray[2] { Icon::Type::bell, Icon::Type::dots };

        void UpdateColumnValues(const sf::RenderWindow& window);
        void UpdateScrollTexture(const sf::RenderWindow& window, const float deltaTime);
        void DrawColumns(sf::RenderWindow& window);
        void DrawScrollBar(sf::RenderWindow& window);
        void DrawIcons(sf::RenderWindow& window);
    public:
        enum UserInputMode { Default, ColumnName, MoveTask, ScrollBar };

        Board(const sf::RenderWindow& target, TaskManager& taskManager, ReminderManager& reminderManager, WindowResizeHandler& windowResizeHandler);
        ~Board();

        void AddColumn(const string& name);
        void RemoveColumn(Column& column);
        void SetActiveColumn(Column* column, UserInputMode mode = Default);
        void SetTaskAsTaken(const Task& task);
        void ReturnTask(std::optional<int> id);
        vector<Task> GetAvailableTasks() const;

        void ProcessLeftClickReleased();

        void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target);
        void ReadUserInput(char c);
        void Update(const sf::RenderWindow& window, const float deltaTime);
        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target);
        void Draw(sf::RenderWindow& window);
    private:
        UserInputMode userInputMode;
        string userInputStr;
    };
}