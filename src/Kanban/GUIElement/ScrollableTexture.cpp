#include <iostream>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "ScrollBar.h"
#include "ScrollableTexture.h"

void ScrollableTexture::UpdateTextureSize(sf::Vector2u size) {
    if (texture_.getSize() != size)
    {
        if (!texture_.create(size.x, size.y))
            std::cerr << "could not create render texture in ScrollableTexture" << std::endl;
    }
}

void ScrollableTexture::UpdateRendering(const float minTarget, const float maxTarget, const float deltaTime) {
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

void ScrollableTexture::UpdateScrollBar(bool enable, float ratio, const float deltaTime) {
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

float ScrollableTexture::GetScrollDelta() const { return valueToLerp_; }

sf::RenderTexture& ScrollableTexture::GetTexture() { return texture_; }

bool ScrollableTexture::ProcessLeftClickReleased() {
    // returning whether scrollBar was deactivated, or not
    if (scrollBarActive_)
    {
        scrollBarActive_ = false;
        return true;
    }

    return false;
}

bool ScrollableTexture::ProcessMouseMove(sf::Vector2f point) {
    if (scrollBarActive_)
        scrollMoveDelta_ = scrollBar_.Move(point);

    return scrollBarActive_;
}

bool ScrollableTexture::CheckScrollBarCollision(sf::Vector2f point) {
    scrollBarActive_ = scrollBar_.CheckCollision(point);
    return scrollBarActive_;
}

void ScrollableTexture::Update(bool enable, float ratio, sf::Vector2u textureSize, std::optional<float> valueToLerp,
    const float minTarget, const float maxTarget, const float deltaTime) {
    valueToLerp_ = valueToLerp.value_or(valueToLerp_);
    UpdateTextureSize(textureSize);
    UpdateRendering(minTarget, maxTarget, deltaTime);
    UpdateScrollBar(enable, ratio, deltaTime);
}

void ScrollableTexture::DrawTexture(sf::RenderTarget& target, sf::IntRect rect, const sf::Transform& parentTransform) {
    texture_.display();
    sf::Sprite sprite(texture_.getTexture());
    if (rect.width != 0)
        sprite.setTextureRect(rect);
    target.draw(sprite, parentTransform);
}

void ScrollableTexture::DrawScrollBar(sf::RenderTarget& target, sf::Vector2f scrollBarPos, sf::Vector2f scrollBarSize,
    float rotation) {
    scrollBar_.Draw(target, scrollBarPos, scrollBarSize, rotation);
}
