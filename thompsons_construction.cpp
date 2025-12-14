#include "thompsons_construction.h"
#include <stack>
#include <cctype>
#include <iostream>

/**
 * FILE: thompsons_construction.cpp
 * DESCRIPTION: Implementation of Thompson's Construction algorithm
 * PROCESS:
 *   Each helper function creates NFA fragments by:
 *   - Allocating new start/end states via StateManager
 *   - Creating epsilon (E) or character transitions
 *   - Returning fragment with start and final state list
 *
 *   regexToNFA processes postfix expression using stack:
 *   - Push character fragments
 *   - Pop operands and apply operators
 *   - Final stack must contain exactly 1 item
 */

NFAFragment makeChar(char c) {
    NFAState* start = StateManager::create();
    NFAState* end = StateManager::create();
    start->transitions[c].push_back(end);
    return {start, {end}};
}

NFAFragment makeUnion(NFAFragment first, NFAFragment second) {
    NFAState* start = StateManager::create();
    NFAState* end = StateManager::create();
   
    start->transitions['E'].push_back(first.start);
    start->transitions['E'].push_back(second.start);

    for (auto f : first.finals) f->transitions['E'].push_back(end);
    for (auto f : second.finals) f->transitions['E'].push_back(end);

    return {start, {end}};
}

NFAFragment makeConcat(NFAFragment first, NFAFragment second) {
    for (auto f : first.finals) {
        f->transitions['E'].push_back(second.start);
    }
    return {first.start, second.finals};
}

NFAFragment makeStar(NFAFragment fragment) {
    NFAState* start = StateManager::create();
    NFAState* end = StateManager::create();

    start->transitions['E'].push_back(fragment.start);
    start->transitions['E'].push_back(end);

    for (auto f : fragment.finals) {
        f->transitions['E'].push_back(fragment.start);
        f->transitions['E'].push_back(end);
    }

    return {start, {end}};
}

NFAFragment regexToNFA(std::string postfix) {
    std::stack<NFAFragment> st;
    for (char c : postfix) {
        if (c == '.') {
            if (st.size() < 2) { 
                std::cerr << "Error: Malformed Regex (Stack Underflow on .)" << std::endl; 
                exit(1); 
            }
            NFAFragment b = st.top(); st.pop();
            NFAFragment a = st.top(); st.pop();
            st.push(makeConcat(a, b));
        } else if (c == '|') {
            if (st.size() < 2) { 
                std::cerr << "Error: Malformed Regex (Stack Underflow on |)" << std::endl; 
                exit(1); 
            }
            NFAFragment b = st.top(); st.pop();
            NFAFragment a = st.top(); st.pop();
            st.push(makeUnion(a, b));
        } else if (c == '*') {
            if (st.empty()) { 
                std::cerr << "Error: Malformed Regex (Stack Underflow on *)" << std::endl; 
                exit(1); 
            }
            NFAFragment a = st.top(); st.pop();
            st.push(makeStar(a));
        } else {
            // Treat any other character as a literal operand
            st.push(makeChar(c));
        }
    }
    if (st.empty()) { 
        std::cerr << "Error: Empty Regex" << std::endl; 
        exit(1); 
    }
   
    if (st.size() != 1) {
        std::cerr << "Error: NFA Construction failed. Stack size: " << st.size() 
                  << " (Missing concatenation?)" << std::endl;
        exit(1);
    }
   
    return st.top();
}
