/*
 * Copyright © 2020 Levi van Rheenen. All rights reserved.
 */
#include "Move.h"

//#include <cassert>

#include "Board.h"

namespace ps {

void assert(bool b) {
	if (!b) {
		abort();
	}
}

SubMove::SubMove(Piece movingPiece, Piece resultingPiece, BoardPosition startPosition, BoardPosition endPosition) :
		moving_piece(std::move(movingPiece)), resulting_piece(std::move(resultingPiece)),
		start_position(std::move(startPosition)), end_position(std::move(endPosition)) {}

Move::Move(BoardPosition startMove) {
		_positions.push_back(std::move(startMove));
}

void Move::AddPosition(BoardPosition to) {
	_positions.push_back(std::move(to));
}

const std::vector<BoardPosition>& Move::GetPositions() const {
	return _positions;
}

std::vector<SubMove> Move::GetSubMoves(const Board& board) const {
	Board dummy = board;
	return _Move(dummy);
}

void Move::PerformOn(Board &board) const {
	_Move(board);
}

std::vector<SubMove> Move::_Move(Board& board) const {
	std::vector<SubMove> submoves;

	assert(_positions.size() > 1);

	Piece movingPiece = board[_positions[0]];
	board[_positions[0]] = Piece();

	// The movement of a single union
	if (movingPiece.GetColor() == Piece::Color::UNION) {
		Piece& toPiece = board[_positions[1]];

		assert(_positions.size() == 2);
		assert(toPiece.GetColor() == Piece::Color::EMPTY);

		toPiece = movingPiece;

		// check for pawn promotions
		// TODO: allow the player to choose the piece
		if (_positions[1].GetRow() == 7 && toPiece.GetWhiteType() == Piece::Type::PAWN) {
			toPiece = Piece(Piece::Type::QUEEN, toPiece.GetBlackType());
		}

		if (_positions[1].GetRow() == 0 && toPiece.GetBlackType() == Piece::Type::PAWN) {
			toPiece = Piece(toPiece.GetWhiteType(), Piece::Type::QUEEN);
		}

		submoves.push_back(SubMove(movingPiece, toPiece, _positions[0], _positions[1]));
		return submoves;
	}

	// The movement of a castling king
	if (movingPiece.GetTypeOfColor(movingPiece.GetColor()) == Piece::Type::KING &&
			abs(_positions[1].GetColumn() - _positions[0].GetColumn()) == 2) {

		Piece& toPiece = board[_positions[1]];

		// move the king
		toPiece = movingPiece;
		movingPiece = Piece();
		submoves.push_back(SubMove(movingPiece, movingPiece, _positions[0], _positions[1]));

		// move the corresponding rook
		int delta = _positions[1].GetColumn() - _positions[0].GetColumn();
		BoardPosition rookStart { _positions[0].GetRow(), delta < 0 ? 0 : 7 };
		BoardPosition rookEnd { _positions[0].GetRow(), delta < 0 ? 3 : 5 };

		submoves.push_back(SubMove(board[rookStart], board[rookStart], rookStart, rookEnd));
		board[rookEnd] = board[rookStart];
		board[rookStart] = Piece();

		return submoves;
	}

	// the movement of a normal piece into a potential chain
	for (size_t i = 0; i < _positions.size() - 1; i++) {
		assert(movingPiece.GetColor() != Piece::Color::EMPTY);

		const BoardPosition& from = _positions[i];
		const BoardPosition& to = _positions[i + 1];
		Piece& toPiece = board[to];

		Piece startingPiece = movingPiece;

		// check for pawn promotion
		// TODO: allow the player to choose the piece
		if (to.GetRow() == 7 && movingPiece.GetWhiteType() == Piece::Type::PAWN) {
			movingPiece = Piece(Piece::Type::QUEEN, movingPiece.GetBlackType());
		}

		if (to.GetRow() == 0 && movingPiece.GetBlackType() == Piece::Type::PAWN) {
			movingPiece = Piece(movingPiece.GetWhiteType(), Piece::Type::QUEEN);
		}

		switch (toPiece.GetColor()) {
			case Piece::Color::EMPTY:
				if (movingPiece.GetTypeOfColor(movingPiece.GetColor()) == Piece::Type::PAWN &&
						from.GetColumn() != to.GetColumn()) {

					// this was an en passant move
					BoardPosition enPassantPosition = { from.GetRow(), to.GetColumn() };
					Piece& epPiece = board[enPassantPosition];

					submoves.push_back(SubMove(epPiece, toPiece, enPassantPosition, to));

					toPiece = epPiece;
					epPiece = Piece();
					movingPiece = toPiece.MakeUnionWith(movingPiece);

					submoves.push_back(SubMove(startingPiece, toPiece, from, to));

				} else {
					toPiece = movingPiece;
					movingPiece = Piece();

					submoves.push_back(SubMove(startingPiece, toPiece, from, to));
				}
				break;
			case Piece::Color::UNION:
				movingPiece = toPiece.MakeUnionWith(movingPiece);
				submoves.push_back(SubMove(startingPiece, toPiece, from, to));
				break;
			case Piece::Color::WHITE:
			case Piece::Color::BLACK:
				toPiece.MakeUnionWith(movingPiece);
				movingPiece = Piece();
				submoves.push_back(SubMove(startingPiece, toPiece, from, to));
				break;
		}
	}

	assert(movingPiece.GetColor() == Piece::Color::EMPTY);
	return submoves;
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
