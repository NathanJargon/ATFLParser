#ifndef NFA_SIMULATOR_H
#define NFA_SIMULATOR_H

#include "nfa_state.h"
#include <string>
#include <set>

/**
 * FILE: nfa_simulator.h
 * DESCRIPTION: NFA simulation and epsilon closure computation (Subset Construction)
 * PROCESS:
 *   
 *   1. getEpsilonClosure(s, visited, closure)
 *      - Recursive DFS to find all states reachable via epsilon (E) transitions
 *      - Uses visited set to prevent infinite loops
 *      - Adds all reachable states to closure set
 *      - Core of subset construction algorithm
 *
 *   2. simulateNFA(nfa, input)
 *      - Simulates NFA execution on input string
 *      - Maintains set of current states (subset of original NFA states)
 *      - For each input character:
 *        * Compute next states from all current states
 *        * Apply epsilon closure to each reachable state
 *      - Returns true if any final state is reached after consuming input
 */

void getEpsilonClosure(NFAState* s, std::set<int>& visited, std::set<NFAState*>& closure);
bool simulateNFA(NFAFragment nfa, std::string input);

#endif
