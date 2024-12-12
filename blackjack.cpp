#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory> // For std::shared_ptr
#include <random>
#include <algorithm>

enum GameState {
    StartScreen,
    GettingCards,
    GameStart
};

// Card class to represent individual cards
class Card {
public:
    std::string rank;
    std::string suit;
    int value;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    Card(const std::string& rank, const std::string& suit, int value, std::shared_ptr<sf::Texture> texture)
        : rank(rank), suit(suit), value(value), texture(texture) {
        if (texture) {
            sprite.setTexture(*texture);
            sprite.setScale(0.15f, 0.15f); // Adjust for table layout
        }
    }

    void setPosition(const sf::Vector2f& position) {
        sprite.setPosition(position);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(sprite);
    }
};

// Function to preload textures
std::unordered_map<std::string, std::shared_ptr< sf::Texture> > preloadTextures() {
    std::unordered_map<std::string, std::shared_ptr< sf::Texture> > textures;
    std::string faces[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
    std::string suits[] = { "S", "H", "D", "C" };

    for (const auto& suit : suits) {
        for (const auto& face : faces) {
            std::string texturePath = "images/" + face + suit + ".png";
            auto texture = std::make_shared<sf::Texture>();
            if (!texture->loadFromFile(texturePath)) {
                std::cerr << "Error loading texture: " << texturePath << std::endl;
            }
            else {
                textures[face + suit] = texture;
            }
        }
    }
    return textures;
}

// Function to create and shuffle the deck
std::vector<Card> createDeck(const std::unordered_map<std::string, std::shared_ptr< sf::Texture> >& textures) {
    std::vector<Card> deck;
    std::string faces[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
    std::string suits[] = { "S", "H", "D", "C" };
    int values[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11 };

    for (const auto& suit : suits) {
        for (int i = 0; i < 13; ++i) {
            std::string key = faces[i] + suit;
            if (textures.find(key) != textures.end()) {
                deck.emplace_back(faces[i], suit, values[i], textures.at(key));
            }
            else {
                std::cerr << "Texture not found for card: " << key << std::endl;
            }
        }
    }

    // Shuffle the deck
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);

    return deck;
}

// Function to calculate the total value of a hand
int calculateHandValue(const std::vector<Card>& hand) {
    int value = 0, aceCount = 0;

    for (const auto& card : hand) {
        value += card.value;
        if (card.rank == "A") aceCount++;
    }

    while (value > 21 && aceCount > 0) {
        value -= 10; // Convert Ace from 11 to 1
        aceCount--;
    }

    return value;
}

// Function to determine the winner
std::string determineWinner(const std::vector<Card>& dealerHand, const std::vector<Card>& playerHand) {
    int dealerValue = calculateHandValue(dealerHand);
    int playerValue = calculateHandValue(playerHand);

    if (playerValue > 21) return "Dealer Wins!";
    if (dealerValue > 21) return "Player Wins!";
    if (playerValue > dealerValue) return "Player Wins!";
    if (dealerValue > playerValue) return "Dealer Wins!";
    return "It's a Tie!";
}

// Adjust card positioning for dealer and player
void adjustCardPositions(std::vector<Card>& dealerCards, std::vector<Card>& playerCards, const sf::Vector2u& windowSize) {
    float cardSpacing = 100; // Space between cards

    // Dealer cards centered
    float dealerX = (windowSize.x - dealerCards.size() * cardSpacing) / 2.0f;
    float dealerY = windowSize.y * 0.15f;

    // Player cards centered
    float playerX = (windowSize.x - playerCards.size() * cardSpacing) / 2.0f;
    float playerY = windowSize.y * 0.55f;

    for (size_t i = 0; i < dealerCards.size(); ++i) {
        dealerCards[i].setPosition(sf::Vector2f(dealerX + i * cardSpacing, dealerY));
    }
    for (size_t i = 0; i < playerCards.size(); ++i) {
        playerCards[i].setPosition(sf::Vector2f(playerX + i * cardSpacing, playerY));
    }
}

// Function to draw the main Blackjack table
void drawTable(sf::RenderWindow& window, sf::Font& font, const sf::Vector2u& windowSize,
    const std::vector<Card>& dealerCards, const std::vector<Card>& playerCards) {
    sf::RectangleShape table(sf::Vector2f(windowSize.x, windowSize.y));
    table.setFillColor(sf::Color(0, 96, 100)); // Green table
    window.draw(table);

    sf::RectangleShape dealerZone(sf::Vector2f(windowSize.x * 0.9f, windowSize.y * 0.1f));
    dealerZone.setFillColor(sf::Color(0, 0, 0, 50)); // Semi-transparent black
    dealerZone.setPosition(windowSize.x * 0.05f, windowSize.y * 0.05f);
    window.draw(dealerZone);

    sf::RectangleShape playerZone(sf::Vector2f(windowSize.x * 0.9f, windowSize.y * 0.1f));
    playerZone.setFillColor(sf::Color(0, 0, 0, 50)); // Semi-transparent black
    playerZone.setPosition(windowSize.x * 0.05f, windowSize.y * 0.85f);
    window.draw(playerZone);

    // Dealer label
    sf::Text dealerText("DEALER", font, 40);
    dealerText.setFillColor(sf::Color::White);
    dealerText.setPosition(windowSize.x * 0.45f, windowSize.y * 0.055f);
    window.draw(dealerText);

    // Dealer card sum
    sf::Text dealerCardSum("Sum: " + std::to_string(calculateHandValue(dealerCards)), font, 50);
    dealerCardSum.setFillColor(sf::Color::White);
    dealerCardSum.setPosition(windowSize.x * 0.05f, windowSize.y * 0.16f); // Slightly below dealer zone
    window.draw(dealerCardSum);

    // Player label
    sf::Text playerText("PLAYER", font, 40);
    playerText.setFillColor(sf::Color::White);
    playerText.setPosition(windowSize.x * 0.45f, windowSize.y * 0.86f);
    window.draw(playerText);

    // Player card sum
    sf::Text playerCardSum("Sum: " + std::to_string(calculateHandValue(playerCards)), font, 50);
    playerCardSum.setFillColor(sf::Color::White);
    playerCardSum.setPosition(windowSize.x * 0.05f, windowSize.y * 0.77f); // Above player zone
    window.draw(playerCardSum);
}

// Function to draw buttons
void drawButtons(sf::RenderWindow& window, sf::Font& font, bool gameOver, const sf::Vector2u& windowSize) {
    sf::Vector2f buttonSize(windowSize.x * 0.1f, windowSize.y * 0.08f);
    float buttonY = windowSize.y * 0.7f; // Vertical position for buttons

    // Adjust horizontal offset
    float leftOffset = windowSize.x * 0.2f; // Distance from the left side of the screen
    float spacing = windowSize.x * 0.02f;   // Spacing between buttons

    if (!gameOver) {
        // "Hit" button
        sf::RectangleShape hitButton(buttonSize);
        hitButton.setFillColor(sf::Color(0, 255, 0));
        hitButton.setPosition(leftOffset, buttonY);
        window.draw(hitButton);

        sf::Text hitText("Hit", font, 40);
        hitText.setFillColor(sf::Color::Black);
        hitText.setPosition(hitButton.getPosition().x + (buttonSize.x - hitText.getLocalBounds().width) / 2,
            hitButton.getPosition().y + (buttonSize.y - hitText.getLocalBounds().height) / 2 - 10);
        window.draw(hitText);

        // "Stand" button
        sf::RectangleShape standButton(buttonSize);
        standButton.setFillColor(sf::Color(255, 0, 0));
        standButton.setPosition(leftOffset + buttonSize.x + spacing, buttonY);
        window.draw(standButton);

        sf::Text standText("Stand", font, 40);
        standText.setFillColor(sf::Color::Black);
        standText.setPosition(standButton.getPosition().x + (buttonSize.x - standText.getLocalBounds().width) / 2,
            standButton.getPosition().y + (buttonSize.y - standText.getLocalBounds().height) / 2 - 10);
        window.draw(standText);
    }
    else {
        // "Restart" button
        sf::RectangleShape restartButton(buttonSize);
        restartButton.setFillColor(sf::Color(255, 255, 0));
        restartButton.setPosition(leftOffset, buttonY);
        window.draw(restartButton);

        sf::Text restartText("Restart", font, 40);
        restartText.setFillColor(sf::Color::Black);
        restartText.setPosition(restartButton.getPosition().x + (buttonSize.x - restartText.getLocalBounds().width) / 2,
            restartButton.getPosition().y + (buttonSize.y - restartText.getLocalBounds().height) / 2 - 10);
        window.draw(restartText);
    }
}

// Function to reset the game state
void resetGame(std::vector<Card>& dealerCards, std::vector<Card>& playerCards, std::vector<Card>& deck,
    const std::unordered_map<std::string, std::shared_ptr< sf::Texture> >& textures, const sf::Vector2u& windowSize) {
    dealerCards.clear();
    playerCards.clear();

    deck = createDeck(textures);

    // Deal initial cards
    dealerCards.push_back(deck.back());
    deck.pop_back();
    dealerCards.push_back(deck.back());
    deck.pop_back();
    playerCards.push_back(deck.back());
    deck.pop_back();
    playerCards.push_back(deck.back());
    deck.pop_back();

    adjustCardPositions(dealerCards, playerCards, windowSize);
}

void resetGameState(GameState& currentGameState, sf::Sprite& initialPlayerCard1,
                    sf::Sprite& initialPlayerCard2, sf::Sprite& initialDealerCard1, sf::Sprite& initialDealerCard2,
                    bool& PlayerCard1Finished, bool& PlayerCard2Finished, bool& DealerCard1Finished, bool& DealerCard2Finished, sf::Vector2f& initialPosition){

                        initialPlayerCard1.setPosition(initialPosition);
                        initialPlayerCard2.setPosition(initialPosition);
                        initialDealerCard1.setPosition(initialPosition);
                        initialDealerCard2.setPosition(initialPosition);
                        

                        PlayerCard1Finished = false;
                        PlayerCard2Finished = false;
                        DealerCard1Finished = false;
                        DealerCard2Finished = false;

                        currentGameState = GettingCards;
}

void resetHitCard(sf::Sprite& hitCard, bool& HitCardFinished, const sf::Vector2f& initialPosition, bool& hit){
    hitCard.setPosition(initialPosition);

    HitCardFinished = false;
    hit = false;
}
void hitGetACard(sf::RenderWindow& window, sf::Font& font, const sf::Vector2u& windowSize,
    sf::Shader& blurShader, sf::RenderTexture& blurRenderTexture, const sf::Texture& gameScreenTexture, 
    const std::vector<Card>& dealerCards, const std::vector<Card>& playerCards, bool& gameOver, sf::Sprite& cardTop, sf::RectangleShape pauseButton, sf::Text pauseText,
    sf::Sprite& hitCard, bool& HitCardFinished, const sf::Vector2f& initialPosition, bool& hit, const sf::Vector2f& TargetHitCardPosition, float speed, float deltaSeconds) {
    // Draw the game texture to the blur render texture with the shader
    blurRenderTexture.clear();
    blurRenderTexture.draw(sf::Sprite(gameScreenTexture));
    blurRenderTexture.display();

    // Draw the blurred texture to the window
    sf::Sprite blurredSprite(blurRenderTexture.getTexture());
    sf::RenderStates states;
    states.shader = &blurShader;

    window.draw(blurredSprite, states);

    drawTable(window, font, windowSize, dealerCards, playerCards);

    for (const auto& card : dealerCards) card.draw(window);
    for (const auto& card : playerCards) card.draw(window);

    sf::Vector2f currentPosition = hitCard.getPosition();
    sf::Vector2f direction = TargetHitCardPosition - currentPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0){
        direction.x /= length;
        direction.y /= length;
    }
    if (length > speed * deltaSeconds){
        hitCard.move(direction * speed * deltaSeconds);
    } else {
        hitCard.setPosition(TargetHitCardPosition);
        resetHitCard(hitCard, HitCardFinished, initialPosition, hit);
    }

    window.draw(cardTop);
    window.draw(pauseButton);
    window.draw(pauseText);

    drawButtons(window, font, gameOver, windowSize);
}
// Function to draw the pause menu with blurred background
void drawPauseMenu(sf::RenderWindow& window, sf::Font& font, const sf::Vector2u& windowSize,
    sf::Shader& blurShader, sf::RenderTexture& blurRenderTexture, const sf::Texture& gameScreenTexture, 
    const std::vector<Card>& dealerCards, const std::vector<Card>& playerCards, bool& gameOver, sf::Sprite& cardTop) {
    // Draw the game texture to the blur render texture with the shader
    blurRenderTexture.clear();
    blurRenderTexture.draw(sf::Sprite(gameScreenTexture));
    blurRenderTexture.display();

    // Draw the blurred texture to the window
    sf::Sprite blurredSprite(blurRenderTexture.getTexture());
    sf::RenderStates states;
    states.shader = &blurShader;

    window.draw(blurredSprite, states);

    drawTable(window, font, windowSize, dealerCards, playerCards);

    for (const auto& card : dealerCards) card.draw(window);
    for (const auto& card : playerCards) card.draw(window);

    window.draw(cardTop);

    drawButtons(window, font, gameOver, windowSize);

    sf::RectangleShape overlay(sf::Vector2f(windowSize.x, windowSize.y));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    // Pause text
    sf::Text pauseText("Paused", font, 50);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setStyle(sf::Text::Bold);
    pauseText.setPosition((windowSize.x - pauseText.getLocalBounds().width) / 2,
        (windowSize.y - pauseText.getLocalBounds().height) / 2 - 100);
    window.draw(pauseText);

    // Resume button
    sf::RectangleShape resumeButton(sf::Vector2f(200, 50));
    resumeButton.setFillColor(sf::Color(100, 200, 100));
    resumeButton.setPosition((windowSize.x - 200) / 2, (windowSize.y - 50) / 2 - 30);
    window.draw(resumeButton);

    sf::Text resumeText("Resume", font, 30);
    resumeText.setFillColor(sf::Color::Black);
    resumeText.setPosition(resumeButton.getPosition().x + (200 - resumeText.getLocalBounds().width) / 2,
        resumeButton.getPosition().y + (50 - resumeText.getLocalBounds().height) / 2 - 5);
    window.draw(resumeText);

    // Quit button
    sf::RectangleShape quitButton(sf::Vector2f(200, 50));
    quitButton.setFillColor(sf::Color(200, 100, 100));
    quitButton.setPosition((windowSize.x - 200) / 2, (windowSize.y - 50) / 2 + 50);
    window.draw(quitButton);

    sf::Text quitText("Quit", font, 30);
    quitText.setFillColor(sf::Color::Black);
    quitText.setPosition(quitButton.getPosition().x + (200 - quitText.getLocalBounds().width) / 2,
        quitButton.getPosition().y + (50 - quitText.getLocalBounds().height) / 2 - 5);
    window.draw(quitText);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Blackjack Game");
    window.setFramerateLimit(60);

    GameState currentGameState = StartScreen;

    // Load background music
    sf::Music backgroundMusic;
    bool musicStarted = false;
    if (!backgroundMusic.openFromFile("audio/jazz-background-music.ogg")) {
        std::cerr << "Error loading background music!" << std::endl;
        // Optionally continue even if music fails to load
    }
    else {
        backgroundMusic.setLoop(true); // Loop the music
        backgroundMusic.setVolume(50); // Adjust volume (0 to 100)
    }

    // Load font
    sf::Font font;
    if (!font.loadFromFile("fonts/PlayfairDisplay-Bold.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    // Load poker back
    sf::Texture cardsback;
    if(!cardsback.loadFromFile("images/BackColor_Red.png")){
        std::cerr << "Error loading poker back image!" << std::endl;
    }

    // Load background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/backgroundImage.jpg")) {
        std::cerr << "Error loading background image!" << std::endl;
        return -1;
    }

    // Preload textures using shared_ptr
    auto textures = preloadTextures();
    auto deck = createDeck(textures);
    std::vector<Card> dealerCards, playerCards;
    sf::Sprite cardTop(cardsback), initialDealerCard1(cardsback),
    initialDealerCard2(cardsback), initialPlayerCard1(cardsback),
    initialPlayerCard2(cardsback), hitCard(cardsback);

    cardTop.setScale(0.15f, 0.15f), initialDealerCard1.setScale(0.15f, 0.15f),
    initialDealerCard2.setScale(0.15f, 0.15f), initialPlayerCard1.setScale(0.15f, 0.15f),
    initialPlayerCard2.setScale(0.15f, 0.15f), hitCard.setScale(0.15f, 0.15f);

    // Initialize game state
    bool playerTurn = true, gameOver = false, paused = false, hit = false;
    std::string resultMessage;
    bool hoverEffect = false;
    cardTop.setPosition(window.getSize().x / 2 + 500, window.getSize().y / 2 - 150);
    hitCard.setPosition(window.getSize().x / 2 + 500, window.getSize().y / 2 - 150);
    initialPlayerCard1.setPosition(window.getSize().x / 2 + 500, window.getSize().y / 2 - 150);
    initialPlayerCard2.setPosition(window.getSize().x / 2 + 500, window.getSize().y / 2 - 150);
    initialDealerCard1.setPosition(window.getSize().x / 2 + 500, window.getSize().y / 2 - 150);
    initialDealerCard2.setPosition(window.getSize().x / 2 + 500, window.getSize().y / 2 - 150);

    sf::Vector2f initialPosition(window.getSize().x / 2 + 500, window.getSize().y / 2 - 150);

    sf::Vector2f TargetInitialPlayerCard1Position(window.getSize().x / 2, 1080);
    sf::Vector2f TargetInitialPlayerCard2Position(window.getSize().x / 2, 1080);
    sf::Vector2f TargetInitialDealerCard1Position(window.getSize().x / 2, -500);
    sf::Vector2f TargetInitialDealerCard2Position(window.getSize().x / 2, -500);

    sf::Vector2f TargetHitCardPosition(window.getSize().x / 2, 1080);

    float speed = 2500.0f;
    sf::Clock clock;

    bool PlayerCard1Finished = false;
    bool PlayerCard2Finished = false;
    bool DealerCard1Finished = false;
    bool DealerCard2Finished = false;

    bool HitCardFinished = false;

    // draw Pause button
    sf::RectangleShape pauseButton(sf::Vector2f(100, 50));
    pauseButton.setFillColor(sf::Color(100, 100, 100, 200));
    pauseButton.setPosition(window.getSize().x - 120, 20);
    sf::Text pauseText("Pause", font, 30);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(pauseButton.getPosition().x + 10, pauseButton.getPosition().y + 5);

    // Load blur shader
    sf::Shader blurShader;
    if (!blurShader.loadFromFile("shaders/blur.frag", sf::Shader::Fragment)) {
        std::cerr << "Error loading blur shader!" << std::endl;
        return -1;
    }

    // Create a render texture for capturing the game screen
    sf::RenderTexture blurRenderTexture;
    blurRenderTexture.create(window.getSize().x, window.getSize().y);
    if (!blurRenderTexture.create(window.getSize().x, window.getSize().y)) {
        std::cerr << "Error creating render texture!" << std::endl;
        return -1;
    }

    // Create a texture to hold the current game screen
    sf::Texture gameScreenTexture;
    gameScreenTexture.create(window.getSize().x, window.getSize().y);

    bool buttonLocked = false;
    // Main loop to display the window and circle
    while (window.isOpen()) {
        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (paused) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    // Resume button
                    float buttonX = (window.getSize().x - 200) / 2;
                    float buttonY = (window.getSize().y - 50) / 2 - 30;
                    if (event.mouseButton.x > buttonX && event.mouseButton.x < buttonX + 200 &&
                        event.mouseButton.y > buttonY && event.mouseButton.y < buttonY + 50) {
                        paused = false;
                    }

                    // Quit button
                    float quitButtonX = (window.getSize().x - 200) / 2;
                    float quitButtonY = (window.getSize().y - 50) / 2 + 50;
                    if (event.mouseButton.x > quitButtonX && event.mouseButton.x < quitButtonX + 200 &&
                        event.mouseButton.y > quitButtonY && event.mouseButton.y < quitButtonY + 50) {
                        window.close();
                    }
                }
                continue; // Skip other logic when paused
            }

            if (currentGameState == StartScreen) {
                // Hover effect logic (optional)
                if (event.type == sf::Event::MouseMoved) {
                    float buttonWidth = 250, buttonHeight = 80;
                    float buttonX = (window.getSize().x - buttonWidth) / 2;
                    float buttonY = 800;

                    // Hover effect for Start button
                    hoverEffect = (event.mouseMove.x > buttonX &&
                                event.mouseMove.x < buttonX + buttonWidth &&
                                event.mouseMove.y > buttonY &&
                                event.mouseMove.y < buttonY + buttonHeight);
                }

                // Button click detection
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    float buttonWidth = 250, buttonHeight = 80;
                    float buttonX = (window.getSize().x - buttonWidth) / 2;
                    float buttonY = 800;

                    // Start button clicked
                    if (event.mouseButton.x > buttonX &&
                        event.mouseButton.x < buttonX + buttonWidth &&
                        event.mouseButton.y > buttonY &&
                        event.mouseButton.y < buttonY + buttonHeight) {
                        std::cout << "Start button clicked!" << std::endl;
                        currentGameState = GettingCards;
                        clock.restart();
                    }
                }
            }

            else {
                // Game logic
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    // Detect "Pause" button
                    float pauseButtonX = window.getSize().x - 120; // Top-right corner
                    float pauseButtonY = 20;
                    if (event.mouseButton.x > pauseButtonX && event.mouseButton.x < pauseButtonX + 100 &&
                        event.mouseButton.y > pauseButtonY && event.mouseButton.y < pauseButtonY + 50) {
                        paused = true;
                    }
                    if (!buttonLocked){

                        // Button sizes and positions
                        sf::Vector2f buttonSize(window.getSize().x * 0.1f, window.getSize().y * 0.08f);
                        float buttonYPos = window.getSize().y * 0.7f;
                        float leftOffset = window.getSize().x * 0.2f;
                        float spacing = window.getSize().x * 0.02f;

                        if (!gameOver) {
                            // "Hit" button detection
                            if (event.mouseButton.x > leftOffset &&
                                event.mouseButton.x < leftOffset + buttonSize.x &&
                                event.mouseButton.y > buttonYPos &&
                                event.mouseButton.y < buttonYPos + buttonSize.y) {
                                // "Hit" button logic
                                if (!deck.empty()) {
                                    buttonLocked = true;
                                    hit = true;
                                    playerCards.push_back(deck.back());
                                    deck.pop_back();
                                    adjustCardPositions(dealerCards, playerCards, window.getSize());
                                    clock.restart();
                                    buttonLocked = false;
                                    if (calculateHandValue(playerCards) > 21) {
                                        resultMessage = "Dealer Wins!";
                                        gameOver = true;
                                        buttonLocked = false;
                                    }
                                }
                            }

                            // "Stand" button detection
                            if (event.mouseButton.x > leftOffset + buttonSize.x + spacing &&
                                event.mouseButton.x < leftOffset + 2 * buttonSize.x + spacing &&
                                event.mouseButton.y > buttonYPos &&
                                event.mouseButton.y < buttonYPos + buttonSize.y) {
                                // "Stand" button logic
                                playerTurn = false;
                                while (calculateHandValue(dealerCards) < 17 && !deck.empty()) {
                                    dealerCards.push_back(deck.back());
                                    deck.pop_back();
                                    adjustCardPositions(dealerCards, playerCards, window.getSize());
                                }
                                resultMessage = determineWinner(dealerCards, playerCards);
                                gameOver = true;
                            }
                        }
                        else {
                            // "Restart" button detection
                            if (event.mouseButton.x > leftOffset &&
                                event.mouseButton.x < leftOffset + buttonSize.x &&
                                event.mouseButton.y > buttonYPos &&
                                event.mouseButton.y < buttonYPos + buttonSize.y) {
                                // "Restart" button logic
                                resetGameState(currentGameState, initialPlayerCard1, initialPlayerCard2, 
                                                initialDealerCard1, initialDealerCard2, PlayerCard1Finished, 
                                                PlayerCard2Finished, DealerCard1Finished, DealerCard2Finished, initialPosition);
                                buttonLocked = true;
                                playerTurn = true;
                                gameOver = false;
                                resultMessage.clear();
                                clock.restart();
                            }
                        }
                    }
                }
            }
        }

        // Capture the current game screen before drawing
        // Create a texture to hold the current frame
        gameScreenTexture.create(window.getSize().x, window.getSize().y);
        gameScreenTexture.update(window);

        window.clear();

        if (paused) {
            window.setView(window.getDefaultView());
            gameScreenTexture.update(window);
            // Draw the blurred background
            drawPauseMenu(window, font, window.getSize(), blurShader, blurRenderTexture, gameScreenTexture, dealerCards, playerCards, gameOver, cardTop);
        } else if (hit) {
            sf::Time deltaTime = clock.restart();
            float deltaSeconds = deltaTime.asSeconds();
            if (!HitCardFinished){
                hitGetACard(window, font, window.getSize(), blurShader, blurRenderTexture, gameScreenTexture, dealerCards, playerCards, gameOver, cardTop, pauseButton, pauseText,
                hitCard, HitCardFinished, initialPosition, hit, TargetHitCardPosition, speed, deltaSeconds);
            } else {
                HitCardFinished = true;
                hit = false;
                buttonLocked = false;
                currentGameState = GameStart;
            }
            window.draw(hitCard);
            
        }
        else if (currentGameState == StartScreen) {
            // Draw Start Screen
            sf::Sprite backgroundSprite;
            backgroundSprite.setTexture(backgroundTexture);
            float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
            float scaleY = static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
            window.draw(backgroundSprite);

            // Draw Start button
            sf::RectangleShape startButton(sf::Vector2f(250, 80));
            sf::Color blackTransparent(0, 0, 0, 200); // Black with 200 alpha for transparency
            startButton.setFillColor(hoverEffect ? sf::Color(50, 50, 50, 255) : blackTransparent); // Slightly darker on hover
            startButton.setOutlineColor(sf::Color(255, 255, 255, 150)); // White outline with slight transparency
            startButton.setOutlineThickness(3);
            startButton.setPosition((window.getSize().x - 250) / 2, 800); // Adjusted position downwards
            window.draw(startButton);

            sf::Text startText("Start", font, 40);
            startText.setFillColor(sf::Color::White);
            startText.setStyle(sf::Text::Bold);
            startText.setPosition(startButton.getPosition().x + (250 - startText.getLocalBounds().width) / 2,
                startButton.getPosition().y + (80 - startText.getLocalBounds().height) / 2 - 10);
            window.draw(startText);

            if (!musicStarted && backgroundMusic.getStatus() == sf::SoundSource::Stopped){
                backgroundMusic.play();
                musicStarted = true;
            }
        } else if (currentGameState == GettingCards) {
            sf::Time deltaTime = clock.restart();
            float deltaSeconds = deltaTime.asSeconds();

            window.setView(window.getDefaultView());

            drawTable(window, font, window.getSize(), dealerCards, playerCards);
            buttonLocked = true;

            if (!PlayerCard1Finished){
                sf::Vector2f currentPosition = initialPlayerCard1.getPosition();
                sf::Vector2f direction = TargetInitialPlayerCard1Position - currentPosition;
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                if (length > 0){
                    direction.x /= length;
                    direction.y /= length;
                }

                if (length > speed * deltaSeconds){
                    initialPlayerCard1.move(direction * speed * deltaSeconds);
                }
                else {
                    initialPlayerCard1.setPosition(TargetInitialPlayerCard1Position);
                    PlayerCard1Finished = true;
                }
            } else if (!PlayerCard2Finished){
                sf::Vector2f currentPosition = initialPlayerCard2.getPosition();
                sf::Vector2f direction = TargetInitialPlayerCard2Position - currentPosition;
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                if (length > 0){
                    direction.x /= length;
                    direction.y /= length;
                }

                if (length > speed * deltaSeconds){
                    initialPlayerCard2.move(direction * speed * deltaSeconds);
                }
                else {
                    initialPlayerCard2.setPosition(TargetInitialPlayerCard2Position);
                    PlayerCard2Finished = true;
                }
            }
            else if (!DealerCard1Finished)
            {
                sf::Vector2f currentPosition = initialDealerCard1.getPosition();
                sf::Vector2f direction = TargetInitialDealerCard1Position - currentPosition;
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                if (length > 0)
                {
                    direction.x /= length;
                    direction.y /= length;
                }

                // Move the sprite towards the target position
                if (length > speed * deltaSeconds)
                {
                    initialDealerCard1.move(direction * speed * deltaSeconds);
                }

                else
                {
                    initialDealerCard1.setPosition(TargetInitialDealerCard1Position);
                    DealerCard1Finished = true;
                }
            } else if (!DealerCard2Finished)
            {
                sf::Vector2f currentPosition = initialDealerCard2.getPosition();
                sf::Vector2f direction = TargetInitialDealerCard2Position - currentPosition;
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                if (length > 0)
                {
                    direction.x /= length;
                    direction.y /= length;
                }

                // Move the sprite towards the target position
                if (length > speed * deltaSeconds)
                {
                    initialDealerCard2.move(direction * speed * deltaSeconds);
                }

                else
                {
                    initialDealerCard2.setPosition(TargetInitialDealerCard2Position);
                    DealerCard2Finished = true;
                }
            }
            else if (PlayerCard1Finished && PlayerCard2Finished && DealerCard1Finished && DealerCard2Finished){
                buttonLocked = false;
                resetGame(dealerCards, playerCards, deck, textures, window.getSize());
                currentGameState = GameStart;
            }

            window.draw(initialPlayerCard1);
            window.draw(initialPlayerCard2);
            window.draw(initialDealerCard1);
            window.draw(initialDealerCard2);
            window.draw(cardTop);
        }
        else if (currentGameState == GameStart) {
            window.setView(window.getDefaultView());
            // Draw Game Table and Elements
            drawTable(window, font, window.getSize(), dealerCards, playerCards);

            window.draw(cardTop);
            
            for (const auto& card : dealerCards) card.draw(window);
            for (const auto& card : playerCards) card.draw(window);

            drawButtons(window, font, gameOver, window.getSize());
            window.draw(pauseButton);
            window.draw(pauseText);

            if (gameOver && !resultMessage.empty()) {
                sf::Text resultText(resultMessage, font, 50);
                resultText.setFillColor(sf::Color::White);
                resultText.setStyle(sf::Text::Bold);
                resultText.setPosition((window.getSize().x - resultText.getLocalBounds().width) / 2,
                                    (window.getSize().y - resultText.getLocalBounds().height) / 2);
                window.draw(resultText);
            }
        }

        window.display();
    }

    return 0;
}
