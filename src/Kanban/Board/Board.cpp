#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "Task.h"
#include "TaskManager.h"
#include "Board.h"
using namespace std;

// ===================================== Private Functions =====================================

void Kanban::Board::UpdateColumnValues(const sf::RenderWindow& window)
{
    auto targetSize = window.getSize();

    // colPerScreen+1 is the number of columns that fit in one screen, if drawn side-by-side
    // where the +1 column is the extra space that provides even padding between the columns
    colWidth_ =  targetSize.x / (colPerScreen + 1);
    colHeight_ = targetSize.y * 0.75f;
    colPaddingX_ = colWidth_ / (colPerScreen + 1);
    colPosY_ = (targetSize.y - colHeight_) / 2;
}

void Kanban::Board::UpdateScrollTexture(const sf::RenderWindow& window, const float deltaTime)
{
    // for updating render texture size
    auto targetSize = window.getSize();
    const int colCount = columns_.size();
    // calculating width of all columns and their padding,
    // plus another column and padding for the addColumnButton
    unsigned int rendTextWidth = (colCount+2) * colPaddingX_ + colWidth_ * (colCount+1);
    rendTextWidth = max(rendTextWidth, targetSize.x);
    sf::Vector2u textureSize(rendTextWidth, targetSize.y);

    // for updating scroll bar
    bool enableScrollBar = columns_.size() >= colPerScreen;
    float scrollBarWidthRatio = window.getSize().x / static_cast<float>(textureSize.x);

    // for updating boardView pos
    float defaultCenterX = window.getSize().x/2.f;
    float maxDistX = (columns_.size() - colPerScreen + 1) * (colWidth_ + colPaddingX_);
    float maxCenterX = defaultCenterX + maxDistX;

    scrollTexture_.Update(enableScrollBar, scrollBarWidthRatio, textureSize,
        boardView.getCenter().x, defaultCenterX, maxCenterX, deltaTime);

    boardView.setCenter(scrollTexture_.GetScrollDelta(), boardView.getCenter().y);
}

void Kanban::Board::DrawColumns(sf::RenderWindow& window) {
    const int colCount = columns_.size();
    unsigned int rendTextWidth = (colCount+2) * colPaddingX_ + colWidth_ * (colCount+1);

    // drawing to render texture to allow for off-screen drawing
    auto& renderTexture = scrollTexture_.GetTexture();
    renderTexture.clear(sf::Color::Transparent);

    // draw background
    sf::RectangleShape r(sf::Vector2f(renderTexture.getSize().x, renderTexture.getSize().y));
    r.setFillColor(Utilities::fill0);
    renderTexture.draw(r);

    // draw columns
    for (int i = 0; i < colCount; i++)
    {
        int xOffset = (i+1) * colPaddingX_ + colWidth_ * i;
        columns_[i]->Render(sf::Vector2f(xOffset, colPosY_), sf::Vector2f(colWidth_, colHeight_), renderTexture);
    }

    // draw add column button at next column pos
    int addColumnPosX = rendTextWidth - colWidth_ - colPaddingX_;
    addColumnButton_.Draw(sf::Vector2f(addColumnPosX, colPosY_), sf::Vector2f(colWidth_, colHeight_), {}, renderTexture);

    // draw render texture within coordinate space of boardView
    window.setView(boardView);
    scrollTexture_.DrawTexture(window);
    window.setView(window.getDefaultView());
}

void Kanban::Board::DrawScrollBar(sf::RenderWindow& window)
{
    window.setView(boardView);

    float windowWidth = window.getSize().x;
    float textureHeight = scrollTexture_.GetTexture().getSize().y;
    float x = window.getView().getCenter().x - windowWidth/2.f; // move scroll bar along with view
    sf::Vector2f pos(x, textureHeight * 0.9625f);
    sf::Vector2f size(windowWidth, textureHeight * 0.0375f);

    scrollTexture_.DrawScrollBar(window, pos, size);

    window.setView(window.getDefaultView());
}

void Kanban::Board::DrawIcons(sf::RenderWindow& window)
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

// ====================================== Public Functions ======================================

Kanban::Board::Board(const sf::RenderWindow& target, TaskManager& taskManager,
    ReminderManager& reminderManager, WindowResizeHandler& windowResizeHandler) :
    boardView(target.getDefaultView()), windowResizeObserver_(boardView),
    activeColumn(nullptr), userInputMode(UserInputMode::Default)
{
    windowPromptManager_ = new WindowPromptManager(target, *this, reminderManager, windowResizeHandler);
    taskManager_ = &taskManager;
    windowResizeHandler.AddObserver(windowResizeObserver_);

    UpdateColumnValues(target);

    for (unsigned int i = 0; i < std::size(iconArray); i++)
    {
        icons_.push_back(new Icon(iconArray[i]));

        // link notification bell icon with reminder manager
        if (iconArray[i] == Icon::Type::bell)
            reminderManager.AddObserver(*icons_[i]);
    }

    // boardView.setViewport(sf::FloatRect(0.1f, 0.15f, 0.75f, 0.75f));
}

Kanban::Board::~Board() {
    delete windowPromptManager_;
    for (unsigned int i = 0; i < icons_.size(); i++)
        delete icons_[i];
    for (unsigned int i = 0; i < columns_.size(); i++)
        delete columns_[i];
}

void Kanban::Board::AddColumn(const string& name = "title")
{
    columns_.push_back(new Column(name, *windowPromptManager_, *this));
}

void Kanban::Board::RemoveColumn(Column& column)
{
    for (auto iter = columns_.begin(); iter != columns_.end(); ++iter)
    {
        if (*iter == &column)
        {
            delete *iter;
            columns_.erase(iter);
            return;
        }
    }
}

void Kanban::Board::SetActiveColumn(Column* column, UserInputMode mode)
{
    activeColumn = column;
    userInputMode = mode;
}

void Kanban::Board::SetTaskAsTaken(const Task& task)
{
    if (task.getId().has_value())
    {
        int id = task.getId().value();
        auto kvp = taskIds_.find(id);
        if (kvp != taskIds_.end() && kvp->second == Available)
            taskIds_[id] = Taken;
    }
}

void Kanban::Board::ReturnTask(std::optional<int> id)
{
    if (id.has_value() && taskIds_.find(id.value()) != taskIds_.end())
        taskIds_[id.value()] = Available;
}

vector<Task> Kanban::Board::GetAvailableTasks() const
{
    vector<Task> tasks;
    for (auto& kvp : taskIds_)
    {
        if (kvp.second == Available)
        {
            auto id = taskManager_->getTask(kvp.first);
            if (id.has_value())
                tasks.push_back(id.value());
        }
    }

    return tasks;
}

void Kanban::Board::ProcessLeftClickReleased()
{
    windowPromptManager_->ProcessLeftClickReleased();

    if (!scrollTexture_.ProcessLeftClickReleased())
    {
        // OnPressed clears active column and sets a new one, if viable
        // OnRelease clears active column for appropriate input modes
        /** thus, upon selecting to rename...
         * In CheckCollision(), active column is cleared then set
         * on click release, active column is not reset
         * user types a new name for column
         * upon clicking or pressing the enter key, active column is reset
         */
        if (userInputMode == UserInputMode::ColumnName)
            return;

        userInputMode = Default;
        if (activeColumn)
            activeColumn->ProcessLeftClickReleased();
        activeColumn = nullptr;
    }
}

void Kanban::Board::ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target)
{
    windowPromptManager_->ProcessMouseMove(pixelPos, target);

    auto mousePos = target.mapPixelToCoords(pixelPos, boardView);
    if (scrollTexture_.ProcessMouseMove(mousePos))
        return;

    if (activeColumn && userInputMode == UserInputMode::ScrollBar)
        activeColumn->ProcessMouseMove(mousePos);
}

void Kanban::Board::ReadUserInput(char c)
{
    if (userInputMode != UserInputMode::ColumnName)
        return;

    // std::cout << "ASCII character typed: " << c << std::endl;

    if (c == '\t')
        return;

    if (c == '\b')
        userInputStr.pop_back();
    else if (c == '\n')
    {
        if (activeColumn)
        {
            activeColumn = nullptr;
            userInputStr.clear();
            userInputMode = UserInputMode::Default;
            return;
        }
    }
    else
        userInputStr.push_back(c);

    if (activeColumn)
        activeColumn->SetName(userInputStr);
}

// todo: add functionality to delete tasks that were removed from TaskManager
void Kanban::Board::Update(const sf::RenderWindow& window, const float deltaTime)
{
    UpdateColumnValues(window);
    UpdateScrollTexture(window, deltaTime);

    for (Column* col : columns_)
        col->Update(window.getSize().x, colWidth_, colHeight_, deltaTime);

    // add new tasks
    auto allTasks = taskManager_->getAllTasks();
    for (Task& task : allTasks)
    {
        if (task.getId().has_value() && taskIds_.find(task.getId().value()) == taskIds_.end())
            taskIds_[task.getId().value()] = Available;
    }

    windowPromptManager_->UpdatePrompts(deltaTime);
}

bool Kanban::Board::CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
{
    // reset active column
    if (activeColumn)
    {
        activeColumn = nullptr;
        userInputMode = UserInputMode::Default;
        userInputStr.clear();
    }

    auto mousePos = target.mapPixelToCoords(point, boardView);

    if (scrollTexture_.CheckScrollBarCollision(mousePos))
        return true;

    if (windowPromptManager_->CheckCollision(point, target))
        return true;

    if (addColumnButton_.CheckCollision(mousePos))
    {
        cout << "adding column" << endl;
        AddColumn();
        return true;
    }

    for (unsigned int i = 0; i < icons_.size(); i++)
    {
        if (icons_[i]->CheckCollision(static_cast<sf::Vector2f>(point)))
        {
            if (icons_[i]->GetType() == Icon::bell)
                windowPromptManager_->ShowPrompt(WindowPrompt::Type::ReminderPrompt);
            else if (icons_[i]->GetType() == Icon::dots)
                windowPromptManager_->ShowPrompt(WindowPrompt::Type::WindowResizePrompt);

            return true;
        }
    }

    for (unsigned int i = 0; i < columns_.size(); i++)
    {
        if (columns_[i]->CheckCollision(mousePos))
            return true;
    }

    return false;
}

void Kanban::Board::Draw(sf::RenderWindow& window)
{
    DrawColumns(window);
    DrawScrollBar(window);
    DrawIcons(window);
    windowPromptManager_->Draw(window);
}