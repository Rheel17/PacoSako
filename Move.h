/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef MOVE_H_
#define MOVE_H_

#include "BoardPosition.h"
#include "Piece.h"

#include <vector>

namespace ps {

class Board;

class SubMove {
	friend class Move;

public:
	const Piece moving_piece;
	const Piece resulting_piece;
	const BoardPosition start_position;
	const BoardPosition end_position;

private:
	SubMove(Piece movingPiece, Piece resultingPiece, BoardPosition startPosition, BoardPosition endPosition);

};

class Move {

public:
	Move() = default;
	Move(BoardPosition startMove);

	void AddPosition(BoardPosition position);

	const std::vector<BoardPosition>& GetPositions() const;

	std::vector<SubMove> GetSubMoves(const Board& board) const;

	void PerformOn(Board& board) const;

	friend std::ostream& operator<<(std::ostream& out, const Move& move);

private:
	std::vector<SubMove> _Move(Board& board) const;

	std::vector<BoardPosition> _positions;

};

}

#endif
