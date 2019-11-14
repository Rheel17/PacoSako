/*
 * Copyright � 2019 Levi van Rheenen. All rights reserved.
 */
#include "Piece.h"

#include <cassert>

namespace ps {

Piece::Piece() :
		_white_type(Type::NONE), _black_type(Type::NONE), _color(Color::EMPTY) {}

Piece::Piece(Type whitePiece, Type blackPiece) :
		_white_type(whitePiece), _black_type(blackPiece) {

	if (whitePiece != Type::NONE) {
		if (blackPiece != Type::NONE) {
			_color = Color::UNION;
		} else {
			_color = Color::WHITE;
		}
	} else {
		if (blackPiece != Type::NONE) {
			_color = Color::BLACK;
		} else {
			_color = Color::EMPTY;
		}
	}
}

Piece::Type Piece::GetWhiteType() const {
	return _white_type;
}

Piece::Type Piece::GetBlackType() const {
	return _black_type;
}

Piece::Type Piece::GetTypeOfColor(Color color) const {
	switch (color) {
		case Color::EMPTY:
		case Color::UNION:
			return Type::NONE;
		case Color::BLACK:
			return _black_type;
		case Color::WHITE:
			return _white_type;
	}

	return Type::NONE;
}

Piece::Color Piece::GetColor() const {
	return _color;
}

std::ostream& operator<<(std::ostream& out, const Piece& piece) {
	if (piece._color == Piece::Color::UNION) {
		return out << "union_white_" << piece._white_type << "_black" << piece._black_type;
	}

	switch (piece._color) {
		case Piece::Color::EMPTY: return out << "empty";
		case Piece::Color::WHITE: return out << "white_" << piece._white_type;
		case Piece::Color::BLACK: return out << "black_" << piece._black_type;
		case Piece::Color::UNION: /* shouldn't happen */ break;
	}

	return out;
}

std::ostream& operator<<(std::ostream& out, Piece::Type piece) {
	switch (piece) {
		case Piece::Type::NONE: return out << "none";
		case Piece::Type::PAWN: return out << "pawn";
		case Piece::Type::ROOK: return out << "rook";
		case Piece::Type::KNIGHT: return out << "knight";
		case Piece::Type::BISHOP: return out << "bishop";
		case Piece::Type::QUEEN: return out << "queen";
		case Piece::Type::KING: return out << "king";
	}

	return out;
}

}
