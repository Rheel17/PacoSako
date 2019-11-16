/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef BOARD_H_
#define BOARD_H_

#include "GameMoveData.h"
#include "Piece.h"

#include <array>
#include <vector>

namespace ps {

class Board {

public:
	Board();

	const Piece& GetPiece(const BoardPosition& position) const;
	Piece& GetPiece(const BoardPosition& position);

	const Piece& operator[](const BoardPosition& position) const;
	Piece& operator[](const BoardPosition& position);

	std::vector<BoardPosition> CalculatePossibleMoves(const BoardPosition& piece, Piece::Color playerColor, const GameMoveData& moveData) const;
	std::vector<BoardPosition> CalculatePossibleMoves(const BoardPosition& origin, const Piece& piece, Piece::Color playerColor, const GameMoveData& moveData) const;

private:
	void _AddPawnMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, const GameMoveData& moveData) const;
	void _AddKnightMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddKingMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, const GameMoveData& moveData) const;
	void _AddDiagonalMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddStraightMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, std::array<BoardPosition, 4> dps) const;

	Piece _squares[8][8] {};

};

}

#endif
