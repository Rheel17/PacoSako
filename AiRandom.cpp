/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "AiRandom.h"

#include <thread>
#include <ctime>
#include <chrono>

namespace ps {

AiRandom::AiRandom(Piece::Color playerColor) :
	 Ai(playerColor), _rng(time(nullptr)) {}

Move AiRandom::MakeMove(const Board& board, const GameMoveData& moveData, std::atomic_bool& stop) {
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	const auto& moves = board.GetAllPossibleMoves(_player_color, moveData);
	std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
	size_t index = dist(_rng);
	return moves[index];
}

}
