#pragma once
#include <cmath>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include "Board.h"
#include "../../include/TaskManager.h"
#include "../../include/Task.h"
#include "../Utilities/Utilities.h"
#include "unordered_map"
#include "WindowPrompt.h"

class WindowPromptManager
{
    sf::View view_;
public:
    WindowPromptManager(const sf::RenderWindow& target) : view_(target.getDefaultView())
    {}
    ~WindowPromptManager()
    {
        for (auto& kvp : prompts_)
            delete kvp.second;
    }
    void AddPrompt(WindowPrompt::PromptType type)
    {
        switch (type)
        {
        case WindowPrompt::AddTask:
            prompts_.emplace(type, new AddTaskWindowPrompt());
            break;
        default:
            break;
        }
    }
    void ShowPrompt(WindowPrompt::PromptType type)
    {
        if (prompts_.find(type) != prompts_.end())
        {
            prompts_[type]->ToggleVisibility(true);
        }
    }
    bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
    {
        auto mousePos = target.mapPixelToCoords(point, view_);
        for (auto& kvp : prompts_)
        {
            if (kvp.second->CheckCollision(mousePos))
                return true;
        }
        return false;
    }
    void Draw(sf::RenderWindow& target, sf::FloatRect viewport)
    {
        view_.setViewport(viewport);
        target.setView(view_);

        for (auto& kvp : prompts_)
            kvp.second->Draw(target);

        target.setView(target.getDefaultView());
    }
private:
    std::unordered_map<WindowPrompt::PromptType, WindowPrompt*> prompts_;
};