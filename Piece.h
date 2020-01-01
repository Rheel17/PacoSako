/*
 * Copyright © 2020 Levi van Rheenen. All rights reserved.
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

	/**
	 * Returns the new free piece, or empty if there is none
	 */
	Piece MakeUnionWith(const Piece& other);

	bool operator==(const Piece& piece) const;

	friend std::ostream& operator<<(std::ostream& out, const Piece& piece);

private:
	Type _white_type;
	Type _black_type;
	Color _color;

};

std::ostream& operator<<(std::ostream& out, Piece::Type t);

Piece::Color opposite(Piece::Color c);
char getTypeCharWhite(Piece::Type t);
char getTypeCharBlack(Piece::Type t);
Piece::Type getTypeWhite(char c);
Piece::Type getTypeBlack(char c);

}

namespace std {

template<>
struct hash<ps::Piece> {
	inline size_t operator()(const ps::Piece& p) const {
		return size_t(p.GetBlackType()) << 4 | size_t(p.GetWhiteType());
	}
};

}

#endif
