/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef AIRANDOM_H_
#define AIRANDOM_H_

#include "Ai.h"

#include <random>

namespace ps {

class AiRandom : public Ai {

public:
	AiRandom(Piece::Color playerColor);

	Move MakeMove(const Board& board, const GameMoveData& moveData, const std::vector<Move>& possible, std::atomic_bool& stop) override;

private:
	std::mt19937_64 _rng;

};

}

#endif
