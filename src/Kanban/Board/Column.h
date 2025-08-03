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
#include "../GUIElement/ScrollableTexture.h"

using std::string;
using std::runtime_error;
using std::cout;
using std::endl;

namespace Kanban
{
    class Board;
    using namespace EventSystem;

    class Column final : private EventSystem::TaskSubject
    {
        const static int tasksPerColummn_ = 6;
        string name_;
        TaskCard* selectedCard_;
        std::vector<TaskCard*> tasks_;

        sf::Transform transformStatic_;
        sf::Transform transformDynamic_;
        sf::RenderTexture textureStatic_;
        sf::RectangleShape bgRect_;

        std::vector<Icon*> icons_;
        sf::Font font_;
        sf::Text text_;
        Board* board_;
        WindowPromptManager* windowPromptManager_;

        float headerHeight_;
        float taskWidth_;
        float taskOffsetX_;
        float taskHeight_;
        float taskPaddingY_;
        unsigned int columnHeight_;
        float defaultTextureHeight_;

        ScrollableTexture scrollTexture_;

        struct ActionObserver : public DataObserver<Observer::ActionEnum>
        {
            Column* column_;
            void OnNotify(Observer::EventEnum event, const Observer::ActionEnum& action) override;
            ActionObserver(Column* column) : column_(column) {}
        } actionObserver_;
        struct TaskObserver : public DataObserver<Task>
        {
            Column* column_;
            void OnNotify(Observer::EventEnum event, const Task& tasks) override;
            TaskObserver(Column* column) : column_(column) {}
        } taskObserver_;
        friend ActionObserver;
        friend TaskObserver;

        void UpdateValues(float width, float height);
        void UpdateRenderTexture(const float width);
        void UpdateScrollTexture(const float width, const float deltaTime);
        void RenderStaticDetails(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target);
        void RenderDynamicDetails(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target);
        void ClearSelectedTask();
    public:
        Column(const string& name, WindowPromptManager& windowPromptManager, Kanban::Board* board);
        ~Column();

        // bool AddTask(Task& task);
        // bool RemoveTask(Kanban::TaskCard& task);
        void RemoveTaskCard(Kanban::TaskCard* card);
        // void ShowAddTaskPrompt();
        void SelectIcon(Icon::Type type);
        void SelectTaskCard(Kanban::TaskCard* card);

        void ProcessLeftClickReleased();
        void ProcessMouseMove(sf::Vector2f mousePosGlobal);
        void Update(const float screenWidth, const float columnWidth, const float columnHeight, const float deltaTime);

        bool CheckCollision(sf::Vector2f point);
        void RenderIcons(sf::RenderTarget& target, sf::Vector2f basePos, int colWidth);
        void Render(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target);

        string GetName() { return name_; }
        void SetName(string name) { name_ = std::move(name); }
    };
}