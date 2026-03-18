#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cctype>


class Card {


    public:
    bool isFaceUp;
    char RANK; //A,2,3,4,5,6,7,8,9,10,J,Q,K
    char SUIT;

    Card(char rank = 'A', char suit = 'S', bool isFU = true)
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

        if (cardVector.empty()) {
            return 0;
        }

        if (cardVector[0] != NULL && cardVector[0]->getValue() == 0) {
            return 0;
        }

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
        if (cardVector.empty()) {
            std::cout << "Deck is empty, cannot pop card." << std::endl;
            return Card();
        }

        Card topCard = *cardVector.back();
        delete cardVector.back();
        cardVector.pop_back();
        return topCard;
    }

    void displayHand() {
        for (int i = 0; i < cardVector.size(); i++) {
            if (cardVector[i]->isFaceUp) {
                std::cout << cardVector[i]->RANK << cardVector[i]->SUIT << " ";
            }
            else {std::cout << "XX ";}
        }
        std::cout << "- " << getTotal() << std::endl;
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

    bool isHitting() {
        if (playerHand.getTotal() <= 16) {
            return true;
        }
        return false;
    }

    void flipFirstCard() {
        if (playerHand.cardVector.size() == 0) {
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

        std::vector<char> ranks = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
        std::vector<char> suits = {'C','D','H','S'};

        for (int suitIter = 0; suitIter < 4; suitIter++) {
            for (int rankIter = 0; rankIter < 13; rankIter++) {
                Card newCard = Card(ranks[rankIter], suits[suitIter]);
                addCard(newCard);
            }
        }

        std::cout << "* *    Deck generated   * * " << std::endl;
    }



    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cardVector.begin(), cardVector.end(), g);
    }



    void deal(GenericPlayer& activePlayer) {
        if (cardVector.empty()) {
            std::cout << "No more cards left to deal." << std::endl;
            return;
        }

        std::cout << "Dealing to " << activePlayer.playerName << std::endl;
        Card topCard = popTop();
        activePlayer.playerHand.addCard(topCard); // modifies the original
    }

    void additionalCards(GenericPlayer& activePlayer) {

        //Check to see if cards can be dealt:
        if (activePlayer.isBusted()) {
            std::cout << activePlayer.playerName << " is busted, cannot add cards..." << std::endl;
            return;
        }

        if (cardVector.empty()) {
            std::cout << "No more cards left to deal." << std::endl;
            return;
        }

        Card topCard = popTop();
        activePlayer.playerHand.addCard(topCard);

        if (activePlayer.isBusted()) {
            activePlayer.bust();
        }
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

        while (playerNumber < 1) {
            std::cout << "Please enter at least 1 player: ";
            std::cin >> playerNumber;
        }

        for (int i = 1; i <= playerNumber; i++) {
            GenericPlayer newPlayer = GenericPlayer("Player_" + std::to_string(i));
            players.push_back(newPlayer);
        }

        gameDeck.populate();
        gameDeck.shuffle();
    }


    void play() {
        std::cout << "Starting Game . . . " << std::endl;

        house.playerHand.clearHand();
        for (int i = 0; i < playerNumber; i++) {
            players[i].playerHand.clearHand();
        }

        if (gameDeck.cardVector.size() < 15) {
            gameDeck.populate();
            gameDeck.shuffle();
        }

        //Deal cards
        for (int i = 0; i < 2; i++) {
            gameDeck.deal(house);

            for (int j = 0; j < playerNumber; j++) {
                gameDeck.deal(players[j]);
            }
        }
        std::cout << "Cards dealt . . . " << std::endl;

        house.flipFirstCard();

        std::cout << "House: ";
        house.playerHand.displayHand();

        for (int j = 0; j < playerNumber; j++) {
            std::cout << "Player_" << j+1 << ": ";
            players[j].playerHand.displayHand();
        }


        //Iterate through each player until done hitting/busted
        for (int j = 0; j < playerNumber; j++) {
            bool isHitting = true;
            while (isHitting && !players[j].isBusted()) {
                std::cout << "Player_" << j+1 << "'s turn, make a move!  -  (S/H)' ";
                players[j].playerHand.displayHand();

                char input;
                std::cin >> input;
                input = tolower(input);

                switch (input) {
                    case 's': {
                        isHitting = false;
                    }break;
                    case 'h': {
                        gameDeck.additionalCards(players[j]);
                        players[j].playerHand.displayHand();
                    }break;
                    default: {
                        std::cout << "Invalid input. Use S or H." << std::endl;
                    }break;
                }
            }
        }

        house.flipFirstCard();
        std::cout << "House: ";
        house.playerHand.displayHand();
        while (house.isHitting()) {
            gameDeck.additionalCards(house);
            std::cout << "House: ";
            house.playerHand.displayHand();
        }


        //Check win/lose/draw
        bool houseBusted = house.isBusted();

        for (int j = 0; j < playerNumber; j++) {

            //Player busted
            if (players[j].isBusted()) {
                std::cout << "Player_" << j+1 << " loses :( " << std::endl;
            }

            
            else {
                //House busted, player not
                if (houseBusted && !players[j].isBusted()) {
                    std::cout << "Player_" << j+1 << " wins!" << std::endl;
                }

                //None bust -> check vals
                else if (house.playerHand.getTotal() > players[j].playerHand.getTotal()) {
                    std::cout << "Player_" << j+1 << " loses :( " << std::endl;
                }
                else if (house.playerHand.getTotal() < players[j].playerHand.getTotal()) {
                    std::cout << "Player_" << j+1 << " wins!" << std::endl;
                }
                else if (house.playerHand.getTotal() == players[j].playerHand.getTotal()) {
                    std::cout << "Player_" << j+1 << " pushes :/" << std::endl;
                }
            }
        }
    }
};


int main(){
    Game newGame = Game();
    newGame.play();
}
