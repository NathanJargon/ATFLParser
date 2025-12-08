#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

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

static std::string runPhase1(const std::string& regex, const std::string& testInputs) {
    if (regex.empty()) return "Error: Please enter a regex pattern.";

    try {
        std::string processed = preprocessRegex(regex);
        std::string postfix = toPostfix(processed);

        std::ostringstream oss;
        oss << "=== Phase 1: Regex -> NFA -> Simulation ===\n";
        oss << "Regex:        " << regex << "\n";
        oss << "Preprocessed: " << processed << "\n";
        oss << "Postfix:      " << postfix << "\n";
        oss << "---\n";

        NFAFragment nfa = regexToNFA(postfix);
        
        if (testInputs.empty()) {
            oss << "Enter characters in the test input field.\n";
        } else {
            oss << "Test Results:\n";
            for (char c : testInputs) {
                if (c == ' ') continue;
                std::string input(1, c);
                bool result = simulateNFA(nfa, input);
                oss << "  '" << input << "' -> " << (result ? "[MATCH]" : "[NO MATCH]") << "\n";
            }
        }

        StateManager::clear();
        return oss.str();
    } catch (const std::exception& e) {
        return std::string("Error: ") + e.what();
    }
}

static std::string runPhase2(const std::string& tokens_str) {
    if (tokens_str.empty()) return "Error: Please enter a token sequence.";

    std::vector<std::string> tokens;
    for (char c : tokens_str) {
        if (c != ' ') tokens.push_back(std::string(1, c));
    }

    std::ostringstream oss;
    oss << "=== Phase 2: Syntactic Analysis (Adaptive) ===\n";
    oss << "Grammar: S -> A S T | G S C | .\n";
    oss << "Tokens: ";
    for (const auto& t : tokens) oss << t;
    oss << "\n---\n";

    AdaptivePDA parser;
    std::string parseOutput = parser.parse(tokens);
    oss << parseOutput;

    return oss.str();
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1100u, 700u}), "ATFLParser - Interactive GUI", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Could not load font." << std::endl;
    }

    // UI Title
    sf::Text title(font, "", 28);
    title.setFillColor(sf::Color(100, 200, 255));
    title.setPosition({20.f, 15.f});

    // LEFT PANEL - CONTROLS
    sf::RectangleShape leftPanel(sf::Vector2f(320.f, 670.f));
    leftPanel.setPosition({10.f, 20.f});
    leftPanel.setFillColor(sf::Color(20, 25, 40));
    leftPanel.setOutlineThickness(2.f);
    leftPanel.setOutlineColor(sf::Color(70, 110, 170));

    // PHASE 1 SECTION
    sf::Text phase1Label(font, "PHASE 1: Regex -> NFA", 16);
    phase1Label.setFillColor(sf::Color(150, 200, 255));
    phase1Label.setPosition({25.f, 35.f});

    sf::Text regexLabel(font, "Regex Pattern:", 13);
    regexLabel.setFillColor(sf::Color(200, 200, 200));
    regexLabel.setPosition({25.f, 65.f});

    InputBox regexInput(font, "(A|G)+", {25.f, 85.f}, {280.f, 32.f});

    sf::Text testLabel(font, "Test Input (chars):", 13);
    testLabel.setFillColor(sf::Color(200, 200, 200));
    testLabel.setPosition({25.f, 130.f});

    InputBox testInput(font, "AGAG", {25.f, 150.f}, {280.f, 32.f});

    sf::Text phase1Ex(font, "e.g. (A|G)+, (A|B)*C, A*", 11);
    phase1Ex.setFillColor(sf::Color(120, 160, 200));
    phase1Ex.setPosition({25.f, 190.f});

    Button btnPhase1(font, "Run Phase 1", {25.f, 215.f});

    // PHASE 2 SECTION
    sf::Text phase2Label(font, "PHASE 2: Grammar Parse", 16);
    phase2Label.setFillColor(sf::Color(150, 200, 255));
    phase2Label.setPosition({25.f, 280.f});

    sf::Text tokensLabel(font, "Token Sequence:", 13);
    tokensLabel.setFillColor(sf::Color(200, 200, 200));
    tokensLabel.setPosition({25.f, 310.f});

    InputBox tokensInput(font, "AG.CU", {25.f, 330.f}, {280.f, 32.f});

    sf::Text phase2Ex(font, "e.g. AG.CT, AG.CU, GGG.TTT", 11);
    phase2Ex.setFillColor(sf::Color(120, 160, 200));
    phase2Ex.setPosition({25.f, 370.f});

    Button btnPhase2(font, "Run Phase 2", {25.f, 395.f});

    // CONTROL BUTTONS
    Button btnClear(font, "Clear All", {25.f, 480.f});
    Button btnQuit(font,  "Quit",      {25.f, 540.f});

    // RIGHT PANEL - OUTPUT LOG
    sf::RectangleShape rightPanel(sf::Vector2f(755.f, 670.f));
    rightPanel.setPosition({335.f, 20.f});
    rightPanel.setFillColor(sf::Color(15, 20, 35));
    rightPanel.setOutlineThickness(2.f);
    rightPanel.setOutlineColor(sf::Color(80, 120, 180));

    sf::Text outputLabel(font, "Output & Results", 16);
    outputLabel.setFillColor(sf::Color(150, 200, 255));
    outputLabel.setPosition({350.f, 30.f});

    std::deque<std::string> logLines;
    logLines.push_back("Ready. Enter patterns/tokens and click Run.");
    
    auto updateHover = [&](sf::Vector2f mpos) {
        btnPhase1.setHover(btnPhase1.contains(mpos));
        btnPhase2.setHover(btnPhase2.contains(mpos));
        btnClear.setHover(btnClear.contains(mpos));
        btnQuit.setHover(btnQuit.contains(mpos));
    };

    int focusedInput = -1; // 0=regex, 1=test, 2=tokens, -1=none

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

            if (const auto* text = event.getIf<sf::Event::TextEntered>()) {
                if (focusedInput == 0) regexInput.handleInput(text->unicode);
                else if (focusedInput == 1) testInput.handleInput(text->unicode);
                else if (focusedInput == 2) tokensInput.handleInput(text->unicode);
            }

            if (const auto* move = event.getIf<sf::Event::MouseMoved>()) {
                sf::Vector2f mpos{static_cast<float>(move->position.x), static_cast<float>(move->position.y)};
                updateHover(mpos);
            }

            if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mpos{static_cast<float>(click->position.x), static_cast<float>(click->position.y)};
                    
                    if (regexInput.box.getGlobalBounds().contains(mpos)) {
                        focusedInput = 0;
                        regexInput.focused = true;
                        testInput.focused = false;
                        tokensInput.focused = false;
                    } else if (testInput.box.getGlobalBounds().contains(mpos)) {
                        focusedInput = 1;
                        regexInput.focused = false;
                        testInput.focused = true;
                        tokensInput.focused = false;
                    } else if (tokensInput.box.getGlobalBounds().contains(mpos)) {
                        focusedInput = 2;
                        regexInput.focused = false;
                        testInput.focused = false;
                        tokensInput.focused = true;
                    } else {
                        regexInput.focused = testInput.focused = tokensInput.focused = false;
                        focusedInput = -1;
                    }

                    if (btnPhase1.contains(mpos)) {
                        std::string result = runPhase1(regexInput.text, testInput.text);
                        logLines.clear();
                        std::istringstream stream(result);
                        std::string line;
                        while (std::getline(stream, line)) {
                            logLines.push_back(line);
                        }
                    } else if (btnPhase2.contains(mpos)) {
                        std::string result = runPhase2(tokensInput.text);
                        logLines.clear();
                        std::istringstream stream(result);
                        std::string line;
                        while (std::getline(stream, line)) {
                            logLines.push_back(line);
                        }
                    } else if (btnClear.contains(mpos)) {
                        regexInput.clear();
                        testInput.clear();
                        tokensInput.clear();
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
        window.draw(title);
        window.draw(phase1Label);
        window.draw(regexLabel);
        window.draw(testLabel);
        window.draw(phase1Ex);
        window.draw(phase2Label);
        window.draw(tokensLabel);
        window.draw(phase2Ex);
        window.draw(outputLabel);
        
        // Draw input boxes
        regexInput.draw(window);
        testInput.draw(window);
        tokensInput.draw(window);
        
        // Draw buttons
        btnPhase1.draw(window);
        btnPhase2.draw(window);
        btnClear.draw(window);
        btnQuit.draw(window);
        
        // Draw output log (scrollable, up to 20 lines, with truncation for very long lines)
        size_t maxLines = 20;
        size_t startIdx = logLines.size() > maxLines ? logLines.size() - maxLines : 0;
        float logY = 70.f;
        const int maxCharsPerLine = 80; // Truncate very long lines
        for (size_t i = startIdx; i < logLines.size(); i++) {
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
        
        window.display();
    }

    return 0;
}
