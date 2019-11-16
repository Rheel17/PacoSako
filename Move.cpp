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

const std::vector<BoardPosition>& Move::GetPositions() const {
	return _positions;
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
				if (movingPiece.GetTypeOfColor(movingPiece.GetColor()) == Piece::Type::PAWN &&
						_positions[i].GetColumn() != to.GetColumn()) {

					// this was an en passant move
					BoardPosition enPassantPosition = { _positions[i].GetRow(), to.GetColumn() };
					Piece& epPiece = board[enPassantPosition];

					toPiece = epPiece;
					epPiece = Piece();
					movingPiece = toPiece.MakeUnionWith(movingPiece);
				} else if (movingPiece.GetColor() != Piece::Color::UNION &&
						movingPiece.GetTypeOfColor(movingPiece.GetColor()) == Piece::Type::KING &&
						abs(_positions[i].GetColumn() - to.GetColumn()) == 2) {

					// this was a castling move
					toPiece = movingPiece;
					movingPiece = Piece();

					int delta = _positions[i].GetColumn() - to.GetColumn();

					if (delta == 2) {
						board[{ to.GetRow(), to.GetColumn() + delta / 2}] = board[{ to.GetRow(), 0}];
						board[{ to.GetRow(), 0 }] = Piece();
					} else if (delta == -2) {
						board[{ to.GetRow(), to.GetColumn() + delta / 2}] = board[{ to.GetRow(), 7}];
						board[{ to.GetRow(), 7 }] = Piece();
					}
				} else {
					toPiece = movingPiece;
					movingPiece = Piece();
				}
				break;
			case Piece::Color::UNION:
				movingPiece = toPiece.MakeUnionWith(movingPiece);
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
