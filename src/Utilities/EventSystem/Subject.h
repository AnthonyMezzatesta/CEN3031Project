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
            GrowPool();
        }
        virtual ~Subject()
        {
            RemoveAllObservers();
            for (; !nodePool.empty(); nodePool.pop())
                delete nodePool.front();
        }
        void GrowPool()
        {
            for (int i = 0; i < nodePoolStartSize; i++)
                nodePool.push(new Node);
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
                head->next_ = nullptr;
                return true;
            }

            node->next_ = head;
            head = node;
            return true;
        }
        bool RemoveObserver(ObserverType observer)
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
            ObserverType* observer_ = nullptr;
            Node* next_ = nullptr;
        };
        Node* head = nullptr;
        // todo: change type from std::queue to a custom circular array
        queue<Node*> nodePool;
        const static int nodePoolStartSize = 10;
        Node* GetFromPool()
        {
            if (nodePool.empty())
                GrowPool();

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
        virtual void Notify(Observer::EventEnum event) = 0;
    };
    struct BasicSubject : public Subject<>
    {
        virtual ~BasicSubject() {}
    protected:
        void Notify(Observer::EventEnum event) override
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event);
                curr = curr->next_;
            }
        }
    };
    struct WindowResizeSubject : public Subject<WindowResizeObserver>
    {
        virtual ~WindowResizeSubject() {}
    protected:
        void Notify(Observer::EventEnum event) override {}
        void Notify(Observer::EventEnum event, const sf::View& view)
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event, view);
                curr = curr->next_;
            }
        }
    };
    struct ActionSubject : public Subject<DataObserver<Observer::ActionEnum>>
    {
        virtual ~ActionSubject() {}
    protected:
        void Notify(Observer::EventEnum event) override {}
        void Notify(Observer::EventEnum event, const Observer::ActionEnum action)
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event, action);
                curr = curr->next_;
            }
            if (action == Observer::Delete)
                RemoveAllObservers();
        }
    };
    struct TaskSubject : public Subject<DataObserver<Task>>
    {
        virtual ~TaskSubject() {}
    protected:
        void Notify(Observer::EventEnum event) override {}
        void Notify(Observer::EventEnum event, const Task& task)
        {
            Node* curr = head;
            while (curr)
            {
                curr->observer_->OnNotify(event, task);
                curr = curr->next_;
            }
        }
    };
}