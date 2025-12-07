#ifndef REGEX_PREPROCESSOR_H
#define REGEX_PREPROCESSOR_H

#include <string>

/**
 * FILE: regex_preprocessor.h
 * DESCRIPTION: Regex preprocessing and infix-to-postfix conversion
 * PROCESS:
 *   1. preprocessRegex() - Two-pass preprocessing:
 *      - PASS 1: Expands '+' operator (A+ -> AA*, (A|B)+ -> (A|B)(A|B)*)
 *      - PASS 2: Inserts explicit concatenation dots between operands
 *   
 *   2. toPostfix() - Shunting-yard algorithm:
 *      - Converts infix regex notation to postfix (RPN)
 *      - Respects operator precedence: * (3) > . (2) > | (1)
 *      - Handles parentheses grouping
 * 
 *   3. precedence() - Helper to determine operator priority
 */

std::string preprocessRegex(std::string regex);
int precedence(char c);
std::string toPostfix(std::string regex);

#endif
