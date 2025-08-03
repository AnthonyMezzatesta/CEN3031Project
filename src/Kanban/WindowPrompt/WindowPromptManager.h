#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "WindowPrompt.h"
#include "AddTaskPrompt.h"
#include "ColumnSettingsPrompt.h"
#include "Observer.h"
#include "../../ReminderManager/ReminderManager.h"

using std::vector;
using namespace EventSystem;

class WindowPromptManager
{
    class TaskObserver : public DataObserver<Task>
    {
        WindowPromptManager* windowPromptManager_;
    public:
        void OnNotify(EventEnum event, const Task& task) override;

        TaskObserver(WindowPromptManager* windowPromptManager);
    };
    friend TaskObserver;

    WindowPrompt::Type activeWindow;
    std::unordered_map<WindowPrompt::Type, WindowPrompt*> prompts_;
public:
    TaskObserver taskObserver_;

    WindowPromptManager(const sf::RenderWindow& window, ReminderManager& reminderManager,
        WindowResizeHandler& windowResizeHandler);

    ~WindowPromptManager();

    template <typename T>
    void ShowPrompt(Observer::EventEnum event, Observer::PromptEnum promptType, DataObserver<T>& observer, std::optional<vector<Task>> tasks = {});

    void ShowPrompt(WindowPrompt::Type type);
    WindowPrompt* GetPrompt(const WindowPrompt::Type type);

    void UpdatePrompts(const float deltaTime);
    void ProcessLeftClickReleased();
    void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target);
    bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target);
    void Draw(sf::RenderWindow& target);
};

template<typename T>
void WindowPromptManager::ShowPrompt(Observer::EventEnum event, Observer::PromptEnum promptType,
    DataObserver<T>& observer, std::optional<vector<Task>> tasks) {
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
            prompt->Activate();
            if (tasks.has_value())
                prompt->UpdateTaskElements(tasks.value());
        }
    }
    else if (promptType == Observer::PromptEnum::Settings)
    {
        if (event == Observer::EventEnum::ShowPrompt && activeWindow != WindowPrompt::SettingsPrompt)
        {
            auto prompt = dynamic_cast<ColumnSettingsPrompt*>(GetPrompt(WindowPrompt::Type::SettingsPrompt));
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
            prompt->Activate();
        }
    }
}
