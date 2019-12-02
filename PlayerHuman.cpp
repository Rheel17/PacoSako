/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "PlayerHuman.h"

#include <chrono>

#include "Window.h"

namespace ps {

PlayerHuman::PlayerHuman(Piece::Color playerColor, Window *window) :
		Player(playerColor), _window(window) {}

Move PlayerHuman::MakeMove(const Board& board, const GameMoveData& moveData, std::atomic_bool& stop) {
	std::future<Move> move = _window->StartMove(_player_color);
	std::future_status status;

	do {
		status = move.wait_for(std::chrono::milliseconds(100));
	} while (status == std::future_status::timeout && !stop);

	if (stop || status != std::future_status::ready) {
		return Move();
	} else {
		return move.get();
	}
}

}
