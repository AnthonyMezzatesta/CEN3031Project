#pragma once
#include <iostream>
#include <string>
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

    void UpdateTextureSize(sf::Vector2u size)
    {
        if (texture_.getSize() != size)
        {
            if (!texture_.create(size.x, size.y))
                std::cerr << "could not create dynamic render texture in Column.Render()" << endl;
        }
    }
    void UpdateRendering(const float minTarget, const float maxTarget, const float deltaTime)
    {
        float target = Utilities::LerpClamped(minTarget, maxTarget, scrollMoveDelta_);
        if (target != valueToLerp_)
        {
            float velocity = SCROLL_SPEED * deltaTime; // pixels per second

            // move towards target position
            valueToLerp_ = Utilities::LerpClamped(valueToLerp_, target, velocity);

            // clamp position if close enough
            if (std::abs(target - valueToLerp_) < 0.0001f)
                valueToLerp_ = target;
        }

        if (valueToLerp_ > maxTarget)
            valueToLerp_ = maxTarget;
    }
    void UpdateScrollBar(bool enable, float ratio, const float deltaTime)
    {
        if (enable)
        {
            scrollBar_.Enable();
            scrollMoveDelta_ = scrollBar_.Update(ratio, deltaTime);
        }
        else
        {
            scrollMoveDelta_ = 0;
            scrollBar_.Disable();
        }
    }
public:

    float GetValue() const { return valueToLerp_; }
    sf::RenderTexture& GetTexture() { return texture_; }

    bool ProcessLeftClickReleased()
    {
        // returning whether scrollBar was deactivated, or not
        if (scrollBarActive_)
        {
            scrollBarActive_ = false;
            return true;
        }

        return false;
    }

    bool ProcessMouseMove(sf::Vector2f point)
    {
        if (scrollBarActive_)
            scrollMoveDelta_ = scrollBar_.Move(point);

        return scrollBarActive_;
    }

    bool CheckScrollBarCollision(sf::Vector2f point)
    {
        scrollBarActive_ = scrollBar_.CheckCollision(point);
        return scrollBarActive_;
    }

    void Update(bool enable, float ratio, sf::Vector2u textureSize, std::optional<float> valueToLerp, const float minTarget, const float maxTarget, const float deltaTime)
    {
        valueToLerp_ = valueToLerp.value_or(valueToLerp_);
        UpdateTextureSize(textureSize);
        UpdateRendering(minTarget, maxTarget, deltaTime);
        UpdateScrollBar(enable, ratio, deltaTime);
    }

    void DrawTexture(sf::RenderTarget& target, sf::IntRect rect = {}, const sf::Transform& parentTransform = sf::Transform::Identity)
    {
        texture_.display();
        sf::Sprite sprite(texture_.getTexture());
        if (rect.width != 0)
            sprite.setTextureRect(rect);
        target.draw(sprite, parentTransform);
    }

    void DrawScrollBar(sf::RenderTarget& target, sf::Vector2f scrollBarPos, sf::Vector2f scrollBarSize, float rotation = 0)
    {
        scrollBar_.Draw(target, scrollBarPos, scrollBarSize, rotation);
    }
};
