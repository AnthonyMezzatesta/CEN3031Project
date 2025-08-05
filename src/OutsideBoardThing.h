#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "GUIStateMachine/GUIStateMachine.h"
#include "Kanban/GUIElement/Icon.h"
#include "Kanban/WindowPrompt/WindowPromptManager.h"
#include "ReminderManager/ReminderManager.h"
using std::vector;
using std::string;

class OutsideBoardThing
{
    struct StateOption : public Kanban::GUIElement
    {
        GUIState::StateEnum state_;
        StateOption(GUIState::StateEnum state) : GUIElement(Utilities::fill2), state_(state) {}
    protected:
        string StateTypeToString(GUIState::StateEnum state)
        {
            switch (state)
            {
                case GUIState::Board:
                    return "Board";
                case GUIState::TaskCreation:
                    return "Task Creation";
                case GUIState::UserLogin:
                    return "User Login";
                default:
                    return "";
            }
        }
        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
        {
            Utilities::DrawText(target, textObj, size, basePos, StateTypeToString(state_), size.y / 4, Utilities::textColor);
        }
    };
    GUIStateMachine* guiStateMachine_;
    WindowPromptManager* windowPromptManager_;
    vector<Icon*> icons_;
    Icon::Type iconArray[3] { Icon::Type::plus, Icon::Type::bell, Icon::Type::dots };

    bool drawStateOptions_ = false;
    vector<StateOption*> stateOptions_;


    void DrawIcons(sf::RenderWindow& window)
    {
        if (icons_.empty())
            return;

        int iconCount = icons_.size();
        int iconWidth = icons_[0]->GetWidth();
        int y = iconWidth / 2;
        int iconPaddingX = iconWidth / 2;
        auto baseX = window.getSize().x - iconWidth - iconPaddingX;
        for (int i = 0; i < iconCount; i++)
        {
            int x = baseX - (iconWidth + iconPaddingX) * (iconCount - i - 1);
            icons_[i]->Draw(x, y, window );
        }
    }
    void DrawStateOptions(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size)
    {
        sf::RectangleShape bg(size);
        bg.setPosition(position);
        bg.setFillColor(Utilities::fill1);
        window.draw(bg);

        unsigned int optionCount = stateOptions_.size();
        float optionWidth = size.x * 0.8f;
        float optionHeight = size.y / (optionCount + 1);
        float optionPaddingY = optionHeight / (optionCount + 1);
        float x = (size.x - optionWidth)/2.f;
        for (unsigned int i = 0; i < optionCount; i++)
        {
            float y = optionPaddingY + (optionHeight + optionPaddingY) * i;
            auto pos = sf::Vector2f(position.x + x, position.y + y);
            stateOptions_[i]->Draw(pos, sf::Vector2f(optionWidth, optionHeight), {}, window);
        }
    }
public:
    OutsideBoardThing(ReminderManager& reminderManager, GUIStateMachine& guiStateMachine, WindowPromptManager& windowPromptManager)
    {
        guiStateMachine_ = &guiStateMachine;
        windowPromptManager_ = &windowPromptManager;

        for (unsigned int i = 0; i < std::size(iconArray); i++)
        {
            icons_.push_back(new Icon(iconArray[i]));

            // link notification bell icon with reminder manager
            if (iconArray[i] == Icon::Type::bell)
                reminderManager.AddObserver(*icons_[i]);
        }

        stateOptions_.push_back(new StateOption(GUIState::UserLogin));
        stateOptions_.push_back(new StateOption(GUIState::TaskCreation));
        stateOptions_.push_back(new StateOption(GUIState::Board));
    }
    ~OutsideBoardThing()
    {
        for (unsigned int i = 0; i < stateOptions_.size(); i++)
            delete stateOptions_[i];
        for (unsigned int i = 0; i < icons_.size(); i++)
            delete icons_[i];
    }

    void Draw(sf::RenderWindow& window)
    {
        DrawIcons(window);

        if (!drawStateOptions_)
            return;

        const auto size = static_cast<sf::Vector2f>(window.getSize());
        DrawStateOptions(window, {}, sf::Vector2f(size.x * 0.3, size.y * 0.3));
    }

    bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
    {
        const auto pt = static_cast<sf::Vector2f>(point);
        if (drawStateOptions_)
        {
            for (unsigned int i = 0; i < stateOptions_.size(); i++)
            {
                auto& option = stateOptions_[i];
                if (option->CheckCollision(pt))
                {
                    guiStateMachine_->SwitchState(option->state_);
                    return true;
                }
            }
        }

        drawStateOptions_ = false;

        for (unsigned int i = 0; i < icons_.size(); i++)
        {
            if (icons_[i]->CheckCollision(pt))
            {
                if (icons_[i]->GetType() == Icon::Type::plus)
                {
                    drawStateOptions_ = true;
                }
                else if (icons_[i]->GetType() == Icon::Type::bell)
                    windowPromptManager_->ShowPrompt(WindowPrompt::Type::ReminderPrompt);
                else if (icons_[i]->GetType() == Icon::Type::dots)
                    windowPromptManager_->ShowPrompt(WindowPrompt::Type::WindowResizePrompt);

                return true;
            }
        }
        return false;
    }
};