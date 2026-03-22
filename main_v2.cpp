#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <thread>
#include <numeric>

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
            value = RANK - '0'; // works for cards 2–9
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

    std::string getRankString() const {
        if (RANK == 'T') {
            return "10";
        }
        return std::string(1, RANK);
    }

    std::string getSuitString() const {
        switch (SUIT) {
            case 'S': return "♠";
            case 'H': return "♥";
            case 'D': return "♦";
            case 'C': return "♣";
            default:  return "?";
        }
    }

    std::vector<std::string> getCardArt() const {
        if (!isFaceUp) {
            return {
                ".--------.",
                "|########|",
                "|########|",
                "|########|",
                "'--------'"
            };
        }

        std::string rank = getRankString();
        std::string suit = getSuitString();

        std::string leftRank = rank;
        std::string rightRank = rank;

        if (rank.length() == 1) {
            leftRank += " ";
            rightRank = " " + rank;
        }

        return {
            ".--------.",
            "|" + leftRank + "      |",
            "|   " + suit + "    |",
            "|      " + rightRank + "|",
            "'--------'"
        };
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
            *iter = nullptr;
        }
        cardVector.clear();
    }

    int getTotal() {

        if (cardVector.empty()) {
            return 0;
        }

        int total = 0;
        bool hasAce = false;

        for (std::vector<Card*>::iterator iter = cardVector.begin(); iter != cardVector.end(); ++iter) {
            if (*iter != nullptr) {
                total += (*iter)->getValue();
                if ((*iter)->RANK == 'A' && (*iter)->isFaceUp) {
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
        if (cardVector.empty()) {
            std::cout << "(empty hand)" << std::endl;
            return;
        }

        for (int line = 0; line < 5; line++) {
            for (int i = 0; i < static_cast<int>(cardVector.size()); i++) {
                if (cardVector[i] != nullptr) {
                    std::vector<std::string> art = cardVector[i]->getCardArt();
                    std::cout << art[line] << "  ";
                }
            }
            std::cout << std::endl;
        }

        //If first card is face down, hide the total for visuals
        if (cardVector[0] != nullptr && !cardVector[0]->isFaceUp) {
            std::cout << "Total: ?" << std::endl;
        }
        else {
            std::cout << "Total: " << getTotal() << std::endl;
        }
    }

};


class GenericPlayer {

public:

    std::string playerName;
    Hand playerHand;
    int bettedAmount = 0;

    GenericPlayer(std::string name) : playerName(name) {}

    bool isBusted() {
        if (playerHand.getTotal() > 21) {
            return true;
        }
        return false;
    }

    bool hasBlackjack() {
        return playerHand.cardVector.size() == 2 && playerHand.getTotal() == 21;
    }

    void bust() {
        std::cout << playerName << " is Busted!" << std::endl;
        std::cout <<
        "██████╗ ██╗   ██╗███████╗████████╗███████╗██████╗ \n"
        "██╔══██╗██║   ██║██╔════╝╚══██╔══╝██╔════╝██╔══██╗\n"
        "██████╔╝██║   ██║███████╗   ██║   █████╗  ██║  ██║\n"
        "██╔══██╗██║   ██║╚════██║   ██║   ██╔══╝  ██║  ██║\n"
        "██████╔╝╚██████╔╝███████║   ██║   ███████╗██████╔╝\n"
        "╚═════╝  ╚═════╝ ╚══════╝   ╚═╝   ╚══════╝╚═════╝ \n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
        std::this_thread::sleep_for(std::chrono::milliseconds(333));
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cardVector.begin(), cardVector.end(), g);
    }

    void deal(GenericPlayer& activePlayer) {
        if (cardVector.empty()) {
            std::cout << "No more cards left to deal. Creating new deck and shuffling . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            populate();
            shuffle();
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
            std::cout << "No more cards left to deal. Creating new deck and shuffling . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            populate();
            shuffle();
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
    int startingCurrency;

    Game(int curr) {
        startingCurrency = curr;
        gameDeck.populate();
        gameDeck.shuffle();
    }

    void play() {

        bool isPlaying = true;
        while (isPlaying) {

            //Determine number of hands to play at a time
            players.clear();
            std::cout << "Enter number of players: ";

            while (true) {
                std::cin >> playerNumber;

                if (std::cin.fail()) {
                    std::cout << "Invalid input. Please enter a number: ";
                    std::cin.clear();              // reset error state
                    std::cin.ignore(1000, '\n');   // discard bad input
                    continue;
                }

                if (playerNumber < 1 || playerNumber > 25) {
                    std::cout << "Please enter a number of at least 1: ";
                    continue;
                }

                break; // valid input
            }

            players.reserve(playerNumber);
            for (int i = 1; i <= playerNumber; i++) {
                GenericPlayer newPlayer = GenericPlayer("Player_" + std::to_string(i));
                players.push_back(newPlayer);
            }

            std::cout << "Starting Game . . . " << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "Starting currency: " << startingCurrency << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            //Distribute currency accross hands
            for (int i = 0; i < playerNumber; i++) {
                players[i].bettedAmount = int(startingCurrency / playerNumber);
            }

            house.playerHand.clearHand();
            for (int i = 0; i < playerNumber; i++) {
                players[i].playerHand.clearHand();
            }

            if (gameDeck.cardVector.size() < 15) {
                gameDeck.populate();
                gameDeck.shuffle();
            }

            bool flippedFC = false;
            //Deal cards
            for (int i = 0; i < 2; i++) {

                gameDeck.deal(house);

                if (!flippedFC) {
                    house.flipFirstCard();
                    flippedFC = true;
                }

                std::cout << "House: " << std::endl;
                house.playerHand.displayHand();
                std::this_thread::sleep_for(std::chrono::milliseconds(250));

                for (int j = 0; j < playerNumber; j++) {
                    gameDeck.deal(players[j]);

                    std::cout << "Player_" << j + 1 << ": " << std::endl;
                    players[j].playerHand.displayHand();
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            }
            std::cout << "Cards dealt . . . " << std::endl;

            std::cout << "██████╗ ██╗      █████╗  ██████╗██╗  ██╗     ██╗ █████╗  ██████╗██╗  ██╗\n";
            std::cout << "██╔══██╗██║     ██╔══██╗██╔════╝██║ ██╔╝     ██║██╔══██╗██╔════╝██║ ██╔╝\n";
            std::cout << "██████╔╝██║     ███████║██║     █████╔╝      ██║███████║██║     █████╔╝ \n";
            std::cout << "██╔══██╗██║     ██╔══██║██║     ██╔═██╗ ██   ██║██╔══██║██║     ██╔═██╗ \n";
            std::cout << "██████╔╝███████╗██║  ██║╚██████╗██║  ██╗╚█████╔╝██║  ██║╚██████╗██║  ██╗\n";
            std::cout << "╚═════╝ ╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝ ╚════╝ ╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝\n";

            std::this_thread::sleep_for(std::chrono::seconds(1));

            //Iterate through each player until done hitting/busted
            for (int j = 0; j < playerNumber; j++) {

                if (players[j].hasBlackjack()) {
                    std::cout << "Player_" << j + 1 << " has BLACKJACK!" << std::endl;
                    players[j].playerHand.displayHand();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    continue;
                }

                bool isHitting = true;
                while (isHitting && !players[j].isBusted()) {
                    std::cout << "House: " << std::endl;
                    house.playerHand.displayHand();
                    std::cout << "Player_" << j + 1 << "'s turn, make a move!  -  (S/H) " << std::endl;
                    players[j].playerHand.displayHand();

                    char input;
                    std::cin >> input;
                    input = static_cast<char>(tolower(input));

                    switch (input) {
                        case 's': {
                            isHitting = false;
                        } break;
                        case 'h': {
                            gameDeck.additionalCards(players[j]);
                            players[j].playerHand.displayHand();
                        } break;
                        default: {
                            std::cout << "Invalid input. Use S or H." << std::endl;
                        } break;
                    }
                }
            }

            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            house.flipFirstCard();
            std::cout << "House: " << std::endl;
            house.playerHand.displayHand();
            std::this_thread::sleep_for(std::chrono::milliseconds(750));

            while (house.isHitting()) {
                gameDeck.additionalCards(house);
                std::cout << "House: " << std::endl;
                house.playerHand.displayHand();
                std::this_thread::sleep_for(std::chrono::milliseconds(750));
            }

            //Check win/lose/draw
            bool houseBusted = house.isBusted();

            for (int j = 0; j < playerNumber; j++) {

                //Player busted
                if (players[j].isBusted()) {
                    std::cout << "Player_" << j + 1 << " loses :( " << std::endl;
                    players[j].bettedAmount = 0;
                    std::cout << " - Result - $" << players[j].bettedAmount << std::endl;
                    std::cout << std::endl;
                }

                else {
                    //House busted, player not
                    if (houseBusted && !players[j].isBusted()) {
                        std::cout << "Player_" << j + 1 << " wins!" << std::endl;
                        players[j].bettedAmount *= 2;
                        std::cout << " - Result - $" << players[j].bettedAmount << std::endl;
                        std::cout << std::endl;
                    }

                    //None bust -> check vals
                    else if (house.playerHand.getTotal() > players[j].playerHand.getTotal()) {
                        std::cout << "Player_" << j + 1 << " loses :( " << std::endl;
                        players[j].bettedAmount = 0;
                        std::cout << " - Result - $" << players[j].bettedAmount << std::endl;
                        std::cout << std::endl;
                    }
                    else if (house.playerHand.getTotal() < players[j].playerHand.getTotal()) {
                        std::cout << "Player_" << j + 1 << " wins!" << std::endl;
                        players[j].bettedAmount *= 2;
                        std::cout << " - Result - $" << players[j].bettedAmount << std::endl;
                        std::cout << std::endl;
                    }
                    else if (house.playerHand.getTotal() == players[j].playerHand.getTotal()) {
                        std::cout << "Player_" << j + 1 << " pushes :/" << std::endl;
                        std::cout << " - Result - $" << players[j].bettedAmount << std::endl;
                        std::cout << std::endl;
                    }
                }
            }

            //Sum all wins and losses
            int totalBets = std::accumulate(players.begin(), players.end(), 0,
                [](int sum, const GenericPlayer& p) { return sum + p.bettedAmount; });
            std::cout << " - - - Resulting sum: $" << totalBets << " - - - " << std::endl;
            std::cout << std::endl;
            startingCurrency = totalBets;

            if (totalBets == 0) {
                std::cout <<
                "██╗   ██╗ ██████╗ ██╗   ██╗    ██╗      ██████╗ ███████╗███████╗   :( \n"
                "╚██╗ ██╔╝██╔═══██╗██║   ██║    ██║     ██╔═══██╗██╔════╝██╔════╝\n"
                " ╚████╔╝ ██║   ██║██║   ██║    ██║     ██║   ██║███████╗█████╗  \n"
                "  ╚██╔╝  ██║   ██║██║   ██║    ██║     ██║   ██║╚════██║██╔══╝  \n"
                "   ██║   ╚██████╔╝╚██████╔╝    ███████╗╚██████╔╝███████║███████╗\n"
                "   ╚═╝    ╚═════╝  ╚═════╝     ╚══════╝ ╚═════╝ ╚══════╝╚══════╝\n";
                return;
            }

            char playAgain;
            std::cout << "Play again? (Y/N): ";
            std::cin >> playAgain;

            if (tolower(playAgain) == 'n') {
                isPlaying = false;
            }
        }
    }
};


int main() {
    Game newGame = Game(1000);
    newGame.play();
}
