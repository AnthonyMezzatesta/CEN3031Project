#include <SFML/Graphics.hpp>
#include <string>
#include "Utilities.h"
#include "../Board/Board.h"
#include "TaskDetailsPrompt.h"
using namespace std;

TaskDetailsPrompt::TaskDetailsPrompt(const sf::RenderWindow& target)
{
    type_ = WindowPrompt::Type::SettingsPrompt;
    view_ = target.getDefaultView();
    view_.setViewport(viewPortLeft_);
    bg_.setFillColor(bgColor_);

    // sf::Font font;
    // if (!font.loadFromFile(Utilities::fontPath))
    //     throw std::runtime_error("could not load font");
    // text_.setFont(font);
}

void TaskDetailsPrompt::Update(const float deltaTime)
{
    if (isActive) isVisible = true;
}

void TaskDetailsPrompt::Deactivate()
{
    isActive = false;
    isVisible = false;
    RemoveAllObservers();
}

bool TaskDetailsPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point)
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

void TaskDetailsPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    target.setView(view_);

    bg_.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
    target.draw(bg_);

    sf::Text text;
    sf::Font font;
    font.loadFromFile(Utilities::fontPath);
    text.setFont(font);

    auto name = task_.getName();
    auto description = task_.getDescription();
    auto deadline = task_.getDeadlineString();
    string details[3] = { name, description, deadline };

    int count = std::size(details);
    auto size = target.getSize();
    float optionWidth = size.x * 0.75f;
    float xOffset = size.x / 8.0f;
    float optionHeight = size.y / (count + 1.0f);
    float yPadding = optionHeight / (count + 1.0f);

    for (int i = 0; i < count; i++)
    {
        float yOffset = (i+1) * yPadding + optionHeight * i;
        Utilities::DrawText(target, text, sf::Vector2f(optionWidth, optionHeight),
            sf::Vector2f(xOffset, yOffset), details[i], optionHeight / 4);
    }

    target.setView(target.getDefaultView());
}