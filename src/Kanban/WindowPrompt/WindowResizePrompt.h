#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "WindowPrompt.h"
#include "../GUIElement/NamedButton.h"
using std::string;
using std::vector;

class WindowResizePrompt final : public WindowPrompt
{
    struct ToggleBox
    {
        sf::RectangleShape box_;
        sf::RectangleShape check;
        bool drawCheck_ = true;

        ToggleBox()
        {
            box_.setFillColor(Utilities::fill3);
            check.setFillColor(Utilities::fill1);
        }

        float GetWidth() const { return box_.getSize().x; }
        bool CheckCollision(sf::Vector2f point, WindowResizeHandler& windowResizeHandler)
        {
            if (!box_.getGlobalBounds().contains(point))
                return false;

            drawCheck_ = !drawCheck_;
            return true;
        }

        void Draw(sf::RenderTarget& target, sf::Vector2f basePos)
        {
            auto size = target.getSize();

            box_.setSize(sf::Vector2f(size.x * 0.025f, size.x * 0.025f));
            box_.setPosition(basePos);
            target.draw(box_);

            if (!drawCheck_)
                return;

            auto boxSize = box_.getSize();
            check.setSize(boxSize * 0.75f);
            check.setPosition(basePos + boxSize * 0.125f);
            target.draw(check);
        }
    } toggleBox_;

    Kanban::NamedButton saveButton_;
    Kanban::NamedButton cancelButton_;

    struct Option : public Kanban::GUIElement
    {
        sf::VideoMode mode_;

        Option(const sf::VideoMode& mode) : GUIElement(Utilities::fill1), mode_(mode) {}
        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
        {
            string name = std::to_string(mode_.width) +
                          " x " +
                          std::to_string(mode_.height);
            Utilities::DrawText(target, textObj, size, basePos, name, size.y / 4, Utilities::textColor);
        }
    };
    vector<Option*> options_;
    Option* selectedOption_ = nullptr;

    sf::Font font_;
    sf::Text text_;
    sf::RectangleShape bg;

    int headerHeight_;
    int footerHeight_;
    float optionWidth_;
    float xOffset_;
    float optionHeight_;
    float optionPaddingY_;
    float defaultTextureHeight_;

    const static int optionsPerScreen_ = 4;
    ScrollableTexture scrollTexture_;
    sf::Transform transformDynamic_;

    WindowResizeHandler* windowResizeHandler_;

    void UpdateValues()
    {
        auto size = view_.getSize();

        headerHeight_ = size.y * 0.15f;
        footerHeight_ = headerHeight_;
        optionWidth_ = size.x * 0.75f;
        xOffset_ = size.x / 8.0f;
        optionHeight_ = (size.y - headerHeight_ - footerHeight_) / (optionsPerScreen_ + 1.0f);
        optionPaddingY_ = optionHeight_ / (optionsPerScreen_ + 1.0f);
        defaultTextureHeight_ = size.y - (optionPaddingY_ + headerHeight_) * 2;

    }
    void UpdateScrollTexture(const float deltaTime)
    {
        auto size = view_.getSize();

        int optionCount = options_.size();
        float extraOptionCount = std::max(optionCount - optionsPerScreen_, 0);

        float defaultStartY = 0;
        float maxDistY = (optionPaddingY_ + optionHeight_) * extraOptionCount;
        float maxStartY = defaultStartY + maxDistY;

        bool enableScrollBar = options_.size() > optionsPerScreen_;
        float textureHeight = defaultTextureHeight_ + maxDistY;
        float scrollBarWidthRatio = defaultTextureHeight_ / textureHeight;
        sf::Vector2u textureSize(size.x, textureHeight);

        scrollTexture_.Update(enableScrollBar, scrollBarWidthRatio,
            textureSize, {}, defaultStartY, maxStartY, deltaTime);
    }
    
    void DrawStaticElements(sf::RenderTarget& target)
    {
        auto size = target.getSize();

        bg.setSize(sf::Vector2f(size.x, size.y));
        target.draw(bg);
        
        // draw title text
        string titleStr = "Resolutions";
        Utilities::DrawText(target, text_, sf::Vector2f(size.x, headerHeight_),
            sf::Vector2f(0, headerHeight_ / 8.f), titleStr, headerHeight_/2, Utilities::textColor);

        // draw horizontal line
        sf::RectangleShape rect(sf::Vector2f(size.x * 0.5f, headerHeight_ * 0.05f));
        rect.setPosition(sf::Vector2f(size.x * 0.25f, headerHeight_ * 0.95f));
        rect.setFillColor(Utilities::fill1);
        target.draw(rect);

        // ==== draw settings ====

        const int buttonCount = 2;
        float xPosOff = + toggleBox_.GetWidth() / 2;
        sf::Vector2f buttonSize(toggleBox_.GetWidth() * 3, toggleBox_.GetWidth());
        sf::Vector2f footerRegionSize(((buttonCount+1) * xPosOff) + (buttonCount * buttonSize.x), footerHeight_ * 0.9f);
        sf::Vector2f footerRegionPos(size.x - footerRegionSize.x - xOffset_, size.y - footerHeight_);

        // draw footer region bg
        sf::Vector2f footerBGSize(optionWidth_, footerRegionSize.y);
        sf::Vector2f footerBGPos(xOffset_, footerRegionPos.y);
        rect.setSize(footerBGSize);
        rect.setPosition(footerBGPos);
        rect.setFillColor(Utilities::fill0);
        target.draw(rect);

        // rect.setFillColor(Utilities::fill0);
        // rect.scale(0.5, 0.5);
        // target.draw(rect);

        // draw fullscreen/windowed toggle
        float boxPosX = footerRegionPos.x + xPosOff;
        sf::Vector2f textPos(boxPosX + footerRegionSize.x / 5, footerRegionPos.y);
        Utilities::DrawText(target, text_, footerRegionSize / 2.f,
            textPos, "Windowed", footerRegionSize.y / 4, Utilities::textColor);
        sf::Vector2f boxPos(boxPosX, footerRegionPos.y + text_.getLocalBounds().height / 2);
        toggleBox_.Draw(target, boxPos);

        // draw save button
        float yPosOff = (footerRegionSize.y - (text_.getLocalBounds().height / 2 + toggleBox_.GetWidth()));
        sf::Vector2f buttonPos(boxPos.x, footerRegionPos.y + yPosOff);
        saveButton_.Draw(buttonPos, buttonSize, {}, target);

        // draw cancel button
        buttonPos = sf::Vector2f(buttonPos.x + buttonSize.x + xPosOff, buttonPos.y);
        cancelButton_.Draw(buttonPos, buttonSize, {}, target);
    }

    void DrawDynamicElements(sf::RenderTarget& target)
    {
        auto size = target.getSize();
        
        auto& textureDynamic = scrollTexture_.GetTexture();
        textureDynamic.clear(sf::Color::Transparent);

        transformDynamic_ = sf::Transform::Identity; // reset
        transformDynamic_.translate(0, headerHeight_ + optionPaddingY_);
        
        int optionCount = std::size(options_);
        for (int i = 0; i < optionCount; i++)
        {
            float yOffset = (optionPaddingY_ + optionHeight_) * i;
            options_[i]->Draw(sf::Vector2f(xOffset_, yOffset),
                              sf::Vector2f(optionWidth_, optionHeight_), {}, textureDynamic);
        }

        // draw scrollable texture
        sf::IntRect rect(0, scrollTexture_.GetScrollDelta(), textureDynamic.getSize().x, defaultTextureHeight_);
        scrollTexture_.DrawTexture(target, rect, transformDynamic_);

        // draw scroll bar
        int taskStartY = headerHeight_ + optionPaddingY_;
        sf::Vector2f barSize(size.y - taskStartY * 2.f, (size.x - optionWidth_)/8.0f);
        sf::Vector2f barPos(size.x - xOffset_ + barSize.y + optionPaddingY_, taskStartY);
        scrollTexture_.DrawScrollBar(target, barPos, barSize, 90);
    }
public:
    WindowResizePrompt(const sf::RenderWindow& target, Kanban::Board& board, WindowResizeHandler& windowResizeHandler) :
        WindowPrompt(windowResizeHandler), saveButton_("Save"), cancelButton_("Cancel") {
        if (!font_.loadFromFile(Utilities::fontPath))
            throw std::runtime_error("could not load font");
        text_.setFont(font_);

        type_ = WindowPrompt::Type::WindowResizePrompt;
        board_ = &board;
        view_ = target.getDefaultView();
        view_.setViewport(viewPortLeft_);
        bg.setFillColor(Utilities::fill2);
        windowResizeHandler_ = &windowResizeHandler;

        // since modes is sorted from best to worst resolution,
        // we're filling options in reverse order for stylistic purposes when drawing
        const auto& modes = sf::VideoMode::getFullscreenModes();
        options_.resize(modes.size());
        for (unsigned int i = 0; i < modes.size(); i++)
            options_[i] = new Option(modes[modes.size()-i-1]);
    }

    ~WindowResizePrompt()
    {
        for (auto* o : options_)
            delete o;
    }

    void Update(const float deltaTime)
    {
        if (isActive) isVisible = true;

        UpdateValues();
        UpdateScrollTexture(deltaTime);
    }

    void Deactivate() {
        if (selectedOption_)
            selectedOption_->Deselect();
        selectedOption_ = nullptr;
        isActive = false;
        isVisible = false;
    }

    void ProcessLeftClickReleased()
    {
        if (!isVisible)
            return;

        scrollTexture_.ProcessLeftClickReleased();
    }

    void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target)
    {
        if (!isVisible)
            return;

        auto mousePos = target.mapPixelToCoords(pixelPos, view_);
        scrollTexture_.ProcessMouseMove(mousePos);
    }
    
    bool CheckCollision(sf::RenderWindow& window, sf::Vector2i point) {
        // transform point from global space to local view space
        auto mousePos = window.mapPixelToCoords(point, view_);

        // exit early if not visible
        if (!isVisible || !bg.getGlobalBounds().contains(mousePos))
        {
            // if point was outside of bounds...
            if (isActive && isVisible)
                Deactivate();

            return false;
        }

        if (toggleBox_.CheckCollision(mousePos, *windowResizeHandler_))
            return true;

        if (saveButton_.CheckCollision(mousePos) && selectedOption_)
        {
            // set selected screen resolution
            WindowResizeHandler::WindowType type = toggleBox_.drawCheck_ ?
                WindowResizeHandler::WindowType::Window : WindowResizeHandler::WindowType::Fullscreen;
            windowResizeHandler_->ResizeWindowOnSelection(window, selectedOption_->mode_, type);
            return true;
        }
        if (cancelButton_.CheckCollision(mousePos))
        {
            Deactivate();
            return true;
        }

        if (scrollTexture_.CheckScrollBarCollision(mousePos))
            return true;

        // transform point from view space to local texture space
        auto dynamicPoint = transformDynamic_.getInverse().transformPoint(mousePos);
        dynamicPoint.y += scrollTexture_.GetScrollDelta(); // adjust by amount scrolled

        for (unsigned int i = 0; i < options_.size(); i++)
        {
            if (options_[i]->CheckCollision(dynamicPoint))
            {
                if (selectedOption_)
                    selectedOption_->Deselect();
                selectedOption_ = options_[i];
                selectedOption_->Select();
                return true;
            }
        }

        return true;
    }

    void Draw(sf::RenderTarget& target)
    {
        if (!isVisible)
            return;

        target.setView(view_);
        
        DrawStaticElements(target);
        DrawDynamicElements(target);
        
        target.setView(target.getDefaultView());
    }
};
