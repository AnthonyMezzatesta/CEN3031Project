#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "GUIState.h"
using std::unordered_map;

struct GUIStateMachine final
{
    GUIStateMachine() {
        currentState_ = new DefaultState;
        states_[GUIState::Default] = currentState_;
    }
    ~GUIStateMachine() { delete states_[GUIState::Default]; }

    bool AddState(GUIState* state) {
        return states_.emplace(state->GetStateType(), state).second;
    }
    void SwitchState(GUIState::StateEnum type) {
        if (states_.find(type) != states_.end())
            currentState_ = states_[type];
    }

    GUIState* GetCurrentState() { return currentState_; }
private:
    struct DefaultState final : public GUIState
    {
        DefaultState() : GUIState(StateEnum::Default) {}
        void ProcessLeftClickReleased() override {}
        void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target) override {}
        void ReadUserInput(char c) override {}
        void Update(const sf::RenderWindow& window, const float deltaTime) override {}
        bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target) override { return false; }
        void Draw(sf::RenderWindow& window) override {}
    };
    unordered_map<GUIState::StateEnum, GUIState*> states_;
    GUIState* currentState_;
};
