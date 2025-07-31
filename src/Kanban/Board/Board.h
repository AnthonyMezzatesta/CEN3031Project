#pragma once
#include <iostream>
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
#include "../GUIElement/ScrollableTexture.h"

class WindowPromptManager;
using std::vector;
using std::string;

namespace Kanban
{
    class Column;

    class Board
    {
        struct ReminderIconObserver final : public EventSystem::BasicObserver
        {
            Icon* icon_;
            ReminderIconObserver(Icon* icon = nullptr) : icon_(icon) {}
        protected:
            void OnNotify(EventSystem::Observer::EventEnum event) override
            {
                if (!icon_)
                    return;

                if (event == EventSystem::Observer::Activate)
                    icon_->ToggleSecondLayer(true);
                else if (event == EventSystem::Observer::Deactivate)
                    icon_->ToggleSecondLayer(false);
            }
        } reminderIconObserver_;
        friend ReminderIconObserver;

        class AddColumnButton final : public GUIElement
        {
            Icon plusIcon;
        public:
            AddColumnButton() : GUIElement(Utilities::fill2), plusIcon(Icon::Type::plus) {}
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

        ScrollableTexture scrollTexture_;
        sf::View boardView;

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
        void ReadUserInput(char c);
        void Update(const sf::RenderWindow& target, const float deltaTime);
        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target);
        void Draw(sf::RenderWindow& window);
    private:
        UserInputMode userInputMode;
        string userInputStr;
    };
}