#include "adaptive_pda.h"
#include <iostream>
#include <stack>
#include <iomanip>

/**
 * FILE: adaptive_pda.cpp
 * DESCRIPTION: Implementation of Adaptive PDA with bio-inspired error recovery
 * PROCESS:
 *
 *   Constructor:
 *   - Initializes LL(1) parsing table for DNA hairpin grammar
 *   - Populates affinity matrix with base pairing probabilities
 *   - Sets start symbol to "S"
 *
 *   adaptiveRepair():
 *   - Called when token mismatch detected during parsing
 *   - Looks up affinity score from matrix
 *   - Applies three-tier decision policy:
 *     * High (>0.8): Log substitution to adaptiveMap
 *     * Medium (>0.5): Warn and update map
 *     * Low (<=0.5): Abort with error
 *   - Allows graceful handling of RNA/DNA transitions or typos
 *
 *   parse():
 *   - Classic LL(1) parsing with stack
 *   - For terminals: match with lookahead or use adaptive repair
 *   - For non-terminals: push production RHS in reverse order
 *   - Terminates when stack reaches $ and lookahead is $
 */

AdaptivePDA::AdaptivePDA() {
    startSymbol = "S";
   
    // Grammar: Nested DNA Hairpin
    // S -> A S T (Valid)
    // S -> G S C (Valid)
    // S -> .     (End)
    grammar.push_back({"S", {"A", "S", "T"}});
    grammar.push_back({"S", {"G", "S", "C"}});
    grammar.push_back({"S", {"."}});            
   
    parsingTable["S"]["A"] = 0;
    parsingTable["S"]["G"] = 1;
    parsingTable["S"]["."] = 2;

    // Initialize Heuristic Data
    // High Value = High Affinity (likely to bind/substitute)
    // Low Value = Clash
   
    // RNA 'U' acts like DNA 'T'
    affinityMatrix["T"]["U"] = 0.95;
   
    // Wobble Base Pairing (G sometimes binds with U in RNA)
    affinityMatrix["C"]["U"] = 0.60;
   
    // Purine-Purine clashing (A cannot replace C)
    affinityMatrix["C"]["A"] = 0.05;
}

void AdaptivePDA::adaptiveRepair(std::string requiredToken, std::string actualToken) {
    std::cout << "\n[!] ALERT: Mismatch. Expected [" << requiredToken << "], Found [" << actualToken << "]" << std::endl;
    std::cout << "[*] BIO-ADAPTIVE UNIT: Calculating affinity heuristic..." << std::endl;
   
    double affinity = 0.0;
    if (affinityMatrix.count(requiredToken) && affinityMatrix[requiredToken].count(actualToken)) {
        affinity = affinityMatrix[requiredToken][actualToken];
    }

    std::cout << "[*] HEURISTIC SCORE: " << affinity << " / 1.0" << std::endl;

    // Threshold for adaptive acceptance
    if (affinity > 0.8) {
        std::cout << "[*] DECISION: High affinity detected. Treating as valid substitution." << std::endl;
        adaptiveMap[actualToken] = requiredToken;
        std::cout << "[*] SYSTEM REPAIRED: Token equivalence map updated." << std::endl;
    }
    else if (affinity > 0.5) {
         std::cout << "[*] DECISION: Moderate affinity. Warning issued but parsing continues (Wobble pair)." << std::endl;
         adaptiveMap[actualToken] = requiredToken;
    }
    else {
        std::cout << "[!] CRITICAL: Low affinity. Structural integrity compromised. Aborting." << std::endl;
        exit(1);
    }
}

void AdaptivePDA::parse(std::vector<std::string> tokens) {
    std::stack<std::string> s;
    s.push("$");
    s.push(startSymbol);
   
    int ptr = 0;
    tokens.push_back("$");

    std::cout << "\n--- Starting DNA Hairpin Parser (Context-Free) ---" << std::endl;
    std::cout << std::left << std::setw(15) << "STACK" << std::setw(15) << "INPUT" << "ACTION" << std::endl;

    while (!s.empty()) {
        std::string top = s.top();
        std::string lookahead = tokens[ptr];
       
        // Visualization
        std::string stackStr = "";
        std::stack<std::string> temp = s;
        while(!temp.empty()) { stackStr += temp.top(); temp.pop(); }
        std::cout << std::left << std::setw(15) << stackStr.substr(0,14) << std::setw(15) << lookahead << " ";

        if (top == "$") {
            if (lookahead == "$") {
                std::cout << "STRUCTURE STABLE" << std::endl;
                return;
            }
        }

        // Case 1: Stack Top is Terminal
        if (parsingTable.find(top) == parsingTable.end()) {
           
            if (top == lookahead) {
                std::cout << "Match " << top << std::endl;
                s.pop(); ptr++;
            }
            // Check Adaptive Map (Learned aliases)
            else if (adaptiveMap.count(lookahead) && adaptiveMap[lookahead] == top) {
                 std::cout << "Match " << top << " (via " << lookahead << ")" << std::endl;
                 s.pop(); ptr++;
            }
            else {
                // Trigger the Generalized Heuristic
                adaptiveRepair(top, lookahead);
            }
        }
        // Case 2: Stack Top is Non-Terminal
        else if (parsingTable.count(top)) {
            if (parsingTable[top].find(lookahead) == parsingTable[top].end()) {
                std::cout << "ERROR: Invalid Start of Structure." << std::endl;
                return;
            }

            int ruleIndex = parsingTable[top][lookahead];
            Production p = grammar[ruleIndex];
           
            std::string rhsStr = "";
            for(const auto& val : p.rhs) rhsStr += val + " ";
            std::cout << "Expand " << p.lhs << " -> " << rhsStr << std::endl;
           
            s.pop();
            for (int i = p.rhs.size() - 1; i >= 0; i--) s.push(p.rhs[i]);
        }
        else {
            std::cout << "Error: Unknown State." << std::endl;
            return;
        }
    }
}
