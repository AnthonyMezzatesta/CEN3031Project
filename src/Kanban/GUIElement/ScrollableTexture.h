#pragma once
#include <SFML/Graphics.hpp>
#include "ScrollBar.h"

class ScrollableTexture
{
    const static int SCROLL_SPEED = 1000;
    float scrollMoveDelta_ = 0; // [0, 1]
    bool scrollBarActive_ = false;
    ScrollBar scrollBar_;
    float valueToLerp_ = 0;

    sf::RenderTexture texture_;

    void UpdateTextureSize(sf::Vector2u size);
    void UpdateRendering(const float minTarget, const float maxTarget, const float deltaTime);
    void UpdateScrollBar(bool enable, float ratio, const float deltaTime);
public:

    float GetScrollDelta() const;
    sf::RenderTexture& GetTexture();

    bool ProcessLeftClickReleased();
    bool ProcessMouseMove(sf::Vector2f point);
    bool CheckScrollBarCollision(sf::Vector2f point);
    void Update(bool enable, float ratio, sf::Vector2u textureSize, std::optional<float> valueToLerp, const float minTarget, const float maxTarget, const float deltaTime);

    void DrawTexture(sf::RenderTarget& target, sf::IntRect rect = {}, const sf::Transform& parentTransform = sf::Transform::Identity);
    void DrawScrollBar(sf::RenderTarget& target, sf::Vector2f scrollBarPos, sf::Vector2f scrollBarSize, float rotation = 0);
};
