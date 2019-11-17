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

class Move {

public:
	Move() = default;
	Move(BoardPosition startMove);

	void AddPosition(BoardPosition position);

	const std::vector<BoardPosition>& GetPositions() const;

	void PerformOn(Board& board) const;

	friend std::ostream& operator<<(std::ostream& out, const Move& move);

private:
	std::vector<BoardPosition> _positions;

};

}

#endif
