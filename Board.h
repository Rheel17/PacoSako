/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef BOARD_H_
#define BOARD_H_

#include "Piece.h"

#include <array>
#include <vector>

namespace ps {

class BoardPosition {

public:
	BoardPosition(int row, int column);
	BoardPosition(const char *name);
	BoardPosition(std::string name);

	int GetRow() const;
	int GetColumn() const;

	bool IsValid() const;

	bool operator==(const BoardPosition& bp) const;
	bool operator!=(const BoardPosition& bp) const;

private:
	int _row;
	int _column;

};

class Board {

public:
	Board();

	const Piece& GetPiece(const BoardPosition& position) const;
	Piece& GetPiece(const BoardPosition& position);

	const Piece& operator[](const BoardPosition& position) const;
	Piece& operator[](const BoardPosition& position);

	std::vector<BoardPosition> CalculatePossibleMoves(const BoardPosition& piece, Piece::Color playerColor) const;

private:
	void _AddPawnMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddKnightMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddDiagonalMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddStraightMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec, std::array<BoardPosition, 4> dps) const;

	Piece _squares[8][8] {};

};

}

#endif
