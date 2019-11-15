/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef PIECE_H_
#define PIECE_H_

#include <iostream>

namespace ps {

class Piece {

public:
	enum class Type {
		NONE, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING
	};

	enum class Color {
		EMPTY, WHITE, BLACK, UNION
	};

public:
	Piece();
	Piece(Type whitePiece, Type blackPiece);

	Type GetWhiteType() const;
	Type GetBlackType() const;
	Type GetTypeOfColor(Color color) const;
	Color GetColor() const;

	void MakeUnionWith(const Piece& other);

	friend std::ostream& operator<<(std::ostream& out, const Piece& piece);

private:
	Type _white_type;
	Type _black_type;
	Color _color;

};

std::ostream& operator<<(std::ostream& out, Piece::Type t);

}

#endif
