/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef PLAYER_H_
#define PLAYER_H_

#include "Board.h"
#include "GameMoveData.h"

namespace ps {

class Player {

public:
	Player(Piece::Color playerColor);
	virtual ~Player();

	/**
	 * Will be called by the host game on the game loop thread.
	 */
	virtual Move MakeMove(const Board& board, const GameMoveData& moveData) = 0;

protected:
	const Piece::Color _player_color;

};

}

#endif
