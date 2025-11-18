#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

class Interface {
public:
    inline Interface(const std::string& fontPath = "arial.ttf",
                     sf::Vector2u windowSize = {480, 240})
        : window_(
              sf::VideoMode(windowSize.x, windowSize.y),
              "Completador de Palavras",
              sf::Style::Titlebar | sf::Style::Close) {
        window_.setVerticalSyncEnabled(true);

        if (!loadFont(fontPath)) {
            throw std::runtime_error(
                "Nao foi possivel carregar uma fonte. Informe um caminho valido.");
        }

        label_.setFont(font_);
        label_.setString("Digite aqui:");
        label_.setCharacterSize(24);
        label_.setFillColor(sf::Color::White);
        label_.setPosition(24.f, 24.f);

        inputBox_.setSize(
            sf::Vector2f(static_cast<float>(windowSize.x) - 48.f, 150.f));
        inputBox_.setPosition(24.f,
                              label_.getPosition().y +
                                  label_.getCharacterSize() + 16.f);
        inputBox_.setFillColor(sf::Color(20, 20, 20));
        inputBox_.setOutlineColor(sf::Color(120, 120, 120));
        inputBox_.setOutlineThickness(2.f);

        inputText_.setFont(font_);
        inputText_.setCharacterSize(24);
        inputText_.setFillColor(sf::Color::White);
        inputText_.setPosition(inputBox_.getPosition().x + textPadding_,
                               inputBox_.getPosition().y + 10.f);
        lineHeight_ =
            static_cast<float>(inputText_.getCharacterSize()) + lineSpacing_;
        maxLines_ = std::max(
            1, static_cast<int>((inputBox_.getSize().y - verticalPadding_) /
                                lineHeight_));
        maxTextWidth_ =
            inputBox_.getSize().x - (textPadding_ * 2.f) - caretSpacing_;

        caret_.setSize(
            sf::Vector2f(2.f, static_cast<float>(inputText_.getCharacterSize())));
        caret_.setFillColor(sf::Color::White);
        caretVisible_ = true;
        reflowText();
    }

    inline void run() {
        while (window_.isOpen()) {
            processEvents();
            render();
        }
    }

    inline std::string value() const {
        const auto utf8 = buffer_.toUtf8();
        return std::string(utf8.begin(), utf8.end());
    }

    inline void setWordCorrector(
        std::function<std::string(const std::string&)> corretor) {
        corretor_ = std::move(corretor);
    }

private:
    sf::RenderWindow window_;
    sf::Font font_;
    sf::Text label_;
    sf::Text inputText_;
    sf::RectangleShape inputBox_;
    sf::RectangleShape caret_;
    sf::Clock caretClock_;
    bool caretVisible_{true};
    const sf::Time caretBlinkInterval_{sf::milliseconds(500)};
    const float textPadding_{12.f};
    const float caretSpacing_{4.f};
    const float lineSpacing_{6.f};
    const float verticalPadding_{20.f};
    float maxTextWidth_{0.f};
    float lineHeight_{0.f};
    int maxLines_{1};
    sf::String buffer_;
    std::vector<sf::String> wrappedLines_;
    std::function<std::string(const std::string&)> corretor_;
    inline bool loadFont(const std::string& preferredPath) {
        const std::vector<std::string> candidates = {
            preferredPath,
            "Arialbd.TTF",
            "/home/flobster23/.local/share/fonts/msttcorefonts/Arialbd.TTF"};

        for (const auto& candidate : candidates) {
            if (candidate.empty()) {
                continue;
            }
            if (font_.loadFromFile(candidate)) {
                return true;
            }
        }
        return false;
    }

    inline void processEvents() {
        sf::Event event{};
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
                return;
            }

            if (event.type == sf::Event::TextEntered) {
                handleTextEntered(event.text.unicode);
            }

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {
                window_.close();
            }
        }
    }

    inline void handleTextEntered(sf::Uint32 unicode) {
        if (unicode == 8) {
            if (!buffer_.isEmpty()) {
                buffer_.erase(buffer_.getSize() - 1, 1);
            }
        } else if (unicode == 13 || unicode == 10 || unicode == 9) {
            return;
        } else if (unicode >= 32 && unicode <= 0x10FFFF) {
            sf::String candidate = buffer_;
            candidate += static_cast<sf::Uint32>(unicode);

            if (!fitsBox(candidate)) {
                return;
            }

            buffer_ = std::move(candidate);
            if (unicode == ' ') {
                corrigirPalavra();
            }
        } else {
            return;
        }

        reflowText();
        resetCaretBlink();
    }

    inline void corrigirPalavra() {
        if (!corretor_ || buffer_.getSize() < 2) {
            return;
        }

        const auto utf8 = buffer_.toUtf8();
        std::string text(utf8.begin(), utf8.end());
        if (text.empty() || text.back() != ' ') {
            return;
        }

        const std::size_t wordEnd = text.size() - 1;
        if (wordEnd == 0) {
            return;
        }

        std::size_t wordStart = text.find_last_of(' ', wordEnd - 1);
        if (wordStart == std::string::npos) {
            wordStart = 0;
        } else {
            ++wordStart;
        }

        if (wordStart >= wordEnd) {
            return;
        }

        const std::string word = text.substr(wordStart, wordEnd - wordStart);
        if (word.empty()) {
            return;
        }

        const std::string corrected = corretor_(word);
        if (corrected.empty() || corrected == word) {
            return;
        }

        text.replace(wordStart, word.length(), corrected);
        buffer_ = sf::String::fromUtf8(text.begin(), text.end());
    }

    inline void render() {
        updateCaretBlink();

        window_.clear(sf::Color(30, 30, 30));
        window_.draw(label_);
        window_.draw(inputBox_);
        window_.draw(inputText_);
        if (caretVisible_) {
            window_.draw(caret_);
        }
        window_.display();
    }

    inline void updateCaretPosition() {
        const std::size_t lineIndex =
            wrappedLines_.empty() ? 0 : wrappedLines_.size() - 1;
        const float x = inputText_.getPosition().x +
                        measureLineWidth(lineIndex < wrappedLines_.size()
                                             ? wrappedLines_[lineIndex]
                                             : sf::String());
        const float y =
            inputText_.getPosition().y + static_cast<float>(lineIndex) * lineHeight_;
        caret_.setPosition(x + caretSpacing_, y);
    }

    inline void updateCaretBlink() {
        if (caretClock_.getElapsedTime() >= caretBlinkInterval_) {
            caretVisible_ = !caretVisible_;
            caretClock_.restart();
        }
    }

    inline void resetCaretBlink() {
        caretVisible_ = true;
        caretClock_.restart();
    }

    inline void reflowText() {
        wrappedLines_ = wrapLines(buffer_);
        sf::String display;
        for (std::size_t i = 0; i < wrappedLines_.size(); ++i) {
            display += wrappedLines_[i];
            if (i + 1 < wrappedLines_.size()) {
                display += '\n';
            }
        }

        inputText_.setString(display);
        updateCaretPosition();
    }

    inline std::vector<sf::String> wrapLines(const sf::String& text) const {
        std::vector<sf::String> lines{sf::String()};
        for (std::size_t i = 0; i < text.getSize(); ++i) {
            const sf::Uint32 codepoint = text[i];
            if (codepoint == '\n') {
                lines.emplace_back();
                continue;
            }

            sf::String candidate = lines.back();
            candidate += codepoint;
            if (measureLineWidth(candidate) > maxTextWidth_) {
                lines.emplace_back();
                lines.back() += codepoint;
            } else {
                lines.back() = std::move(candidate);
            }
        }

        return lines;
    }

    inline float measureLineWidth(const sf::String& line) const {
        sf::Text measuring;
        measuring.setFont(font_);
        measuring.setCharacterSize(inputText_.getCharacterSize());
        measuring.setString(line);
        const auto bounds = measuring.getLocalBounds();
        return bounds.width + bounds.left;
    }

    inline bool fitsBox(const sf::String& candidate) const {
        const auto lines = wrapLines(candidate);
        return lines.size() <= static_cast<std::size_t>(maxLines_);
    }
};
