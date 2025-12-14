#ifndef NFA_STATE_H
#define NFA_STATE_H

#include <vector>
#include <map>
#include <memory>

/**
 * FILE: nfa_state.h
 * DESCRIPTION: Core data structures for NFA (Non-deterministic Finite Automaton)
 * PROCESS: 
 *   1. NFAState: Represents individual states in the NFA with:
 *      - Unique integer ID for state tracking
 *      - Transition map: maps input characters to next states (supports epsilon transitions)
 *      - Global ID counter to ensure unique identifiers
 * 
 *   2. StateManager: Global memory manager for NFA states
 *      - Creates and stores NFAState objects in unique_ptr containers
 *      - Ensures automatic cleanup and prevents memory leaks
 *      - Provides interface to create new states and clear all states
 */

struct NFAState;

// Global Owner for Automatic Memory Management
struct StateManager {
    static std::vector<std::unique_ptr<NFAState>> stateStore;
    static NFAState* create();
    static void clear();
    static int getStateCount();
    static void resetID();
};

struct NFAState {
    int id;
    std::map<char, std::vector<NFAState*>> transitions;  // Character -> Next States mapping
    static int globalID;

    NFAState() { id = globalID++; }
};

// Fragment representation: Start state + list of final states
struct NFAFragment {
    NFAState* start;
    std::vector<NFAState*> finals;
};

#endif
