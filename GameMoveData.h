/*
 * Copyright © 2020 Levi van Rheenen. All rights reserved.
 */
#ifndef GAMEMOVEDATA_H_
#define GAMEMOVEDATA_H_

#include "BoardPosition.h"

namespace ps {

struct GameMoveData {
	BoardPosition en_passant_position = { -1, -1 };
	bool can_white_castle_king_side = true;
	bool can_white_castle_queen_side = true;
	bool can_black_castle_king_side = true;
	bool can_black_castle_queen_side = true;
};

}

#endif
