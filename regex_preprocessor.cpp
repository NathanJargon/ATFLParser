#include "regex_preprocessor.h"
#include <stack>
#include <cctype>

/**
 * FILE: regex_preprocessor.cpp
 * DESCRIPTION: Implementation of regex preprocessing and postfix conversion
 * PROCESS:
 *   
 *   preprocessRegex():
 *   - PASS 0: Expand character classes [a-z], [0-9], [^...], \d, \w, \s
 *     Example: [a-z] -> (a|b|c|...|z), [0-9] -> (0|1|2|...|9)
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

// Helper function to expand character ranges
std::string expandCharacterClass(const std::string& classContent, bool negate) {
    std::string chars = "";
    
    // Parse the class content for ranges like a-z or literal chars
    for (size_t i = 0; i < classContent.length(); i++) {
        if (i + 2 < classContent.length() && classContent[i + 1] == '-') {
            // Range detected: a-z, 0-9, etc.
            char start = classContent[i];
            char end = classContent[i + 2];
            for (char c = start; c <= end; c++) {
                chars += c;
            }
            i += 2; // Skip the '-' and end char
        } else {
            // Literal character
            chars += classContent[i];
        }
    }
    
    if (negate) {
        // For negation [^...], we'll support only ASCII printable chars (33-126)
        std::string negatedChars = "";
        for (char c = 33; c <= 126; c++) {
            if (chars.find(c) == std::string::npos) {
                negatedChars += c;
            }
        }
        chars = negatedChars;
    }
    
    // Build union: (a|b|c|...)
    if (chars.empty()) return "";
    if (chars.length() == 1) return std::string(1, chars[0]);
    
    std::string result = "(";
    for (size_t i = 0; i < chars.length(); i++) {
        result += chars[i];
        if (i < chars.length() - 1) result += "|";
    }
    result += ")";
    return result;
}

std::string preprocessRegex(std::string regex) {
    // PASS 0: Expand character classes and escape sequences
    std::string withClasses = "";
    for (size_t i = 0; i < regex.length(); i++) {
        char c = regex[i];
        
        // Handle escape sequences
        if (c == '\\' && i + 1 < regex.length()) {
            char next = regex[i + 1];
            if (next == 'd') {
                // \d -> [0-9]
                withClasses += "(0|1|2|3|4|5|6|7|8|9)";
                i++;
            } else if (next == 'w') {
                // \w -> [A-Za-z0-9_]
                withClasses += "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9|_)";
                i++;
            } else if (next == 's') {
                // \s -> [ \t\n\r]
                withClasses += "( )"; // Simplified: just space
                i++;
            } else {
                // Unknown escape, keep literal
                withClasses += c;
            }
        }
        // Handle character classes [...]
        else if (c == '[') {
            size_t end = regex.find(']', i + 1);
            if (end != std::string::npos) {
                std::string classContent = regex.substr(i + 1, end - i - 1);
                bool negate = false;
                
                // Check for negation [^...]
                if (!classContent.empty() && classContent[0] == '^') {
                    negate = true;
                    classContent = classContent.substr(1);
                }
                
                withClasses += expandCharacterClass(classContent, negate);
                i = end; // Skip to closing ]
            } else {
                withClasses += c; // Malformed, keep literal
            }
        } else {
            withClasses += c;
        }
    }
    
    regex = withClasses;
    
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
           
            // Insert dot between operands
            // Left can produce: literal, ), or *
            // Right can consume: literal or (
            
            bool leftProduces = (c != '|' && c != '(');  // anything except | and (
            bool rightConsumes = (next != '|' && next != '*' && next != ')');  // anything except operators and )
           
            if (leftProduces && rightConsumes) {
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
        if (c == '.' || c == '|' || c == '*') {
            // It's an operator
            while (!opStack.empty() && precedence(opStack.top()) >= precedence(c)) {
                postfix += opStack.top();
                opStack.pop();
            }
            opStack.push(c);
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
            // It's a literal operand (any character that's not an operator or paren)
            postfix += c;
        }
    }
    while (!opStack.empty()) {
        postfix += opStack.top();
        opStack.pop();
    }
    return postfix;
}
