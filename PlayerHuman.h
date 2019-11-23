/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef PLAYERHUMAN_H_
#define PLAYERHUMAN_H_

#include "Player.h"

namespace ps {

class PlayerHuman : public Player {

public:
	PlayerHuman(Piece::Color playerColor);
	~PlayerHuman();

};

}

#endif
