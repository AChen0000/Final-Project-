#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
using namespace std;

struct Card {
    string suit; // "♥", "♦", "♠", "♣"
    string rank; // "A","2",..."K"
    bool faceUp;
    
    string display() {
        if (!faceUp) return "🂠";
        if (suit == "♥" || suit == "♦") {
            return "\033[31m" + rank + suit + "\033[0m"; // Red color
        }
        return rank + suit; // Black color
    }
    
    bool isRed() {
        return suit == "♥" || suit == "♦";
    }
};

class FortyThieves {
private:
    vector<vector<Card>> tableau; // Columns C1-C10 
    vector<vector<Card>> foundations; 
    vector<Card> stock;
    vector<Card> waste;
    
    vector<Card> createDeck() {
        vector<Card> deck;
        string suits[] = {"♥", "♦", "♠", "♣"};
        string ranks[] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
        
        for(int d = 0; d < 2; d++) {
            for(string s : suits) {
                for(string r : ranks) {
                    deck.push_back({s, r, false});
                }
            }
        }
        return deck;
    }
    
    void shuffleDeck(vector<Card>& deck) {
        srand(time(0));
        for(size_t i = 0; i < deck.size(); i++) {
            int j = rand() % deck.size();
            swap(deck[i], deck[j]);
        }
    }
    
    void setupGame() {
        vector<Card> deck = createDeck();
        shuffleDeck(deck);
        
        tableau.resize(10);
        for(int col = 0; col < 10; col++) {
            for(int i = 0; i < 4; i++) {
                Card card = deck.back();
                card.faceUp = (i == 3); // Only bottom card is face up
                tableau[col].push_back(card);
                deck.pop_back();
            }
        }
        
        stock = deck;
        foundations.resize(8);
    }
    
    bool canPlaceOnTableau(Card moving, Card target) {
        return (moving.suit == target.suit) && 
               ((moving.rank == "K" && target.rank == "Q") ||
                (moving.rank == "Q" && target.rank == "J") ||
                (moving.rank == "J" && target.rank == "10") ||
                (moving.rank == "10" && target.rank == "9") ||
                (moving.rank == "9" && target.rank == "8") ||
                (moving.rank == "8" && target.rank == "7") ||
                (moving.rank == "7" && target.rank == "6") ||
                (moving.rank == "6" && target.rank == "5") ||
                (moving.rank == "5" && target.rank == "4") ||
                (moving.rank == "4" && target.rank == "3") ||
                (moving.rank == "3" && target.rank == "2") ||
                (moving.rank == "2" && target.rank == "A"));
    }

    void printHorizontalLine() {
        cout << " +";
        for(int i = 0; i < 10; i++) {
            cout << "----+";
        }
        cout << endl;
    }

public:
    FortyThieves() {
        setupGame();
    }
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    void showGame() {
        clearScreen();
        cout << "\nFOUNDATIONS:\n";
        for(int i = 0; i < 8; i++) {
            if(foundations[i].empty()) cout << "[  ] ";
            else cout << "[" << foundations[i].back().display() << "] ";
        }
        
        cout << "\n\nWASTE: ";
        if(waste.empty()) cout << "[  ]";
        else cout << "[" << waste.back().display() << "]";
        cout << "  STOCK: " << stock.size() << " cards\n\n";
        
        cout << "TABLEAU:\n";
        // Display column headers (C1-C10)
        cout << " ";
        for(int col = 1; col <= 10; col++) {
            cout << "  C" << col << " ";
        }
        cout << endl;
        
        printHorizontalLine();
        
        int maxHeight = 0;
        for(auto col : tableau) {
            if(col.size() > maxHeight) maxHeight = col.size();
        }
        
        for(int row = 0; row < maxHeight; row++) {
            cout << " |";
            for(int col = 0; col < 10; col++) {
                if(row < tableau[col].size()) {
                    cout << " " << tableau[col][row].display() << " |";
                } else {
                    cout << "    |";
                }
            }
            cout << endl;
            printHorizontalLine();
        }
    }
    
    bool moveTableauToTableau(int fromCol, int toCol) {
        fromCol--;
        toCol--;
        
        if(fromCol < 0 || fromCol > 9 || toCol < 0 || toCol > 9) return false;
        if(tableau[fromCol].empty()) return false;
        
        Card moving = tableau[fromCol].back();
        
        if(tableau[toCol].empty()) {
            tableau[toCol].push_back(moving);
            tableau[fromCol].pop_back();
            if (!tableau[fromCol].empty()) {
                tableau[fromCol].back().faceUp = true;
            }
            return true;
        }
        
        Card target = tableau[toCol].back();
        if(canPlaceOnTableau(moving, target)) {
            tableau[toCol].push_back(moving);
            tableau[fromCol].pop_back();
            if (!tableau[fromCol].empty()) {
                tableau[fromCol].back().faceUp = true;
            }
            return true;
        }
        
        return false;
    }
    
    bool moveTableauToFoundation(int col) {
        col--;
        
        if(col < 0 || col > 9) return false;
        if(tableau[col].empty()) return false;
        
        Card moving = tableau[col].back();
        
        for(auto& pile : foundations) {
            if(pile.empty()) {
                if(moving.rank == "A") {
                    pile.push_back(moving);
                    tableau[col].pop_back();
                    if (!tableau[col].empty()) {
                        tableau[col].back().faceUp = true;
                    }
                    return true;
                }
            } else {
                if(pile.back().suit == moving.suit) {
                    if((pile.back().rank == "A" && moving.rank == "2") ||
                       (pile.back().rank == "2" && moving.rank == "3") ||
                       (pile.back().rank == "3" && moving.rank == "4") ||
                       (pile.back().rank == "4" && moving.rank == "5") ||
                       (pile.back().rank == "5" && moving.rank == "6") ||
                       (pile.back().rank == "6" && moving.rank == "7") ||
                       (pile.back().rank == "7" && moving.rank == "8") ||
                       (pile.back().rank == "8" && moving.rank == "9") ||
                       (pile.back().rank == "9" && moving.rank == "10") ||
                       (pile.back().rank == "10" && moving.rank == "J") ||
                       (pile.back().rank == "J" && moving.rank == "Q") ||
                       (pile.back().rank == "Q" && moving.rank == "K")) {
                        pile.push_back(moving);
                        tableau[col].pop_back();
                        if (!tableau[col].empty()) {
                            tableau[col].back().faceUp = true;
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    void drawCard() {
        if(!stock.empty()) {
            Card card = stock.back();
            card.faceUp = true;
            waste.push_back(card);
            stock.pop_back();
        }
    }
    
    bool moveWasteToFoundation() {
        if(waste.empty()) return false;
        
        Card moving = waste.back();
        
        for(auto& pile : foundations) {
            if(pile.empty()) {
                if(moving.rank == "A") {
                    pile.push_back(moving);
                    waste.pop_back();
                    return true;
                }
            } else {
                if(pile.back().suit == moving.suit) {
                    if((pile.back().rank == "A" && moving.rank == "2") ||
                       (pile.back().rank == "2" && moving.rank == "3") ||
                       (pile.back().rank == "3" && moving.rank == "4") ||
                       (pile.back().rank == "4" && moving.rank == "5") ||
                       (pile.back().rank == "5" && moving.rank == "6") ||
                       (pile.back().rank == "6" && moving.rank == "7") ||
                       (pile.back().rank == "7" && moving.rank == "8") ||
                       (pile.back().rank == "8" && moving.rank == "9") ||
                       (pile.back().rank == "9" && moving.rank == "10") ||
                       (pile.back().rank == "10" && moving.rank == "J") ||
                       (pile.back().rank == "J" && moving.rank == "Q") ||
                       (pile.back().rank == "Q" && moving.rank == "K")) {
                        pile.push_back(moving);
                        waste.pop_back();
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    bool moveWasteToTableau(int col) {
        col--;
        
        if(col < 0 || col > 9) return false;
        if(waste.empty()) return false;
        
        Card moving = waste.back();
        
        if(tableau[col].empty()) {
            tableau[col].push_back(moving);
            waste.pop_back();
            return true;
        }
        
        Card target = tableau[col].back();
        if(canPlaceOnTableau(moving, target)) {
            tableau[col].push_back(moving);
            waste.pop_back();
            return true;
        }
        
        return false;
    }
    
    bool checkWin() {
        for(auto pile : foundations) {
            if(pile.empty() || pile.back().rank != "K") return false;
        }
        return true;
    }
    
    void play() {
        while(true) {
            showGame();
            
            if(checkWin()) {
                cout << "\nCongratulations! You won/finished the game!\n";
                break;
            }
            
            cout << "\nOptions:\n";
            cout << "1. Move within tableau (columns C1-C10)\n";
            cout << "2. Move to foundation (from column C1-C10)\n";
            cout << "3. Draw card\n";
            cout << "4. Move waste to foundation\n";
            cout << "5. Move waste to tableau (column C1-C10)\n";
            cout << "0. Quit/Go Back\n";
            cout << "Choose: ";
            
            int choice;
            cin >> choice;
            
            switch(choice) {
                case 1: {
                    int from, to;
                    cout << "From column (C1-C10,just type in column number): ";
                    cin >> from;
                    cout << "To column (C1-C10,just type in column number): ";
                    cin >> to;
                    if(!moveTableauToTableau(from, to))
                        cout << "Invalid move!\n";
                    break;
                }
                case 2: {
                    int col;
                    cout << "Column (C1-C10,just type in column number): ";
                    cin >> col;
                    if(!moveTableauToFoundation(col))
                        cout << "Invalid move!\n";
                    break;
                }
                case 3:
                    drawCard();
                    break;
                case 4:
                    if(!moveWasteToFoundation())
                        cout << "Invalid move!\n";
                    break;
                case 5: {
                    int col;
                    cout << "Column (C1-C10, just type in column number): ";
                    cin >> col;
                    if(!moveWasteToTableau(col))
                        cout << "Invalid move!\n";
                    break;
                }
                case 0:
                    return;
                default:
                    cout << "Invalid choice!\n";
            }
        }
    }
};

int main() {
    FortyThieves game;
    game.play();
    return 0;
}
