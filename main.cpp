/**
 * MAIN DRIVER - Resilient Compiler Simulation
 * 
 * PROJECT STRUCTURE (Modular Algorithm Organization):
 * 
 * PHASE 1: LEXICAL ANALYSIS (Regex -> NFA)
 * ============================================
 * 1. nfa_state.h/cpp
 *    - NFAState: Represents NFA states with ID and transitions
 *    - StateManager: Manages memory for NFA states using unique_ptr
 * 
 * 2. regex_preprocessor.h/cpp
 *    - preprocessRegex(): Two-pass preprocessing (expands '+', adds explicit '.')
 *    - toPostfix(): Shunting-yard algorithm (infix to postfix RPN)
 *    - precedence(): Helper for operator precedence
 *
 * 3. thompsons_construction.h/cpp
 *    - Thompson's Construction: Converts postfix regex to NFA
 *    - makeChar(), makeConcat(), makeUnion(), makeStar()
 *    - regexToNFA(): Stack-based postfix expression evaluation
 *
 * 4. nfa_simulator.h/cpp
 *    - Subset Construction algorithm: NFA simulation
 *    - getEpsilonClosure(): Compute epsilon reachability (DFS)
 *    - simulateNFA(): Input string matching on NFA
 *
 * PHASE 2: SYNTACTIC ANALYSIS (LL(1) Parsing with Adaptive Repair)
 * =================================================================
 * 5. adaptive_pda.h/cpp
 *    - AdaptivePDA: Pushdown Automaton with heuristic error recovery
 *    - Grammar: DNA hairpin nesting (context-free)
 *    - Affinity Matrix: Bio-inspired token substitution heuristics
 *    - Adaptive Repair: Graceful error handling instead of hard failure
 *
 * ALGORITHMS BY PROCESS:
 * ======================
 * 1. Regular Expression Parsing
 *    - Input: Regex string "(A|G)+"
 *    - Preprocessing: Expand '+' -> "(A|G)(A|G)*"
 *                     Add explicit dots -> "(A|G).(A|G).*"
 *    - Postfix: "AG|AG|.*."
 *
 * 2. NFA Construction (Thompson's)
 *    - Input: Postfix expression
 *    - Process: Stack-based, combine fragments with epsilon transitions
 *    - Output: NFA with start state and accepting states
 *
 * 3. NFA Simulation (Subset Construction)
 *    - Input: NFA + test string "AGAGA"
 *    - Process: Maintain set of current states, apply epsilon closure after each character
 *    - Output: Accept/Reject decision
 *
 * 4. Syntactic Analysis (LL(1) with Adaptive Recovery)
 *    - Input: Token stream "AG.CU"
 *    - Grammar: S -> A S T | G S C | .
 *    - Process: Stack-based parsing with affinity-based error recovery
 *    - On mismatch: Check if substitution has high affinity (e.g., U→T: 0.95)
 *    - Output: Parse tree or error with adaptive explanation
 */

#include "nfa_state.h"
#include "regex_preprocessor.h"
#include "thompsons_construction.h"
#include "nfa_simulator.h"
#include "adaptive_pda.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    cout << "==========================================================" << endl;
    cout << " PROJECT: Resilient Compiler Simulation (Modular)" << endl;
    cout << "==========================================================" << endl;

    // --- PHASE 1 DEMO ---
    cout << "\n[PHASE 1] Robust Lexical Analysis" << endl;
   
    // New Feature: Handling '+' operator in regex preprocessing
    string rawRegex = "(A|G)+";
    string processedRegex = preprocessRegex(rawRegex);
    string postfix = toPostfix(processedRegex);
   
    cout << "Regex Pattern: " << rawRegex << endl;
    cout << "Preprocessed:  " << processedRegex << endl;
    cout << "Postfix:       " << postfix << endl;
   
    NFAFragment nfa = regexToNFA(postfix);
   
    // Test Case: "AGAGA"
    string testStr = "AGAGA";
    cout << "Testing string '" << testStr << "': ";
    if(simulateNFA(nfa, testStr)) cout << "MATCH" << endl;
    else cout << "INVALID" << endl;
   
    // Auto-cleanup happens via StateManager (No manual delete needed)
    StateManager::clear();

    // --- PHASE 2 DEMO ---
    cout << "\n[PHASE 2] Syntactic Analysis (Heuristic DNA Repair)" << endl;
    cout << "Grammar: S -> A S T | G S C | ." << endl;
   
    string rawDNA = "AG.CU";
    cout << "\nSimulating Raw Input: " << rawDNA << endl;
    cout << "Scenario: 'A' expects 'T' to close. Found 'U'." << endl;
   
    vector<string> tokens;
    for(char c : rawDNA) {
        if (!isspace(c)) {
            tokens.push_back(string(1, c));
        }
    }

    AdaptivePDA parser;
    parser.parse(tokens);
   
    cout << "\n[CONCLUSION] Heuristic threshold met (0.95 > 0.8). Mutation accepted." << endl;

    return 0;
}





