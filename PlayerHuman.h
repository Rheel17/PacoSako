/*
 * Copyright © 2020 Levi van Rheenen. All rights reserved.
 */
#ifndef PLAYERHUMAN_H_
#define PLAYERHUMAN_H_

#include "Player.h"

namespace ps {

class Window;

class PlayerHuman : public Player {

public:
	PlayerHuman(Piece::Color playerColor, Window *window);

	Move MakeMove(const Board& board, const GameMoveData& moveData, const std::vector<Move>& possible, std::atomic_bool& stop) override;

private:
	Window *_window;

};

}

#endif
