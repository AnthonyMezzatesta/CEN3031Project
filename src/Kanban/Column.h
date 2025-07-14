#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Task.h"
#include "Icon.h"
#include "TaskCard.h"
#include "WindowPromptManager.h"
#include "../Utilities/EventSystem/Subject.h"
#include "../Utilities/EventSystem/Observer.h"

using std::string;
using std::runtime_error;
using std::cout;
using std::endl;

class Column final
{
    // int taskLimit;
    string name_;
    sf::Vector2f size_;
    std::vector<Kanban::TaskCard*> tasks_;
    // sf::Sprite sprite_;
    // sf::RenderTexture renderTexture_;
    sf::RectangleShape rect_;
    std::vector<Icon*> icons_;
    sf::Font font_;
    sf::Text text_;
    const static int tasksPerColummn_ = 5;

    struct ColumnPromptTaskSubject : public EventSystem::ColumnPromptTaskSubject
    {
        friend Column;
    };
    struct ColumnPromptConfigSubject : public EventSystem::ColumnPromptConfigSubject
    {
        friend Column;
    };
    struct ActionObserver : public EventSystem::ActionObserver
    {
        Column* column_;
        void OnNotify(Event event, Action action) override;
        ActionObserver(Column* column) : column_(column) {}
    };
    struct TaskObserver : public EventSystem::TaskObserver
    {
        Column* column_;
        void OnNotify(Event event, vector<Task>& tasks) override;
        TaskObserver(Column* column) : column_(column) {}
    };
    ActionObserver actionObserver_;
    TaskObserver taskObserver_;
    ColumnPromptTaskSubject columnPromptTaskSubject_;
    ColumnPromptConfigSubject columnPromptConfigSubject_;
public:
    Column(const string& name, const float width, const float height,
        WindowPromptManager& windowPromptManager);
    ~Column();

    bool AddTask(Task& task);
    bool RemoveTask(Kanban::TaskCard& task);
    void ShowAddTaskPrompt();
    void SelectIcon(Icon::Type type);
    void SelectTask(Kanban::TaskCard* task);

    bool CheckCollision(sf::Vector2f point);
    void RenderIcons(sf::RenderTarget& target, sf::Vector2f basePos);
    void Render(sf::Vector2f position, sf::RenderTarget& target, int tasksPerColumn = tasksPerColummn_);

    string GetName() { return name_; }
    void SetName(string name) { name_ = std::move(name); }
    // sf::RenderTexture& GetRenderTexture() { return renderTexture_; }
};
