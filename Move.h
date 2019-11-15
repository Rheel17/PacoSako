/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef MOVE_H_
#define MOVE_H_

#include "Board.h"

namespace ps {

class Move {

public:
	Move() = default;
	Move(BoardPosition startMove);

	void AddPosition(BoardPosition position);

	void PerformOn(Board& board) const;

	friend std::ostream& operator<<(std::ostream& out, const Move& move);

private:
	std::vector<BoardPosition> _positions;

};

}

#endif
