#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "TaskManager.h"
#include "Task.h"
#include "Column.h"
#include "Board.h"
#include "../GUIElement/GUIElement.h"
#include "../GUIElement/ScrollBar.h"
#include "../GUIElement/Icon.h"
#include "../GUIElement/TaskCard.h"
#include "../WindowPrompt/WindowPromptManager.h"
#include "Utilities.h"
#include "Subject.h"

using namespace std;
using namespace EventSystem;

// ===================================== Private Functions =====================================

void Kanban::Column::ActionObserver::OnNotify(Observer::EventEnum event, Observer::ActionEnum& action)
{
    if (event == Observer::EventEnum::Action)
    {
        if (action == Observer::ActionEnum::Rename)
        {
            cout << "renaming column" << endl;
            column_->board_->SetActiveColumn(column_, Board::UserInputMode::ColumnName);
        }
        if (action == Observer::ActionEnum::Delete)
        {
            cout << "deleting column" << endl;
            column_->board_->RemoveColumn(*column_);
        }
    }
}

void Kanban::Column::TaskObserver::OnNotify(Observer::EventEnum event, Task& task)
{
    if (event == Observer::EventEnum::TransferTask)
    {
        column_->board_->SetTaskAsTaken(task);
        column_->tasks_.push_back(new Kanban::TaskCard(column_, task));
    }
}

void Kanban::Column::UpdateValues(float height)
{
    columnHeight_ = height;
    int taskCount = tasks_.size();
    int iconHeight = Icon::GetDefaultWidth() * 2;
    yHeader_ = iconHeight + iconHeight / 2.f;
    taskHeight_ = (columnHeight_ - yHeader_) / (tasksPerColummn_ + 1.0f);
    taskPaddingY_ = taskHeight_ / tasksPerColummn_;
    float extraTaskCount = std::max(taskCount - tasksPerColummn_, 0);
    // texturePaddingY_ = 2 * taskPaddingY_;
    textureHeight_ = columnHeight_ + (taskPaddingY_ + taskHeight_) * extraTaskCount;
}

void Kanban::Column::UpdateRenderTexture(const float width)
{
    if (textureStatic_.getSize().x != width || textureStatic_.getSize().y != columnHeight_)
    {
        if (!textureStatic_.create(width, columnHeight_))
            std::cerr << "could not create static render texture in Column.Render()" << endl;
    }

    if (textureDynamic_.getSize().x != width || textureDynamic_.getSize().y != textureHeight_)
    {
        if (!textureDynamic_.create(width, textureHeight_))
            std::cerr << "could not create dynamic render texture in Column.Render()" << endl;
    }
}

void Kanban::Column::UpdateRendering(const float deltaTime)
{
    int taskCount = tasks_.size();
    float defaultStartY = 0;
    float extraTaskCount = std::max(taskCount - tasksPerColummn_, 0);
    // float maxDistY = taskPaddingY_ + (taskPaddingY_ + taskHeight_) * extraTaskCount;
    float maxDistY = (taskPaddingY_ + taskHeight_) * extraTaskCount;
    float maxStartY = defaultStartY + maxDistY;

    float targetPosY = Utilities::LerpClamped(defaultStartY, maxStartY, scrollMoveDelta_);
    if (targetPosY != startRenderY_)
    {
        float velocity = SCROLL_SPEED * deltaTime; // pixels per second

        // move towards target position
        startRenderY_ = Utilities::LerpClamped(startRenderY_, targetPosY, velocity);

        // clamp position if close enough
        if (std::abs(targetPosY - startRenderY_) < 0.0001f)
            startRenderY_ = targetPosY;
    }

    // clamp position of view so that it is within bounds of renderTexture
    // useful for adjusting view when tasks are removed
    if (startRenderY_ > maxStartY)
        startRenderY_ = maxStartY;
}

void Kanban::Column::UpdateScrollBar(const float deltaTime)
{
    if (tasks_.size() > tasksPerColummn_)
    {
        float scrollBarWidthRatio = columnHeight_ / static_cast<float>(textureHeight_);
        scrollBar_.Enable();
        scrollMoveDelta_ = scrollBar_.Update(scrollBarWidthRatio, deltaTime);
    }
    else
    {
        scrollMoveDelta_ = 0;
        scrollBar_.Disable();
    }
}

void Kanban::Column::RenderStaticDetails(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target)
{
    textureStatic_.clear(sf::Color::Transparent);

    // draw background
    bgRect_.setFillColor(Utilities::fill0);
    bgRect_.setSize(sf::Vector2f(size.x, columnHeight_));
    textureStatic_.draw(bgRect_);

    // draw column title
    int titleWidth = size.x;
    int titleHeight = yHeader_;
    sf::Vector2f textSize(titleWidth, titleHeight);
    Utilities::DrawText(textureStatic_, text_, textSize, {}, name_, titleHeight / 2, textColor_);

    RenderIcons(textureStatic_, {}, size.x);

    // draw scroll bar
    float taskWidth = size.x * 0.75f;
    sf::Vector2f barPos(size.x, yHeader_);
    sf::Vector2f barSize(columnHeight_ - yHeader_, (size.x - taskWidth)/4.f);
    scrollBar_.Draw(textureStatic_, barPos, barSize, 90);

    transformStatic_ = sf::Transform::Identity;
    transformStatic_.translate(position);

    textureStatic_.display();
    target.draw(sf::Sprite(textureStatic_.getTexture()), transformStatic_);
}

void Kanban::Column::ClearSelectedTask()
{
    if (selectedCard_)
    {
        selectedCard_->Deselect();
        selectedCard_ = nullptr;
    }
}

// ====================================== Public Functions ======================================

Kanban::Column::Column(const string& name, sf::Vector2f initSize, WindowPromptManager& windowPromptManager, Kanban::Board& board) :
    name_(name), actionObserver_(this), taskObserver_(this)
{
    UpdateValues(initSize.y);
    UpdateRenderTexture(initSize.x);

    font_.loadFromFile(Utilities::fontPath);
    text_.setFont(font_);
    icons_.push_back(new Icon(Icon::Type::plus));
    icons_.push_back(new Icon(Icon::Type::dots));
    windowPromptManager_ = &windowPromptManager;
    board_ = &board;
    TaskSubject::AddObserver(windowPromptManager_->taskObserver_);
    selectedCard_ = nullptr;
}

Kanban::Column::~Column() {
    for (unsigned int i = 0; i < icons_.size(); i++)
        delete icons_[i];
    for (unsigned int i = 0; i < tasks_.size(); i++)
    {
        board_->ReturnTask(tasks_[i]->GetId());
        delete tasks_[i];
    }
}

void Kanban::Column::RemoveTaskCard(Kanban::TaskCard* card)
{
    for (auto iter = tasks_.begin(); iter != tasks_.end(); ++iter)
    {
        if ((*iter)->GetId() == card->GetId())
        {
            board_->ReturnTask(card->GetId());
            delete *iter;
            tasks_.erase(iter);
            return;
        }
    }
}

// bool Kanban::Column::AddTask(Task& task) {
//     for (unsigned int i = 0; i < tasks_.size(); i++)
//     {
//         if (tasks_[i]->getId() == task.getId())
//             return false;
//     }
//     tasks_.push_back(new Kanban::TaskCard(task));
//     return true;
// }

// bool Kanban::Column::RemoveTask(Kanban::TaskCard& task) {
//     for (auto iter = tasks_.begin(); iter != tasks_.end(); ++iter)
//     {
//         if ((*iter)->getId() == task.getId())
//         {
//             delete *iter;
//             tasks_.erase(iter);
//             return true;
//         }
//     }
//     return false;
// }

void Kanban::Column::SelectIcon(Icon::Type type) {
    switch (type)
    {
        case Icon::Type::plus:
            cout << "plus icon selected" << endl;
            windowPromptManager_->OnNotify(
                Observer::EventEnum::ShowPrompt,
                Observer::PromptEnum::AddTask,
                taskObserver_
            );
            break;
        case Icon::Type::dots:
            cout << "dots icon selected" << endl;
            windowPromptManager_->OnNotify(
                Observer::EventEnum::ShowPrompt,
                Observer::PromptEnum::Settings,
                actionObserver_
            );
            break;
        default:
            break;
    }
}

void Kanban::Column::SelectTaskCard(Kanban::TaskCard* card) {
    if (selectedCard_) selectedCard_->Deselect();

    selectedCard_ = card;
    card->Select();
    TaskSubject::Notify(Observer::EventEnum::ShowPrompt, card->GetTask());
}

void Kanban::Column::ProcessLeftClickReleased()
{
    if (scrollBarActive)
    {
        scrollBarActive = false;
        return;
    }

    // move task stuff here
}

void Kanban::Column::ProcessMouseMove(sf::Vector2f mousePosGlobal)
{
    if (!scrollBarActive)
        return;

    sf::Vector2f localPoint = transformStatic_.getInverse().transformPoint(mousePosGlobal);
    scrollMoveDelta_ = scrollBar_.Move(localPoint);
}

void Kanban::Column::Update(const float columnWidth, const float columnHeight, const float deltaTime)
{
    UpdateValues(columnHeight);
    UpdateRenderTexture(columnWidth);
    UpdateRendering(deltaTime);
    UpdateScrollBar(deltaTime);
}

bool Kanban::Column::CheckCollision(sf::Vector2f point)
{
    // transform point from global space to local space
    auto staticPoint = transformStatic_.getInverse().transformPoint(point);
    auto dynamicPoint = transformDynamic_.getInverse().transformPoint(point);
    dynamicPoint += sf::Vector2f(0, startRenderY_); // adjust by amount scrolled

    ClearSelectedTask();

    if (!bgRect_.getGlobalBounds().contains(staticPoint))
        return false;

    scrollBarActive = scrollBar_.CheckCollision(staticPoint);
    if (scrollBarActive)
    {
        board_->SetActiveColumn(this, Board::UserInputMode::ScrollBar);
        return true;
    }

    for (unsigned int i = 0; i < icons_.size(); i++)
    {
        if (icons_[i]->CheckCollision(staticPoint))
        {
            SelectIcon(icons_[i]->GetType());
            return true;
        }
    }
    for (unsigned int i = 0; i < tasks_.size(); i++)
    {
        if (tasks_[i]->CheckCollision(dynamicPoint))
        {
            SelectTaskCard(tasks_[i]);
            return true;
        }
    }

    return true;
}

void Kanban::Column::RenderIcons(sf::RenderTarget& target, sf::Vector2f basePos, int colWidth)
{
    const int iconCount = icons_.size();
    for (int i = 0; i < iconCount; i++)
    {
        const int iconWidth = icons_[i]->GetWidth();
        int bgXPadding = iconWidth / 2;
        int iconXPadding = iconWidth;

        int totalIconWidth = (bgXPadding * iconCount) + (iconXPadding * iconCount) + (iconWidth * iconCount);
        int x = basePos.x + colWidth - totalIconWidth;
        int y = basePos.y + iconWidth;

        int xOffset = (bgXPadding * (i+1)) + (iconXPadding * i) + (iconWidth * i);
        icons_[i]->Draw(x + xOffset, y, target);
    }
}

void Kanban::Column::Render(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target) {

    // we keep collisions and drawing in local space
    // and translate what we draw to global space
    // then when doing collision detection, we convert from global space to local

    textureDynamic_.clear(sf::Color::Transparent);
    RenderStaticDetails(position, size, target);

    if (!tasks_.empty())
    {
        float taskWidth = size.x * 0.75f;
        float taskOffsetX = size.x / 8.0f;
        float borderPaddingY = 0;//texturePaddingY_ / 2;
        for (unsigned int i = 0; i < tasks_.size(); i++)
        {
            float yOffset = borderPaddingY + (taskPaddingY_ + taskHeight_) * i;
            tasks_[i]->Draw(sf::Vector2f(taskOffsetX, yOffset),
                            sf::Vector2f(taskWidth, taskHeight_), {}, textureDynamic_);
        }
    }

    transformDynamic_ = sf::Transform::Identity; // reset
    transformDynamic_.translate(position.x, position.y + yHeader_);

    // draw column texture
    textureDynamic_.display();
    sf::Sprite sprite(textureDynamic_.getTexture());
    sprite.setTextureRect(sf::IntRect(0, startRenderY_, textureDynamic_.getSize().x, columnHeight_ - yHeader_));
    target.draw(sprite, transformDynamic_);
}
