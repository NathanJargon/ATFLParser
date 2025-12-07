#include "nfa_state.h"

/**
 * FILE: nfa_state.cpp
 * DESCRIPTION: Implementation of NFA state management
 * PROCESS:
 *   1. Initialize global state storage vector
 *   2. Initialize global state ID counter to 0
 *   3. StateManager::create() - allocates new NFAState and stores in unique_ptr
 *   4. StateManager::clear() - deallocates all stored states automatically
 */

int NFAState::globalID = 0;
std::vector<std::unique_ptr<NFAState>> StateManager::stateStore;

NFAState* StateManager::create() {
    auto state = std::make_unique<NFAState>();
    NFAState* ptr = state.get();
    stateStore.push_back(std::move(state));
    return ptr;
}

void StateManager::clear() {
    stateStore.clear();
}
