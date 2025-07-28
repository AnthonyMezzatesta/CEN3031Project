#pragma once
#include <iostream>
#include <valarray>
#include <SFML/Graphics.hpp>
#include "GUIElement.h"
#include "Utilities.h"
using std::cout;
using std::endl;

namespace Kanban
{
    class ScrollBar
    {
        struct Bar
        {
            unsigned int width_ = 0;
            void SetPosition(float x, float y) { rect_.setPosition(x, y); }
            sf::Vector2f GetPosition() { return rect_.getPosition(); }
            void Draw(sf::Vector2f size, sf::RenderTarget& target, sf::Transform& parentTransform)
            {
                rect_.setPosition(rect_.getPosition().x, size.y / 4);
                rect_.setSize(sf::Vector2f(width_, size.y/2));
                rect_.setFillColor(sf::Color(120,122,123,255));
                target.draw(rect_, parentTransform);
            }
            bool CheckCollision(sf::Vector2f point) { return rect_.getGlobalBounds().contains(point); }
        private:
            sf::RectangleShape rect_;
        } bar_;

        const static int SCROLL_SPEED = 1000;
        bool isVisible_ = false;
        int anchorXLocal = 0;
        float targetPosX_ = 0.0f;
        sf::Color bgColor_;
        sf::RectangleShape rectBG_;
        sf::Transform transform_;
    public:
        ScrollBar() : bgColor_(63,66,68,255), rectBG_(sf::Vector2f(10, 10)) {}

        void Enable()
        {
            isVisible_ = true;
        }
        void Disable()
        {
            isVisible_ = false;
            targetPosX_ = 0;
            bar_.SetPosition(0, 0);
        }

        float Move(sf::Vector2f point)
        {
            float maxPosX = rectBG_.getSize().x - bar_.width_;

            // world to local
            point = transform_.getInverse().transformPoint(point);

            // origin is top left corner, (0,0) locally
            // so we're setting rect.pos.x to where the mouse is and translating the scroll bar's pos...
            // so that the point at which we started dragging is centered at the current mouse position
            // we're also clamping the offset so that we keep the scroll bar within the window's bounds
            targetPosX_ = std::clamp(point.x - anchorXLocal, 0.f, maxPosX);

            // return normalized position of targetPosX [0, 1]
            return Utilities::InvLerp(0, maxPosX, targetPosX_);
        }

        float Update(float ratio, const float deltaTime)
        {
            // set scroll bar width based on ratio of texture being scrolled
            bar_.width_ = ratio * rectBG_.getSize().x;

            // update pos, in case the size of the texture being scrolled has changed
            float maxPosX = rectBG_.getSize().x - bar_.width_;
            targetPosX_ = std::min(targetPosX_, maxPosX);

            // move scroll bar independent from frame rate
            float posX = Utilities::LerpClamped(bar_.GetPosition().x, targetPosX_, SCROLL_SPEED * deltaTime);
            bar_.SetPosition(posX, 0);

            // return normalized position of targetPosX [0, 1]
            return Utilities::InvLerpClamped(0, maxPosX, targetPosX_);
        }

        bool CheckCollision(sf::Vector2f point)
        {
            if (!isVisible_)
                return false;

            // world to local
            point = transform_.getInverse().transformPoint(point);

            // returning true only if collided with bar for bar drag state
            if (bar_.CheckCollision(point))
            {
                // get offset from bar's origin to mouse pos for use as an 'origin' when dragging the scroll bar
                anchorXLocal = point.x - bar_.GetPosition().x;
                return true;
            }
            return false;
        }

        void Draw(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size, float rotation = 0)
        {
            if (!isVisible_)
                return;

            rectBG_.setSize(size);
            rectBG_.setFillColor(bgColor_);

            transform_ = sf::Transform::Identity; // reset
            transform_.translate(position.x, position.y);
            transform_.rotate(rotation);

            target.draw(rectBG_, transform_);

            bar_.Draw(size, target, transform_);
        }
    };
}
