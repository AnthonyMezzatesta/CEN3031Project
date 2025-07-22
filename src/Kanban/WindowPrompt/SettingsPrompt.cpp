#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "../Board/Board.h"
#include "SettingsPrompt.h"

const char* SettingsPrompt::OptionEnumToString(OptionEnum value) {
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

void SettingsPrompt::SettingsOption::DrawDetails(sf::RenderTarget& target, sf::Vector2f size,
    sf::Vector2f basePos) {
    // draw name
    Utilities::DrawText(target, textObj, size, basePos, name_, 24/*size.y * 0.15*/);
}

SettingsPrompt::SettingsOption::SettingsOption(OptionEnum type): GUIElement(sf::Color(190, 190, 190, 255)), type_(type), name_(OptionEnumToString(type_)) {
    if (!font.loadFromFile(Utilities::fontPath))
        throw std::runtime_error("could not load font");
    textObj.setFont(font);
}

SettingsPrompt::OptionEnum SettingsPrompt::SettingsOption::GetType() { return type_; }

SettingsPrompt::SettingsPrompt(const sf::RenderWindow& target) {
    type_ = WindowPrompt::Type::SettingsPrompt;
    view_ = target.getDefaultView();
    view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
    bg.setFillColor(bgColor);

    // sf::Font font;
    // if (!font.loadFromFile(Utilities::fontPath))
    //     throw std::runtime_error("could not load font");
    // text_.setFont(font);

    options_[0] = new SettingsOption(Rename);
    options_[1] = new SettingsOption(Delete);
}

SettingsPrompt::~SettingsPrompt() {
    for (auto* o : options_)
        delete o;
}

void SettingsPrompt::Update() {
    if (isActive) isVisible = true;
}

void SettingsPrompt::Deactivate() {
    isActive = false;
    isVisible = false;
    RemoveAllObservers();
}

bool SettingsPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point) {
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

void SettingsPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    target.setView(view_);

    bg.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
    target.draw(bg);

    auto size = target.getSize();
    int optionCount = std::size(options_);
    float optionWidth = size.x * 0.75f;
    float xOffset = size.x / 8.0f;
    float optionHeight = size.y / (optionCount + 1.0f);
    float yPadding = optionHeight / (optionCount + 1.0f);
    for (int i = 0; i < optionCount; i++)
    {
        float yOffset = (i+1) * yPadding + optionHeight * i;
        options_[i]->Draw(sf::Vector2f(xOffset, yOffset),
                          sf::Vector2f(optionWidth, optionHeight), {}, target);
    }

    target.setView(target.getDefaultView());
}
