#include <string>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "../Board/Board.h"
#include "ColumnSettingsPrompt.h"
using std::string;

const char* ColumnSettingsPrompt::OptionEnumToString(OptionEnum value) {
    switch (value)
    {
        case Rename:
            return "Rename";
        case Delete:
            return "Delete";
        default:
            return "\0";
    }
}

void ColumnSettingsPrompt::SettingsOption::DrawDetails(sf::RenderTarget& target, sf::Vector2f size,
    sf::Vector2f basePos) {
    // draw name
    Utilities::DrawText(target, textObj, size, basePos, name_, size.y / 4, Utilities::textColor);
}

ColumnSettingsPrompt::SettingsOption::SettingsOption(OptionEnum type): GUIElement(Utilities::fill1), type_(type), name_(OptionEnumToString(type_)) {}

ColumnSettingsPrompt::OptionEnum ColumnSettingsPrompt::SettingsOption::GetType() { return type_; }

ColumnSettingsPrompt::ColumnSettingsPrompt(const sf::RenderWindow& target,
    WindowResizeHandler& windowResizeHandler) : WindowPrompt(windowResizeHandler) {
    type_ = WindowPrompt::Type::SettingsPrompt;
    view_ = target.getDefaultView();
    view_.setViewport(viewPortLeft_);
    bg.setFillColor(Utilities::fill2);

    options_[0] = new SettingsOption(Rename);
    options_[1] = new SettingsOption(Delete);
}

ColumnSettingsPrompt::~ColumnSettingsPrompt() {
    for (auto* o : options_)
        delete o;
}

void ColumnSettingsPrompt::Update(const float deltaTime) {
    if (isActive) isVisible = true;
}

void ColumnSettingsPrompt::Deactivate() {
    isActive = false;
    isVisible = false;
    RemoveAllObservers();
}

bool ColumnSettingsPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point) {
    auto mousePos = target.mapPixelToCoords(point, view_);

    // exit early if not visible
    if (!isVisible || !bg.getGlobalBounds().contains(mousePos))
    {
        // if point was outside of bounds...
        if (isActive && isVisible)
            Deactivate();

        return false;
    }

    int size = std::size(options_);
    for (int i = 0; i < size; i++)
    {
        if (options_[i]->CheckCollision(mousePos))
        {
            if (options_[i]->GetType() == OptionEnum::Rename)
            {
                Notify(EventSystem::Observer::EventEnum::Action, EventSystem::Observer::ActionEnum::Rename);
            }
            else if (options_[i]->GetType() == OptionEnum::Delete)
            {
                Notify(EventSystem::Observer::EventEnum::Action, EventSystem::Observer::ActionEnum::Delete);
            }
            Deactivate();
        }
    }

    return true;
}

void ColumnSettingsPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    target.setView(view_);
    auto size = target.getSize();

    bg.setSize(sf::Vector2f(size.x, size.y));
    target.draw(bg);

    int optionCount = std::size(options_);
    float optionWidth = size.x * 0.75f;
    float xOffset = size.x / 8.0f;
    float optionHeight = size.y / (optionCount + 1.0f);
    float yPadding = optionHeight / (optionCount + 1.0f);
    for (int i = 0; i < optionCount; i++)
    {
        float yOffset = yPadding * (i+1) + optionHeight * i;
        options_[i]->Draw(sf::Vector2f(xOffset, yOffset),
                          sf::Vector2f(optionWidth, optionHeight), {}, target);
    }

    target.setView(target.getDefaultView());
}
