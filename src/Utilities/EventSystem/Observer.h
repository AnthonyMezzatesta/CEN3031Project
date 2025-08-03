#pragma once
#include <vector>
#include "Task.h"

using std::vector;

namespace EventSystem
{
    struct Observer
    {
        enum EventEnum { TransferTask, Action, ShowPrompt, HidePrompt, Update, Activate, Deactivate };
        enum PromptEnum { AddTask, Settings };
        enum ActionEnum { Add, Rename, Move, Delete };
        virtual ~Observer() = 0;
    };
    inline Observer::~Observer() {}

    struct BasicObserver : public Observer
    {
        virtual ~BasicObserver() {}
        virtual void OnNotify(EventEnum event) = 0;
    };
    template <typename T>
    struct DataObserver : public Observer
    {
        virtual ~DataObserver() {}
        virtual void OnNotify(EventEnum event, const T& data) = 0;
    };
    struct WindowResizeObserver : public Observer
    {
        WindowResizeObserver(sf::View& view) : view_(&view) {}
        virtual ~WindowResizeObserver() {}
        virtual void OnNotify(EventEnum event, const sf::View& view)
        {
            // resize view, while keeping viewPort ratio intact
            auto viewPort = view_->getViewport();
            *view_ = view;
            view_->setViewport(viewPort);
        }
    protected:
        WindowResizeObserver() : view_(nullptr) {}
    private:
        sf::View* view_;
    };
}