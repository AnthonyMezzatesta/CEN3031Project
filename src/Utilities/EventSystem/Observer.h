#pragma once
#include <vector>
#include "Task.h"

using std::vector;

namespace EventSystem
{
    struct Observer
    {
        enum Event { AddTask, Action, ShowPrompt, HidePrompt };
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
        virtual void OnNotify(Event event, vector<Task>& data) = 0;
    };
    struct ActionObserver : public Observer
    {
        enum Action { Rename, Move, Delete };
        virtual ~ActionObserver() {}
        virtual void OnNotify(Event event, Action action) = 0;
    };

    template <typename T>
    struct ColumnPromptObserver : public Observer
    {
        enum Prompt { AddTask, Settings };
        virtual ~ColumnPromptObserver() {}
        virtual void OnNotify(Event event, Prompt promptType, T& observer) = 0;
    };
    // struct ColumnPromptTaskObserver : public ColumnPromptObserver
    // {
    //     virtual ~ColumnPromptTaskObserver() {}
    // };
    // struct ColumnPromptConfigObserver : public ColumnPromptObserver
    // {
    //     virtual ~ColumnPromptConfigObserver() {}
    //     virtual void OnNotify(Event event, Prompt promptType, ActionObserver& observer) = 0;
    // };
}