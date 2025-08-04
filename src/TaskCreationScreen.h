#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "GUIStateMachine/GUIState.h"
using std::cout;
using std::endl;

class TaskCreationScreen final : public GUIState
{
    sf::Font font_;
    sf::Text text_;
    TaskManager* taskManager_;
    vector<Task> tasks_;

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
        int taskCount = tasks_.size();
        float minTarget = 0;
        float extraTaskCount = std::max(0, taskCount - tasksPerTexture_);
        float maxDistY = (taskHeight_ + taskPaddingY_) * extraTaskCount;

        bool enableScrollBar = extraTaskCount > 0;
        float textureHeight = defaultTextureHeight_ + maxDistY;
        float barWidthRatio = defaultTextureHeight_ / textureHeight;
        sf::Vector2u textureSize(textureWidth_, textureHeight);

        scrollTexture_.Update(enableScrollBar, barWidthRatio, textureSize, {}, minTarget, maxDistY, deltaTime);
    }
public:
    TaskCreationScreen(TaskManager& taskManager) : GUIState(StateEnum::TaskCreation), taskManager_(&taskManager), activeTask_(nullptr)
    {
        if (!font_.loadFromFile(Utilities::fontPath))
            std::cerr << "could not load font: " << Utilities::fontPath << endl;
        text_.setFont(font_);

        tasks_ = taskManager.getAllTasks();
        tasksOptions_.resize(tasks_.size());
        for (unsigned int i = 0; i < tasks_.size(); i++)
            tasksOptions_[i] = new Kanban::TaskOption(tasks_[i]);
    }
    ~TaskCreationScreen()
    {
        for (unsigned int i = 0; i < tasksOptions_.size(); i++)
            delete tasksOptions_[i];
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
    }

    void SelectTask(Kanban::TaskOption* task)
    {
        if (activeTask_)
        {
            activeTask_->Deselect();
            activeTask_ = nullptr;
        }

        activeTask_ = task;
        activeTask_->Select();
    }
    bool CheckCollision(sf::Vector2i pixelPos, sf::RenderWindow& target) override
    {
        auto staticPoint = static_cast<sf::Vector2f>(pixelPos);
        auto dynamicTexturePoint = textureTransform_.getInverse().transformPoint(staticPoint);
        dynamicTexturePoint.y += scrollTexture_.GetScrollDelta(); // adjust by amount scrolled

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

        for (int i = 0; i < tasks_.size(); i++)
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

    }
};