#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>

#include "Observer.h"
#include "ReminderPrompt.h"
#include "AddTaskPrompt.h"
#include "DummyPrompt.h"
#include "ColumnSettingsPrompt.h"
#include "TaskDetailsPrompt.h"
#include "WindowResizePrompt.h"
#include "../../ReminderManager/ReminderManager.h"
#include "WindowPromptManager.h"

using namespace EventSystem;

void WindowPromptManager::TaskObserver::OnNotify(EventEnum event, const Task& task) {
    if (event != EventEnum::ShowPrompt)
        return;

    auto* prompt = dynamic_cast<TaskDetailsPrompt*>(windowPromptManager_->GetPrompt(WindowPrompt::Type::TaskDetailsPrompt));
    if (!prompt)
    {
        std::cerr << "could not recast WindowPrompt to TaskDetailsPrompt" << endl;
        return;
    }

    prompt->SetTask(task);
    prompt->Activate();
}

WindowPromptManager::TaskObserver::TaskObserver(WindowPromptManager* windowPromptManager): windowPromptManager_(windowPromptManager) {}

WindowPromptManager::WindowPromptManager(const sf::RenderWindow& window, ReminderManager& reminderManager,
    WindowResizeHandler& windowResizeHandler): taskObserver_(this) {
    activeWindow = WindowPrompt::Default;
    prompts_[WindowPrompt::Type::Default] = new DummyPrompt(windowResizeHandler);
    prompts_[WindowPrompt::Type::AddTaskPrompt] = new AddTaskPrompt(window, windowResizeHandler);
    prompts_[WindowPrompt::Type::SettingsPrompt] = new ColumnSettingsPrompt(window, windowResizeHandler);
    prompts_[WindowPrompt::Type::TaskDetailsPrompt] = new TaskDetailsPrompt(window, windowResizeHandler);
    prompts_[WindowPrompt::Type::ReminderPrompt] = new ReminderPrompt(window, reminderManager, windowResizeHandler);
    prompts_[WindowPrompt::Type::WindowResizePrompt] = new WindowResizePrompt(window, windowResizeHandler);

    UpdatePrompts(0.f);
}

WindowPromptManager::~WindowPromptManager() {
    for (auto& kvp: prompts_)
        delete kvp.second;
}

void WindowPromptManager::ShowPrompt(WindowPrompt::Type type) {
    if (prompts_[activeWindow]->IsActive())
        prompts_[activeWindow]->Deactivate();

    activeWindow = type;
    prompts_[activeWindow]->Activate();
}

WindowPrompt* WindowPromptManager::GetPrompt(const WindowPrompt::Type type) {
    if (prompts_.find(type) != prompts_.end())
        return prompts_[type];

    return prompts_[WindowPrompt::Type::Default];
}

void WindowPromptManager::UpdatePrompts(const float deltaTime) {
    for (auto& kvp: prompts_)
        kvp.second->Update(deltaTime);
}

void WindowPromptManager::ProcessLeftClickReleased() {
    prompts_[activeWindow]->ProcessLeftClickReleased();
}

void WindowPromptManager::ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target) {
    prompts_[activeWindow]->ProcessMouseMove(pixelPos, target);
}

bool WindowPromptManager::CheckCollision(sf::Vector2i point, sf::RenderWindow& target) {
    for (auto& kvp: prompts_)
    {
        if (kvp.second->CheckCollision(target, point))
            return true;
    }
    return false;
}

void WindowPromptManager::Draw(sf::RenderWindow& target) {
    for (auto& kvp: prompts_)
        kvp.second->Draw(target);
}
