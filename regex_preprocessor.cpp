#include "regex_preprocessor.h"
#include <stack>
#include <cctype>

/**
 * FILE: regex_preprocessor.cpp
 * DESCRIPTION: Implementation of regex preprocessing and postfix conversion
 * PROCESS:
 *   
 *   preprocessRegex():
 *   - PASS 1: Iterates through regex, replaces '+' with duplication + '*'
 *     Example: A+ -> AA*, (A|B)+ -> (A|B)(A|B)*
 *   - PASS 2: Scans result, inserts '.' between consecutive operands
 *     Rules: Insert if (prev is alnum/)/\* ) AND (next is alnum/()
 *
 *   toPostfix():
 *   - Uses operator stack for precedence handling
 *   - Operands (alphanumeric): directly output
 *   - Operators: push/pop based on precedence comparison
 *   - Parentheses: manage stack for grouping
 */

std::string preprocessRegex(std::string regex) {
    // PASS 1: Expand '+' operator (Syntactic Sugar)
    // A+ -> AA*
    // (A|B)+ -> (A|B)(A|B)*
    std::string expanded = "";
    for (size_t i = 0; i < regex.length(); i++) {
        char c = regex[i];
        if (c == '+') {
            if (expanded.empty()) continue;
            char prev = expanded.back();
           
            if (prev == ')') {
                // Duplicate group: find matching '('
                int depth = 1;
                int j = expanded.length() - 2;
                while (j >= 0 && depth > 0) {
                    if (expanded[j] == ')') depth++;
                    else if (expanded[j] == '(') depth--;
                   
                    if (depth == 0) break;
                    j--;
                }
               
                if (depth == 0 && j >= 0) {
                    std::string group = expanded.substr(j);
                    expanded += group;
                    expanded += '*';
                }
            } else {
                // Simple char: A -> AA*
                expanded += prev;
                expanded += '*';
            }
        } else {
            expanded += c;
        }
    }

    // PASS 2: Insert Explicit Concatenation '.'
    std::string res = "";
    for (size_t i = 0; i < expanded.length(); i++) {
        char c = expanded[i];
        res += c;
       
        if (i + 1 < expanded.length()) {
            char next = expanded[i + 1];
           
            // Insert dot if between two operands
            bool isLeftOperand = std::isalnum(c) || c == ')' || c == '*';
            bool isRightOperand = std::isalnum(next) || next == '(';
           
            if (isLeftOperand && isRightOperand) {
                res += '.';
            }
        }
    }
    return res;
}

int precedence(char c) {
    if (c == '*') return 3;
    if (c == '.') return 2;
    if (c == '|') return 1;
    return 0;
}

std::string toPostfix(std::string regex) {
    std::string postfix = "";
    std::stack<char> opStack;
    for (char c : regex) {
        if (std::isalnum(c)) {
            postfix += c;
        } else if (c == '(') {
            opStack.push(c);
        } else if (c == ')') {
            while (!opStack.empty() && opStack.top() != '(') {
                postfix += opStack.top();
                opStack.pop();
            }
            if (!opStack.empty()) {
                opStack.pop();
            }
        } else {
            while (!opStack.empty() && precedence(opStack.top()) >= precedence(c)) {
                postfix += opStack.top();
                opStack.pop();
            }
            opStack.push(c);
        }
    }
    while (!opStack.empty()) {
        postfix += opStack.top();
        opStack.pop();
    }
    return postfix;
}
