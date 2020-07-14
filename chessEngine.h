#include <iostream>
#include <string>
#include<fstream>


// Definations
// pieces
#define PC_KING 0
#define PC_QUEEN 1
#define PC_BISHOP 2
#define PC_KNIGHT 3
#define PC_ROOK 4
#define PC_PAWN 5
#define PC_EMPTY 12

//color
#define BLACK 0
#define WHITE 6

//temp def
#define POS_BLACKPAWNINIT 2
#define POS_WHITEPAWNINIT 7

// Errors
// piece errors
#define NO_PIECE 1

// not turn
#define NOT_TURN 12

// move errors
// take input again from user
#define ME_DESTOUTOFBOARD 2
#define ME_SAMECOORDS 3
#define ME_INVALIDMOVE 4
#define ME_BLOCKEDMOVE 5
#define ME_SELFCHECK 9
#define ME_SELFCHECKMATE 10
#define ME_SELFSTALEMATE 11

// Notifications
// check
//#define CK_NOCHECK 0
//warn the nest player
#define CK_CHECK 6
#define CK_CHECKMATE 7
#define CK_STALEMATE 8

//Actions
#define MISC_INPUTPROMOTETO 13

//macros for internal process
//pathTracing modes
#define PT_DIAG 0
#define PT_CROSS 1

class COORD
{
public:
	int x;
	int y;

	COORD()
	{
		x = 0;
		y = 0;
	}
	COORD(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

class MATRIX
{
public:
	int m[9][9] = { PC_EMPTY };

	void refreshMatrix();
	void copyMatrix(MATRIX source);
};

class PIECE
{
public:
	COORD coord;
	int type;
	int color;

	void setPiece(COORD coord, int type, int color);

	PIECE(COORD coord, int type, int color)
	{
		this->coord = coord;
		this->type = type;
		this->color = color;
	}
	PIECE()
	{
		coord.x = 0;
		coord.y = 0;
		type = PC_EMPTY;
		color = WHITE;
	}
};

class GAME
{
protected:
	int turn = WHITE;
	PIECE pieces[32];

public:
	void setTurn(int turn)
	{
		this->turn = turn;
	}
	void setPieces(PIECE pieces[])
	{
		for (int i = 0; i < 32; i++)
			this->pieces[i] = pieces[i];
	}
};

class chessEngine : protected GAME
{
private:
	MATRIX board;
	int turn; 
	PIECE pieces[32];
	PIECE killedPiece;
	PIECE movedPiece;

	PIECE* getPieceByCoord(COORD coord);
	int validMove(PIECE piece, COORD destCoord);
	int pathTracing(COORD source, COORD dest, int mode);
	bool pieceExistsAtCoord(COORD coord);
	int isCheckmate(int turn);
	COORD getKingPos(int turn);
	void saveMove(PIECE sourcePiece, PIECE killPiece);
	void executeMove(PIECE* piecePtr, COORD destCoord, int notTurn);
	void updateBoard();
	void reverseMove();
	void promotePiece(PIECE* ptrPiece, int promoteType);
	void updateClientMatrix(MATRIX* ptrClientMatrix);
	void initPieces();

public:
	int makeMove(COORD sourceCoord, COORD destCoord, MATRIX* ptrClientMatrix, int optionalArgs);
	int gameSetup(std::ifstream *gamedata, MATRIX* ptrClientMatrix);
	int getTurn();
	int saveGame(std::ofstream *gamedata);
};