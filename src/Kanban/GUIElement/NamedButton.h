#pragma once
#include <string>
#include "GUIElement.h"
using std::string;

namespace Kanban
{
    struct NamedButton : public GUIElement
    {
        string name_;
        NamedButton(string name) : GUIElement(Utilities::fill1), name_(name) {}
    protected:
        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
        {
            Utilities::DrawText(target, textObj, size, basePos, name_, size.y / 2, Utilities::textColor);
        }
    };
}
