#include "Connect4.h"
#include <iostream>

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
            Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
            
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
    bool isAI = (getAIPlayer() == playerNumber);
    bit->LoadTextureFromFile(isAI ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}


// AI Section Downward

void Connect4::updateAI() {
    int bestVal = -10000;
    int bestMove = -1;
    std::string state = stateString();
    
    std::cout << "\n=== AI MOVE EVALUATION ===" << std::endl;
    std::cout << "Current board state: " << state << std::endl;

    for( int column = 0; column < CONNECT4_COLS; column++){
        int lowestRow = -1;
        for (int row = CONNECT4_ROWS - 1 ; row >= 0; row--){
            int index = row * CONNECT4_COLS + column;
            if (state[index] == '0'){
                lowestRow = row;
                break;
            }
        }

        if (lowestRow == -1) {
            //std::cout << "Column " << column << ": FULL (skipped)" << std::endl;
            continue;
        }

        int index = lowestRow * CONNECT4_COLS + column;
        //std::cout << "Column " << column << ": placing at row " << lowestRow << " (index " << index << ")" << std::endl;
        
        char aiChar = '0' + (getAIPlayer() + 1);
        state[index] = aiChar;
        int moveTaken = -negamax(state, 0, getHumanPlayer());
        //std::cout << "  -> negamax returned: " << moveTaken << std::endl;

        state[index] = '0'; //undo move

        if (moveTaken > bestVal){
            //std::cout << "  -> NEW BEST! (was " << bestVal << ", now " << moveTaken << ")" << std::endl;
            bestVal = moveTaken;
            bestMove = column; 
        } else {
            //std::cout << "  -> not better than current best " << bestVal << std::endl;
        }
    }
    
    //std::cout << "FINAL DECISION: Column " << bestMove << " with score " << bestVal << std::endl;
    //std::cout << "=========================" << std::endl;
    
    // Actually make the best move
    if (bestMove != -1) {
        auto* topSquare = _grid->getSquare(bestMove, 0);
        if (topSquare) {
            actionForEmptyHolder(*topSquare);
        }      
    }



}
int evaluateAIBoard(const std::string& state){
    const int values[] = {
        3,3,2,1,2,3,3,
        3,3,2,1,2,3,3,
        5,3,2,1,2,3,5,
        5,3,2,1,2,3,5,
        3,3,2,1,2,3,3,
        3,3,2,1,2,3,3
    };

    // const int values[] = {
    //     1,1,2,3,2,1,1,
    //     1,1,2,3,2,1,1,
    //     1,1,2,5,2,1,1,
    //     1,1,2,5,2,1,1,
    //     2,1,2,3,2,1,1,
    //     2,1,2,3,2,1,2
    // };

    int value = 0;
    for(int index = 0; index < (CONNECT4_COLS * CONNECT4_ROWS); index++) {
        char piece = state[index];
        if (piece != '0') {
            int pieceValue = piece == '1' ? values[index] : -values[index];
            value += pieceValue;
            // Uncomment next line for very detailed evaluation debugging:
            // std::cout << "    idx " << index << " (col " << (index%7) << ", row " << (index/7) << "): piece '" << piece << "' value " << pieceValue << std::endl;
        }
    }
    return value;
}

static bool isAIBoardFull(const std::string& state) {
    return state.find('0') == std::string::npos;
}

int Connect4::negamax(std::string& state, int depth, int playerColor) 
{
    std::string indent(depth * 2, ' ');
    //std::cout << indent << "negamax(depth=" << depth << ", player=" << playerColor << ")" << std::endl;
    
    // static evaluation
    int eval = evaluateAIBoard(state);
    //std::cout << indent << "  static eval: " << eval << std::endl;

    // depth limit: return evaluation from current player's perspective
    const int MAX_DEPTH = 2;
    if (depth >= MAX_DEPTH) {
        int result = eval * playerColor;
        //std::cout << indent << "  MAX DEPTH reached, returning: " << result << std::endl;
        return result;
    }

    if (isAIBoardFull(state)) {
       // std::cout << indent << "  BOARD FULL, returning 0" << std::endl;
        return 0; // Draw
    }

    int bestVal = -100000;
    std::cout << indent << "  trying moves:" << std::endl;

    for (int col = 0; col < CONNECT4_COLS; col++) {
        // find lowest empty row in this column
        int lowestRow = -1;
        for (int row = CONNECT4_ROWS - 1; row >= 0; row--) {
            int idx = row * CONNECT4_COLS + col;
            if (state[idx] == '0') {
                lowestRow = row;
                break;
            }
        }
        if (lowestRow == -1) {
            //std::cout << indent << "    col " << col << ": FULL" << std::endl;
            continue; // Column is full
        }

        int idx = lowestRow * CONNECT4_COLS + col;
        // place the piece for current player
        char playerChar = '0' + (playerColor + 1);
        state[idx] = playerChar;
        //std::cout << indent << "    col " << col << ": placed '" << state[idx] << "' at idx " << idx << std::endl;

        // evaluate recursively (negate for opponent)
        int val = -negamax(state, depth + 1, -playerColor);
        //std::cout << indent << "    col " << col << ": recursive result = " << val << std::endl;

        // undo move
        state[idx] = '0';

        if (val > bestVal) {
            //std::cout << indent << "    col " << col << ": NEW BEST (" << val << " > " << bestVal << ")" << std::endl;
            bestVal = val;
        }
    }

    if (bestVal == -100000) {
        //std::cout << indent << "  NO MOVES FOUND, returning 0" << std::endl;
        return 0;
    }
    
    //std::cout << indent << "  returning bestVal: " << bestVal << std::endl;
    return bestVal;
}