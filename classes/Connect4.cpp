#include "Connect4.h"

#define CONNECT4_COLS 7
#define CONNECT4_ROWS 6

Connect4::Connect4(){
    _grid = new Grid(7,6);
}

Connect4::~Connect4(){
    delete _grid;
}

//Kyle was here heheheehe
//Kyle is still here hehehehe


void Connect4::setUpBoard(){
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");
    //_gameHasAnimalInstinct = true;  

    // if (gameHasAI()) {
    //     setAIPlayer(AI_PLAYER);
    // }

    startGame();
} 


std::string Connect4::initialStateString(){ 
    return "000000000000000000000000000000000000000000";
}

// Not critical, this is only used for loading from a file
void Connect4::setStateString(const std::string &s){ //TODO
    
}


std::string Connect4::stateString(){
    std::string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}


Player* Connect4::checkForWinner() { //TODO 
    return 0; 
}




bool Connect4::checkForDraw() {
    return false;
}


bool Connect4::actionForEmptyHolder(BitHolder &holder) //TODO 
{
    auto clicked = dynamic_cast<ChessSquare*>(&holder);
   if (!clicked) return false;
    //take the column (row Y) something is clicked on
   int col = clicked->getColumn();
    //put a bit on the top and make it move to the bottom 
    for(int row = CONNECT4_ROWS -1; row >= 0; --row){
        auto* square = _grid->getSquare(col,row);
        if (square && !square->bit()){
            Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
            
            square->setBit(bit); /// dedtination 
            bit->setPosition(clicked->getPosition());
            //Coming from top left corner for some reason?
            bit->moveTo(square->getPosition());
            endTurn();
            return true;
        }
    }

    return false;
    
}


bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in connect 4
    return false;
}
bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in connect 4
    return false;
}



void Connect4::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper function for the winner check
//
Player* Connect4::ownerAt(int index ) const
{
    auto square = _grid->getSquare(index % 6, index / 7);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}




Bit* Connect4::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}


// AI Section Downward

void Connect4::updateAI() {

}


static int evaluateAIBoard(const std::string& state){
    const int values[] = {
        1,1,2,3,2,1,1,
        1,1,2,3,2,1,1,
        1,1,2,5,2,1,1,
        1,1,2,5,2,1,1,
        2,1,2,3,2,1,1,
        2,1,2,3,2,1,2
    };

    int value = 0;
    for(int index = 0; index < (CONNECT4_COLS * CONNECT4_ROWS); index++){
        char piece = state[index];
        if(piece != '0'){
            value += piece == '1' ? values[index] : -values[index];
        }
    }
    return value;
}

static bool isAIBoardFull(const std::string& state) {
    return state.find('0') == std::string::npos;
}

int Connect4::negamax(std::string& state, int depth, int playerColor) 
{
    int score = evaluateAIBoard(state);

    // Check if AI wins, human wins, or draw
    if(depth == 5) { 
        // A winning state is a loss for the player whose turn it is.
        // The previous player made the winning move.
        return score * playerColor; 
    }

    if(isAIBoardFull(state)) {
        return 0; // Draw
    }

    int bestVal = -1000; // Min value
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            // Check if cell is empty
            if (state[y * 3 + x] == '0') {
                // Make the move
                state[y * 3 + x] = playerColor == HUMAN_PLAYER ? '1' : '2'; // Set the cell to the current player's color
                bestVal = std::max(bestVal, -negamax(state, depth + 1, -playerColor));
                // Undo the move for backtracking
                state[y * 3 + x] = '0';
            }
        }
    }

    return bestVal;
}