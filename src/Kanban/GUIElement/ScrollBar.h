#pragma once
#include <SFML/Graphics.hpp>

class ScrollBar
{
    struct BarGraphic
    {
        unsigned int width_ = 0;
        sf::Vector2f GetPosition() const { return rect_.getPosition(); }
        void SetPosition(float x, float y) { rect_.setPosition(x, y); }
        void Draw(sf::Vector2f size, sf::RenderTarget& target, sf::Transform& parentTransform);
        bool CheckCollision(sf::Vector2f point) const;
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
    ScrollBar();

    void Enable();
    void Disable();

    float Move(sf::Vector2f point);
    float Update(float ratio, const float deltaTime);
    bool CheckCollision(sf::Vector2f point);
    void Draw(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size, float rotation = 0);
};
