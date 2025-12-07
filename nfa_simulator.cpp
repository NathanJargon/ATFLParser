#include "nfa_simulator.h"

/**
 * FILE: nfa_simulator.cpp
 * DESCRIPTION: Implementation of NFA simulation with subset construction
 * PROCESS:
 *
 *   getEpsilonClosure():
 *   - DFS traversal following epsilon transitions
 *   - Marks visited states by ID to prevent revisiting
 *   - Collects all epsilon-reachable states
 *   - Time: O(|states| + |transitions|)
 *
 *   simulateNFA():
 *   - Start: Compute epsilon closure of NFA start state
 *   - For each character in input:
 *     1. For each current state, find direct transitions on that character
 *     2. Apply epsilon closure to each reachable state
 *     3. Update current state set
 *     4. If no states remain, input rejected
 *   - Accept: If any current state equals an NFA final state
 *   - Time: O(|input| * |states|^2) worst case
 */

void getEpsilonClosure(NFAState* s, std::set<int>& visited, std::set<NFAState*>& closure) {
    if (visited.count(s->id)) return;
    visited.insert(s->id);
    closure.insert(s);
    if (s->transitions.count('E')) {
        for (auto next : s->transitions['E']) {
            getEpsilonClosure(next, visited, closure);
        }
    }
}

bool simulateNFA(NFAFragment nfa, std::string input) {
    std::set<NFAState*> currentStates;
    std::set<int> visited;
    getEpsilonClosure(nfa.start, visited, currentStates);

    for (char c : input) {
        std::set<NFAState*> nextStates;
        for (auto s : currentStates) {
            if (s->transitions.count(c)) {
                for (auto next : s->transitions[c]) {
                    std::set<int> v;
                    getEpsilonClosure(next, v, nextStates);
                }
            }
        }
        currentStates = nextStates;
        if (currentStates.empty()) return false;
    }

    for(auto s : currentStates) {
        for(auto f : nfa.finals) {
            if(s == f) return true;
        }
    }
    return false;
}
