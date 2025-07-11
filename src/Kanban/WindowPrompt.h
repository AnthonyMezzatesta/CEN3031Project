#pragma once
#include <cmath>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include "Board.h"
#include "../../include/TaskManager.h"
#include "../../include/Task.h"
#include "../Utilities/Utilities.h"
#include "unordered_map"
using std::cout;
using std::endl;

struct WindowPrompt
{
    enum PromptType { Default, AddTask };
    PromptType type = Default;
    bool isVisible = false;

    virtual void Execute() = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;
    virtual void ToggleVisibility(bool value) { isVisible = value; }
    virtual bool CheckCollision(sf::Vector2f point) = 0;
    virtual ~WindowPrompt() {}
protected:
    sf::Text text_;
};
class AddTaskWindowPrompt final : public WindowPrompt
{
    std::queue<Kanban::Element*> tasksToAdd_;
    sf::RectangleShape bg;
    sf::Color bgColor = sf::Color(174, 234, 222, 255);
    std::vector<Kanban::Element*> taskElements_;
public:
    AddTaskWindowPrompt(const PromptType type = Default)
    {
        this->type = type;
        bg.setFillColor(bgColor);

        sf::Font font;
        if (!font.loadFromFile(Utilities::fontPath))
            throw std::runtime_error("could not load font");
        text_.setFont(font);
    }

    void Execute() override
    {

    }

    void ToggleVisibility(bool value) override
    {
        isVisible = value;
        cout << "AddTaskWindowPrompt toggled on" << endl;
    }

    void Draw(sf::RenderTarget& target) override
    {
        if (isVisible)
        {
            bg.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
            target.draw(bg);
        }
    }

    bool CheckCollision(sf::Vector2f point) override
    {
        if (bg.getGlobalBounds().contains(point))
        {
            for (unsigned int i = 0; i < taskElements_.size(); i++)
            {
                if (taskElements_[i]->CheckCollision(point))
                {
                    tasksToAdd_.push(taskElements_[i]);
                    return true;
                }
            }
        }
        // for (unsigned int i = 0; i < columns.size(); i++)
        // {
        //     if (columns[i]->CheckCollision(mousePos))
        //         return true;
        // }
        // return false;
    }

    void DrawElements(sf::RenderTarget& target)
    {

    }

};