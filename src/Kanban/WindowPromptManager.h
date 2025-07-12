#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include "../Utilities/EventSystem/Observer.h"
#include "TaskManager.h"
#include "WindowPrompt.h"

class WindowPromptManager : public EventSystem::ColumnPromptObserver
{
public:
    WindowPromptManager(const sf::RenderWindow& target, TaskManager& taskManager) : view_(target.getDefaultView())
    {
        prompts_[WindowPrompt::Type::Default] = new DummyPrompt;
        prompts_[WindowPrompt::Type::AddTaskPrompt] = new AddTaskWindowPrompt(taskManager,
                                                                                WindowPrompt::Type::AddTaskPrompt);
        UpdatePrompts();
    }
    ~WindowPromptManager()
    {
        for (auto& kvp : prompts_)
            delete kvp.second;
    }
    WindowPrompt* GetPrompt(const WindowPrompt::Type type)
    {
        if (prompts_.find(type) != prompts_.end())
            return prompts_[type];

        return prompts_[WindowPrompt::Type::Default];
    }
    // void ShowPrompt(WindowPrompt::Type type)
    // {
    //     if (prompts_.find(type) != prompts_.end())
    //     {
    //         prompts_[type]->ToggleVisibility(true);
    //     }
    // }
    void OnNotify(Event event, Prompt promptType, EventSystem::TaskObserver* observer) override
    {
        if (promptType == ColumnPromptObserver::Prompt::AddTask)
        {
            if (event == Event::ShowPrompt)
            {
                // show prompt and add column as an observer to prompt's TaskSubject event
                auto prompt = GetPrompt(WindowPrompt::Type::AddTaskPrompt);
                if (prompt->GetType() == WindowPrompt::Type::Default)
                    cout << "retrieved Dummy prompt when AddTask was expected" << endl;
                if (!prompt->IsActive())
                {
                    prompt->AddObserver(observer);
                    prompt->SetActive(true);
                }
            }
        }
    }
    void UpdatePrompts()
    {
        for (auto& kvp : prompts_)
            kvp.second->Update();
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
    sf::View view_;
    std::unordered_map<WindowPrompt::Type, WindowPrompt*> prompts_;
};