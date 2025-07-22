#pragma once
#include <vector>
#include "Task.h"

using std::vector;

namespace EventSystem
{
    struct Observer
    {
        enum EventEnum { TransferTask, Action, ShowPrompt, HidePrompt, Update };
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
        virtual void OnNotify(EventEnum event, T& data) = 0;
    };
}