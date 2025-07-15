#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Task.h"
#include "Observer.h"
#include "../GUIElement/Icon.h"
#include "../GUIElement/TaskCard.h"
#include "../WindowPrompt/WindowPromptManager.h"
#include "../Utilities/EventSystem/Subject.h"
#include "Board.h"

using std::string;
using std::runtime_error;
using std::cout;
using std::endl;

namespace Kanban
{
    using namespace EventSystem;

    class Column final : private EventSystem::TaskSubject
    {
        // int taskLimit;
        string name_;
        sf::Vector2f size_;
        TaskCard* selectedCard_;
        std::vector<Kanban::TaskCard*> tasks_;
        sf::RectangleShape rect_;
        std::vector<Icon*> icons_;
        sf::Font font_;
        sf::Text text_;
        Kanban::Board* board_;
        WindowPromptManager* windowPromptManager_;
        const static int tasksPerColummn_ = 5;

        struct ActionObserver : public DataObserver<Observer::ActionEnum>
        {
            Column* column_;
            void OnNotify(Observer::EventEnum event, Observer::ActionEnum& action) override;
            ActionObserver(Column* column) : column_(column) {}
        };
        struct TaskObserver : public DataObserver<Task>
        {
            Column* column_;
            void OnNotify(Observer::EventEnum event, Task& tasks) override;
            TaskObserver(Column* column) : column_(column) {}
        };
        friend ActionObserver;
        friend TaskObserver;
        ActionObserver actionObserver_;
        TaskObserver taskObserver_;
    public:
        Column(const string& name, const float width, const float height,
            WindowPromptManager& windowPromptManager, Kanban::Board& board);
        ~Column();

        // bool AddTask(Task& task);
        // bool RemoveTask(Kanban::TaskCard& task);
        void RemoveTaskCard(Kanban::TaskCard* card);
        // void ShowAddTaskPrompt();
        void SelectIcon(Icon::Type type);
        void SelectTaskCard(Kanban::TaskCard* card);

        bool CheckCollision(sf::Vector2f point);
        void RenderIcons(sf::RenderTarget& target, sf::Vector2f basePos);
        void Render(sf::Vector2f position, sf::RenderTarget& target, int tasksPerColumn = tasksPerColummn_);

        string GetName() { return name_; }
        void SetName(string name) { name_ = std::move(name); }
        // sf::RenderTexture& GetRenderTexture() { return renderTexture_; }
    };
}