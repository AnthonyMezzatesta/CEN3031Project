#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Task.h"
#include "Board.h"
#include "../GUIElement/GUIElement.h"
#include "../GUIElement/Icon.h"
#include "../GUIElement/TaskCard.h"
#include "../WindowPrompt/WindowPromptManager.h"
#include "Utilities.h"
#include "Subject.h"
#include "Column.h"

using namespace std;
using namespace EventSystem;

// ===================================== Private Functions =====================================

void Kanban::Column::ActionObserver::OnNotify(Observer::EventEnum event, const Observer::ActionEnum& action)
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

void Kanban::Column::TaskObserver::OnNotify(Observer::EventEnum event, const Task& task)
{
    if (event == Observer::EventEnum::TransferTask)
    {
        column_->board_->SetTaskAsTaken(task);
        column_->tasks_.push_back(new Kanban::TaskCard(column_, task));
    }
}

void Kanban::Column::UpdateValues(float width, float height)
{
    columnHeight_ = height;
    taskWidth_ = width * 0.75f;
    taskOffsetX_ = width / 8.0f;
    headerHeight_ = icons_[0]->GetWidth() * 1.5f;
    taskHeight_ = (columnHeight_ - headerHeight_) / (tasksPerColummn_ + 1.0f);
    taskPaddingY_ = taskHeight_ / (tasksPerColummn_ + 1.0f);
    defaultTextureHeight_ = columnHeight_ - (taskPaddingY_ + headerHeight_);
}

void Kanban::Column::UpdateRenderTexture(const float width)
{
    if (textureStatic_.getSize().x != width || textureStatic_.getSize().y != columnHeight_)
    {
        if (!textureStatic_.create(width, columnHeight_))
            std::cerr << "could not create static render texture in Column.Render()" << endl;
    }
}

void Kanban::Column::UpdateScrollTexture(const float columnWidth, const float deltaTime)
{
    int taskCount = tasks_.size();
    float defaultStartY = 0;
    float extraTaskCount = std::max(taskCount - tasksPerColummn_, 0);
    float maxDistY = (taskPaddingY_ + taskHeight_) * extraTaskCount;
    float maxStartY = defaultStartY + maxDistY;

    bool enableScrollBar = tasks_.size() > tasksPerColummn_;
    float textureHeight_ = defaultTextureHeight_ + maxDistY;
    float scrollBarWidthRatio = defaultTextureHeight_ / textureHeight_;
    sf::Vector2u textureSize(columnWidth, textureHeight_);

    scrollTexture_.Update(enableScrollBar, scrollBarWidthRatio,
        textureSize, {}, defaultStartY, maxStartY, deltaTime);
}

void Kanban::Column::RenderStaticDetails(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target)
{
    textureStatic_.clear(sf::Color::Transparent);

    // draw background
    bgRect_.setFillColor(Utilities::fill1);
    bgRect_.setSize(sf::Vector2f(size.x, columnHeight_));
    textureStatic_.draw(bgRect_);

    // draw column title
    int titleWidth = size.x;
    int titleHeight = headerHeight_;
    sf::Vector2f textSize(titleWidth, titleHeight);
    Utilities::DrawText(textureStatic_, text_, textSize, {}, name_, titleHeight / 2, Utilities::textColor);

    RenderIcons(textureStatic_, {}, size.x);

    // draw scroll bar
    int taskStartY = headerHeight_ + taskPaddingY_;
    sf::Vector2f barPos(size.x, taskStartY);
    sf::Vector2f barSize(defaultTextureHeight_ - taskPaddingY_, (size.x - taskWidth_)/4.f);
    scrollTexture_.DrawScrollBar(textureStatic_, barPos, barSize, 90);

    transformStatic_ = sf::Transform::Identity;
    transformStatic_.translate(position);

    textureStatic_.display();
    target.draw(sf::Sprite(textureStatic_.getTexture()), transformStatic_);
}

void Kanban::Column::RenderDynamicDetails(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target)
{
    auto& textureDynamic = scrollTexture_.GetTexture();
    textureDynamic.clear(sf::Color::Transparent);

    if (!tasks_.empty())
    {
        float borderPaddingY = taskPaddingY_;
        for (unsigned int i = 0; i < tasks_.size(); i++)
        {
            float yOffset = borderPaddingY + (taskPaddingY_ + taskHeight_) * i;
            tasks_[i]->Draw(sf::Vector2f(taskOffsetX_, yOffset),
                            sf::Vector2f(taskWidth_, taskHeight_), {}, textureDynamic);
        }
    }

    transformDynamic_ = sf::Transform::Identity; // reset
    transformDynamic_.translate(position.x, position.y + headerHeight_);

    // draw column texture
    sf::IntRect rect(0, scrollTexture_.GetScrollDelta(), textureDynamic.getSize().x, defaultTextureHeight_);
    scrollTexture_.DrawTexture(target, rect, transformDynamic_);
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

Kanban::Column::Column(const string& name,
    WindowPromptManager& windowPromptManager, Kanban::Board* board) :
    name_(name), actionObserver_(this), taskObserver_(this)
{
    font_.loadFromFile(Utilities::fontPath);
    text_.setFont(font_);
    icons_.push_back(new Icon(Icon::Type::plus));
    icons_.push_back(new Icon(Icon::Type::dots));
    windowPromptManager_ = &windowPromptManager;
    board_ = board;
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
            windowPromptManager_->ShowPrompt(
                Observer::EventEnum::ShowPrompt,
                Observer::PromptEnum::AddTask,
                taskObserver_,
                board_->GetAvailableTasks()
            );
            break;
        case Icon::Type::dots:
            windowPromptManager_->ShowPrompt(
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
    scrollTexture_.ProcessLeftClickReleased();

    // move task stuff here
}

void Kanban::Column::ProcessMouseMove(sf::Vector2f mousePosGlobal)
{
    sf::Vector2f localPoint = transformStatic_.getInverse().transformPoint(mousePosGlobal);
    scrollTexture_.ProcessMouseMove(localPoint);
}

void Kanban::Column::Update(const float screenWidth, const float columnWidth, const float columnHeight, const float deltaTime)
{
    for (auto* icon : icons_)
        icon->Update(screenWidth);

    for (auto* card : tasks_)
        card->UpdateIcons(screenWidth);

    UpdateValues(columnWidth, columnHeight);
    UpdateRenderTexture(columnWidth);
    UpdateScrollTexture(columnWidth, deltaTime);
}

bool Kanban::Column::CheckCollision(sf::Vector2f point)
{
    // transform point from global space to local space
    auto staticPoint = transformStatic_.getInverse().transformPoint(point);
    auto dynamicPoint = transformDynamic_.getInverse().transformPoint(point);
    dynamicPoint += sf::Vector2f(0, scrollTexture_.GetScrollDelta()); // adjust by amount scrolled

    ClearSelectedTask();

    if (!bgRect_.getGlobalBounds().contains(staticPoint))
        return false;

    if (scrollTexture_.CheckScrollBarCollision(staticPoint))
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
        int iconPadding = iconWidth / 4;

        int totalIconWidth = (iconPadding + iconWidth) * iconCount;
        int x = basePos.x + colWidth - totalIconWidth;
        int y = basePos.y + iconPadding;

        int xOffset = (iconPadding + iconWidth) * i;
        icons_[i]->Draw(x + xOffset, y, target);
    }
}

void Kanban::Column::Render(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target) {

    // we keep collisions and drawing in local space
    // and translate what we draw to global space
    // then when doing collision detection, we convert from global space to local


    RenderStaticDetails(position, size, target);
    RenderDynamicDetails(position, size, target);
}
