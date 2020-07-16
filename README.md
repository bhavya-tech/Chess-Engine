# chessEngine

Import it to your project and include the header ``` #include"chessEngine.h"``` to never worry about the chess backend logic.

## How to use?

1. Create object ```chessEngine```.
1. Create a object of ```MATRIX``` in which the output will be given from the library.

    * _It is a 9x9 array in which 1,1 to 8,8 will represent the board._

1. Begin the game by initializing:

    Call the fuction ```int gameSetup(std::ifstream *gamedata, MATRIX* ptrClientMatrix)``` and pass the pointer to std::ifstream which has the corresponding file open and points at the beginning of the file. (This is a test version so only .txt files are supported)
    
    1. To initiate a new game, the file should be empty.
    1. To load the game, the ifstream should point at the beginning of where the ```saveGame()``` began to start writing.(_It is advisable to keep this file only for game data._)
    
1. When user performs the move save the source and destination coordinates in the ```COORD``` object.

1. Call the function ```int makeMove(COORD sourceCoord, COORD destCoord, MATRIX* ptrClientMatrix, int optionalArgs)```:
   * optionalArgs will be NULL in most cases. It will be used to take input when a pawn is to be promoted.
   
   The function will return three types of messsages:
   
   1. Error Mesages:
   
      _Input is inconsistant with the rules of chess so input is to be taken again_

   Message | Meaning
   --------|--------
   NO_PIECE | No piece is found at source
   ME_DESTOUTOFBOARD | Destination coordinates are not on the board
   ME_INVALIDMOVE | Move is against chess rules
   ME_BLOCKEDMOVE | A piece blocks the path of the move
   ME_SELFCHECK | Performed move causes check to its own king
   ME_SELFCHECKMATE | Performed move causes checkmate to its own king
   ME_SELFSTALEMATE | Performed move causes stalemate to its own king
   
   2. Warning Messages:
      _These messages are to display the next player_
      
   Message | Meaning
   --------|-------
   CK_CHECK | Check to the next player
   CK_CHECKMATE | Checkmate of the next player
   CK_STALEMATE | Stalemate of the next player
   
   3. Other messages:
   
    Message | Meaning
   --------|-------
   MISC_INPUTPROMOTETO | Pawn of the player is to be promoted
   
      To handle this message, take the input from the player to which piece it needs to promote. 
   
      Then again call the function with source and dest coords without setting any value, address to matrix (to update) and optionalArgs with the value of the piece to which it is to be promoted.
      
      
# Sample program:
A [sample program](https://github.com/bhavya-tech/chessConsoleClient) in the terminal is made for any reference to how to implement it into code.
      
      
# Bugs:

    -Stalemate and checkmate messages even when not
 
# Features left to program:

	-The conditions for draw when a player has no legal move left 
	-50 step draw after only king surviving
	-Castling and En Passant
