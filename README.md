# chessEngine

Import it to your project and include the header ```chessEngine.h``` to never worry about the chess backend logic.

# How to use?

# Initiate the game:
1. Create object ```chessEngine```.
1. Create a object of ```MATRIX``` in which the output will be given from the library.

    * _It is a 9x9 array in which 1,1 to 8,8 will represent the board._

1. Begin the game by initializing:

    1. To start the game without any need to save it use ```int initGame(MATRIX* ptrClientMatrix)```.

    1. Call the fuction ```int gameSetup(std::ifstream *gamedata, MATRIX* ptrClientMatrix)``` and pass the pointer to std::ifstream which has the corresponding file open and points at the beginning of the file. (This is a test version so only .txt files are supported)
    
	    1. To initiate a new game, the file should be empty.
	    1. To load the game, the ifstream should point at the beginning of where the ```saveGame()``` began to start writing.(_It is advisable to keep this file only for game data._)
	    
## Play the game:
    
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
   
      To handle pormote message, take the input from the player to which piece it needs to promote. Then again call the function with source and dest coords without setting any value, address to matrix (to update) and optionalArgs with the value of the piece to which it is to be promoted.

## Save the game:
To save the game, call the function ```int saveGame(std::ofstream *gamedata)``` with std::ofstream pointing at beginning of the file.(_Since this is test version, only .txt format is supported._)

## Undo move:
Currently this feature is not porvided by the library. However, if developer needs to implement it then after each move save the game. When the user wants to undo, simply load the game at the previous move.(_This feature may be rolled out with with optimizations in further updates_) 

# Sample program:
A [sample program](https://github.com/bhavya-tech/chessConsoleClient) in the terminal is made for any reference to how to implement it into code.
      
      
# Bugs:

 -Stalemate and checkmate messages even when not:
 
The program identifies the check and checkmate conditions properly. However, it cannot be determine if any opponent piece can be killed or obstructed to prevent it. This is the reason why the game is not terminated at checkmate. _(An algorithm is currently being tested for the same. It will soon be implemented with an update.)_
 
# Features left to program:

	-The conditions for draw when a player has no legal move left 
	-50 step draw after only king surviving
	-Castling and En Passant
