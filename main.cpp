#include <algorithm>
#include <iostream>
#include <vector>
#include <string>


class Card {

private:
    bool isFaceUp;

    public:
    char RANK; //A,1,2,3,4,5,6,7,8,9,10,J,Q,K
    char SUIT;

    Card(char rank = 'A', char suit = 'S', bool isFU = false)
        : RANK(rank), SUIT(suit), isFaceUp(isFU) {
    }
// returns the blackjack value of the card
    int getValue() const {
        int value = 0;
        if (!isFaceUp) {
            return value;
        }
        //try to convert to int value first
        if (isdigit(RANK)) {
            value = RANK - '0'; // works for cards 2–10
        }
        else {
            if (RANK == 'A') {
                value = 1;
            }
            else if (RANK == 'T' || RANK == 'J' || RANK == 'Q' || RANK == 'K') {
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
                if ((*iter)->RANK == 'A') {
                    hasAce = true;
                }
            }
        }
        if (hasAce && total <= 11) {
            total += 10;
        }

        return total;
    }

    Card popTop() {
        Card topCard = *cardVector.back();
        delete cardVector.back();
        cardVector.pop_back();
        return topCard;
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





class House : public GenericPlayer {
public:
    House(std::string n) : GenericPlayer(n) {}

    void isHitting(Card card) {
        while (playerHand.getTotal() <= 16) {
            playerHand.addCard(card);
        }
    }

    void flipFirstCard() {
        if (playerHand.cardVector.size() != 2) {
            std::cout << "      -       There are not enough cards to flip      -       " << std::endl;
            return;
        }
        playerHand.cardVector[0]->flip();
    }
};



class Deck : public Hand {


public:

    Deck() {
        cardVector.reserve(52);
        populate();
    }

    void populate() {
        std::cout << "Populating new deck..." << std::endl;
        clearHand();

        std::vector<char> ranks = {'A',1,2,3,4,5,6,7,8,9,'T','J','Q','K'};
        std::vector suits = {'C','D','H','S'};

        for (int suitIter = 0; suitIter < 4; suitIter++) {
            for (int rankIter = 0; rankIter < 13; rankIter++) {
                Card newCard = Card(ranks[rankIter], suits[suitIter]);
                addCard(newCard);
            }
        }

        std::cout << "* *    Deck generated   * * " << std::endl;
    }



    void shuffle() {
        random_shuffle(cardVector.begin(), cardVector.end());
    }



    void deal(GenericPlayer activePlayer) {
        Hand activeHand = activePlayer.playerHand;
        Card topCard = popTop();
        activeHand.addCard(topCard);
    }


    void additionalCards(GenericPlayer activePlayer) {

        //Check to see if cards can be dealt:
        if (activePlayer.isBusted()) {
            std::cout << activePlayer.playerName << " is busted, cannot add cards..." << std::endl;
            return;
        }

        Card topCard = popTop();
        activePlayer.playerHand.addCard(topCard);
    }

};


class Game {
public:

    int playerNumber = 1;
    std::vector<GenericPlayer> players;
    House house = House("House");
    //   ^ ^   Best line of code I've ever written ^ ^
    Deck gameDeck = Deck();

    Game(){
        std::cout << "Enter number of players: ";
        std::cin >> playerNumber;

        for (int i = 1; i <= playerNumber; i++) {
            GenericPlayer newPlayer = GenericPlayer("Player_" + std::to_string(i));
        }

        gameDeck.populate();
        gameDeck.shuffle();
    }


    void play() {
        std::cout << "Starting Game . . . " << std::endl;

        for (int i = 0; i < 2; i++) {
            gameDeck.deal(house);

            for (int j = 0; j < playerNumber; j++) {
                GenericPlayer activePlayer = players[0];

                gameDeck.deal(activePlayer);
            }
        }
        house.flipFirstCard();

    }
};


int main(){
    Card aceSpades = Card('A', 'S');
    Card tenHearts = Card('T', 'H');
    Hand hand;
    hand.addCard(aceSpades);
    hand.addCard(tenHearts);
    std::cout << hand.getTotal() << std::endl;
}