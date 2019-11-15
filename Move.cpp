/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "Move.h"

#include <cassert>

namespace ps {

Move::Move(BoardPosition startMove) {
	_positions.push_back(std::move(startMove));
}

void Move::AddPosition(BoardPosition to) {
	_positions.push_back(std::move(to));
}

void Move::PerformOn(Board &board) const {
	assert(_positions.size() > 1);

	Piece movingPiece = board[_positions[0]];
	board[_positions[0]] = Piece();

	if (movingPiece.GetColor() == Piece::Color::UNION) {
		Piece& toPiece = board[_positions[1]];

		assert(_positions.size() == 2);
		assert(toPiece.GetColor() == Piece::Color::EMPTY);

		toPiece = movingPiece;
		return;
	}

	for (size_t i = 0; i < _positions.size() - 1; i++) {
		assert(movingPiece.GetColor() != Piece::Color::EMPTY);

		const BoardPosition& to = _positions[i + 1];
		Piece& toPiece = board[to];

		switch (toPiece.GetColor()) {
			case Piece::Color::EMPTY:
				toPiece = movingPiece;
				movingPiece = Piece();
				break;
			case Piece::Color::UNION:
				// TODO: chain
				break;
			case Piece::Color::WHITE:
			case Piece::Color::BLACK:
				toPiece.MakeUnionWith(movingPiece);
				movingPiece = Piece();
				break;
		}
	}

	assert(movingPiece.GetColor() == Piece::Color::EMPTY);
}

std::ostream& operator<<(std::ostream& out, const Move& move) {
	if (move._positions.empty()) {
		return out << "[ no moves ]";
	}

	out << move._positions[0].GetName();

	for (size_t i = 1; i < move._positions.size(); i++) {
		out << " -> " << move._positions[i].GetName();
	}

	return out;
}

}
