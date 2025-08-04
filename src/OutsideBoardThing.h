#pragma once
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "GUIStateMachine/GUIStateMachine.h"
#include "Kanban/GUIElement/Icon.h"
#include "Kanban/WindowPrompt/WindowPromptManager.h"
#include "ReminderManager/ReminderManager.h"
using std::vector;

class OutsideBoardThing
{
    GUIStateMachine* guiStateMachine_;
    WindowPromptManager* windowPromptManager_;
    vector<Icon*> icons_;
    Icon::Type iconArray[3] { Icon::Type::plus, Icon::Type::bell, Icon::Type::dots };

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

    }
    ~OutsideBoardThing()
    {
        for (unsigned int i = 0; i < icons_.size(); i++)
            delete icons_[i];
    }

    void Draw(sf::RenderWindow& window)
    {
        DrawIcons(window);
    }

    bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
    {
        for (unsigned int i = 0; i < icons_.size(); i++)
        {
            if (icons_[i]->CheckCollision(static_cast<sf::Vector2f>(point)))
            {
                if (icons_[i]->GetType() == Icon::Type::plus)
                    guiStateMachine_->SwitchState(GUIState::TaskCreation);
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