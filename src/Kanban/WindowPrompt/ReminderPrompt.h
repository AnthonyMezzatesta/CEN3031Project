#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "../ReminderManager/ReminderManager.h"
#include "WindowPrompt.h"

using std::string;

class ReminderPrompt final : public WindowPrompt
{
    enum DetailType { name, priority, deadline};
    DetailType typeArray[3] = { name, priority, deadline };

    sf::Font font_;
    sf::Text text_;
    sf::RectangleShape bg_;
    ReminderManager* reminderManager_;
public:
    ReminderPrompt(const sf::RenderWindow& target, ReminderManager& reminderManager)
    {
        if (!font_.loadFromFile(Utilities::fontPath))
            throw std::runtime_error("could not load font");
        text_.setFont(font_);

        reminderManager_ = &reminderManager;
        type_ = WindowPrompt::Type::ReminderPrompt;
        view_ = target.getDefaultView();
        view_.setViewport(viewPortLeft_);
        bg_.setFillColor(Utilities::fill2);
    }

    void Update(const float deltaTime) override
    {
        if (isActive) isVisible = true;
    }

    void Deactivate()
    {
        isActive = false;
        isVisible = false;
    }

    void DrawColumn(DetailType type, sf::Vector2f size, sf::Vector2f basePos, vector<Task>& tasks, sf::RenderTarget& target)
    {
        string titleText;
        if (type == name)
            titleText = "Task";
        else if (type == priority)
            titleText = "Priority";
        else
            titleText = "Deadline";

        // draw background
        sf::RectangleShape rect(size * 1.1875f);
        rect.setPosition(basePos - size * 0.1875f/2.0f);
        rect.setFillColor(Utilities::fill1);
        target.draw(rect);

        // draw title
        int headerHeight = size.y * 0.1f;
        Utilities::DrawText(target, text_, sf::Vector2f(size.x, headerHeight),
            basePos, titleText, headerHeight, Utilities::textColor);

        // draw horizontal line
        rect.setSize(sf::Vector2f(size.x * 0.9f, headerHeight * 0.1f));
        rect.setPosition(sf::Vector2f(size.x * 0.05f, headerHeight) + basePos);
        rect.setFillColor(Utilities::fill0);
        target.draw(rect);

        int taskCount = tasks.size();
        int textStartY = headerHeight * 1.5f;
        float optionWidth = size.x * 0.75f;
        float xOffset = basePos.x + size.x / 8.0f;
        float optionHeight = size.y / (taskCount + 1.0f);
        float yPadding = optionHeight / (taskCount + 1.0f);
        for (int i = 0; i < taskCount; i++)
        {
            float yOffset = textStartY + yPadding * (i+1) + optionHeight * i;

            string str;
            sf::Color color(Utilities::textColor);
            if (type == name)
                str = tasks[i].getName();
            else if (type == priority)
            {
                auto priority = tasks[i].getPriority();
                str = Utilities::TaskPriorityToString(priority);
                color = Utilities::TaskPriorityToColor(priority);
            }
            else
                str = tasks[i].getDeadlineString();

            Utilities::DrawText(target, text_, sf::Vector2f(optionWidth, optionHeight),
                sf::Vector2f(xOffset, yOffset), str, optionHeight/4, color);
        }
    }

    void Draw(sf::RenderTarget& target) override
    {
        if (!isVisible)
            return;

        target.setView(view_);

        bg_.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
        target.draw(bg_);

        auto tasks = reminderManager_->GetTasks();

        const int colCount = std::size(typeArray);
        auto size = target.getSize();
        int headerHeight = size.y * 0.1f;
        int colWidth = size.x / (colCount+1);
        int colHeight = size.y - (2 * headerHeight);
        int xPadding = colWidth / (colCount+1);

        for (int i = 0; i < colCount; i++)
        {
            int x = colWidth * i + xPadding * (i+1);
            DrawColumn(typeArray[i], sf::Vector2f(colWidth, colHeight), sf::Vector2f(x, headerHeight), tasks, target);
        }

        target.setView(target.getDefaultView());
    }

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override
    {
        auto mousePos = target.mapPixelToCoords(point, view_);

        // exit early if not visible
        if (!isVisible || !bg_.getGlobalBounds().contains(mousePos))
        {
            // if point was outside of bounds...
            if (isActive && isVisible)
                Deactivate();

            return false;
        }

        return true;
    }
};