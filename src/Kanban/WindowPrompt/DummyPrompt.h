#pragma once
#include <SFML/Graphics.hpp>
#include "WindowResizeHandler.h"
#include "WindowPrompt.h"

struct DummyPrompt : public WindowPrompt
{
    DummyPrompt(WindowResizeHandler& windowResizeHandler): WindowPrompt(windowResizeHandler) { type_ = Default; }

    void Update(const float deltaTime) override {}
    void Draw(sf::RenderTarget& target) override {}
    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override { return false; }
};
