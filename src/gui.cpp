// gui.cpp — Gautor Complete live autocomplete demo (SFML 2.5+)
//
// Controls:  type a-z  |  Backspace  |  TAB = switch structure  |  Esc = quit
//
// The TAB toggle is the demo's punchline: same prefix, watch the latency
// readout change by orders of magnitude.

#include "../include/loader.h"
#include "../include/trie.h"
#include "../include/hashmap.h"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

using Clock = std::chrono::steady_clock;

int main(int argc, char** argv) {
    const std::string path = (argc > 1) ? argv[1] : "data/unigram_freq.csv";

    LoadStats ls;
    std::cout << "loading " << path << " ...\n";
    auto data = loadWords(path, ls);
    std::cout << "accepted " << ls.accepted << " words\n";

    Trie trie;
    HashMap hash(10);
    for (const auto& e : data) {
        trie.insert(e.word, e.freq);
        hash.insert(e.word, e.freq);
    }
    std::cout << "structures built. launching window.\n";

    sf::RenderWindow window(sf::VideoMode(760, 520), "Gautor Complete");
    window.setFramerateLimit(60);

    sf::Font font;
    // Put any .ttf at assets/DejaVuSans.ttf, or rely on a system font.
    const char* fontPaths[] = {
        "assets/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
    bool fontLoaded = false;
    for (const char* fp : fontPaths)
        if (font.loadFromFile(fp)) { fontLoaded = true; break; }
    if (!fontLoaded) {
        std::cerr << "ERROR: no font found. Put a .ttf at assets/DejaVuSans.ttf\n";
        return 1;
    }

    std::string typed;
    bool useTrie = true;
    std::vector<std::string> suggestions;
    double lastQueryMs = 0.0;

    auto refresh = [&]() {
        auto t0 = Clock::now();
        suggestions = useTrie ? trie.getTopK(typed, 5) : hash.getTopK(typed, 5);
        auto t1 = Clock::now();
        lastQueryMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    };
    refresh();

    auto text = [&](const std::string& s, float x, float y, unsigned sz, sf::Color c) {
        sf::Text t(s, font, sz);
        t.setPosition(x, y);
        t.setFillColor(c);
        return t;
    };

    const sf::Color BG(250, 250, 252), INK(31, 41, 51), MUTED(123, 135, 148), ACCENT(46, 90, 136);

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();

            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) window.close();
                if (ev.key.code == sf::Keyboard::Tab) { useTrie = !useTrie; refresh(); }
            }

            if (ev.type == sf::Event::TextEntered) {
                uint32_t u = ev.text.unicode;
                if (u == 8) {                                   // backspace
                    if (!typed.empty()) { typed.pop_back(); refresh(); }
                } else if (u >= 'A' && u <= 'Z') {              // fold to lowercase
                    typed += static_cast<char>(u - 'A' + 'a');
                    refresh();
                } else if (u >= 'a' && u <= 'z') {
                    typed += static_cast<char>(u);
                    refresh();
                }
                // everything else ignored: the structures only hold [a-z]
            }
        }

        window.clear(BG);
        window.draw(text("Gautor Complete", 40, 28, 26, ACCENT));

        // ---- search box ----
        sf::RectangleShape box(sf::Vector2f(680, 54));
        box.setPosition(40, 78);
        box.setFillColor(sf::Color::White);
        box.setOutlineColor(sf::Color(97, 110, 124));
        box.setOutlineThickness(2);
        window.draw(box);

        window.draw(text(typed.empty() ? "type a prefix..." : typed, 54, 90, 24,
                         typed.empty() ? MUTED : INK));

        // ---- suggestions ----
        float y = 156;
        if (suggestions.empty() && !typed.empty()) {
            window.draw(text("no matches", 54, y, 18, MUTED));
        }
        for (size_t i = 0; i < suggestions.size(); ++i) {
            sf::RectangleShape row(sf::Vector2f(680, 44));
            row.setPosition(40, y - 6);
            row.setFillColor(i % 2 ? sf::Color(255, 255, 255) : sf::Color(243, 246, 249));
            window.draw(row);

            window.draw(text(std::to_string(i + 1) + ".", 54, y, 20, MUTED));
            window.draw(text(suggestions[i], 90, y, 22, INK));
            y += 44;
        }

        // ---- status bar ----
        std::string status = std::string("structure: ") + (useTrie ? "TRIE" : "HASHMAP") +
                             "   (TAB to switch)";
        window.draw(text(status, 40, 448, 18, ACCENT));

        char buf[96];
        std::snprintf(buf, sizeof(buf), "last query: %.4f ms", lastQueryMs);
        window.draw(text(buf, 40, 474, 18, MUTED));

        window.display();
    }
    return 0;
}