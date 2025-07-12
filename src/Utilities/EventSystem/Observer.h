#pragma once
#include <vector>
#include "Task.h"

using std::vector;

namespace EventSystem
{
    struct Observer
    {
        enum Event { AddTask, ShowPrompt, HidePrompt };
        virtual ~Observer() = 0;
    };
    inline Observer::~Observer() {}

    struct BasicObserver : public Observer
    {
        virtual ~BasicObserver() {}
        virtual void OnNotify(Event event) = 0;
    };
    struct TaskObserver : public Observer
    {
        virtual ~TaskObserver() {}
        virtual void OnNotify(Event event, vector<Task>& tasks) = 0;
    };
    struct ColumnPromptObserver : public Observer
    {
        enum Prompt { AddTask };
        virtual ~ColumnPromptObserver() {}
        virtual void OnNotify(Event event, Prompt promptType, TaskObserver* taskObserver) = 0;
    };
}