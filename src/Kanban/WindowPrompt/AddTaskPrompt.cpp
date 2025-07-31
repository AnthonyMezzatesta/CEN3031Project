#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "Task.h"
#include "Utilities.h"
#include "Subject.h"
#include "../GUIElement/TaskOption.h"
#include "../Board/Board.h"
#include "AddTaskPrompt.h"

void AddTaskPrompt::UpdateScrollTexture(const float deltaTime)
{
    auto size = view_.getSize();

    int taskCount = taskElements_.size();
    int headerHeight = size.y * 0.15f;
    float taskHeight = (size.y - headerHeight) / (tasksPerScreen_ + 1.0f);
    float taskPaddingY = taskHeight / (tasksPerScreen_ + 1.0f);
    float extraTaskCount = std::max(taskCount - tasksPerScreen_, 0);

    float defaultStartY = 0;
    float maxDistY = (taskPaddingY + taskHeight) * extraTaskCount;
    float maxStartY = defaultStartY + maxDistY;

    bool enableScrollBar = taskElements_.size() > tasksPerScreen_;
    float defaultTextureHeight = size.y - headerHeight;
    float textureHeight = defaultTextureHeight + (taskPaddingY * (extraTaskCount+1) + taskHeight * extraTaskCount);
    float scrollBarWidthRatio = defaultTextureHeight / textureHeight;
    sf::Vector2u textureSize(size.x, textureHeight);

    scrollTexture_.Update(enableScrollBar, scrollBarWidthRatio,
        textureSize, {}, defaultStartY, maxStartY, deltaTime);
}

AddTaskPrompt::AddTaskPrompt(const sf::RenderWindow& target, Kanban::Board& board) {
    if (!font_.loadFromFile(Utilities::fontPath))
        throw std::runtime_error("could not load font");
    text_.setFont(font_);

    type_ = WindowPrompt::Type::AddTaskPrompt;
    board_ = &board;
    view_ = target.getDefaultView();
    view_.setViewport(viewPortLeft_);
    bg.setFillColor(Utilities::fill2);
}

AddTaskPrompt::~AddTaskPrompt() {
    ClearTaskElements();
}

void AddTaskPrompt::ClearTaskElements() {
    for (auto& kvp : taskElements_)
        delete kvp.second;
    taskElements_.clear();
}

// todo: add option to get tasks by filter
void AddTaskPrompt::Update(const float deltaTime) {
    if (isActive) isVisible = true;

    UpdateScrollTexture(deltaTime);

    // update window prompt with currently available tasks
    if (!isActive)
    {
        ClearTaskElements();
        auto allTasks = board_->GetAvailableTasks();
        for (Task& task : allTasks)
        {
            if (task.getId().has_value())
                taskElements_[task.getId().value()] = new Kanban::TaskOption(task);
        }
    }
}

void AddTaskPrompt::Deactivate() {
    isActive = false;
    isVisible = false;
    RemoveAllObservers();
}

void AddTaskPrompt::ProcessLeftClickReleased()
{
    if (!isVisible)
        return;

    if (!scrollTexture_.ProcessLeftClickReleased())
        return;

    // // deactivate prompt if released click outside of it
    // if (isActive && isVisible)
    //     Deactivate();

}

void AddTaskPrompt::ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target)
{
    if (!isVisible)
        return;

    auto mousePos = target.mapPixelToCoords(pixelPos, view_);
    if (scrollTexture_.ProcessMouseMove(mousePos))
        return;
}

bool AddTaskPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point) {
    // transform point from global space to local view space
    auto mousePos = target.mapPixelToCoords(point, view_);

    // // place view on screen based on position of mouse in target's space
    // if (point.x > target.getSize().x / 2)
    //     view_.setViewport(viewPortLeft_);
    // else
    //     view_.setViewport(viewPortRight_);

    // exit early if not visible
    if (!isVisible || !bg.getGlobalBounds().contains(mousePos))
    {
        // if point was outside of bounds...
        if (isActive && isVisible)
            Deactivate();

        return false;
    }

    if (scrollTexture_.CheckScrollBarCollision(mousePos))
        return true;

    // transform point from view space to local space
    auto dynamicPoint = transformDynamic_.getInverse().transformPoint(mousePos);
    dynamicPoint.y += scrollTexture_.GetValue(); // adjust by amount scrolled
    for (auto iter = taskElements_.begin(); iter != taskElements_.end(); ++iter)
    {
        if (iter->second->CheckCollision(dynamicPoint))
        {
            // send task to observers
            Notify(EventSystem::Observer::TransferTask, iter->second->GetTask());
            taskElements_.erase(iter);
            delete iter->second;
            return true;
        }
    }

    return true;
}

void AddTaskPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    target.setView(view_);
    auto size = target.getSize();
    int headerHeight = size.y * 0.15f;

    // static elements
    bg.setSize(sf::Vector2f(size.x, size.y));
    target.draw(bg);

    // draw title text
    string titleStr = "Available Tasks";
    Utilities::DrawText(target, text_, sf::Vector2f(size.x, headerHeight),
        {}, titleStr, headerHeight/2, Utilities::textColor);

    // draw horizontal line
    sf::RectangleShape line(sf::Vector2f(size.x * 0.5f, headerHeight * 0.05f));
    line.setPosition(sf::Vector2f(size.x * 0.25f, headerHeight));
    line.setFillColor(Utilities::fill1);
    target.draw(line);


    // dynamic elements
    if (taskElements_.empty())
        return;

    auto& textureDynamic = scrollTexture_.GetTexture();
    textureDynamic.clear(sf::Color::Transparent);

    transformDynamic_ = sf::Transform::Identity; // reset
    transformDynamic_.translate(0, headerHeight);

    float taskWidth = size.x * 0.75f;
    float xOffset = size.x / 8.0f;
    float taskHeight = (size.y - headerHeight) / (tasksPerScreen_ + 1.0f);
    float yPadding = taskHeight / (tasksPerScreen_ + 1.0f);
    int i = 0;
    for (auto& kvp : taskElements_)
    {
        float yOffset = yPadding * (i+1) + taskHeight * i;
        kvp.second->Draw(sf::Vector2f(xOffset, yOffset),
                           sf::Vector2f(taskWidth, taskHeight), {}, textureDynamic);
        i++;
    }

    // draw scrollable texture
    float defaultTextureHeight = size.y - headerHeight;
    sf::IntRect rect(0, scrollTexture_.GetValue(), textureDynamic.getSize().x, defaultTextureHeight);
    scrollTexture_.DrawTexture(target, rect, transformDynamic_);

    // draw scroll bar
    int taskStartY = headerHeight + yPadding;
    sf::Vector2f barPos(size.x, taskStartY);
    sf::Vector2f barSize(size.y - taskStartY, (size.x - taskWidth)/4.f);
    scrollTexture_.DrawScrollBar(target, barPos, barSize, 90);

    target.setView(target.getDefaultView());
}
