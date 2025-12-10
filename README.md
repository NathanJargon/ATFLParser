# Formal Language Hierarchy in Compiler Design

A comprehensive C++ implementation demonstrating the **formal language hierarchy** in compiler design, focusing on regular expressions and finite automata for lexical analysis.

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Pattern Examples & Test Cases](#pattern-examples--test-cases)
3. [Regular Grammar Representation](#regular-grammar-representation)
4. [Architecture](#architecture)
5. [Lexical Analysis Process](#lexical-analysis-process)
6. [Build & Run](#build--run)
7. [Algorithm Details](#algorithm-details)

---

## Project Overview

This project implements the **formal language hierarchy** in compiler design, focusing on **regular languages** for lexical analysis:

### Regular Languages (Lexical Analysis)
Regular expressions define lexical patterns of programming language tokens:
- **Identifiers**: `[a-zA-Z][a-zA-Z0-9]*` (letter followed by letters/digits)
- **Numbers**: `[0-9]+(.[0-9]+)?` (integers and decimals)
- **Keywords**: `if|while|for|return`

These regular expressions are converted into **NFAs** using Thompson's construction, then simulated using subset construction (equivalent to DFA execution), demonstrating the equivalence between regular expressions and finite state machines.

### Key Features
✓ **Thompson's Construction**: Efficient regex → NFA conversion  
✓ **Subset Construction**: NFA simulation with DFA-like behavior  
✓ **Character Classes**: Support for `[a-z]`, `[0-9]`, `[^...]`, `\d`, `\w`  
✓ **Regular Grammar**: Tokens as regular languages  
✓ **Interactive GUI**: Visual testing of lexical patterns  
✓ **Modular Design**: Clean separation of concerns

---

## Pattern Examples & Test Cases

### Basic Patterns

**1. Literal Strings**
```
Pattern: hello
Test: hello hi helloworld
Results: "hello" → [MATCH], "hi" → [NO MATCH], "helloworld" → [NO MATCH]
```

**2. Alternation (OR)**
```
Pattern: cat|dog
Test: cat dog bird catdog
Results: "cat" → [MATCH], "dog" → [MATCH], "bird" → [NO MATCH], "catdog" → [NO MATCH]
```

**3. Kleene Star (Zero or More)**
```
Pattern: ab*
Test: a ab abb abbb b
Results: "a" → [MATCH], "ab" → [MATCH], "abb" → [MATCH], "abbb" → [MATCH], "b" → [NO MATCH]
```

**4. Plus (One or More)**
```
Pattern: ab+
Test: a ab abb abbb
Results: "a" → [NO MATCH], "ab" → [MATCH], "abb" → [MATCH], "abbb" → [MATCH]
```

### Character Classes

**5. Lowercase Letters**
```
Pattern: [a-z]+
Test: hello world ABC Hello123
Results: "hello" → [MATCH], "world" → [MATCH], "ABC" → [NO MATCH], "Hello123" → [NO MATCH]
```

**6. Digits**
```
Pattern: [0-9]+
Test: 123 456 abc hello123
Results: "123" → [MATCH], "456" → [MATCH], "abc" → [NO MATCH], "hello123" → [NO MATCH]
```

**7. Alphanumeric**
```
Pattern: [a-zA-Z0-9]+
Test: Hello123 test ABC 456 hello_world
Results: "Hello123" → [MATCH], "test" → [MATCH], "ABC" → [MATCH], "456" → [MATCH], "hello_world" → [NO MATCH]
```

**8. Negation (NOT)**
```
Pattern: [^0-9]+
Test: hello 123 world abc
Results: "hello" → [MATCH], "123" → [NO MATCH], "world" → [MATCH], "abc" → [MATCH]
```

### Programming Language Tokens

**9. Identifiers (Variable Names)**
```
Pattern: [a-zA-Z][a-zA-Z0-9]*
Test: myVar x counter123 _invalid 123abc
Results: "myVar" → [MATCH], "x" → [MATCH], "counter123" → [MATCH]
         "_invalid" → [NO MATCH] (starts with _), "123abc" → [NO MATCH] (starts with digit)

Explanation: Matches identifiers that start with a letter, followed by any letters/digits
Regular Grammar:
  Identifier → letter IdentifierTail
  IdentifierTail → letter IdentifierTail | digit IdentifierTail | ε
```

**10. Integer Numbers**
```
Pattern: [0-9]+
Test: 42 123 0 abc 12.5
Results: "42" → [MATCH], "123" → [MATCH], "0" → [MATCH], "abc" → [NO MATCH], "12.5" → [NO MATCH]
```

**11. Decimal Numbers (Integers or Floats)**
```
Pattern: [0-9]+(.[0-9]+)?
Test: 42 123.45 0.5 .5 abc
Results: "42" → [MATCH], "123.45" → [MATCH], "0.5" → [MATCH]
         ".5" → [NO MATCH] (no leading digit), "abc" → [NO MATCH]
```

**12. Keywords**
```
Pattern: if|while|for|return|int|void
Test: if while myvar return for123
Results: "if" → [MATCH], "while" → [MATCH], "myvar" → [NO MATCH]
         "return" → [MATCH], "for123" → [NO MATCH]
```

**13. Operators**
```
Pattern: +|-|*|/|=|==|!=|<|>|<=|>=
Test: + - * / = == != < > <= >=
Results: All individual operators match
```

### Escape Sequences

**14. Digit Shorthand**
```
Pattern: \d+
Test: 123 abc 456
Results: Equivalent to [0-9]+, matches "123" and "456"
```

**15. Word Character Shorthand**
```
Pattern: \w+
Test: Hello_World 123 test-case
Results: "Hello_World" → [MATCH], "123" → [MATCH], "test-case" → [NO MATCH]
Explanation: \w matches [A-Za-z0-9_]
```

### Complex Patterns

**16. Email-like Pattern (Simplified)**
```
Pattern: [a-zA-Z0-9]+@[a-zA-Z]+.[a-z]+
Test: user@example.com admin@site.org 123@test
Results: "user@example.com" → [MATCH], "admin@site.org" → [MATCH]
         "123@test" → [NO MATCH] (missing domain extension)
```

**17. Hex Color Codes**
```
Pattern: #[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]
Test: #FF5733 #abc123 #GGGGGG #12345
Results: "#FF5733" → [MATCH], "#abc123" → [MATCH], "#GGGGGG" → [NO MATCH], "#12345" → [NO MATCH]
```

**18. C-style Comments**
```
Pattern: //*[a-zA-Z0-9 ]*
Test: //hello //comment123 /single
Results: "//hello" → [MATCH], "//comment123" → [MATCH], "/single" → [NO MATCH]
```

---

## Regular Grammar Representation

The tokens recognized by finite automata constitute **regular languages** described by regular grammars:

### Identifier Grammar
```
Identifier → letter IdentifierTail
IdentifierTail → letter IdentifierTail
IdentifierTail → digit IdentifierTail
IdentifierTail → ε
```

**Example Derivation** for "var123":
```
Identifier ⇒ v IdentifierTail
           ⇒ v a IdentifierTail
           ⇒ v a r IdentifierTail
           ⇒ v a r 1 IdentifierTail
           ⇒ v a r 1 2 IdentifierTail
           ⇒ v a r 1 2 3 IdentifierTail
           ⇒ v a r 1 2 3 ε
           = var123
```

### Number Grammar
```
Number → digit NumberTail
NumberTail → digit NumberTail
NumberTail → ε
```

### Keyword Grammar (Finite Choice)
```
Keyword → if | while | for | return | int | void
```

---

## Architecture

### File Organization

```
atflparser/
├── gui_main.cpp                  # Interactive GUI entry point
├── README.md                     # This file
│
├── [LEXICAL ANALYSIS - REGULAR LANGUAGES]
├── nfa_state.h / nfa_state.cpp              # NFA state & memory management
├── regex_preprocessor.h / .cpp              # Character class expansion, postfix conversion
├── thompsons_construction.h / .cpp          # Thompson's NFA construction
└── nfa_simulator.h / nfa_simulator.cpp      # Subset construction & simulation
```

### Module Dependencies

```
gui_main.cpp
   ├─→ nfa_state.h                (Base: State structures)
   ├─→ regex_preprocessor.h       (Expands [a-z], converts to postfix)
   ├─→ thompsons_construction.h   (Builds NFA from postfix)
   └─→ nfa_simulator.h            (Simulates NFA using subset construction)
```

---

## Lexical Analysis Process

### Step-by-Step: Regular Expression → NFA → Simulation

#### Step 1: Input Regular Expression
```
Raw Input: "[a-zA-Z][a-zA-Z0-9]*"
```
Represents: Valid identifier (starts with letter, followed by letters/digits)

#### Step 2: Character Class Expansion
**Location**: `regex_preprocessor.cpp - preprocessRegex() Pass 0`

Expand character classes into unions:
```
[a-z] → (a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)
[0-9] → (0|1|2|3|4|5|6|7|8|9)
\d    → (0|1|2|3|4|5|6|7|8|9)
\w    → (A|B|...|Z|a|b|...|z|0|1|...|9|_)
```

**Input**: `[a-zA-Z][a-zA-Z0-9]*`  
**Output**: `(a|b|...|z|A|...|Z)(a|b|...|z|A|...|Z|0|1|...|9)*`

#### Step 3: Expand '+' Operator
**Location**: `regex_preprocessor.cpp - preprocessRegex() Pass 1`

Transform `A+` → `AA*`:
```
Pattern: a+   →  aa*
Pattern: (a|b)+  →  (a|b)(a|b)*
```

#### Step 4: Insert Explicit Concatenation
**Location**: `regex_preprocessor.cpp - preprocessRegex() Pass 2`

Insert '.' between consecutive operands:
```
ab → a.b
(a|b)c → (a|b).c
```

**Output**: Fully explicit infix expression

#### Step 5: Infix to Postfix Conversion (Shunting-Yard)
**Location**: `regex_preprocessor.cpp - toPostfix()`

Convert to Reverse Polish Notation using operator precedence:
- `*` (Kleene star): Precedence 3
- `.` (Concatenation): Precedence 2
- `|` (Alternation): Precedence 1

**Example**:
```
Infix:  a.b|c
Postfix: ab.c|
```

#### Step 6: Thompson's Construction (Postfix → NFA)
**Location**: `thompsons_construction.cpp - regexToNFA()`

Build NFA using four basic constructions:

**A. Single Character** (`makeChar(c)`)
```
start --[c]--> end
```

**B. Concatenation** (`makeConcat(A, B)`)
```
A.finals --[ε]--> B.start
```

**C. Alternation** (`makeUnion(A, B)`)
```
       -[ε]-> A -[ε]-
      /              \
start                end
      \              /
       -[ε]-> B -[ε]-
```

**D. Kleene Star** (`makeStar(A)`)
```
     -[ε]-> (loop back)
    /                 \
start -[ε]-> A -[ε]-> end
    \                 /
     -[ε]-> (bypass) -
```

**Stack-Based Processing**:
```
For each char in postfix:
  - Operand: push makeChar(c)
  - '.': pop 2, concat, push
  - '|': pop 2, union, push
  - '*': pop 1, star, push
```

**Output**: Complete NFA with start state and final states

#### Step 7: NFA Simulation (Subset Construction)
**Location**: `nfa_simulator.cpp - simulateNFA()`

Simulate NFA on input string:

**Algorithm**:
```
1. currentStates = epsilonClosure(start)
2. For each character c in input:
     nextStates = {}
     For each state s in currentStates:
       For each transition on c:
         nextStates += epsilonClosure(target)
     currentStates = nextStates
3. Accept if any current state is final
```

**Epsilon Closure** (DFS):
```
closure(state):
  visited = {state}
  for each ε-transition to next:
    if next not visited:
      visited += closure(next)
  return visited
```

**Example** (Pattern: `[a-z]+`, Input: "hello"):
```
Start: currentStates = {0}
Read 'h': transition → currentStates = {1, 2}
Read 'e': transition → currentStates = {1, 2}
Read 'l': transition → currentStates = {1, 2}
Read 'l': transition → currentStates = {1, 2}
Read 'o': transition → currentStates = {1, 2}
Check: state 2 is final → [MATCH]
```

---

## Build & Run

### Prerequisites
- C++17 or later
- SFML 3 (for GUI)
- MinGW-w64 or GCC

### SFML GUI Installation (Windows - MSYS2)

1. **Install MSYS2**:
   - Download from https://www.msys2.org/
   - Run installer

2. **Open UCRT64 terminal** (from Start Menu)

3. **Update and install toolchain + SFML**:
```bash
pacman -Syu
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-sfml
```

4. **Build the project**:
```bash
cd /c/Nash/Projects/atflparser

g++ -std=c++17 -Wall -Wextra -O2 \
    gui_main.cpp \
    nfa_state.cpp \
    regex_preprocessor.cpp \
    thompsons_construction.cpp \
    nfa_simulator.cpp \
    adaptive_pda.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -o output/gui.exe

./output/gui.exe
```

### GUI Usage

1. **Enter regex pattern** in the top input field
   - Example: `[a-zA-Z][a-zA-Z0-9]*`

2. **Enter test inputs** (space-separated) in the second field
   - Example: `myVar x counter123 _invalid 123abc`

3. **Click "Run Phase 1"** to see results

4. **Try different patterns** from the examples above

---

## Algorithm Details

### Complexity Analysis

| Algorithm | Time | Space | Notes |
|-----------|------|-------|-------|
| Character Class Expansion | O(n × m) | O(n × m) | n = regex length, m = class size |
| Regex Preprocessing | O(n) | O(n) | Two passes, linear scan |
| Shunting-Yard (Postfix) | O(n) | O(n) | n = regex length |
| Thompson's Construction | O(n) | O(n) | Creates ~2n states |
| Epsilon Closure (DFS) | O(\|S\| + \|T\|) | O(\|S\|) | S = states, T = transitions |
| NFA Simulation | O(\|I\| × \|S\|²) | O(\|S\|) | I = input, worst case subset enumeration |

### Theoretical Foundations

**Regular Language Hierarchy:**
```
Regular Expressions ≡ Finite Automata ≡ Regular Grammars
```

**Closure Properties:**
- Regular languages are closed under union, concatenation, Kleene star
- Implemented via Thompson's construction

**Equivalence:**
- NFA with ε-transitions → NFA without ε (via epsilon closure)
- NFA → DFA (via subset construction, shown in simulation)
- DFA → Minimal DFA (not implemented, but possible via Hopcroft's algorithm)

---

## Extension Points

### Adding New Character Classes
Modify `regex_preprocessor.cpp - expandCharacterClass()`:
```cpp
if (classContent == "alpha") {
    return expandRange('a', 'z') + expandRange('A', 'Z');
}
```

### Optimizing to DFA
Modify `nfa_simulator.cpp` to precompute all state transitions:
```cpp
// Build DFA transition table before simulation
map<set<int>, map<char, set<int>>> dfaTable;
```

### Adding More Operators
- `?` (zero or one): `a? → (a|ε)`
- `{n}` (exactly n): `a{3} → aaa`
- `{n,m}` (n to m): `a{2,4} → aa(a)?(a)?`

---

## References

- **Thompson's Construction**: Thompson, K. (1968). "Regular Expression Search Algorithm"
- **Subset Construction**: Rabin & Scott (1959). "Finite automata and their decision problems"
- **Shunting-Yard Algorithm**: Dijkstra (1961)
- **Formal Languages**: Hopcroft, Ullman (1979). "Introduction to Automata Theory"

---

## License

Educational project. Free to use for learning purposes.

---

**Last Updated**: December 10, 2025
