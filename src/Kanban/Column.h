#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Icon.h"
#include "TaskCard.h"
#include "../../include/Task.h"
#include "WindowPromptManager.h"

using std::string;
using std::runtime_error;
using std::cout;
using std::endl;

class Column
{
    // int taskLimit;
    string name_;
    sf::Vector2f size_;
    std::vector<Kanban::TaskCard*> tasks_;
    // sf::Sprite sprite_;
    // sf::RenderTexture renderTexture_;
    sf::RectangleShape rect_;
    std::vector<Icon*> icons_;

    WindowPromptManager* windowPromptManager;
public:
    Column(const string& name, const float width, const float height,
        WindowPromptManager* windowPromptManager);
    ~Column();

    bool AddTask(Task& task);
    bool RemoveTask(Kanban::TaskCard& task);
    void ShowAddTaskPrompt();
    void SelectIcon(Icon::Types type);
    void SelectTask(Kanban::TaskCard* task);

    bool CheckCollision(sf::Vector2f point);
    void RenderIcons(sf::RenderTarget& target, sf::Vector2f basePos);
    void Render(sf::Vector2f position, sf::RenderTarget& target);

    string GetName() { return name_; }
    void SetName(string name) { name_ = name; }
    // sf::RenderTexture& GetRenderTexture() { return renderTexture_; }
};
