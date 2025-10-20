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
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 7 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber-1));
        } else {
            square->setBit(nullptr);
        }
    });
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
    /// iterate through every single cell 
    for (int column = 0; column < CONNECT4_COLS; column++){
        for (int row = 0; row < CONNECT4_ROWS ; row++){
            
        Player* player = owner(column, row);
            if (!player) continue; // skip empty squares
            
            //check all directions for a run of 4
            auto hasRun = [&](int deltaX, int deltaY) -> bool {
                for (int step = 1; step < 4; ++step){
                    int checkX = column + deltaX * step;
                    int checkY = row + deltaY * step;

                    //check if out of bounds
                    if(checkX < 0 || checkX >= CONNECT4_COLS || checkY < 0 || checkY >= CONNECT4_ROWS) {
                        return false;
                    }
                    if (owner(checkX, checkY) != player) {return false ;}
                }
                return true;
            };


            if (hasRun(1,0) || hasRun(0,1) || hasRun(1,1) || hasRun(1,-1)) {
                return player;
            }
        }
    }

    return nullptr; 
}


Player* Connect4::owner(int i, int j){
    ChessSquare* currentSquare = _grid->getSquare(i,j); 
    if ( !currentSquare ) return nullptr;
    Bit* bit = currentSquare->bit();
    return bit ? bit->getOwner() : nullptr;
}



bool Connect4::checkForDraw() {
    
    //stole directly from tic tac toe
     bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
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
            
            square->setBit(bit); /// destination 
            bit->setPosition(clicked->getPosition());
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
Player* Connect4::ownerAt(int index ) const // pass in the index of the grid, not the space? 
{
    auto square = _grid->getSquare(index % 7, index / 7);
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
    int bestVal = -10000;
    int bestMove = -1;
    std::string state = stateString();

    for( int column = 0; column < CONNECT4_COLS; column++){
        int lowestRow = -1;
        for (int row = CONNECT4_ROWS - 1 ; row >= 0; row--){
            int index = row * CONNECT4_COLS + column;
            if (state[index] == '0'){
                lowestRow = row;
                break;
            }
        }

        if (lowestRow == -1) continue;

        int index = lowestRow * CONNECT4_COLS + column;
        state[index] = '2';
        int moveTaken = -negamax(state, 0, HUMAN_PLAYER);

        state[index] = '0'; //undo move

        if (moveTaken > bestVal){
            bestVal = moveTaken;
            bestMove = column; 
        }
    }
    
    // Actually make the best move
    if (bestMove != -1) {
        auto* topSquare = _grid->getSquare(bestMove, 0);
        if (topSquare) {
            actionForEmptyHolder(*topSquare);
        }      
    }



}
int evaluateteAIBoard(const std::string& stat e){
    const int values[] = {
        1,1,2,3,2,1,1,
        1,1,2,3,2,1,1,
        1,1,2,5,2,1,1,
        1,1,2,5,2,1,1,
        2,1,2,3,2,1,1,
        2,1,2,3,2,1,2
    };

    int value = 0;
    for(int index 0; index < (CONNECT4_COLS * CONNECT4_ROWS); index++) {
        char piece = state[index];
        if (piece != '0') {
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
    if(depth == 2) { 
        // A winning state is a loss for the player whose turn it is.
        // The previous player made the winning move.
         return evaluateAIBoard(state) * playerColor;
    }

    if(isAIBoardFull(state)) {
        return 0; // Draw
    }

    int bestVal = -10000; // Min value
    for (int col = 0; col < CONNECT4_COLS; col++) {

        //lowest empty row 
        int lowestRow = -1;
        for (int row = CONNECT4_ROWS - 1 ; row >= 0; row-- ){
            int index = row * CONNECT4_COLS + col; 
            if (state[index] == '0'){
                lowestRow = row;
                break;
            }
        }
        if (lowestRow == -1) continue; // Column is full

        int index = lowestRow * CONNECT4_COLS + col;
        state[index] = playerColor == HUMAN_PLAYER ? '1' : '2';
        
        int moveTaken = -negamax(state, depth + 1, -playerColor);
        
    }

    return bestVal;
}
            //   state[index] = '0';  // Undo the move!    return