#ifndef THOMPSONS_CONSTRUCTION_H
#define THOMPSONS_CONSTRUCTION_H

#include "nfa_state.h"
#include <string>

/**
 * FILE: thompsons_construction.h
 * DESCRIPTION: Thompson's Construction algorithm - converts postfix regex to NFA
 * PROCESS:
 *   Thompson's Construction converts regular expressions to NFAs incrementally:
 *   
 *   1. makeChar(c) - Base case: Single character transition
 *      Creates: start -[c]-> end
 *
 *   2. makeConcat(A, B) - Concatenation operator (implicit in regex)
 *      Connects: A.end -[ε]-> B.start
 *      Result: A's final states point to B's start
 *
 *   3. makeUnion(A, B) - Alternation operator (|)
 *      Creates new start/end states with epsilon transitions:
 *      - new_start -[ε]-> A.start
 *      - new_start -[ε]-> B.start
 *      - A.end -[ε]-> new_end
 *      - B.end -[ε]-> new_end
 *
 *   4. makeStar(A) - Kleene star operator (*)
 *      Creates loops with epsilon transitions:
 *      - new_start -[ε]-> A.start
 *      - new_start -[ε]-> new_end
 *      - A.end -[ε]-> A.start (loop back)
 *      - A.end -[ε]-> new_end
 *
 *   5. regexToNFA(postfix) - Driver function
 *      Uses stack to process postfix expression
 *      Validates stack operations
 */

NFAFragment makeChar(char c);
NFAFragment makeUnion(NFAFragment first, NFAFragment second);
NFAFragment makeConcat(NFAFragment first, NFAFragment second);
NFAFragment makeStar(NFAFragment fragment);
NFAFragment regexToNFA(std::string postfix);

#endif
