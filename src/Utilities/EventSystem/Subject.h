#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "Observer.h"

using std::vector;
using std::queue;

namespace EventSystem
{
    template <typename ObserverType = BasicObserver>
    struct Subject
    {
        Subject()
        {
            for (int i = 0; i < nodePoolMaxSize; i++)
                nodePool.push(new Node);
        }
        virtual ~Subject()
        {
            RemoveAllObservers();
            for (; !nodePool.empty(); nodePool.pop())
                delete nodePool.front();
        }
        bool AddObserver(ObserverType& observer)
        {
            Node* node = GetFromPool();
            if (!node)
            {
                std::cerr << "ran out of nodes in nodePool" << std::endl;
                return false;
            }

            node->observer_ = &observer;

            if (!head)
            {
                head = node;
                node->next_ = nullptr;
                return true;
            }

            node->next_ = head;
            head = node;
            return true;
        }
        bool RemoveObserver(ObserverType& observer)
        {
            Node* prev = head;
            Node* curr = head;
            while (curr)
            {
                if (curr->observer_ == &observer)
                {
                    if (curr == head)
                        head = nullptr;
                    prev->next_ = curr->next_;
                    curr->observer_ = nullptr;
                    curr->next_ = nullptr;
                    ReturnToPool(curr);
                    return true;
                }

                prev = curr;
                curr = curr->next_;
            }
            return false;
        }
    protected:
        struct Node
        {
            ObserverType* observer_;
            Node* next_;
        };
        Node* head = nullptr;
        // todo: change type from std::queue to a custom circular array
        queue<Node*> nodePool;
        const static int nodePoolMaxSize = 10;
        Node* GetFromPool()
        {
            if (nodePool.empty())
                return nullptr;

            Node* node = nodePool.front();
            nodePool.pop();
            return node;
        }
        void ReturnToPool(Node* node)
        {
            nodePool.push(node);
        }
        void RemoveAllObservers()
        {
            // returns all active observer nodes to pool
            Node* curr = head;
            while (curr)
            {
                Node* next = curr->next_;
                curr->observer_ = nullptr;
                curr->next_ = nullptr;
                ReturnToPool(curr);
                curr = next;
            }
            head = nullptr;
        }
        virtual void Notify(Observer::Event event) = 0;
    };

    struct ActionSubject : public Subject<ActionObserver>
    {
        virtual ~ActionSubject() {}
    protected:
        void Notify(Observer::Event event) override {}
        void Notify(Observer::Event event, ActionObserver::Action action)
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event, action);
                curr = curr->next_;
            }
        }
    };
    struct TaskSubject : public Subject<TaskObserver>
    {
        virtual ~TaskSubject() {}
    protected:
        vector<Task> tasksToDeliver_;
        void Notify(Observer::Event event) override
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event, tasksToDeliver_);
                curr = curr->next_;
            }
            tasksToDeliver_.clear();
        }
    };

    struct ColumnPromptTaskSubject : public Subject<ColumnPromptObserver<TaskObserver>>
    {
        virtual ~ColumnPromptTaskSubject() {}
    protected:
        void Notify(Observer::Event event) override {}
        void Notify(Observer::Event event, ColumnPromptObserver<TaskObserver>::Prompt promptType, TaskObserver& observer)
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event, promptType, observer);
                curr = curr->next_;
            }
        }
    };
    struct ColumnPromptConfigSubject : public Subject<ColumnPromptObserver<ActionObserver>>
    {
        virtual ~ColumnPromptConfigSubject() {}
    protected:
        void Notify(Observer::Event event) override {}
        void Notify(Observer::Event event, ColumnPromptObserver<ActionObserver>::Prompt promptType, ActionObserver& observer)
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event, promptType, observer);
                curr = curr->next_;
            }
        }
    };
}