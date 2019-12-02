/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "AiRandom.h"

#include <thread>
#include <ctime>
#include <chrono>

namespace ps {

AiRandom::AiRandom(Piece::Color playerColor) :
	 Ai(playerColor) {

	auto seed = time(nullptr) * std::intptr_t(this);
	std::cout << (playerColor == Piece::Color::WHITE ? "white" : "black") << " seed: " << seed << std::endl;
	_rng = std::mt19937_64(seed);
}

Move AiRandom::MakeMove(const Board& board, const GameMoveData& moveData, const std::vector<Move>& possible, std::atomic_bool& stop) {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::uniform_int_distribution<size_t> dist(0, possible.size() - 1);
	size_t index = dist(_rng);
	return possible[index];
}

}
