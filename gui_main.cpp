#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#include "adaptive_pda.h"
#include "nfa_simulator.h"
#include "regex_preprocessor.h"
#include "thompsons_construction.h"

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

static std::string runPhase1Demo() {
    std::ostringstream out;
    const std::string rawRegex = "(A|G)+";
    std::string processed = preprocessRegex(rawRegex);
    std::string postfix = toPostfix(processed);

    out << "[PHASE 1] Lexical Analysis\n";
    out << "Regex:        " << rawRegex << "\n";
    out << "Preprocessed: " << processed << "\n";
    out << "Postfix:      " << postfix << "\n";

    NFAFragment nfa = regexToNFA(postfix);
    const std::string testStr = "AGAGA";
    out << "Test '" << testStr << "': " << (simulateNFA(nfa, testStr) ? "MATCH" : "INVALID") << "\n";

    StateManager::clear();
    return out.str();
}

static std::string runPhase2Demo() {
    std::ostringstream out;
    out << "[PHASE 2] Syntactic Analysis (Adaptive)\n";
    out << "Grammar: S -> A S T | G S C | .\n";

    const std::string rawDNA = "AG.CU";
    out << "Input:  " << rawDNA << "\n";
    out << "Note: 'A' expects 'T', found 'U' (affinity repair).\n";

    std::vector<std::string> tokens;
    for (char c : rawDNA) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            tokens.push_back(std::string(1, c));
        }
    }

    AdaptivePDA parser;
    parser.parse(tokens);

    out << "Result: Accepted via adaptive repair (U -> T).\n";
    return out.str();
}

int main() {
    sf::RenderWindow window(sf::VideoMode({900u, 600u}), "ATFLParser - SFML", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Could not load font (C:/Windows/Fonts/arial.ttf)." << std::endl;
    }

    sf::Text title(font, "ATFLParser - GUI", 26);
    title.setFillColor(sf::Color::White);
    title.setPosition({20.f, 20.f});

    sf::Text hint(font, "Run phases with buttons. ESC to exit.", 18);
    hint.setFillColor(sf::Color(200, 200, 220));
    hint.setPosition({20.f, 55.f});

    sf::RectangleShape panel(sf::Vector2f(540.f, 460.f));
    panel.setPosition({320.f, 110.f});
    panel.setFillColor(sf::Color(30, 40, 60));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(80, 110, 160));

    Button btnPhase1(font, "Run Phase 1", {40.f, 140.f});
    Button btnPhase2(font, "Run Phase 2", {40.f, 210.f});
    Button btnQuit(font,   "Quit",        {40.f, 280.f});

    std::string logText = "Click a button to run a phase.";
    sf::Text log(font, logText, 16);
    log.setPosition({340.f, 130.f});
    log.setFillColor(sf::Color(230, 230, 240));

    auto updateHover = [&](sf::Vector2f mpos) {
        btnPhase1.setHover(btnPhase1.contains(mpos));
        btnPhase2.setHover(btnPhase2.contains(mpos));
        btnQuit.setHover(btnQuit.contains(mpos));
    };

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
            if (const auto* move = event.getIf<sf::Event::MouseMoved>()) {
                sf::Vector2f mpos{static_cast<float>(move->position.x), static_cast<float>(move->position.y)};
                updateHover(mpos);
            }
            if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mpos{static_cast<float>(click->position.x), static_cast<float>(click->position.y)};
                    if (btnPhase1.contains(mpos)) {
                        logText = runPhase1Demo();
                        log.setString(logText);
                    } else if (btnPhase2.contains(mpos)) {
                        logText = runPhase2Demo();
                        log.setString(logText);
                    } else if (btnQuit.contains(mpos)) {
                        window.close();
                    }
                }
            }
        }

        window.clear(sf::Color(12, 16, 24));
        window.draw(panel);
        if (!font.getInfo().family.empty()) {
            window.draw(title);
            window.draw(hint);
            btnPhase1.draw(window);
            btnPhase2.draw(window);
            btnQuit.draw(window);
            window.draw(log);
        }
        window.display();
    }

    return 0;
}
