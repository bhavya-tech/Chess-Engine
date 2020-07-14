#include"chessEngine.h"

/*
	Bugs:

	-Stalemate and checkmate messages even when not


	Features left to program:

	-The conditions for draw when a player has no legal move left 
	-50 step draw after only king surviving
	-Castling and En Passant
*/
/*
	BLACK color pieces will be with lower coords
*/

void chessEngine::saveMove(PIECE sourcePiece, PIECE killPiece)
{
	movedPiece = sourcePiece;
	killedPiece = killPiece;
}

// May have bugs
void chessEngine::reverseMove()
{
	// resotre killed piece
	for (int i = killedPiece.color; i < killedPiece.color + 16; i++)
		if (killedPiece.type == pieces[i].type && pieces[i].coord.x == -1)
			pieces[i] = killedPiece;

	// restore moved piece
	for (int i = movedPiece.color; i < movedPiece.color + 16; i++)
		if (pieces[i].type == movedPiece.type && pieces[i].coord.x == -1)
			pieces[i] = movedPiece;
}

int chessEngine::gameSetup(std::ifstream *gamedata, MATRIX* ptrClientMatrix)
{
	char status;
	*gamedata >> status;
	if (status == 'C')
	{
		// load from file
		*gamedata >> turn;

		for (int i = 0; i < 32; i++)
		{
			*gamedata >> pieces[i].color;
			*gamedata >> pieces[i].coord.x;
			*gamedata >> pieces[i].coord.y;
			*gamedata >> pieces[i].type;
		}
	}
	else
	{
		// initialize new game

		turn = WHITE;
		initPieces();
		updateBoard();
		updateClientMatrix(ptrClientMatrix);
		return 0;
	}
	
	updateBoard();
	updateClientMatrix(ptrClientMatrix);

	return 0;
}

void PIECE::setPiece(COORD coord, int type, int color)
{
		this->coord = coord;
		this->type = type;
		this->color = color;
}

void chessEngine::executeMove(PIECE * piecePtr, COORD destCoord, int notTurn)
{
	bool pieceSaved = false;
	int index;
	if (notTurn)
		index = 0;
	else
		index = 16;

	// Kill piece at destiantion, if exists
	for (int i = index; i < index + 16; i++)
	{	
		if (pieces[i].coord.x == destCoord.x && pieces[i].coord.y == destCoord.y)
		{
			saveMove(*piecePtr, pieces[i]);
			pieceSaved = true;
			pieces[i].coord.x = pieces[i].coord.y = -1;
			break;
		}
	}

	PIECE nullpiece;
	if (!pieceSaved)
		saveMove(*piecePtr,nullpiece);

	// update coords of piece to dest
	piecePtr->coord.x = destCoord.x;
	piecePtr->coord.y = destCoord.y;

	// update the board by new list
	updateBoard();
}

int chessEngine::isCheckmate(int turn)
{
	int notTurn;
	int cntAlivePiecesofTurn = 0;
	if (turn)
		notTurn = 0;
	else
		notTurn = 16;

	COORD kingPos = getKingPos(turn);

	COORD check[9];
	
	for (int i = -1, ind = 0; i <= 1; i++)
		for (int j = -1; j <= 1; j++, ind++)
			check[ind] = COORD(i + kingPos.x, j + kingPos.y);
		
	// if any positions goes out of board mark it (-1,-1)
	for (int i = 0; i < 9; i++)
		if (check[i].x < 1 || check[i].x > 8 || check[i].y < 1 || check[i].y > 8)
			check[i] = COORD(-1,-1);

	// mark pieces of same color blocking the possible escape
	for(int i = turn; i < turn + 16; i++)
	{
		COORD temp = pieces[i].coord;
		if (temp.x == -1 || pieces[i].type == PC_KING)
			continue;

		// keep count of alive pieces to account for stalemate
		cntAlivePiecesofTurn++;		

		for (int j = 0; j < 9; j++)
			if (temp.x == check[j].x && temp.y == check[j].y)
				check[j] = COORD(-1,-1);
	}

	// mark all the possible moves of opponent pieces on the check matrix
	for (int i = notTurn; i < notTurn + 16; i++)
	{
		PIECE temp = pieces[i];
		if (temp.coord.x == -1 || (temp.type == PC_KING && temp.color == turn))
			continue;

		for (int j = 0; j < 9; j++)
			if(check[j].x != -1)
				if(!validMove(temp,check[j]))
					check[j] = COORD(-1,-1);
	}

	int blocked = 0;
	for (int i = 0; i < 9; i++)
		if (check[i].x == -1)
			blocked++;

	switch (blocked)
	{
	case 9:
		return CK_CHECKMATE;
	case 8:
		if (check[5].x != -1 && cntAlivePiecesofTurn == 1)
			return CK_STALEMATE;
		return 0;
	case 1:
		if (check[5].x == -1)
			return CK_CHECK;
		return 0;
	default:
		return 0;
	}
	return 0;
}

int chessEngine::validMove(PIECE piece, COORD destCoord)
{
	if (destCoord.x > 8 || destCoord.x < 1 || destCoord.y < 1 || destCoord.y > 8)
		return ME_DESTOUTOFBOARD;

	COORD r;
	r.x = piece.coord.x - destCoord.x;
	r.y = piece.coord.y - destCoord.y;

	if (r.x == 0 && r.y == 0)
		return ME_SAMECOORDS;

	switch (piece.type)
	{
	case PC_KING:
		if (r.x <= -1 || r.x >= 1 || r.y <= -1 || r.y >= -1)
			return ME_INVALIDMOVE;

		break;

	case PC_QUEEN:
		if (!(r.x == -r.y || r.x == r.y || r.x == 0 || r.y == 0))
			return ME_INVALIDMOVE;

		if((abs(r.x) == abs(r.y) && pathTracing(piece.coord, destCoord, PT_DIAG)) || ((r.x == 0 || r.y == 0) && pathTracing(piece.coord, destCoord, PT_DIAG)))
			return ME_BLOCKEDMOVE;

		break;

	case PC_ROOK:
		if(!(r.x == 0 || r.y == 0))
			return ME_INVALIDMOVE;

		else if (pathTracing(piece.coord, destCoord, PT_CROSS))
			return ME_BLOCKEDMOVE;

		break;

	case PC_KNIGHT:
		if (!((r.x == 2 && r.y == 1) && (r.x == -2 && r.y == 1) && (r.x == 2 && r.y == -1) && (r.x == -2 && r.y == -1) && (r.x == 1 && r.y == 2) && (r.x == -1 && r.y == 2) && (r.x == 1 && r.y == -2) && (r.x == -1 && r.y == -2)))
			return ME_INVALIDMOVE;

		break;

	case PC_BISHOP:
		if(!(r.x == r.y || r.x == -r.y))
			return ME_INVALIDMOVE;

		if(pathTracing(piece.coord, destCoord, PT_DIAG))
			return ME_BLOCKEDMOVE;

		break;

	case PC_PAWN:
		//exceptions may exist --- to be checked
		if(piece.color == BLACK)
		{
			// Move in opposite direction
			if(r.y > 0)
				return ME_INVALIDMOVE;
			// Move more than two steps in first move
			if (piece.coord.y == POS_BLACKPAWNINIT && r.y < -2)
				return ME_INVALIDMOVE;

			// Move more than one step in next move
			if (r.y < -1 && piece.coord.y != POS_BLACKPAWNINIT)
				return ME_INVALIDMOVE;

			// Move more than two steps horizontal
			if (abs(r.x) >= 2)
				return ME_INVALIDMOVE;

			// Making horizontal move if no opponent at destiantion
			if (!pieceExistsAtCoord(destCoord) && abs(r.x) == 1)
				return ME_INVALIDMOVE;

			// Piece blocking forward move
			if (pieceExistsAtCoord(destCoord) && r.x == 0)
				return ME_BLOCKEDMOVE;
		}
		else
		{
			// Move in opposite direction
			if (r.y < 0)
				return ME_INVALIDMOVE;
			// Move more than two steps in first move
			if (piece.coord.y == POS_WHITEPAWNINIT && r.y > 2)
				return ME_INVALIDMOVE;
			// Move more than one step in next move
			if (r.y > 1 && piece.coord.y != POS_WHITEPAWNINIT)
				return ME_INVALIDMOVE;

			// Move more than two steps horizontal
			if (abs(r.x) >= 2)
				return ME_INVALIDMOVE;

			// Making horizontal move if no opponent at destiantion
			if (!pieceExistsAtCoord(destCoord) && abs(r.x) == 1)
				return ME_INVALIDMOVE;

			// Piece blocking forward move
			if (pieceExistsAtCoord(destCoord) && r.x == 0)
				return ME_BLOCKEDMOVE;
		}
		break;
	}
	return 0;
}

int chessEngine::pathTracing(COORD source, COORD dest, int mode)
{
	if (mode == PT_CROSS)
	{
		int i = 0, j = 0;
		if (source.x < dest.x)
			i++;
		else if (source.x > dest.x)
			i--;

		if (source.y < dest.y)
			j++;
		else if (source.y > dest.y)
			j--;

		if ((board.m[source.x + i][source.y + j] != ' ') && (source.x + i != dest.x || source.y + j != dest.y))
			return ME_BLOCKEDMOVE;
	}
	else if (mode == PT_DIAG)
	{
		int i = 0, j = 0;

		if (source.x < dest.x)
			i++;
		else if (source.x > dest.x)
			i--;

		if (source.y < dest.y)
			j++;
		else if (source.y > dest.y)
			j--;

		if ((board.m[source.x + i][source.y + j] != ' ') && (source.x + i != dest.x || source.y + j != dest.y))
			return ME_BLOCKEDMOVE;
	}
	return 0;
}

int chessEngine::makeMove(COORD sourceCoord, COORD destCoord, MATRIX * ptrClientMatrix, int optionalArgs)
{
	int notTurn;
	if (turn)
		notTurn = BLACK;
	else
		notTurn = WHITE;

	PIECE * sourcePiece = getPieceByCoord(sourceCoord);

	// check if piece exists
	if (sourcePiece == nullptr)
		return NO_PIECE;

	// return message of not turn if the selected piece belongs to other player
	if (sourcePiece->color != turn)
		return NOT_TURN;

	// if optionalargs is not null and source piece can be promoted then promote it to optionalargs
	if ((sourcePiece->coord.y == 8 || sourcePiece->coord.y == 1) && sourcePiece->type == PC_PAWN && optionalArgs >= 0 && optionalArgs <= 5)
	{
		promotePiece(sourcePiece, optionalArgs);
		updateBoard();
		updateClientMatrix(ptrClientMatrix);
		return 0;
	}

	// check if move is valid
	int validMoveStatus = validMove(*sourcePiece, destCoord);
	if (validMoveStatus)
		return validMoveStatus;

	// execute move in internal board and pieces list
	executeMove(sourcePiece, destCoord, turn);

	// check if move causes self check
	int checkConditionOfTurn = isCheckmate(turn);
	
	// if there is self check then reverse the move
	// return the corresponding error to the client
	switch (checkConditionOfTurn)
	{
	case CK_CHECK:
		reverseMove();
		return ME_SELFCHECK;
	case CK_CHECKMATE:
		reverseMove();
		return ME_SELFCHECKMATE;
	case CK_STALEMATE:
		reverseMove();
		return ME_SELFSTALEMATE;
	default:
		break;
	}

	// update the client matrix
	updateClientMatrix(ptrClientMatrix);

	//if piece is pawn check if it can be promoted and return the notification to client
	if (sourcePiece->type == PC_PAWN)
		if (sourcePiece->coord.y == 8 || sourcePiece->coord.y == 1)
			return MISC_INPUTPROMOTETO;

	//check if move gives check to opponent and return same notification
	int checkConditionOfNotTurn = isCheckmate(notTurn);

	//flip the turn
	turn = notTurn;

	if (checkConditionOfNotTurn)
		return checkConditionOfNotTurn;

	return 0;
}

void chessEngine::promotePiece(PIECE* ptrPiece, int promoteType)
{
	ptrPiece->type = promoteType;
}

void chessEngine::initPieces()
{
	int index;
	// BLACK
	// initialize pawns
	for (index = BLACK; index < BLACK + 8; index++)
		pieces[index].setPiece(COORD(index + 1, POS_BLACKPAWNINIT), PC_PAWN, BLACK);

	// init rook
	pieces[8].setPiece(COORD(1, POS_BLACKPAWNINIT - 1), PC_ROOK, BLACK);
	pieces[9].setPiece(COORD(8, POS_BLACKPAWNINIT - 1), PC_ROOK, BLACK);

	// init knight
	pieces[10].setPiece(COORD(2, POS_BLACKPAWNINIT - 1), PC_KNIGHT, BLACK);
	pieces[11].setPiece(COORD(7, POS_BLACKPAWNINIT - 1), PC_KNIGHT, BLACK);

	// init bishop
	pieces[12].setPiece(COORD(3, POS_BLACKPAWNINIT - 1), PC_BISHOP, BLACK);
	pieces[13].setPiece(COORD(6, POS_BLACKPAWNINIT - 1), PC_BISHOP, BLACK);

	// init king
	pieces[14].setPiece(COORD(4, POS_BLACKPAWNINIT - 1), PC_KING, BLACK);

	// init queen
	pieces[15].setPiece(COORD(5, POS_BLACKPAWNINIT - 1), PC_QUEEN, BLACK);



	// WHITE
	// initialize pawns
	for (index = 16; index < 16 + 8; index++)
		pieces[index].setPiece(COORD(index - 15, POS_WHITEPAWNINIT), PC_PAWN, WHITE);

	// init rook
	pieces[24].setPiece(COORD(1, POS_WHITEPAWNINIT + 1), PC_ROOK, WHITE);
	pieces[25].setPiece(COORD(8, POS_WHITEPAWNINIT + 1), PC_ROOK, WHITE);

	// init knight
	pieces[26].setPiece(COORD(2, POS_WHITEPAWNINIT + 1), PC_KNIGHT, WHITE);
	pieces[27].setPiece(COORD(7, POS_WHITEPAWNINIT + 1), PC_KNIGHT, WHITE);

	// init bishop
	pieces[28].setPiece(COORD(3, POS_WHITEPAWNINIT + 1), PC_BISHOP, WHITE);
	pieces[29].setPiece(COORD(6, POS_WHITEPAWNINIT + 1), PC_BISHOP, WHITE);

	// init king
	pieces[30].setPiece(COORD(4, POS_WHITEPAWNINIT + 1), PC_KING, WHITE);

	// init queen
	pieces[31].setPiece(COORD(5, POS_WHITEPAWNINIT + 1), PC_QUEEN, WHITE);
}

bool chessEngine::pieceExistsAtCoord(COORD coord)
{
	for (int i = 0; i < 32; i++)
		if (coord.x == pieces[i].coord.x && coord.y == pieces[i].coord.y)
			return true;

	return false;
}

void MATRIX::refreshMatrix()
{
	for (int* ptr = &m[0][0]; ptr <= &m[8][8]; ptr++)
		*ptr = PC_EMPTY;
}

void MATRIX::copyMatrix(MATRIX source)
{
	int* ptrSource = &source.m[0][0];
	for (int* ptr = &m[0][0]; ptr <= &m[8][8]; ptr++,ptrSource++)
		*ptr = *ptrSource;
}

PIECE* chessEngine::getPieceByCoord(COORD coord)
{
	for (int i = 0; i < 32; i++)
		if (coord.x == pieces[i].coord.x && coord.y == pieces[i].coord.y)
			return &pieces[i];

	return nullptr;
}

void chessEngine::updateBoard()
{
	board.refreshMatrix();
	for (int i = 0; i < 32; i++)
		board.m[pieces[i].coord.x][pieces[i].coord.y] = pieces[i].type + pieces[i].color;
}

COORD chessEngine::getKingPos(int turn)
{
	for (int i = 0; i < 32; i++)
		if (pieces[i].type == PC_KING && pieces[i].color == turn)
			return pieces[i].coord;
}

int chessEngine::getTurn()
{
	return turn;
}

int chessEngine::saveGame(std::ofstream *gamedata)
{
	*gamedata << 'C';
	*gamedata << turn;
	*gamedata << "\n";
	
	for (int i = 0; i < 32; i++)
	{
		*gamedata << pieces[i].color;
		*gamedata << "\n";
		*gamedata << pieces[i].coord.x;
		*gamedata << "\n";
		*gamedata << pieces[i].coord.y;
		*gamedata << "\n";
		*gamedata << pieces[i].type;
		*gamedata << "\n";
	}

	return 0;
}

void chessEngine::updateClientMatrix(MATRIX* ptrClientMatrix)
{
	// refresh client matrix for no data overwirting
	ptrClientMatrix->refreshMatrix();

	//update the client matrix
	ptrClientMatrix->copyMatrix(board);
}
