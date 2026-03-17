#include <iostream>
#include <vector>
#include <string>

class Card {

private:
    bool isFaceUp;

    public:
    std::string RANK; //A,1,2,3,4,5,6,7,8,9,10,J,Q,K
    char SUIT;

    Card(std::string rank = "A", char suit = 'S', bool isFU = true)
        : RANK(rank), SUIT(suit), isFaceUp(isFU) {
    }
// returns the blackjack value of the card
    int getValue() const {
        int value = 0;
        if (!isFaceUp) {
            return value;
        }
        //try to convert to int value first
        try {
            value = std::stoi(RANK); // works for cards 2–10
        }
        catch (const std::invalid_argument&) {
            if (RANK == "A") {
                value = 1;
            }
            else if (RANK == "J" || RANK == "Q" || RANK == "K") {
                value = 10;
            }
        }
        return value;
    }

    void flip() {
        isFaceUp = !isFaceUp;
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& card) {
        if (card.isFaceUp) {
            os << card.RANK << card.SUIT;
        }
        else {
            os << "XX";
        }
        return os;
    }

};


class Hand {

public:
    std::vector<Card*> cardVector;

    Hand() {
        cardVector.reserve(7);
    }

    ~Hand() {
        clearHand();
    }

    void addCard(const Card& card) {
        cardVector.push_back(new Card(card));
    }

    void clearHand() {
        std::vector<Card*>::iterator iter;

        for (iter = cardVector.begin(); iter != cardVector.end(); ++iter) {
            delete *iter;
            *iter = NULL;
        }
        cardVector.clear();
    }

    int getTotal() {

        int total = 0;
        bool hasAce = false;

        for (std::vector<Card*>::iterator iter = cardVector.begin(); iter != cardVector.end(); ++iter) {
            if (*iter != NULL) {
                total += (*iter)->getValue();
                if ((*iter)->RANK == "A") {
                    hasAce = true;
                }
            }
        }
        if (hasAce && total <= 11) {
            total += 10;
        }

        return total;
    }

};


class GenericPlayer {

public:

    std::string playerName;
    Hand playerHand;

    GenericPlayer(std::string name) : playerName(name) {}

    bool isBusted() {
        if (playerHand.getTotal() > 21) {
            return true;
        }
        return false;
    }

    void bust() {
        std::cout << playerName << " is Busted!"<< std::endl;
    }

};


class Player : public GenericPlayer {

    public:
    Player(std::string n) : GenericPlayer(n) {}

    void isHitting(Card card) {
        if (!isBusted()) {
            playerHand.addCard(card);
        }
        else {
            bust();
        }
    }

    void win() {
        std::cout << playerName << " has won!" << std::endl;
    }

    void lose() {
        std::cout << playerName << " has lost!" << std::endl;
    }

    void push() {
        std::cout << playerName << " has pushed!" << std::endl;
    }

};


int main(){
    Card aceSpades = Card("A", 'S');
    Card tenHearts = Card("10", 'H');
    Hand hand;
    hand.addCard(aceSpades);
    hand.addCard(tenHearts);
    std::cout << hand.getTotal() << std::endl;
}
