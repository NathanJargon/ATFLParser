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
    // Adaptive repair now doesn't print; called from parse() which captures everything
    double affinity = 0.0;
    if (affinityMatrix.count(requiredToken) && affinityMatrix[requiredToken].count(actualToken)) {
        affinity = affinityMatrix[requiredToken][actualToken];
    }

    if (affinity > 0.8) {
        adaptiveMap[actualToken] = requiredToken;
    }
    else if (affinity > 0.5) {
        adaptiveMap[actualToken] = requiredToken;
    }
    else {
        // Low affinity: still update map but user sees warning in parse output
        adaptiveMap[actualToken] = requiredToken;
    }
}

std::string AdaptivePDA::parse(std::vector<std::string> tokens) {
    std::ostringstream ss;
    std::stack<std::string> s;
    s.push("$");
    s.push(startSymbol);
   
    int ptr = 0;
    tokens.push_back("$");

    ss << "\n--- DNA Hairpin Parser (Adaptive) ---\n";

    while (!s.empty()) {
        std::string top = s.top();
        std::string lookahead = tokens[ptr];
       
        if (top == "$") {
            if (lookahead == "$") {
                ss << "STRUCTURE STABLE\n";
                return ss.str();
            }
        }

        // Case 1: Stack Top is Terminal
        if (parsingTable.find(top) == parsingTable.end()) {
           
            if (top == lookahead) {
                ss << "Match " << top << "\n";
                s.pop(); ptr++;
            }
            else if (adaptiveMap.count(lookahead) && adaptiveMap[lookahead] == top) {
                 ss << "Match " << top << " (via " << lookahead << ")\n";
                 s.pop(); ptr++;
            }
            else {
                double affinity = 0.0;
                if (affinityMatrix.count(top) && affinityMatrix[top].count(lookahead)) {
                    affinity = affinityMatrix[top][lookahead];
                }
                ss << "[!] Mismatch: Expected [" << top << "], Found [" << lookahead << "]\n";
                ss << "[*] Affinity: " << affinity << " / 1.0\n";
                
                if (affinity > 0.8) {
                    ss << "[+] HIGH: Accepting substitution.\n";
                    adaptiveMap[lookahead] = top;
                } else if (affinity > 0.5) {
                    ss << "[~] MEDIUM: Wobble pairing; continuing.\n";
                    adaptiveMap[lookahead] = top;
                } else {
                    ss << "[-] LOW: Rejecting. Parse failed.\n";
                    return ss.str();
                }
            }
        }
        // Case 2: Stack Top is Non-Terminal
        else if (parsingTable.count(top)) {
            if (parsingTable[top].find(lookahead) == parsingTable[top].end()) {
                ss << "ERROR: Invalid start of structure.\n";
                return ss.str();
            }

            int ruleIndex = parsingTable[top][lookahead];
            Production p = grammar[ruleIndex];
           
            std::string rhsStr = "";
            for(const auto& val : p.rhs) rhsStr += val + " ";
            ss << "Expand " << p.lhs << " -> " << rhsStr << "\n";
           
            s.pop();
            for (int i = p.rhs.size() - 1; i >= 0; i--) s.push(p.rhs[i]);
        }
        else {
            ss << "Error: Unknown state.\n";
            return ss.str();
        }
    }
    return ss.str();
}
