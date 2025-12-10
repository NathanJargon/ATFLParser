# Formal Language Hierarchy in Compiler Design

A comprehensive C++ implementation demonstrating the **formal language hierarchy** in compiler design, from regular expressions and finite automata for lexical analysis to pushdown automata for syntactic analysis.

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Pattern Examples & Test Cases](#pattern-examples--test-cases)
4. [Lexical Analysis (Regular Languages)](#lexical-analysis-regular-languages)
5. [Syntactic Analysis (Context-Free Languages)](#syntactic-analysis-context-free-languages)
6. [Complete Process Flow](#complete-process-flow)
7. [Build & Run](#build--run)
8. [Algorithm Details](#algorithm-details)

---

## Project Overview

This project implements the **formal language hierarchy** in compiler design:

### Regular Languages (Lexical Analysis)
Regular expressions define lexical patterns of programming language tokens:
- **Identifiers**: `[a-zA-Z][a-zA-Z0-9]*` (letter followed by letters/digits)
- **Numbers**: `[0-9]+(.[0-9]+)?` (integers and decimals)
- **Keywords**: `if|while|for|return`

These regular expressions are converted into **NFAs** using Thompson's construction, then transformed into optimized **DFAs** through subset construction, demonstrating the equivalence between regular expressions and finite state machines.

### Context-Free Languages (Syntactic Analysis)
Pushdown Automata (PDA) recognize context-free grammar constructions:
- **Balanced parentheses**: `S → (S) | SS | ε`
- **Nested statement blocks**: `S → {S} | SS | ε`
- **Expression parsing**: `E → E + T | T, T → T * F | F`

### Key Features
✓ **Thompson's Construction**: Efficient regex → NFA conversion  
✓ **Subset Construction**: NFA → DFA optimization  
✓ **Character Classes**: Support for `[a-z]`, `[0-9]`, `[^...]`, `\d`, `\w`  
✓ **Pushdown Automaton**: Stack-based context-free parsing  
✓ **Regular Grammar**: Tokens as regular languages  
✓ **Modular Design**: Clean separation of lexical and syntactic phases

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

Note: The '.' is literal here (not concatenation operator when inside brackets)
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
Results: Equivalent to [0-9]+
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

**18. Phone Numbers (Simplified)**
```
Pattern: [0-9][0-9][0-9]-[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]
Test: 123-456-7890 555-1234 abc-def-ghij
Results: "123-456-7890" → [MATCH], others → [NO MATCH]
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

### Keyword Grammar
```
Keyword → if | while | for | return | int | void
```

---

## Lexical Analysis (Regular Languages)

### File Organization

```
atflparser/
├── main.cpp                      # Entry point & demo
├── README.md                     # This file
│
├── [PHASE 1: LEXICAL ANALYSIS]
├── nfa_state.h / nfa_state.cpp              # NFA state & memory management
├── regex_preprocessor.h / .cpp              # Regex preprocessing & postfix conversion
├── thompsons_construction.h / .cpp          # NFA building algorithm
├── nfa_simulator.h / nfa_simulator.cpp      # NFA execution & epsilon closure
│
├── [PHASE 2: SYNTACTIC ANALYSIS]
└── adaptive_pda.h / adaptive_pda.cpp        # Adaptive pushdown automaton with recovery
```

### Module Dependencies

```
main.cpp
   ├─→ nfa_state.h                (Base: State structures)
   ├─→ regex_preprocessor.h       (Converts regex to postfix)
   ├─→ thompsons_construction.h   (Builds NFA from postfix)
   ├─→ nfa_simulator.h            (Executes NFA)
   └─→ adaptive_pda.h             (Parses with error recovery)
```

### Compiling All Files

cd /c/Nash/Projects/atflparser

g++ -std=c++17 -Wall -Wextra -O2 gui_main.cpp nfa_state.cpp regex_preprocessor.cpp thompsons_construction.cpp nfa_simulator.cpp adaptive_pda.cpp -lsfml-graphics -lsfml-window -lsfml-system -o output/gui.exe 2>&1

ls -la output/gui.exe; output/gui.exe
---

## Phase 1: Lexical Analysis

### Objective
Convert a regular expression pattern into an NFA and verify if input strings match.

### Process Flow

#### Step 1: Input Regular Expression
```
Raw Input: "(A|G)+"
```
Represents: "One or more occurrences of A or G"  
Examples: "A", "AG", "AGAG", "AAA", "GGG" → **MATCH**  
Counter-example: "GT", "TT" → **NO MATCH**

#### Step 2: Regex Preprocessing (Two-Pass Algorithm)
**Location**: `regex_preprocessor.cpp - preprocessRegex()`

##### Pass 1: Expand '+' Operator
The '+' operator (one-or-more) is **syntactic sugar** for the pattern `XX*`:
```
(A|G)+ → (A|G)(A|G)*
```

**Algorithm**:
- Scan input character by character
- When '+' encountered:
  - If preceded by ')': Find matching '(' and duplicate the entire group
  - If preceded by single char: Duplicate that char and add '*'
- Example: `A+ → AA*`, `(X|Y)+ → (X|Y)(X|Y)*`

**Input**: `(A|G)+`  
**Output**: `(A|G)(A|G)*`

##### Pass 2: Insert Explicit Concatenation Dots
Make implicit concatenation explicit for postfix conversion:
```
(A|G)(A|G)* → (A|G).(A|G).*
```

**Algorithm**:
- Scan result from Pass 1
- Insert '.' between consecutive operands
- Rules: Insert if (previous is alnum/)/\*) AND (next is alnum/()

**Input**: `(A|G)(A|G)*`  
**Output**: `(A|G).(A|G).*`

#### Step 3: Infix to Postfix Conversion (Shunting-Yard Algorithm)
**Location**: `regex_preprocessor.cpp - toPostfix()`

Converts infix notation to Reverse Polish Notation (RPN) using operator precedence:
- `*` (Kleene star): Precedence 3 (highest)
- `.` (Concatenation): Precedence 2
- `|` (Alternation): Precedence 1 (lowest)

**Algorithm** (Classic Shunting-Yard):
```
For each character in infix expression:
  - If operand (alphanumeric): Output immediately
  - If '(': Push to operator stack
  - If ')': Pop operators until '(' found, output each
  - If operator: Pop operators with ≥ precedence, output them, then push current
  
After loop: Pop all remaining operators
```

**Example**:
```
Infix:  (A|G).(A|G).*
Tokens: ( A | G ) . ( A | G ) . *

Process:
1. '(': push → Stack: [(]
2. 'A': output → Output: A
3. '|': push → Stack: [(, |]
4. 'G': output → Output: AG
5. ')': pop until '(' → Output: AG|, Stack: []
6. '.': push → Stack: [.]
7. '(': push → Stack: [., (]
8. 'A': output → Output: AG|A
9. '|': push → Stack: [., (, |]
10. 'G': output → Output: AG|AG
11. ')': pop until '(' → Output: AG|AG|, Stack: [.]
12. '.': pop '.' (equal precedence) → Output: AG|AG|., push new '.' → Stack: [.]
13. '*': pop '.' (higher) → Output: AG|AG|.*, Stack: [*]
14. End: pop all → Output: AG|AG|.*.

Final Postfix: AG|AG|.*. 
(Represents: (A|G).(A|G).*)
```

**Input**: `(A|G).(A|G).*`  
**Output**: `AG|AG|.*. `

#### Step 4: Thompson's Construction (Postfix → NFA)
**Location**: `thompsons_construction.cpp`

Builds NFA fragment-by-fragment using a stack, processing postfix expression:

##### NFA Fragment Representation
```cpp
struct NFAFragment {
    NFAState* start;              // Entry state
    vector<NFAState*> finals;     // Accept states (can be multiple for OR)
};
```

##### Four Basic Building Blocks

**A. Single Character** (`makeChar(c)`)
```
Creates: start -[c]-> end
```
```
A
  ↓
(0) --[A]--> (1)
    start     final
```

**B. Concatenation** (`makeConcat(A, B)`)
Connects A's final states to B's start with epsilon transition:
```
A.B
  ↓
(0) --[A]--> (1) --[ε]--> (2) --[B]--> (3)
   start                            final
   
Merge: Finals of A point to start of B
```

**C. Alternation/Union** (`makeUnion(A, B)`)
Creates new start & end states, connects with epsilon:
```
A|B
  ↓
         -[ε]--> (1) --[A]--> (2) -[ε]--
       /                                \
(0) -|-[ε]--> (3) --[B]--> (4) -[ε]--> (5)
    \                                   /
     -[ε]--> (new_start)... (new_end) -

New start has epsilon to both A.start and B.start
Both A.finals and B.finals have epsilon to new end
```

**D. Kleene Star** (`makeStar(A)`)
Creates loops with epsilon transitions:
```
A*
  ↓
     -[ε]-----
    /         \
(0) --[ε]--> (1) --[A]--> (2) --[ε]--> (3)
 \                          /   ↑      /
  --[ε]----[loop]----------
   (epsilon bypass & loop-back)

New start has epsilon to A.start and new end
A.finals loop back to A.start
A.finals also have epsilon to new end
```

##### Stack-Based Processing

**Algorithm** (Process postfix expression):
```
For each character in postfix:
  - If operand: Create character fragment, push to stack
  - If '.': Pop 2 fragments, concatenate, push result
  - If '|': Pop 2 fragments, union, push result
  - If '*': Pop 1 fragment, star, push result

Final: Stack must contain exactly 1 fragment (the complete NFA)
```

**Example** (Postfix: `AG|AG|.*. `):
```
1. A → push makeChar(A)           Stack: [Frag_A]
2. G → push makeChar(G)           Stack: [Frag_A, Frag_G]
3. | → pop 2, union              Stack: [Frag_(A|G)]
4. A → push makeChar(A)           Stack: [Frag_(A|G), Frag_A]
5. G → push makeChar(G)           Stack: [Frag_(A|G), Frag_A, Frag_G]
6. | → pop 2, union              Stack: [Frag_(A|G), Frag_(A|G)]
7. . → pop 2, concat             Stack: [Frag_((A|G).(A|G))]
8. * → pop 1, star               Stack: [Frag_((A|G).(A|G))* ]
9. . → pop 2, concat             Stack: [Frag_((A|G).(A|G).*)]

Result: Complete NFA for pattern (A|G)+
```

**Output**: `NFAFragment { start: (0), finals: {...} }`

#### Step 5: NFA Simulation (Subset Construction Algorithm)
**Location**: `nfa_simulator.cpp`

Simulates the NFA on an input string using subset construction.

##### Epsilon Closure
**Function**: `getEpsilonClosure(state, visited, closure)`

Computes all states reachable from a given state via epsilon (ε) transitions:

```
Algorithm (DFS):
1. Mark current state as visited (by ID)
2. Add to closure set
3. Follow all epsilon (E) transitions
4. Recursively apply to each reachable state
```

**Time**: O(|states| + |transitions|)

Example from our NFA:
```
If state 0 has: 0 --[ε]--> 1 --[ε]--> 2 --[A]--> 3
Then: getEpsilonClosure(0) = {0, 1, 2}
(Does NOT include 3 because it requires consuming 'A')
```

##### String Matching
**Function**: `simulateNFA(nfa, input_string)`

**Algorithm**:
```
1. currentStates = getEpsilonClosure(nfa.start)
   (All states reachable without consuming any input)

2. For each character c in input string:
     a. nextStates = {}
     b. For each state s in currentStates:
        - If s has transition on c: 
          - For each reachable state n via c:
            - Add getEpsilonClosure(n) to nextStates
     c. currentStates = nextStates
     d. If currentStates is empty → REJECT (dead state)

3. Accept if any state in currentStates is in nfa.finals
```

**Example** (Test string: "AGAGA"):
```
NFA Pattern: (A|G)+ → represents A, AG, AAA, AGAG, AGAGA, etc.

Initial: currentStates = epsilon_closure(start)

Character 'A':
  - From currentStates, follow [A] transitions
  - Get epsilon closure of reachable states
  - currentStates = {state_after_A}

Character 'G':
  - From state_after_A, follow [G] transitions
  - currentStates = {state_after_AG}

Character 'A':
  - currentStates = {state_after_AGA}

Character 'G':
  - currentStates = {state_after_AGAG}

Character 'A':
  - currentStates = {state_after_AGAGA}

Check: Is state_after_AGAGA a final state? YES → MATCH ✓
```

**Time Complexity**: O(|input| × |states|²) worst case

---

## Phase 2: Syntactic Analysis

### Objective
Parse a token stream using context-free grammar with bio-inspired adaptive error recovery.

### Process Flow

#### Grammar Definition
**DNA Hairpin Nesting** (Context-Free):
```
S → A S T          (Rule 0: Nest with A-T pair)
S → G S C          (Rule 1: Nest with G-C pair)
S → .              (Rule 2: Terminate)
```

**Meaning**:
- Valid sequences: ".", "A.T", "G.C", "AAGCTT", "AG.CT"
- Invalid: "AT", "GC" (missing terminal dot), "AU.T" (mismatched pair)

**Example parse** for "AG.CT":
```
String: A G . C T

Derivation:
S ⇒ A S T          (Apply Rule 0)
  ⇒ A (G S C) T    (Apply Rule 1)
  ⇒ A (G (.) C) T  (Apply Rule 2)
  ⇒ A G . C T      (Terminate)

Parse tree:
       S
      /│\
     A │ T
       S
      /│\
     G │ C
       S
       │
       .
```

#### LL(1) Parsing Table

**Definition**: Maps (non-terminal, lookahead) → production rule

```
        A    G    .
    ┌────┬────┬────┐
  S │ 0  │ 1  │ 2  │
    └────┴────┴────┘

Meaning:
- S with lookahead A → Apply rule 0 (S → A S T)
- S with lookahead G → Apply rule 1 (S → G S C)
- S with lookahead . → Apply rule 2 (S → .)
```

#### Parsing Algorithm
**Location**: `adaptive_pda.cpp - parse()`

**Classic LL(1) Stack-Based Parser**:

```
Algorithm:
1. Initialize: stack = [$, S], ptr = 0, tokens += [$]
2. While stack not empty:
     a. top = stack.pop()
     b. lookahead = tokens[ptr]
     
     c. If top is terminal (not in parsing table):
        - If top == lookahead: match! → pop stack, advance ptr
        - Else: ERROR (or try adaptive repair)
        
     d. If top is non-terminal (in parsing table):
        - Look up parsingTable[top][lookahead]
        - Get production rule
        - Pop stack
        - Push RHS of rule in REVERSE order
        
3. Accept if stack = $ and lookahead = $
```

**Example** (Parse "AG.CT"):
```
Stack            Input         Action
---              ---           ------
[$, S]           AG.CT         Expand S → A S T (rule 0)
[$, T, S, A]     AG.CT         Match A, advance
[$, T, S]        G.CT          Expand S → G S C (rule 1)
[$, T, C, S, G]  G.CT          Match G, advance
[$, T, C, S]     .CT           Expand S → . (rule 2)
[$, T, C, .]     .CT           Match ., advance
[$, T, C]        CT            Match C, advance
[$, T]           T             Match T, advance
[$]              $             Accept! ✓
```

**Time Complexity**: O(|input| + |grammar|)

#### Affinity Matrix (Bio-Inspired Heuristic)

When a token mismatch occurs, instead of hard failure, compute **affinity score**:

```cpp
affinityMatrix[expected][actual] = probability_score (0.0 to 1.0)

Example:
T ↔ U:  0.95  (RNA base U acts like DNA T → HIGH)
C ↔ U:  0.60  (Wobble pairing in RNA → MEDIUM)
C ↔ A:  0.05  (Purine clash → LOW)
```

**Biological Inspiration**:
- DNA uses A-T and G-C pairs
- RNA uses A-U and G-C pairs
- Wobble pairing allows some flexibility
- Incompatible bases cannot substitute

#### Adaptive Repair Mechanism
**Location**: `adaptive_pda.cpp - adaptiveRepair()`

**Three-Tier Decision Policy**:

```
Mismatch detected: Expected [T], Found [U]

1. Look up affinity = affinityMatrix[T][U] = 0.95

2. Apply threshold logic:
   - If affinity > 0.8:     HIGH confidence
     → Accept substitution, learn mapping (U→T)
     
   - Else if affinity > 0.5: MEDIUM confidence
     → Warn but continue (wobble pair)
     
   - Else:                   LOW confidence
     → ABORT (structural conflict)

3. Maintain adaptiveMap: learned token equivalences
   - Once U→T is learned, U automatically matches T
```

**Example** (Input: "AG.CU" with grammar expecting "AG.CT"):
```
Process:
1. Tokens: [A, G, ., C, U]
2. Parse A, G, . → successful
3. At position C: successful
4. At position U: Expected T, found U
   - affinity[T][U] = 0.95
   - 0.95 > 0.8 → ACCEPT
   - Learn: U → T
5. Continue parsing with learned mapping
6. Result: ACCEPTED ✓

Without adaptive repair: HARD REJECTION ✗
```

#### Full Parsing Trace with Adaptive Recovery

```
[PHASE 2] Syntactic Analysis (Heuristic DNA Repair)
Grammar: S -> A S T | G S C | .

Simulating Raw Input: AG.CU
Scenario: 'A' expects 'T' to close. Found 'U'.

--- Starting DNA Hairpin Parser (Context-Free) ---
STACK              INPUT           ACTION
---------------    ---------------  -----------------------
$S                 AG.CU$           Expand S -> A S T
$TSA               AG.CU$           Match A
$TS                G.CU$            Expand S -> G S C
$TSCSG             G.CU$            Match G
$TSC               .CU$             Expand S -> .
$TSC.              .CU$             Match .
$TSC               CU$              Match C
$TS                U$               
[!] ALERT: Mismatch. Expected [T], Found [U]
[*] BIO-ADAPTIVE UNIT: Calculating affinity heuristic...
[*] HEURISTIC SCORE: 0.95 / 1.0
[*] DECISION: High affinity detected. Treating as valid substitution.
[*] SYSTEM REPAIRED: Token equivalence map updated.
$TS                U$               Match T (via U)
$S                 $                Match $
STRUCTURE STABLE

[CONCLUSION] Heuristic threshold met (0.95 > 0.8). Mutation accepted.
```

---

## Complete Process Flow

### Unified Workflow

```
┌─────────────────────────────────────────────────────────────┐
│           PHASE 1: LEXICAL ANALYSIS                        │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Input: Regex "(A|G)+"                                      │
│    ↓                                                          │
│  [preprocessRegex]  →  Expand '+', add explicit '.'         │
│    ↓                                                          │
│  Intermediate: "(A|G).(A|G).*"                              │
│    ↓                                                          │
│  [toPostfix]  →  Convert to RPN (Shunting-yard)             │
│    ↓                                                          │
│  Postfix: "AG|AG|.*."                                        │
│    ↓                                                          │
│  [regexToNFA]  →  Build NFA (Thompson's Construction)       │
│    ↓                                                          │
│  NFA Fragment: {start: 0, finals: {...}}                    │
│    ↓                                                          │
│  [simulateNFA]  →  Test input "AGAGA" (Subset Construction) │
│    ↓                                                          │
│  Result: MATCH ✓                                             │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│           PHASE 2: SYNTACTIC ANALYSIS                       │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Input: Tokens [A, G, ., C, U]                              │
│    ↓                                                          │
│  Grammar: S → A S T | G S C | .                             │
│    ↓                                                          │
│  [parse]  →  LL(1) stack-based parsing                      │
│    ↓                                                          │
│  Mismatch at U (expected T)                                  │
│    ↓                                                          │
│  [adaptiveRepair]  →  Check affinity[T][U] = 0.95          │
│    ↓                                                          │
│  0.95 > 0.8 → HIGH confidence → ACCEPT                      │
│    ↓                                                          │
│  Learn mapping: U → T                                        │
│    ↓                                                          │
│  Continue parsing with learned equivalence                  │
│    ↓                                                          │
│  Result: STRUCTURE STABLE ✓                                 │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## Build & Run

### Prerequisites
- C++11 or later
- Standard compiler (g++, clang, MSVC)

### SFML GUI (recommended on Windows via MSYS2)
If you want the GUI window with buttons:
1) Install MSYS2, open the **UCRT64** shell.
2) Update and install toolchain + SFML:
  ```bash
  pacman -Syu
  pacman -Syu
  pacman -S mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-sfml
  ```
3) In the UCRT64 shell, build the GUI:
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
4) The GUI shows buttons for Phase 1 (regex -> NFA) and Phase 2 (adaptive parser). Press ESC or Quit to exit.

### Compilation

**Single command** (compile all files):
```bash
g++ -std=c++11 -o atflparser \
    main.cpp \
    nfa_state.cpp \
    regex_preprocessor.cpp \
    thompsons_construction.cpp \
    nfa_simulator.cpp \
    adaptive_pda.cpp
```

**Or with Make** (if Makefile exists):
```bash
make
```

### Execution
```bash
./atflparser
```

### Expected Output
```
==================================================
 PROJECT: Resilient Compiler Simulation (Modular)
==================================================

[PHASE 1] Robust Lexical Analysis
Regex Pattern: (A|G)+
Preprocessed:  (A|G).(A|G).*
Postfix:       AG|AG|.*.
Testing string 'AGAGA': MATCH

[PHASE 2] Syntactic Analysis (Heuristic DNA Repair)
Grammar: S -> A S T | G S C | .

Simulating Raw Input: AG.CU
Scenario: 'A' expects 'T' to close. Found 'U'.

--- Starting DNA Hairpin Parser (Context-Free) ---
STACK              INPUT           ACTION
$S                 AG.CU$          Expand S -> A S T
[... parsing trace ...]

[CONCLUSION] Heuristic threshold met (0.95 > 0.8). Mutation accepted.
```

---

## Algorithm Details

### Complexity Analysis

| Algorithm | Time | Space | Notes |
|-----------|------|-------|-------|
| Regex Preprocessing | O(n) | O(n) | Two passes, linear scan |
| Shunting-Yard (Postfix) | O(n) | O(n) | n = regex length |
| Thompson's Construction | O(n) | O(n) | n = postfix length, creates ~2n states |
| Epsilon Closure (DFS) | O(\|S\| + \|T\|) | O(\|S\|) | S = states, T = transitions |
| NFA Simulation | O(\|I\| × \|S\|²) | O(\|S\|) | I = input, worst case subset enumeration |
| LL(1) Parsing | O(\|I\|) | O(\|G\|) | I = tokens, G = grammar size |

### Space Optimization

**StateManager with unique_ptr**:
- Automatic memory management
- No manual delete needed
- Clear() deallocates all in bulk
- Prevents memory leaks

### Error Handling

**Phase 1**: Stack validation, regex syntax checking  
**Phase 2**: Token affinity heuristics, graceful degradation

---

## Extension Points

### Adding New Operators
Modify `regex_preprocessor.cpp`:
1. Add precedence in `precedence()`
2. Handle in `preprocessRegex()` if syntactic sugar
3. Add handler in `regexToNFA()` stack processing

### Custom Grammar
Modify `adaptive_pda.cpp`:
1. Add productions to `grammar` vector
2. Populate `parsingTable` with LL(1) entries
3. Update `affinityMatrix` with domain-specific heuristics

### Alternative Parsing
Replace `adaptive_pda.cpp` with:
- SLR(1) for larger grammars
- Earley parser for ambiguous grammars
- LALR(1) for better performance

---

## References

- **Thompson's Construction**: Thompson, K. (1968). "Regular Expression Search Algorithm"
- **Subset Construction**: Rabin & Scott (1959). "Finite automata and their decision problems"
- **Shunting-Yard Algorithm**: Dijkstra (1961). "The shunting yard algorithm"
- **LL(1) Parsing**: Aho, Sethi, Ullman (1986). "Compilers: Principles, Techniques, and Tools"

---

## Author Notes

This project demonstrates:
✓ Fundamental compiler concepts in modular structure  
✓ Classic algorithms (Thompson, subset construction, shunting-yard, LL(1))  
✓ Bio-inspired heuristics for graceful error recovery  
✓ Modern C++ practices (unique_ptr, STL containers)  
✓ Clear separation of concerns for educational value  

**Key Innovation**: The adaptive repair mechanism combines formal parsing with probabilistic heuristics, allowing the parser to recover from errors intelligently rather than failing hard.

---

## License

This is an educational project. Use freely for learning purposes.

---

**Last Updated**: December 7, 2025