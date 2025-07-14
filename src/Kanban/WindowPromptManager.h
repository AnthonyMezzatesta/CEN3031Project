#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include "../Utilities/EventSystem/Observer.h"
#include "TaskManager.h"
#include "WindowPrompt.h"

class WindowPromptManager
{
    class ColumnPromptTaskObserver final : public EventSystem::ColumnPromptObserver<EventSystem::TaskObserver>
    {
        WindowPromptManager* windowPromptManager;
        void OnNotify(Event event, Prompt promptType, EventSystem::TaskObserver& observer) override
        {
            if (promptType == ColumnPromptTaskObserver::Prompt::AddTask)
            {
                if (event == Event::ShowPrompt)
                {
                    // show prompt and add column as an observer to prompt's TaskSubject event
                    auto prompt = dynamic_cast<AddTaskWindowPrompt*>(windowPromptManager->GetPrompt(WindowPrompt::Type::AddTaskPrompt));

                    if (!prompt)
                    {
                        std::cerr << "could not recast WindowPrompt to SettingsWindowPrompt" << endl;
                        return;
                    }

                    if (!prompt->IsActive())
                    {
                        prompt->AddObserver(observer);
                        prompt->SetActive(true);
                    }
                }
            }
        }
    public:
        ColumnPromptTaskObserver(WindowPromptManager* manager) : windowPromptManager(manager) {}
        };
    class ColumnPromptConfigObserver final : public EventSystem::ColumnPromptObserver<EventSystem::ActionObserver>
    {
        WindowPromptManager* windowPromptManager;
        void OnNotify(Event event, Prompt promptType, EventSystem::ActionObserver& observer) override
        {
            if (promptType == ColumnPromptConfigObserver::Prompt::Settings)
            {
                if (event == Event::ShowPrompt)
                {
                    auto prompt = dynamic_cast<SettingsWindowPrompt*>(windowPromptManager->GetPrompt(WindowPrompt::Type::SettingsPrompt));

                    if (!prompt)
                    {
                        std::cerr << "could not recast WindowPrompt to SettingsWindowPrompt" << endl;
                        return;
                    }

                    if (!prompt->IsActive())
                    {
                        prompt->AddObserver(observer);
                        prompt->SetActive(true);
                    }
                }
            }
        }
    public:
        ColumnPromptConfigObserver(WindowPromptManager* manager) : windowPromptManager(manager) {}
    };
public:
    ColumnPromptTaskObserver columnPromptTaskObserver;
    ColumnPromptConfigObserver columnPromptConfigObserver;

    WindowPromptManager(const sf::RenderWindow& target, TaskManager& taskManager) : columnPromptTaskObserver(this), columnPromptConfigObserver(this)
    {
        prompts_[WindowPrompt::Type::Default] = new DummyPrompt;
        prompts_[WindowPrompt::Type::AddTaskPrompt] = new AddTaskWindowPrompt(
            target,
            taskManager,
            WindowPrompt::Type::AddTaskPrompt
        );
        prompts_[WindowPrompt::Type::SettingsPrompt] = new SettingsWindowPrompt(
            target,
            taskManager,
            WindowPrompt::Type::SettingsPrompt);
        UpdatePrompts();
    }

    ~WindowPromptManager()
    {
        for (auto& kvp: prompts_)
            delete kvp.second;
    }

    WindowPrompt* GetPrompt(const WindowPrompt::Type type)
    {
        if (prompts_.find(type) != prompts_.end())
            return prompts_[type];

        return prompts_[WindowPrompt::Type::Default];
    }

    void UpdatePrompts()
    {
        for (auto& kvp: prompts_)
            kvp.second->Update();
    }

    bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
    {
        for (auto& kvp: prompts_)
        {
            if (kvp.second->CheckCollision(target, point))
                return true;
        }
        return false;
    }

    void Draw(sf::RenderWindow& target)
    {
        for (auto& kvp: prompts_)
            kvp.second->Draw(target);
    }

private:
    std::unordered_map<WindowPrompt::Type, WindowPrompt*> prompts_;
};
