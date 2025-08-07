#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Subject.h"
#include "GUIStateMachine/GUIState.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;

class TaskConfigScreen final : public GUIState, private EventSystem::TaskSubject
{
    enum ButtonEnum { createTask, deleteTask, viewTask };
    struct Button : public Kanban::GUIElement
    {
        ButtonEnum type_;
        string name_;
        Button(ButtonEnum type, const string& name) : GUIElement(Utilities::fill2), type_(type), name_(name) {}

    protected:
        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
        {
            Utilities::DrawText(target, textObj, size, basePos, name_, size.y / 4, Utilities::textColor);
        }
    };
    vector<Button*> buttons_;
    ButtonEnum buttonTypes[3] = {createTask, deleteTask, viewTask};
    string buttonNames[3] = {"Create", "Delete", "View Details" };

    sf::Font font_;
    sf::Text text_;
    TaskManager* taskManager_;
    WindowPromptManager* windowPromptManager_;
    // vector<Task> tasks_;

    sf::Transform staticTransform_;
    sf::Transform textureTransform_;

    vector<Kanban::TaskOption*> tasksOptions_;
    Kanban::TaskOption* activeTask_;
    ScrollableTexture scrollTexture_;

    const static int tasksPerTexture_ = 6;
    float spaceWidth_;
    float spaceHeight_;
    float screenPaddingX_;
    float headerHeight_;
    float taskWidth_;
    float taskHeight_;
    float taskPaddingX_;
    float taskPaddingY_;
    float textureWidth_;
    float defaultTextureHeight_;
    float texturePaddingX_;
    float texturePaddingY_;

    void UpdateValues(const float screenWidth, const float screenHeight)
    {
        spaceWidth_ = screenWidth * 0.7f;
        screenPaddingX_ = (screenWidth - spaceWidth_)/2.f;
        spaceHeight_ = screenHeight;
        headerHeight_ = spaceHeight_ * 0.15f;

        textureWidth_ = spaceWidth_ * 0.45f;
        texturePaddingX_ = spaceWidth_ * 0.025f;
        defaultTextureHeight_ = spaceHeight_ * 0.75f;
        texturePaddingY_ = spaceHeight_ * 0.025f;

        taskWidth_ = textureWidth_ * 0.8f;
        taskPaddingX_ = (textureWidth_ - taskWidth_)/2.f;
        taskHeight_ = defaultTextureHeight_ / (tasksPerTexture_+1);
        taskPaddingY_ = taskHeight_ / (tasksPerTexture_+1);
    }
    void UpdateScrollTexture(const float deltaTime)
    {
        int taskCount = tasksOptions_.size();
        float minTarget = 0;
        float extraTaskCount = std::max(0, taskCount - tasksPerTexture_);
        float maxDistY = (taskHeight_ + taskPaddingY_) * extraTaskCount;

        bool enableScrollBar = extraTaskCount > 0;
        float textureHeight = defaultTextureHeight_ + maxDistY;
        float barWidthRatio = defaultTextureHeight_ / textureHeight;
        sf::Vector2u textureSize(textureWidth_, textureHeight);

        scrollTexture_.Update(enableScrollBar, barWidthRatio, textureSize, {}, minTarget, maxDistY, deltaTime);
    }

    void UpdateTasks()
    {
        // Get all updated tasks from database
        auto allTasks = taskManager_->getAllTasks();

        // Update existing TaskCards with fresh data from database
        for (auto iter = tasksOptions_.begin(); iter != tasksOptions_.end(); ++iter)
        {
            Kanban::TaskOption* option = *iter;

            if (option && option->getId().has_value())
            {
                int taskId = option->getId().value();
                bool foundInDatabase = false;

                // Find the updated task in the database
                for (const auto& updatedTask : allTasks)
                {
                    if (updatedTask.getId().has_value() && updatedTask.getId().value() == taskId)
                    {
                        // Update the TaskCard's task with fresh data
                        option->UpdateTask(updatedTask);
                        foundInDatabase = true;
                        break;
                    }
                }

                // If task was deleted from database, remove the TaskCard
                if (!foundInDatabase)
                {
                    delete *iter;
                    iter = tasksOptions_.erase(iter);
                }
            }
        }
    }

    void PerformButtonOperation(Button* button)
    {
        if (button->type_ == createTask)
        {
            auto now = std::chrono::system_clock::now();
            Task task("unnamed", "description", now, {});
            taskManager_->addTask(task);
            tasksOptions_.push_back(new Kanban::TaskOption(task));
        }

        if (!activeTask_)
            return;

        if (button->type_ == deleteTask)
        {
            for (auto iter = tasksOptions_.begin(); iter != tasksOptions_.end(); ++iter)
            {
                if (*iter == activeTask_)
                {
                    activeTask_->Deselect();
                    taskManager_->removeTask(activeTask_->getId().value());
                    delete activeTask_;
                    activeTask_ = nullptr;
                    tasksOptions_.erase(iter);
                    return;
                }
            }
        }
        else if (button->type_ == viewTask)
        {
            Notify(Observer::ShowPrompt, activeTask_->GetTask());
        }

    }
    void DrawTaskSettings(sf::RenderWindow& window)
    {
        const unsigned int buttonCount = buttons_.size();
        float buttonHeight = spaceHeight_ * 0.3f / (buttonCount + 1);
        float buttonPaddingY = buttonHeight / (buttonCount + 1);

        sf::Vector2f buttonSize(spaceWidth_ * 0.3f, buttonHeight);
        float x = spaceWidth_ - buttonSize.x * 0.75f;
        float startY = spaceHeight_ * 0.65f;
        auto pos = staticTransform_.transformPoint(sf::Vector2f(x, startY));

        buttons_[ButtonEnum::createTask]->Draw(pos, buttonSize, {}, window);

        if (!activeTask_)
            return;


        for (unsigned int i = 1; i < buttonCount; i++)
        {
            float y = startY + (buttonHeight + buttonPaddingY) * i;
            pos = staticTransform_.transformPoint(sf::Vector2f(x, y));
            buttons_[i]->Draw(pos, buttonSize, {}, window);
        }

    }

public:
    TaskConfigScreen(TaskManager& taskManager, WindowPromptManager& windowPromptManager) :
        GUIState(StateEnum::TaskCreation), taskManager_(&taskManager), windowPromptManager_(&windowPromptManager), activeTask_(nullptr)
    {
        if (!font_.loadFromFile(Utilities::fontPath))
            std::cerr << "could not load font: " << Utilities::fontPath << endl;
        text_.setFont(font_);

        TaskSubject::AddObserver(windowPromptManager_->taskObserver_);
        auto tasks_ = taskManager.getAllTasks();
        tasksOptions_.resize(tasks_.size());
        for (unsigned int i = 0; i < tasks_.size(); i++)
            tasksOptions_[i] = new Kanban::TaskOption(tasks_[i]);

        for (unsigned int i = 0; i < std::size(buttonNames); i++)
            buttons_.push_back(new Button(buttonTypes[i], buttonNames[i]));
    }
    ~TaskConfigScreen()
    {
        for (unsigned int i = 0; i < tasksOptions_.size(); i++)
            delete tasksOptions_[i];
        for (unsigned int i = 0; i < buttons_.size(); i++)
            delete buttons_[i];
    }

    void ProcessLeftClickReleased() override
    {
        scrollTexture_.ProcessLeftClickReleased();
    }

    void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target) override
    {
        scrollTexture_.ProcessMouseMove(static_cast<sf::Vector2f>(pixelPos));
    }

    void ReadUserInput(char c) override
    {}

    void Update(const sf::RenderWindow& window, const float deltaTime) override
    {
        auto windowSize = window.getSize();
        UpdateValues(windowSize.x, windowSize.y);
        UpdateScrollTexture(deltaTime);

        UpdateTasks(); // not ideal, but a temp fix
    }

    void ClearSelectedTask()
    {
        if (activeTask_)
        {
            activeTask_->Deselect();
            activeTask_ = nullptr;
        }
    }
    void SelectTask(Kanban::TaskOption* task)
    {
        if (activeTask_) activeTask_->Deselect();

        activeTask_ = task;
        activeTask_->Select();
    }
    bool CheckCollision(sf::Vector2i pixelPos, sf::RenderWindow& target) override
    {
        auto staticPoint = static_cast<sf::Vector2f>(pixelPos);
        auto dynamicTexturePoint = textureTransform_.getInverse().transformPoint(staticPoint);
        dynamicTexturePoint.y += scrollTexture_.GetScrollDelta(); // adjust by amount scrolled

        for (unsigned int i = 0; i < buttons_.size(); i++)
        {
            if (buttons_[i]->CheckCollision(staticPoint))
            {
                PerformButtonOperation(buttons_[i]);
                ClearSelectedTask();
                return true;
            }
        }

        ClearSelectedTask();

        if (scrollTexture_.CheckScrollBarCollision(staticPoint))
            return true;

        for (unsigned int i = 0; i < tasksOptions_.size(); i++)
        {
            if (tasksOptions_[i]->CheckCollision(dynamicTexturePoint))
            {
                SelectTask(tasksOptions_[i]);
                return true;
            }
        }

        return false;
    }

    void Draw(sf::RenderWindow& window) override
    {
        staticTransform_ = sf::Transform::Identity;
        staticTransform_.translate(screenPaddingX_, 0);

        sf::Vector2f headerSize(spaceWidth_, headerHeight_);
        sf::RectangleShape bg;

        // draw bg of screen
        sf::Vector2f bgSize(spaceWidth_, spaceHeight_);
        bg.setSize(bgSize);
        bg.setFillColor(Utilities::fill1);
        window.draw(bg, staticTransform_);

        // draw title
        auto textPos = staticTransform_.transformPoint(sf::Vector2f(0, 0));
        Utilities::DrawText(window, text_, headerSize, textPos, "Tasks", headerSize.y / 2, Utilities::textColor);

        auto& dynamicTexture = scrollTexture_.GetTexture();
        dynamicTexture.clear(sf::Color::Transparent);

        for (int i = 0; i < tasksOptions_.size(); i++)
        {
            float y = taskPaddingY_ + (taskHeight_ + taskPaddingY_) * i;
            tasksOptions_[i]->Draw(sf::Vector2f(taskPaddingX_, y),
                                   sf::Vector2f(taskWidth_, taskHeight_), {}, dynamicTexture);
        }

        textureTransform_ = staticTransform_;
        textureTransform_.translate(0, headerSize.y);

        // draw texture border
        bg.setSize(sf::Vector2f(textureWidth_ + texturePaddingX_ * 2, defaultTextureHeight_ + texturePaddingY_ * 2));
        bg.setFillColor(Utilities::fill0);
        window.draw(bg, textureTransform_);


        // draw texture bg
        // bg.setSize(sf::Vector2f(textureWidth_, defaultTextureHeight_));
        // bg.setFillColor(Utilities::fill0);
        // window.draw(bg, textureTransform_);

        // draw scroll bar
        sf::Vector2f barPos(bg.getSize().x, 0);
        barPos = textureTransform_.transformPoint(barPos);
        sf::Vector2f barSize(bg.getSize().y, texturePaddingX_);
        scrollTexture_.DrawScrollBar(window, barPos, barSize, 90);

        textureTransform_.translate(texturePaddingX_, texturePaddingY_);

        // draw texture
        sf::IntRect scrollRect(0, scrollTexture_.GetScrollDelta(), textureWidth_, defaultTextureHeight_);
        scrollTexture_.DrawTexture(window, scrollRect, textureTransform_);

        DrawTaskSettings(window);
    }
};