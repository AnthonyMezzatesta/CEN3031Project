#pragma once
#include <SFML/Graphics.hpp>
#include "WindowPrompt.h"

struct DummyPrompt : public WindowPrompt
{
    DummyPrompt() { type_ = Default; }
    void Update() override {}
    void Draw(sf::RenderTarget& target) override {}
    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override { return false; }
};
