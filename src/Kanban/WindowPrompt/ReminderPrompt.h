#pragma once
#include <SFML/Graphics.hpp>
#include "../ReminderManager/ReminderManager.h"
#include "WindowPrompt.h"

class ReminderPrompt final : public WindowPrompt
{
    sf::RectangleShape bg_;
    sf::Color bgColor_ = sf::Color(128, 128, 128, 255);
    ReminderManager* reminderManager_;
public:
    ReminderPrompt(const sf::RenderWindow& target, ReminderManager& reminderManager)
    {
        reminderManager_ = &reminderManager;
        type_ = WindowPrompt::Type::ReminderPrompt;
        view_ = target.getDefaultView();
        view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
        bg_.setFillColor(bgColor_);
    }

    void Update() override
    {
        if (isActive) isVisible = true;
    }

    void Deactivate()
    {
        isActive = false;
        isVisible = false;
    }

    void Draw(sf::RenderTarget& target) override
    {
        if (!isVisible)
            return;

        target.setView(view_);

        bg_.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
        target.draw(bg_);

        sf::Font font;
        sf::Text text;
        if (!font.loadFromFile(Utilities::fontPath))
            throw std::runtime_error("could not load font");
        text.setFont(font);

        auto tasks = reminderManager_->GetTasks();

        auto size = target.getSize();
        int taskCount = tasks.size();
        float optionWidth = size.x * 0.75f;
        float xOffset = size.x / 8.0f;
        float optionHeight = size.y / (taskCount + 1.0f);
        float yPadding = optionHeight / (taskCount + 1.0f);
        for (int i = 0; i < taskCount; i++)
        {
            float yOffset = (i+1) * yPadding + optionHeight * i;
            string str = "Reminder: " + tasks[i].getName();
            Utilities::DrawText(target, text,
                sf::Vector2f(optionWidth, optionHeight),
                sf::Vector2f(xOffset, yOffset), str, 24);
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