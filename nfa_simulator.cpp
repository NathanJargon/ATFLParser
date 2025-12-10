#include "nfa_simulator.h"
#include <sstream>

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

std::string simulateNFAWithTrace(NFAFragment nfa, std::string input) {
    std::ostringstream trace;
    std::set<NFAState*> currentStates;
    std::set<int> visited;
    
    getEpsilonClosure(nfa.start, visited, currentStates);
    
    trace << "      Step 0: Initial ε-closure from state q" << nfa.start->id << "\n";
    trace << "              Current states: {";
    bool first = true;
    for (auto s : currentStates) {
        if (!first) trace << ", ";
        trace << "q" << s->id;
        first = false;
    }
    trace << "}\n\n";

    int step = 1;
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        std::set<NFAState*> nextStates;
        
        trace << "      Step " << step++ << ": Read '" << c << "' (position " << i << ")\n";
        
        for (auto s : currentStates) {
            if (s->transitions.count(c)) {
                trace << "              State q" << s->id << " --[" << c << "]--> ";
                bool firstTrans = true;
                for (auto next : s->transitions[c]) {
                    if (!firstTrans) trace << ", ";
                    trace << "q" << next->id;
                    firstTrans = false;
                    
                    std::set<int> v;
                    getEpsilonClosure(next, v, nextStates);
                }
                trace << "\n";
            }
        }
        
        currentStates = nextStates;
        
        trace << "              After ε-closure: {";
        first = true;
        for (auto s : currentStates) {
            if (!first) trace << ", ";
            trace << "q" << s->id;
            first = false;
        }
        trace << "}\n";
        
        if (currentStates.empty()) {
            trace << "              DEAD STATE - No valid transitions\n";
            return trace.str();
        }
        trace << "\n";
    }

    trace << "      Final Check: ";
    bool accepted = false;
    for(auto s : currentStates) {
        for(auto f : nfa.finals) {
            if(s == f) {
                accepted = true;
                trace << "State q" << s->id << " is a final state\n";
                break;
            }
        }
        if (accepted) break;
    }
    
    if (!accepted) {
        trace << "No current state is a final state\n";
    }
    
    return trace.str();
}
