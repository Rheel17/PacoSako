/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef AI_H_
#define AI_H_

#include "Player.h"

namespace ps {

class Ai : public Player {

protected:
	Ai(Piece::Color playerColor);

};

}

#endif
