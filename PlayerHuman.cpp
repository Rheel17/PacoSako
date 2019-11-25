/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "PlayerHuman.h"

#include <chrono>

#include "Window.h"

namespace ps {

PlayerHuman::PlayerHuman(Piece::Color playerColor, Window *window) :
		Player(playerColor), _window(window) {}

Move PlayerHuman::MakeMove(const Board& board, const GameMoveData& moveData) {
	return _window->StartMove(_player_color).get();
}

}
