/*
 * Copyright © 2020 Levi van Rheenen. All rights reserved.
 */
#ifndef PLAYER_H_
#define PLAYER_H_

#include "Board.h"
#include "GameMoveData.h"
#include "Move.h"

#include <atomic>

namespace ps {

class Player {

public:
	Player(Piece::Color playerColor);
	virtual ~Player();

	/**
	 * Will be called by the host game on the game loop thread.
	 */
	virtual Move MakeMove(const Board& board, const GameMoveData& moveData, const std::vector<Move>& possible, std::atomic_bool& stop) = 0;

protected:
	const Piece::Color _player_color;

};

}

#endif
