#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>

using namespace std;

struct Card {
    string face;
    string suit;
    int value;
};

vector<Card> createDeck() {
    vector<Card> deck;
    string faces[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    string suits[] = {"♠", "♥", "♦", "♣"};
    int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 13; ++j) {
            Card card = {faces[j], suits[i], values[j]};
            deck.push_back(card);
        }
    }

    random_device rd;
    mt19937 g(rd());
    shuffle(deck.begin(), deck.end(), g);

    return deck;
}

void displayCard(const Card& card) {
    cout << "┌─────────┐" << endl;
    if (card.face == "10") {
        cout << "│" << card.face << "       │" << endl;
    } else {
        cout << "│" << card.face << "        │" << endl;
    }
    cout << "│         │" << endl;
    cout << "│    " << card.suit << "    │" << endl;
    cout << "│         │" << endl;
    if (card.face == "10") {
        cout << "│       " << card.face << "│" << endl;
    } else {
        cout << "│        " << card.face << "│" << endl;
    }
    cout << "└─────────┘" << endl;
}

void displayCards(const vector<Card>& cards) {
    for (size_t i = 0; i < cards.size(); ++i) {
        cout << "┌─────────┐  ";
    }
    cout << endl;

    for (const auto& card : cards) {
        if (card.face == "10") {
            cout << "│" << card.face << "       │  ";
        } else {
            cout << "│" << card.face << "        │  ";
        }
    }
    cout << endl;

    for (size_t i = 0; i < cards.size(); ++i) {
        cout << "│         │  ";
    }
    cout << endl;

    for (const auto& card : cards) {
        cout << "│    " << card.suit << "    │  ";
    }
    cout << endl;

    for (size_t i = 0; i < cards.size(); ++i) {
        cout << "│         │  ";
    }
    cout << endl;

    for (const auto& card : cards) {
        if (card.face == "10") {
            cout << "│       " << card.face << "│  ";
        } else {
            cout << "│        " << card.face << "│  ";
        }
    }
    cout << endl;

    for (size_t i = 0; i < cards.size(); ++i) {
        cout << "└─────────┘  ";
    }
    cout << endl;
}

int calculateTotal(const vector<Card>& cards) {
    int total = 0;
    int aces = 0;

    for (const auto& card : cards) {
        total += card.value;
        if (card.value == 11) aces++;
    }

    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }

    return total;
}

int main() {
    vector<Card> deck = createDeck();

    // Draw 5 random cards from the deck
    vector<Card> hand(deck.begin(), deck.begin() + 5);

    // Display the 5 cards
    displayCards(hand);

    // Optionally calculate the total points of the hand
    int totalPoints = calculateTotal(hand);
    cout << "Total Points: " << totalPoints << endl;

    return 0;
}
