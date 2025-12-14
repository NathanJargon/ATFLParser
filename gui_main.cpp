#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <set>

#include "adaptive_pda.h"
#include "nfa_simulator.h"
#include "regex_preprocessor.h"
#include "thompsons_construction.h"

struct InputBox {
    sf::RectangleShape box;
    sf::Text placeholder;
    sf::Text content;
    std::string text;
    bool focused = false;

    InputBox() = default;
    InputBox(const sf::Font& font, const std::string& hint, sf::Vector2f pos, sf::Vector2f size)
        : placeholder(font, hint, 14), content(font, "", 14) {
        box.setSize(size);
        box.setPosition(pos);
        box.setFillColor(sf::Color(25, 35, 55));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(100, 140, 200));

        placeholder.setFillColor(sf::Color(150, 150, 170));
        placeholder.setPosition(pos + sf::Vector2f{8.f, 8.f});

        content.setFillColor(sf::Color(220, 220, 240));
        content.setPosition(pos + sf::Vector2f{8.f, 8.f});
    }

    void handleInput(unsigned int c) {
        if (focused && text.length() < 50) {
            if (c == 8) { // Backspace
                if (!text.empty()) text.pop_back();
            } else if (c >= 32 && c < 127) {
                text += static_cast<char>(c);
            }
            content.setString(text);
        }
    }

    void clear() {
        text.clear();
        content.setString("");
    }

    void draw(sf::RenderWindow& w) const {
        w.draw(box);
        if (text.empty()) w.draw(placeholder);
        else w.draw(content);
    }
};

struct Button {
    sf::RectangleShape box;
    sf::Text label;
    bool hovered = false;

    Button() = default;
    Button(const sf::Font& font, const std::string& text, sf::Vector2f pos)
        : label(font, text, 20) {
        box.setSize({200.f, 48.f});
        box.setPosition(pos);
        box.setFillColor(sf::Color(50, 80, 130));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(90, 130, 180));

        label.setFillColor(sf::Color::White);
        label.setPosition(pos + sf::Vector2f{12.f, 10.f});
    }
    
    Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size, unsigned int fontSize)
        : label(font, text, fontSize) {
        box.setSize(size);
        box.setPosition(pos);
        box.setFillColor(sf::Color(50, 80, 130));
        box.setOutlineThickness(1.f);
        box.setOutlineColor(sf::Color(90, 130, 180));

        label.setFillColor(sf::Color::White);
        label.setPosition(pos + sf::Vector2f{6.f, 4.f});
    }

    bool contains(sf::Vector2f p) const {
        return box.getGlobalBounds().contains(p);
    }

    void setHover(bool h) {
        hovered = h;
        box.setFillColor(h ? sf::Color(70, 110, 170) : sf::Color(50, 80, 130));
    }

    void draw(sf::RenderWindow& w) const {
        w.draw(box);
        w.draw(label);
    }
};

static std::string runPhase1(const std::string& regex, const std::string& testInputs, bool showTrace = false) {
    if (regex.empty()) return "Error: Please enter a regex pattern.";

    try {
        // IMPORTANT: Clear previous states to prevent accumulation
        StateManager::clear();
        StateManager::resetID();
        
        std::string processed = preprocessRegex(regex);
        std::string postfix = toPostfix(processed);

        std::ostringstream oss;
        oss << "=== LEXICAL ANALYSIS: Regular Expression -> NFA ===\n\n";
        
        // Step 1: Pattern
        oss << "[1] Input Pattern:\n";
        oss << "    " << regex << "\n\n";
        
        // Step 2: Preprocessing
        oss << "[2] Character Class Expansion & Preprocessing:\n";
        oss << "    " << processed << "\n";
        oss << "    (Character classes expanded to unions)\n\n";
        
        // Step 3: Postfix
        oss << "[3] Postfix Notation (RPN):\n";
        oss << "    " << postfix << "\n";
        oss << "    (Ready for Thompson's NFA construction)\n\n";

        // Step 4: Build NFA
        NFAFragment nfa = regexToNFA(postfix);
        
        // Count states
        int stateCount = StateManager::getStateCount();
        oss << "[4] Thompson's NFA Construction:\n";
        oss << "    States created: " << stateCount << "\n";
        oss << "    Start state: q" << nfa.start->id << "\n";
        oss << "    Final states: ";
        for (size_t i = 0; i < nfa.finals.size(); i++) {
            oss << "q" << nfa.finals[i]->id;
            if (i < nfa.finals.size() - 1) oss << ", ";
        }
        oss << "\n\n";
        
        // Show sample transitions (BFS to find key paths)
        oss << "    Key Transitions:\n";
        std::set<NFAState*> visited;
        std::deque<NFAState*> queue;
        queue.push_back(nfa.start);
        visited.insert(nfa.start);
        int pathCount = 0;
        
        while (!queue.empty() && pathCount < 5) {
            NFAState* current = queue.front();
            queue.pop_front();
            
            for (const auto& [ch, nexts] : current->transitions) {
                for (NFAState* next : nexts) {
                    if (visited.find(next) == visited.end()) {
                        std::string display = (ch == 'E') ? "e" : std::string(1, ch);
                        oss << "      q" << current->id << " --[" << display << "]--> q" << next->id << "\n";
                        visited.insert(next);
                        queue.push_back(next);
                        pathCount++;
                        if (pathCount >= 5) break;
                    }
                }
                if (pathCount >= 5) break;
            }
        }
        if (pathCount > 0) oss << "\n";
        
        // Step 5: Test strings
        if (testInputs.empty()) {
            oss << "[5] NFA Simulation:\n";
            oss << "    Enter test strings separated by spaces.\n";
        } else {
            oss << "[5] NFA Simulation (Subset Construction):\n";
            oss << "    Testing strings against NFA...\n\n";
            
            // Split test inputs by spaces
            std::istringstream stream(testInputs);
            std::string word;
            int testNum = 1;
            while (stream >> word) {
                bool result = simulateNFA(nfa, word);
                oss << "    Test " << testNum << ": \"" << word << "\"\n";
                oss << "      Result: " << (result ? "[MATCH] Accepted" : "[NO MATCH] Rejected") << "\n";
                
                if (showTrace && testNum == 1) {
                    oss << "\n      --- Detailed Trace (First Test Only) ---\n";
                    oss << simulateNFAWithTrace(nfa, word);
                    oss << "      --- End Trace ---\n";
                }
                
                testNum++;
            }
            
            oss << "\n";
        }
        
        // Summary
        oss << "--- Summary ---\n";
        oss << "Regular Language: Recognized by finite automaton\n";
        oss << "Equivalence: Regex == NFA == DFA == Regular Grammar\n";

        StateManager::clear();
        return oss.str();
    } catch (const std::exception& e) {
        return std::string("Error: ") + e.what();
    }
}

static std::string runPDA(const std::string& input) {
    if (input.empty()) return "Error: Please enter a string to check.";
    
    std::ostringstream oss;
    oss << "=== SYNTACTIC ANALYSIS: Pushdown Automaton ===\n\n";
    oss << "[Context-Free Language: a^n b^n (equal a's then b's)]\n\n";
    oss << "Grammar:\n";
    oss << "  S -> a S b | S S | ε\n";
    oss << "  (Letters instead of parentheses)\n\n";
    oss << "Input: " << input << "\n\n";
    
    // Align with 5-step style and note missing lexical steps
    oss << "[1] Regex / NFA: Not applicable for PDA (uses stack instead)\n";
    oss << "[2] Thompson NFA: Not applicable\n";
    oss << "[3] DFA Minimization: Not applicable\n";
    oss << "[4] PDA Stack Simulation (push on 'a', pop on 'b'):\n\n";
    
    std::vector<char> stack;
    bool valid = true;
    int step = 1;
    
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == ' ') continue; // ignore spaces

        oss << "  Step " << step++ << ": Read '" << c << "' at position " << i << "\n";
        
        if (c == 'a') {
            stack.push_back('A');
            oss << "           Action: PUSH 'A' onto stack (saw 'a')\n";
        } else if (c == 'b') {
            if (stack.empty()) {
                oss << "           Action: POP failed - Stack is empty!\n";
                oss << "           ERROR: Extra 'b' with no matching 'a'\n";
                valid = false;
                break;
            }
            char top = stack.back();
            stack.pop_back();
            oss << "           Action: POP '" << top << "' (matched 'b')\n";
        } else {
            oss << "           Action: REJECT - Only 'a' then 'b' are allowed\n";
            valid = false;
            break;
        }
        
        oss << "           Stack: [";
        for (size_t j = 0; j < stack.size(); j++) {
            oss << stack[j];
            if (j < stack.size() - 1) oss << ", ";
        }
        if (stack.empty()) oss << "empty";
        oss << "]\n\n";
    }
    
    if (valid && !stack.empty()) {
        oss << "  Final Check: Stack not empty!\n";
        oss << "  ERROR: Remaining 'a' without matching 'b': ";
        for (char c : stack) oss << c;
        oss << "\n";
        valid = false;
    }
    
    oss << "[5] Result:\n";
    if (valid) {
        oss << "[ACCEPT] String is in a^n b^n (equal a's then b's)\n";
        oss << "This is a valid context-free language string\n";
    } else {
        oss << "[REJECT] String is not in a^n b^n\n";
    }
    
    oss << "\nNote: PDAs use a stack (infinite memory) unlike\n";
    oss << "      finite automata (finite memory).\n";
    oss << "      This allows recognition of context-free languages.\n";
    
    return oss.str();
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1100u, 700u}), "Formal Language Hierarchy - Lexical & Syntactic Analysis", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Could not load font." << std::endl;
    }

    // LEFT PANEL - CONTROLS
    sf::RectangleShape leftPanel(sf::Vector2f(320.f, 670.f));
    leftPanel.setPosition({10.f, 20.f});
    leftPanel.setFillColor(sf::Color(20, 25, 40));
    leftPanel.setOutlineThickness(2.f);
    leftPanel.setOutlineColor(sf::Color(70, 110, 170));

    // MODE SELECTOR
    sf::Text modeLabel(font, "Select Mode:", 16);
    modeLabel.setFillColor(sf::Color(150, 200, 255));
    modeLabel.setPosition({25.f, 55.f});

    sf::Text mode1(font, "[ ] Regular Languages (NFA)", 13);
    mode1.setFillColor(sf::Color(200, 200, 200));
    mode1.setPosition({25.f, 85.f});

    sf::Text mode2(font, "[ ] Context-Free (PDA)", 13);
    mode2.setFillColor(sf::Color(200, 200, 200));
    mode2.setPosition({25.f, 105.f});

    bool isRegularMode = true;  // Toggle between modes

    // REGULAR LANGUAGE INPUT
    sf::Text regexLabel(font, "Regular Expression:", 13);
    regexLabel.setFillColor(sf::Color(200, 200, 200));
    regexLabel.setPosition({25.f, 140.f});

    InputBox regexInput(font, "[a-z]+", {25.f, 160.f}, {280.f, 32.f});

    sf::Text testLabel(font, "Test Strings (space-separated):", 13);
    testLabel.setFillColor(sf::Color(200, 200, 200));
    testLabel.setPosition({25.f, 205.f});

    InputBox testInput(font, "hello world abc123", {25.f, 225.f}, {280.f, 32.f});

    // PDA INPUT (Context-Free)
    sf::Text pdaLabel(font, "Letters String (a^n b^n):", 13);
    pdaLabel.setFillColor(sf::Color(200, 200, 200));
    pdaLabel.setPosition({25.f, 140.f});

    InputBox pdaInput(font, "aaabbb", {25.f, 160.f}, {280.f, 32.f});

    // OPTIONS
    sf::Text optionsLabel(font, "Options:", 11);
    optionsLabel.setFillColor(sf::Color(120, 160, 200));
    optionsLabel.setPosition({25.f, 275.f});

    sf::Text traceOption(font, "[ ] Show step-by-step trace", 10);
    traceOption.setFillColor(sf::Color(180, 180, 180));
    traceOption.setPosition({25.f, 295.f});

    bool showTrace = false;

    sf::Text phase1Ex(font, "Sample Inputs:", 11);
    phase1Ex.setFillColor(sf::Color(120, 160, 200));
    phase1Ex.setPosition({25.f, 325.f});

    // Sample buttons for Regular mode
    Button btnSample1(font, "[a-z]+", {25.f, 345.f}, {130.f, 25.f}, 10);
    Button btnSample2(font, "[a-zA-Z][a-zA-Z0-9]*", {165.f, 345.f}, {140.f, 25.f}, 10);
    Button btnSample3(font, "[0-9]+", {25.f, 375.f}, {130.f, 25.f}, 10);
    
    // Sample buttons for PDA mode
    Button btnSamplePDA1(font, "aaabbb", {25.f, 345.f}, {100.f, 25.f}, 10);
    Button btnSamplePDA2(font, "aabb", {135.f, 345.f}, {100.f, 25.f}, 10);
    Button btnSamplePDA3(font, "aaabb", {245.f, 345.f}, {60.f, 25.f}, 10);

    Button btnAnalyze(font, "Run Analysis", {25.f, 410.f});
    Button btnToggleMode(font, "Switch Mode", {25.f, 470.f});
    Button btnClear(font, "Clear All", {25.f, 530.f});
    Button btnQuit(font,  "Quit",      {25.f, 590.f});

    // RIGHT PANEL - OUTPUT LOG (taller for better visibility)
    sf::RectangleShape rightPanel(sf::Vector2f(755.f, 645.f));
    rightPanel.setPosition({335.f, 20.f});
    rightPanel.setFillColor(sf::Color(15, 20, 35));
    rightPanel.setOutlineThickness(2.f);
    rightPanel.setOutlineColor(sf::Color(80, 120, 180));

    sf::Text outputLabel(font, "Analysis Results", 16);
    outputLabel.setFillColor(sf::Color(150, 200, 255));
    outputLabel.setPosition({350.f, 30.f});

    std::deque<std::string> logLines;
    logLines.push_back("Select a mode and enter input to begin.");
    logLines.push_back("Regular Languages: Finite automata (no memory)");
    logLines.push_back("Context-Free: Pushdown automata (stack memory)");
    
    int scrollOffset = 0;  // Track scroll position
    
    auto updateHover = [&](sf::Vector2f mpos) {
        btnAnalyze.setHover(btnAnalyze.contains(mpos));
        btnToggleMode.setHover(btnToggleMode.contains(mpos));
        btnClear.setHover(btnClear.contains(mpos));
        btnQuit.setHover(btnQuit.contains(mpos));
        
        if (isRegularMode) {
            btnSample1.setHover(btnSample1.contains(mpos));
            btnSample2.setHover(btnSample2.contains(mpos));
            btnSample3.setHover(btnSample3.contains(mpos));
        } else {
            btnSamplePDA1.setHover(btnSamplePDA1.contains(mpos));
            btnSamplePDA2.setHover(btnSamplePDA2.contains(mpos));
            btnSamplePDA3.setHover(btnSamplePDA3.contains(mpos));
        }
    };

    int focusedInput = -1; // 0=regex/pda, 1=test, -1=none

    while (window.isOpen()) {
        while (auto eventOpt = window.pollEvent()) {
            const sf::Event& event = *eventOpt;
            
            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scan::Escape) {
                    window.close();
                }
            }
            
            // Handle scroll wheel for output panel
            if (const auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>()) {
                int maxScroll = static_cast<int>(logLines.size()) - 20;
                if (scroll->delta > 0) {
                    scrollOffset = std::max(0, scrollOffset - 3);
                } else {
                    scrollOffset = std::min(maxScroll, scrollOffset + 3);
                }
            }

            if (const auto* text = event.getIf<sf::Event::TextEntered>()) {
                if (isRegularMode) {
                    if (focusedInput == 0) regexInput.handleInput(text->unicode);
                    else if (focusedInput == 1) testInput.handleInput(text->unicode);
                } else {
                    if (focusedInput == 0) pdaInput.handleInput(text->unicode);
                }
            }

            if (const auto* move = event.getIf<sf::Event::MouseMoved>()) {
                sf::Vector2f mpos{static_cast<float>(move->position.x), static_cast<float>(move->position.y)};
                updateHover(mpos);
            }

            if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mpos{static_cast<float>(click->position.x), static_cast<float>(click->position.y)};
                    
                    // Mode selection clicks
                    if (mode1.getGlobalBounds().contains(mpos)) {
                        isRegularMode = true;
                    } else if (mode2.getGlobalBounds().contains(mpos)) {
                        isRegularMode = false;
                    }
                    
                    // Trace option toggle
                    if (traceOption.getGlobalBounds().contains(mpos)) {
                        showTrace = !showTrace;
                    }
                    
                    // Input focus
                    if (isRegularMode) {
                        if (regexInput.box.getGlobalBounds().contains(mpos)) {
                            focusedInput = 0;
                            regexInput.focused = true;
                            testInput.focused = false;
                            pdaInput.focused = false;
                        } else if (testInput.box.getGlobalBounds().contains(mpos)) {
                            focusedInput = 1;
                            regexInput.focused = false;
                            testInput.focused = true;
                            pdaInput.focused = false;
                        } else {
                            regexInput.focused = testInput.focused = pdaInput.focused = false;
                            focusedInput = -1;
                        }
                    } else {
                        if (pdaInput.box.getGlobalBounds().contains(mpos)) {
                            focusedInput = 0;
                            pdaInput.focused = true;
                            regexInput.focused = false;
                            testInput.focused = false;
                        } else {
                            regexInput.focused = testInput.focused = pdaInput.focused = false;
                            focusedInput = -1;
                        }
                    }

                    // Sample button clicks - auto-run after loading
                    bool shouldRun = false;
                    if (isRegularMode) {
                        if (btnSample1.contains(mpos)) {
                            regexInput.text = "[a-z]+";
                            testInput.text = "hello world abc";
                            shouldRun = true;
                        } else if (btnSample2.contains(mpos)) {
                            regexInput.text = "[a-zA-Z][a-zA-Z0-9]*";
                            testInput.text = "myVar _test 123invalid";
                            shouldRun = true;
                        } else if (btnSample3.contains(mpos)) {
                            regexInput.text = "[0-9]+";
                            testInput.text = "123 456 789";
                            shouldRun = true;
                        }
                    } else {
                        if (btnSamplePDA1.contains(mpos)) {
                            pdaInput.text = "aaabbb";
                            shouldRun = true;
                        } else if (btnSamplePDA2.contains(mpos)) {
                            pdaInput.text = "aabb";
                            shouldRun = true;
                        } else if (btnSamplePDA3.contains(mpos)) {
                            pdaInput.text = "aaabb"; // intentionally unbalanced example
                            shouldRun = true;
                        }
                    }
                    
                    if (btnAnalyze.contains(mpos) || shouldRun) {
                        std::string result;
                        if (isRegularMode) {
                            result = runPhase1(regexInput.text, testInput.text, showTrace);
                        } else {
                            result = runPDA(pdaInput.text);
                        }
                        logLines.clear();
                        scrollOffset = 0;  // Reset scroll to top when new analysis runs
                        std::istringstream stream(result);
                        std::string line;
                        while (std::getline(stream, line)) {
                            logLines.push_back(line);
                        }
                    } else if (btnToggleMode.contains(mpos)) {
                        isRegularMode = !isRegularMode;
                        logLines.clear();
                        logLines.push_back(isRegularMode ? "Mode: Regular Languages (NFA)" : "Mode: Context-Free (PDA)");
                    } else if (btnClear.contains(mpos)) {
                        regexInput.clear();
                        testInput.clear();
                        pdaInput.clear();
                        logLines.clear();
                        logLines.push_back("Cleared.");
                    } else if (btnQuit.contains(mpos)) {
                        window.close();
                    }
                }
            }
        }

        window.clear(sf::Color(10, 14, 20));
        
        // Draw panels
        window.draw(leftPanel);
        window.draw(rightPanel);
        
        // Draw title & labels
        window.draw(modeLabel);
        
        // Update mode checkboxes dynamically
        mode1.setString((isRegularMode ? "[X] " : "[ ] ") + std::string("Regular Languages (NFA)"));
        mode2.setString((!isRegularMode ? "[X] " : "[ ] ") + std::string("Context-Free (PDA)"));
        traceOption.setString((showTrace ? "[X] " : "[ ] ") + std::string("Show step-by-step trace"));
        
        window.draw(mode1);
        window.draw(mode2);
        window.draw(optionsLabel);
        window.draw(traceOption);
        
        // Draw mode-specific inputs and examples
        if (isRegularMode) {
            window.draw(regexLabel);
            window.draw(testLabel);
            regexInput.draw(window);
            testInput.draw(window);
            window.draw(phase1Ex);
            btnSample1.draw(window);
            btnSample2.draw(window);
            btnSample3.draw(window);
        } else {
            window.draw(pdaLabel);
            pdaInput.draw(window);
            window.draw(phase1Ex);
            btnSamplePDA1.draw(window);
            btnSamplePDA2.draw(window);
            btnSamplePDA3.draw(window);
        }
        window.draw(outputLabel);
        
        // Draw buttons
        btnAnalyze.draw(window);
        btnToggleMode.draw(window);
        btnClear.draw(window);
        btnQuit.draw(window);
        
        // Draw output log (scrollable, up to 40 lines, with truncation for very long lines)
        size_t maxLines = 40;
        size_t startIdx = scrollOffset;
        if (startIdx >= logLines.size()) startIdx = logLines.size() > maxLines ? logLines.size() - maxLines : 0;
        
        float logY = 70.f;
        const int maxCharsPerLine = 80; // Truncate very long lines
        for (size_t i = startIdx; i < logLines.size() && i < startIdx + maxLines; i++) {
            std::string displayText = logLines[i];
            // Truncate if too long
            if (displayText.length() > maxCharsPerLine) {
                displayText = displayText.substr(0, maxCharsPerLine) + "...";
            }
            sf::Text logLine(font, displayText, 12);
            logLine.setFillColor(sf::Color(220, 220, 240));
            logLine.setPosition({350.f, logY});
            window.draw(logLine);
            logY += 15.f;
        }
        
        // Draw scroll indicator
        if (logLines.size() > maxLines) {
            sf::Text scrollHint(font, "Scroll: UP/DOWN", 10);
            scrollHint.setFillColor(sf::Color(150, 150, 170));
            scrollHint.setPosition({350.f, 680.f});
            window.draw(scrollHint);
        }
        
        window.display();
    }

    return 0;
}
