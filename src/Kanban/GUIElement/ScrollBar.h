#pragma once
#include <iostream>
#include <valarray>
#include <SFML/Graphics.hpp>
#include "GUIElement.h"
using std::cout;
using std::endl;

namespace Kanban
{
    class ScrollBar
    {
        struct Bar final : public GUIElement
        {
            unsigned int width_ = 0;
            void SetPosition(float x, float y) { rect.setPosition(x, y); }
            sf::Vector2f GetPosition() { return rect.getPosition(); }
            void Draw(sf::Vector2f position, sf::Vector2f size, sf::Vector2f origin, sf::RenderTarget& target) override
            {
                rect.setPosition(rect.getPosition().x, position.y + size.y / 4);
                rect.setSize(sf::Vector2f(width_, size.y/2));
                rect.setFillColor(sf::Color::Black);
                // rect.setOrigin(origin);
                target.draw(rect);
            }
        protected:
            void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override {}
        };

        const static int SCROLL_SPEED = 1000;
        bool isVisible_ = false;
        int anchorXLocal = 0;
        float targetPosX_ = 0.0f;
        Bar bar_;
        sf::View view_;
        sf::RectangleShape rectBG_;
    public:
        ScrollBar(const sf::RenderTarget& target) : view_(target.getDefaultView()), rectBG_(sf::Vector2f(target.getSize().x, target.getSize().y))
        {
            view_.setViewport(sf::FloatRect(0.f, 0.95f, 1.f, 0.05f));
        }

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

        float Move(sf::Vector2i point, sf::RenderWindow& target)
        {
            auto mousePos = target.mapPixelToCoords(point, view_);
            float maxPosX = rectBG_.getSize().x - bar_.width_;

            // origin is top left corner, (0,0) locally
            // so we're setting rect.pos.x to where the mouse is and translating the scroll bar's pos...
            // so that the point at which we started dragging is centered at the current mouse position
            // we're also clamping the offset so that we keep the scroll bar within the window's bounds
            targetPosX_ = std::clamp(mousePos.x - anchorXLocal, 0.f, maxPosX);

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

        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
        {
            if (!isVisible_)
                return false;

            auto mousePos = target.mapPixelToCoords(point, view_);

            // returning true only if collided with bar for bar drag state
            if (bar_.CheckCollision(mousePos))
            {
                // get offset from bar's origin to mouse pos for use as an 'origin' when dragging the scroll bar
                anchorXLocal = mousePos.x - bar_.GetPosition().x;
                return true;
            }
            return false;
        }

        void Draw(sf::RenderTarget& target)
        {
            if (!isVisible_)
                return;

            sf::Vector2f size(target.getSize().x, target.getSize().y);

            target.setView(view_);

            rectBG_.setSize(size);
            rectBG_.setFillColor(sf::Color::White);
            target.draw(rectBG_);

            bar_.Draw({}, size, {}, target);

            target.setView(target.getDefaultView());
        }
    };
}
