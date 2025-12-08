#ifndef ADAPTIVE_PDA_H
#define ADAPTIVE_PDA_H

#include <string>
#include <vector>
#include <map>

/**
 * FILE: adaptive_pda.h
 * DESCRIPTION: Adaptive Pushdown Automaton with heuristic error recovery (Phase 2)
 * PROCESS:
 *   
 *   AdaptivePDA simulates bio-inspired syntactic analysis (DNA hairpin structure parsing):
 *   
 *   1. Grammar Definition
 *      - Context-Free Grammar for nested DNA structures
 *      - Productions: S -> A S T | G S C | .
 *      - Represents complementary base pair nesting (A-T, G-C)
 *
 *   2. Parsing Table
 *      - LL(1) parsing table for deterministic decision-making
 *      - Maps (non-terminal, lookahead) -> production rule index
 *
 *   3. Affinity Matrix (Bio-inspired Heuristic)
 *      - Stores probability of base/token substitution (0.0 to 1.0)
 *      - RNA 'U' substitutes for DNA 'T' (0.95 affinity)
 *      - Wobble base pairs like G-U (0.60 affinity)
 *      - Incompatible pairs have low affinity (0.05)
 *
 *   4. Adaptive Repair Mechanism
 *      - When token mismatch occurs:
 *        1. Look up affinity score
 *        2. If affinity > 0.8: Accept substitution (high confidence)
 *        3. If affinity > 0.5: Warn but continue (wobble pair)
 *        4. If affinity <= 0.5: Abort (structural conflict)
 *
 *   5. Parse Function
 *      - Standard stack-based LL(1) parser
 *      - Uses adaptive repair instead of hard failure
 *      - Learns token equivalences in adaptiveMap
 */

struct Production {
    std::string lhs;
    std::vector<std::string> rhs;
};

class AdaptivePDA {
    std::map<std::string, std::map<std::string, int>> parsingTable;
    std::vector<Production> grammar;
    std::string startSymbol;
    std::map<std::string, std::string> adaptiveMap;
    std::map<std::string, std::map<std::string, double>> affinityMatrix;

public:
    AdaptivePDA();
    void adaptiveRepair(std::string requiredToken, std::string actualToken);
    std::string parse(std::vector<std::string> tokens);
};

#endif
