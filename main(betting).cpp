#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

class Card {
public:
    enum RANK {
        ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN,
        EIGHT, NINE, TEN, JACK, QUEEN, KING
    };

    enum SUIT {
        CLUBS, DIAMONDS, HEARTS, SPADES
    };

    Card(RANK rank = ACE, SUIT suit = SPADES, bool isFaceUp = true)
        : rank(rank), suit(suit), isFaceUp(isFaceUp) {
    }

    int getValue() const {

        int value = rank;

        if (value > 10) {
            value = 10;
        }

        if (!isFaceUp) {
            value = 0;
        }

        return value;
    }

    void flip() {
        isFaceUp = !isFaceUp;
    }

    // PERSONAL ADDITION:
    // This visual card display is not part of the original assignment.
    // It was added to improve the user experience.
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

        static const std::string RANKS[] = {
            "0", "A", "2", "3", "4", "5", "6", "7",
            "8", "9", "10", "J", "Q", "K"
        };

        static const std::string SUITS[] = {
            "C", "D", "H", "S"
        };

        std::string rankString = RANKS[rank];
        std::string suitString = SUITS[suit];

        std::string leftRank = rankString;
        std::string rightRank = rankString;

        if (rankString.length() == 1) {
            leftRank += " ";
            rightRank = " " + rankString;
        }

        return {
            ".--------.",
            "|" + leftRank + "      |",
            "|   " + suitString + "    |",
            "|      " + rightRank + "|",
            "'--------'"
        };
    }

    friend std::ostream& operator<<(std::ostream& out, const Card& aCard) {
        static const std::string RANKS[] = {
            "0", "A", "2", "3", "4", "5", "6", "7",
            "8", "9", "10", "J", "Q", "K"
        };

        static const std::string SUITS[] = {
            "C", "D", "H", "S"
        };

        if (aCard.isFaceUp) {
            out << RANKS[aCard.rank] << SUITS[aCard.suit];
        } else {
            out << "XX";
        }

        return out;
    }

protected:
    RANK rank;
    SUIT suit;
    bool isFaceUp;
};

class Hand {
public:
    Hand() {
        cardVector.reserve(7);
    }

    virtual ~Hand() {
        clearHand();
    }

    void add(Card* cardPtr) {
        cardVector.push_back(cardPtr);
    }

    void clearHand() {
        std::vector<Card*>::iterator iter;

        for (iter = cardVector.begin(); iter != cardVector.end(); ++iter) {
            delete *iter;
            *iter = nullptr;
        }

        cardVector.clear();
    }

    int getTotal() const {
        if (cardVector.empty()) {
            return 0;
        }

        int total = 0;

        std::vector<Card*>::const_iterator iter;
        for (iter = cardVector.begin(); iter != cardVector.end(); ++iter) {
            total += (*iter)->getValue();
        }

        for (iter = cardVector.begin(); iter != cardVector.end(); ++iter) {
            if ((*iter)->getValue() == 1 && total <= 11) {
                total += 10;
                return total;
            }
        }

        return total;
    }

    const std::vector<Card*>& getCards() const {
        return cardVector;
    }

    // PERSONAL ADDITION:
    // Fancy card display is not part of the original assignment.
    // It was added to improve the user experience.
    void displayFancy() const {
        if (cardVector.empty()) {
            std::cout << "(empty hand)" << std::endl;
            return;
        }

        for (int line = 0; line < 5; ++line) {
            for (std::size_t i = 0; i < cardVector.size(); ++i) {
                std::vector<std::string> art = cardVector[i]->getCardArt();
                std::cout << art[line] << "  ";
            }
            std::cout << std::endl;
        }

        std::cout << "Total: " << getTotal() << std::endl;
    }

protected:
    std::vector<Card*> cardVector;
};

class GenericPlayer : public Hand {
public:
    GenericPlayer(const std::string& name = "")
        : name(name) {
    }

    virtual ~GenericPlayer() {
    }

    virtual bool isHitting() const = 0;

    bool isBusted() const {
        if (getTotal() > 21) {
            return true;
        } else {
            return false;
        }
    }

    void bust() const {
        std::cout << name << " busts." << std::endl;

        // PERSONAL ADDITION:
        // Decorative output below is not part of the original assignment.
        std::cout
            << "██████╗ ██╗   ██╗███████╗████████╗\n"
            << "██╔══██╗██║   ██║██╔════╝╚══██╔══╝\n"
            << "██████╔╝██║   ██║███████╗   ██║   \n"
            << "██╔══██╗██║   ██║╚════██║   ██║   \n"
            << "██████╔╝╚██████╔╝███████║   ██║   \n"
            << "╚═════╝  ╚═════╝ ╚══════╝   ╚═╝   \n";
    }

    std::string getName() const {
        return name;
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    friend std::ostream& operator<<(std::ostream& out, const GenericPlayer& aGenericPlayer) {
        out << aGenericPlayer.name << "\t";

        if (!aGenericPlayer.cardVector.empty()) {
            std::vector<Card*>::const_iterator iter;
            for (iter = aGenericPlayer.cardVector.begin(); iter != aGenericPlayer.cardVector.end(); ++iter) {
                out << *(*iter) << "\t";
            }

            if (aGenericPlayer.getTotal() != 0) {
                out << "(" << aGenericPlayer.getTotal() << ")";
            }
        } else {
            out << "<empty>";
        }

        return out;
    }

protected:
    std::string name;
};

class Player : public GenericPlayer {
public:
    Player(const std::string& name = "")
        : GenericPlayer(name) {
    }

    virtual ~Player() {
    }

    virtual bool isHitting() const {
        char response;

        std::cout << name << ", do you want a hit? (Y/N): ";
        std::cin >> response;
        response = static_cast<char>(std::tolower(response));

        return (response == 'y');
    }

    void win() const {
        std::cout << name << " wins." << std::endl;
        std::cout <<
"        ✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨\n"
"$$$     ██╗   ██╗ ██████╗ ██╗   ██╗    ██╗    ██╗██╗███╗   ██╗     $$$\n"
"$$$     ╚██╗ ██╔╝██╔═══██╗██║   ██║    ██║    ██║██║████╗  ██║     $$$\n"
"***      ╚████╔╝ ██║   ██║██║   ██║    ██║ █╗ ██║██║██╔██╗ ██║     ***\n"
"$$$       ╚██╔╝  ██║   ██║██║   ██║    ██║███╗██║██║██║╚██╗██║     $$$\n"
"$$$        ██║   ╚██████╔╝╚██████╔╝    ╚███╔███╔╝██║██║ ╚████║     $$$\n"
"***        ╚═╝    ╚═════╝  ╚═════╝      ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝     ***\n"
"        ✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨\n";
    }

    void lose() const {
        std::cout << name << " loses." << std::endl;
        std::cout <<
"██╗   ██╗ ██████╗ ██╗   ██╗    ██╗      ██████╗ ███████╗███████╗\n"
"╚██╗ ██╔╝██╔═══██╗██║   ██║    ██║     ██╔═══██╗██╔════╝██╔════╝\n"
" ╚████╔╝ ██║   ██║██║   ██║    ██║     ██║   ██║███████╗█████╗  \n"
"  ╚██╔╝  ██║   ██║██║   ██║    ██║     ██║   ██║╚════██║██╔══╝  \n"
"   ██║   ╚██████╔╝╚██████╔╝    ███████╗╚██████╔╝███████║███████╗\n"
"   ╚═╝    ╚═════╝  ╚═════╝     ╚══════╝ ╚═════╝ ╚══════╝╚══════╝\n";
    }

    void push() const {
        std::cout << name << " pushes." << std::endl;
        std::cout << "*** PUSH! ***" << std::endl;
    }
};

class House : public GenericPlayer {
public:
    House(const std::string& name = "Dealer")
        : GenericPlayer(name) {
    }

    virtual ~House() {
    }

    virtual bool isHitting() const {
        if (getTotal() <= 16) {
            return true;
        } else {
            return false;
        }
    }

    void flipFirstCard() const {
        if (!cardVector.empty()) {
            cardVector[0]->flip();
        } else {
            std::cout << "No cards to flip." << std::endl;
        }
    }
};

class Deck : public Hand {
public:
    Deck() {
        cardVector.reserve(52);
        populate();
    }

    virtual ~Deck() {
    }

    void populate() {
        clearHand();

        for (int suit = Card::CLUBS; suit <= Card::SPADES; ++suit) {
            for (int rank = Card::ACE; rank <= Card::KING; ++rank) {
                add(new Card(static_cast<Card::RANK>(rank), static_cast<Card::SUIT>(suit)));
            }
        }
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(cardVector.begin(), cardVector.end(), generator);
    }

    void deal(Hand& aHand) {
        if (!cardVector.empty()) {
            aHand.add(cardVector.back());
            cardVector.pop_back();
        } else {
            std::cout << "Out of cards. Unable to deal." << std::endl;
        }
    }

    void additionalCards(GenericPlayer& aGenericPlayer) {
        while (!(aGenericPlayer.isBusted()) && aGenericPlayer.isHitting()) {
            deal(aGenericPlayer);
            std::cout << aGenericPlayer << std::endl;

            // PERSONAL ADDITION:
            aGenericPlayer.displayFancy();

            if (aGenericPlayer.isBusted()) {
                aGenericPlayer.bust();
            }
        }
    }
};

class Game {
public:
    Game(const std::string& name)
        : player(name), playerMoney(1000), currentBet(0) {
        player.setName(name);
        deck.populate();
        deck.shuffle();
    }

    ~Game() {
    }

    void play() {
        if (playerMoney <= 0) {
            std::cout << "You have no money left to bet." << std::endl;
            return;
        }

        std::cout << "\n==================================" << std::endl;
        std::cout << "          NEW ROUND               " << std::endl;
        std::cout << "==================================" << std::endl;

        // PERSONAL ADDITION:
        // The currency and betting system below is NOT part of the original assignment. It was added as our own extra feature.
        std::cout << "Current balance: $" << playerMoney << std::endl;

        currentBet = 0;
        do {
            std::cout << "Enter your bet: $";
            std::cin >> currentBet;

            if (std::cin.fail() || currentBet <= 0 || currentBet > playerMoney) {
                std::cout << "Invalid bet. Enter a value between 1 and " << playerMoney << "." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                currentBet = 0;
            }
        } while (currentBet == 0);

        player.clearHand();
        house.clearHand();

        if (deck.getCards().size() < 15) {
            deck.populate();
            deck.shuffle();
        }

        for (int i = 0; i < 2; ++i) {
            deck.deal(player);
            deck.deal(house);
        }

        house.flipFirstCard();

        std::cout << "\nRequired formatted output:" << std::endl;
        std::cout << player << std::endl;
        std::cout << house << std::endl;

        std::cout << "\nPlayer hand:" << std::endl;
        player.displayFancy();

        std::cout << "\nDealer hand:" << std::endl;
        house.displayFancy();

        deck.additionalCards(player);

        house.flipFirstCard();

        std::cout << "\nDealer reveals first card:" << std::endl;
        std::cout << house << std::endl;
        house.displayFancy();

        deck.additionalCards(house);

        if (house.isBusted()) {
            if (!player.isBusted()) {
                player.win();
                playerMoney += currentBet;
                std::cout << "You won $" << currentBet << "." << std::endl;
            }
        } else {
            if (!player.isBusted()) {
                if (player.getTotal() > house.getTotal()) {
                    player.win();
                    playerMoney += currentBet;
                    std::cout << "You won $" << currentBet << "." << std::endl;
                } else if (player.getTotal() < house.getTotal()) {
                    player.lose();
                    playerMoney -= currentBet;
                    std::cout << "You lost $" << currentBet << "." << std::endl;
                } else {
                    player.push();
                    std::cout << "Your bet was returned." << std::endl;
                }
            } else {
                playerMoney -= currentBet;
                std::cout << "You lost $" << currentBet << "." << std::endl;
            }
        }

        std::cout << "Updated balance: $" << playerMoney << std::endl;
    }

private:
    Deck deck;
    House house;
    Player player;

    // PERSONAL ADDITION:
    // The currency and betting system below is NOT part of the original assignment. // It was added as our own extra feature.
    int playerMoney;
    int currentBet;
};

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "       WELCOME TO BLACKJACK       " << std::endl;
    std::cout << "==================================" << std::endl;

    char again = 'y';
    std::string name;

    std::cout << "Enter player name: ";
    std::getline(std::cin, name);

    Game game(name);

    while (std::tolower(again) == 'y') {
        game.play();

        std::cout << "\nDo you want to play again? (Y/N): ";
        std::cin >> again;
        again = static_cast<char>(std::tolower(again));
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "Thanks for playing Blackjack!" << std::endl;
    return 0;
}
