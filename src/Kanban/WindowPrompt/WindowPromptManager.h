#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include "Observer.h"
#include "WindowPrompt.h"
#include "../Board/Board.h"
using namespace EventSystem;

struct WindowPromptManager
{
    WindowPromptManager(const sf::RenderWindow& target, Kanban::Board& board)
    {
        activeWindow = WindowPrompt::Default;
        prompts_[WindowPrompt::Type::Default] = new DummyPrompt;
        prompts_[WindowPrompt::Type::AddTaskPrompt] = new AddTaskWindowPrompt(
            target,
            board
        );
        prompts_[WindowPrompt::Type::SettingsPrompt] = new SettingsWindowPrompt(
            target,
            board
        );

        UpdatePrompts();
    }

    ~WindowPromptManager()
    {
        for (auto& kvp: prompts_)
            delete kvp.second;
    }

    template <typename T>
    void OnNotify(Observer::EventEnum event, Observer::PromptEnum promptType, DataObserver<T>& observer)
    {
        if (!prompts_[activeWindow]->IsActive())
            activeWindow = WindowPrompt::Type::Default;

        if (promptType == Observer::PromptEnum::AddTask)
        {
            if (event == Observer::EventEnum::ShowPrompt && activeWindow != WindowPrompt::AddTaskPrompt)
            {
                // show prompt and add column as an observer to prompt's TaskSubject event
                auto prompt = dynamic_cast<AddTaskWindowPrompt*>(GetPrompt(WindowPrompt::Type::AddTaskPrompt));
                if (!prompt)
                {
                    std::cerr << "could not recast WindowPrompt to AddTask Window Prompt" << endl;
                    return;
                }

                auto o = dynamic_cast<DataObserver<Task>*>(&observer);
                if (!o)
                {
                    std::cerr << "could not recast DataObserver<T> to DataObserver<Task>" << endl;
                    return;
                }

                activeWindow = WindowPrompt::AddTaskPrompt;
                prompt->AddObserver(*o);
                prompt->SetActive(true);
            }
        }
        if (promptType == Observer::PromptEnum::Settings)
        {
            if (event == Observer::EventEnum::ShowPrompt && activeWindow != WindowPrompt::SettingsPrompt)
            {
                auto prompt = dynamic_cast<SettingsWindowPrompt*>(GetPrompt(WindowPrompt::Type::SettingsPrompt));
                if (!prompt)
                {
                    std::cerr << "could not recast WindowPrompt to Settings Window Prompt" << endl;
                    return;
                }

                auto o = dynamic_cast<DataObserver<Observer::ActionEnum>* >(&observer);
                if (!o)
                {
                    std::cerr << "could not recast DataObserver<T> to DataObserver<Observer::ActionEnum>" << endl;
                    return;
                }

                activeWindow = WindowPrompt::SettingsPrompt;
                prompt->AddObserver(*o);
                prompt->SetActive(true);
            }
        }
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
    WindowPrompt::Type activeWindow;
    std::unordered_map<WindowPrompt::Type, WindowPrompt*> prompts_;
};
