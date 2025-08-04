#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>

#include "AddTaskPrompt.h"
#include "DummyPrompt.h"
#include "Observer.h"
#include "ReminderPrompt.h"
#include "SettingsPrompt.h"
#include "TaskDetailsPrompt.h"
#include "WindowPrompt.h"
#include "../../ReminderManager/ReminderManager.h"
#include "../Board/Board.h"
using namespace EventSystem;

class WindowPromptManager
{
    class TaskObserver : public DataObserver<Task>
    {
        WindowPromptManager* windowPromptManager_;
    public:
        void OnNotify(EventEnum event, Task& task) override
        {
            if (event != EventEnum::ShowPrompt)
                return;

            auto* prompt = dynamic_cast<TaskDetailsPrompt*>(windowPromptManager_->GetPrompt(WindowPrompt::Type::TaskDetailsPrompt));
            if (!prompt)
            {
                std::cerr << "could not recast WindowPrompt to TaskDetailsPrompt" << endl;
                return;
            }

            windowPromptManager_->activeWindow = WindowPrompt::Type::TaskDetailsPrompt;
            prompt->SetTask(task);
            prompt->SetActive(true);
        }
        TaskObserver(WindowPromptManager* windowPromptManager) : windowPromptManager_(windowPromptManager) {}
    };
    friend TaskObserver;

    WindowPrompt::Type activeWindow;
    std::unordered_map<WindowPrompt::Type, WindowPrompt*> prompts_;
public:
    TaskObserver taskObserver_;

    WindowPromptManager(const sf::RenderWindow& target, TaskManager& taskManager, Kanban::Board& board, ReminderManager& reminderManager) : taskObserver_(this)
    {
        activeWindow = WindowPrompt::Default;
        prompts_[WindowPrompt::Type::Default] = new DummyPrompt;
        prompts_[WindowPrompt::Type::AddTaskPrompt] = new AddTaskPrompt(target, board);
        prompts_[WindowPrompt::Type::SettingsPrompt] = new SettingsPrompt(target);
        prompts_[WindowPrompt::Type::TaskDetailsPrompt] = new TaskDetailsPrompt(target, taskManager, board);
        prompts_[WindowPrompt::Type::ReminderPrompt] = new ReminderPrompt(target, reminderManager);

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
                auto prompt = dynamic_cast<AddTaskPrompt*>(GetPrompt(WindowPrompt::Type::AddTaskPrompt));
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
        else if (promptType == Observer::PromptEnum::Settings)
        {
            if (event == Observer::EventEnum::ShowPrompt && activeWindow != WindowPrompt::SettingsPrompt)
            {
                auto prompt = dynamic_cast<SettingsPrompt*>(GetPrompt(WindowPrompt::Type::SettingsPrompt));
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

    void ShowReminderPrompt()
    {
        if (!prompts_[activeWindow]->IsActive())
            activeWindow = WindowPrompt::Type::Default;

        activeWindow = WindowPrompt::ReminderPrompt;
        prompts_[activeWindow]->SetActive(true);
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
    void ReadUserInput(char c)
    {
        prompts_[activeWindow]->ReadUserInput(c);
    }
    void ProcessKeyEvent(const sf::Keyboard::Key key)
    {
        prompts_[activeWindow]->ProcessKeyEvent(key);
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
        {
            if (kvp.second && kvp.second->isVisible) {
                kvp.second->Draw(target);
            }
        }
    }
};
