/*
 * Copyright � 2019 Levi van Rheenen. All rights reserved.
 */
#include "Board.h"

namespace ps {

Board::Board() {
	Board& b = *this;

//	// white pieces
//	b["a1"] = Piece(Piece::Type::ROOK, Piece::Type::NONE);
//	b["b1"] = Piece(Piece::Type::KNIGHT, Piece::Type::NONE);
//	b["c1"] = Piece(Piece::Type::BISHOP, Piece::Type::NONE);
//	b["d1"] = Piece(Piece::Type::QUEEN, Piece::Type::NONE);
//	b["e1"] = Piece(Piece::Type::KING, Piece::Type::NONE);
//	b["f1"] = Piece(Piece::Type::BISHOP, Piece::Type::NONE);
//	b["g1"] = Piece(Piece::Type::KNIGHT, Piece::Type::NONE);
//	b["h1"] = Piece(Piece::Type::ROOK, Piece::Type::NONE);
//
//	// black pieces
//	b["a8"] = Piece(Piece::Type::NONE, Piece::Type::ROOK);
//	b["b8"] = Piece(Piece::Type::NONE, Piece::Type::KNIGHT);
//	b["c8"] = Piece(Piece::Type::NONE, Piece::Type::BISHOP);
//	b["d8"] = Piece(Piece::Type::NONE, Piece::Type::QUEEN);
//	b["e8"] = Piece(Piece::Type::NONE, Piece::Type::KING);
//	b["f8"] = Piece(Piece::Type::NONE, Piece::Type::BISHOP);
//	b["g8"] = Piece(Piece::Type::NONE, Piece::Type::KNIGHT);
//	b["h8"] = Piece(Piece::Type::NONE, Piece::Type::ROOK);
//
//	for (unsigned i = 0; i < 8; i++) {
//		// a2-z2 (white pawns)
//		_squares[1][i] = Piece(Piece::Type::PAWN, Piece::Type::NONE);
//
//		// a7-z7 (black pawns)
//		_squares[6][i] = Piece(Piece::Type::NONE, Piece::Type::PAWN);
//	}

	// Setup for sako-check
	b["a1"] = Piece(Piece::Type::ROOK, Piece::Type::NONE);
	b["c1"] = Piece(Piece::Type::BISHOP, Piece::Type::NONE);
	b["d1"] = Piece(Piece::Type::QUEEN, Piece::Type::NONE);
	b["e1"] = Piece(Piece::Type::KING, Piece::Type::NONE);
	b["f1"] = Piece(Piece::Type::BISHOP, Piece::Type::NONE);
	b["h1"] = Piece(Piece::Type::ROOK, Piece::Type::NONE);
	b["a8"] = Piece(Piece::Type::NONE, Piece::Type::ROOK);
	b["b8"] = Piece(Piece::Type::NONE, Piece::Type::KNIGHT);
	b["c8"] = Piece(Piece::Type::NONE, Piece::Type::BISHOP);
	b["d8"] = Piece(Piece::Type::NONE, Piece::Type::QUEEN);
	b["f8"] = Piece(Piece::Type::NONE, Piece::Type::BISHOP);
	b["g8"] = Piece(Piece::Type::NONE, Piece::Type::ROOK);

	b["b4"] = Piece(Piece::Type::NONE, Piece::Type::KING);
	b["e4"] = Piece(Piece::Type::KNIGHT, Piece::Type::KNIGHT);
	b["c3"] = Piece(Piece::Type::KNIGHT, Piece::Type::PAWN);

	for (unsigned i = 0; i < 8; i++) {
		// a2-z2 (white pawns)
		_squares[1][i] = Piece(Piece::Type::PAWN, Piece::Type::NONE);

		// a7-z7 (black pawns)
		if (i != 3) {
			_squares[6][i] = Piece(Piece::Type::NONE, Piece::Type::PAWN);
		}
	}
}

const Piece& Board::GetPiece(const BoardPosition& position) const {
	return _squares[position.GetRow()][position.GetColumn()];
}

Piece& Board::GetPiece(const BoardPosition& position) {
	return _squares[position.GetRow()][position.GetColumn()];
}

const Piece& Board::operator[](const BoardPosition& position) const {
	return _squares[position.GetRow()][position.GetColumn()];
}

Piece& Board::operator[](const BoardPosition& position) {
	return _squares[position.GetRow()][position.GetColumn()];
}

std::vector<Move> Board::GetAllPossibleMoves() const {
	return _GetAllPossibleMoves(true);
}

std::vector<BoardPosition> Board::CalculatePossibleMoves(const BoardPosition &piece, Piece::Color playerColor, const GameMoveData& moveData) const {
	return CalculatePossibleMoves(piece, GetPiece(piece), playerColor, moveData);
}

std::vector<BoardPosition> Board::CalculatePossibleMoves(const BoardPosition& origin, const Piece& piece, Piece::Color playerColor, const GameMoveData& moveData) const {
	std::vector<BoardPosition> vec;
	Piece::Type type = piece.GetTypeOfColor(playerColor);

	switch (type) {
		case Piece::Type::NONE: break;
		case Piece::Type::PAWN:
			_AddPawnMoves(origin, piece, playerColor, vec, moveData);
			break;
		case Piece::Type::ROOK:
			_AddStraightMoves(origin, piece, playerColor, vec);
			break;
		case Piece::Type::KNIGHT:
			_AddKnightMoves(origin, piece, playerColor, vec);
			break;
		case Piece::Type::BISHOP:
			_AddDiagonalMoves(origin, piece, playerColor, vec);
			break;
		case Piece::Type::QUEEN:
			_AddStraightMoves(origin, piece, playerColor, vec);
			_AddDiagonalMoves(origin, piece, playerColor, vec);
			break;
		case Piece::Type::KING:
			_AddKingMoves(origin, piece, playerColor, vec, moveData);
			break;
	}

	return vec;
}

void Board::_AddPawnMoves(const BoardPosition &position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition> &vec, const GameMoveData& moveData) const {
	int startingRow = playerColor == Piece::Color::WHITE ? 0 : 7;
	int forward = playerColor == Piece::Color::WHITE ? 1 : -1;

	// cannot make a union if we are a union
	if (piece.GetColor() != Piece::Color::UNION) {
		BoardPosition diagLeft = { position.GetRow() + forward, position.GetColumn() - 1 };
		BoardPosition diagRight = { position.GetRow() + forward, position.GetColumn() + 1 };

		if (0 <= diagLeft.GetColumn()) {
			Piece::Color color = GetPiece(diagLeft).GetColor();
			if (!(color == playerColor || color == Piece::Color::EMPTY)) {
				vec.push_back(diagLeft);
			}
		}

		if (diagRight.GetColumn() < 8) {
			Piece::Color color = GetPiece(diagRight).GetColor();
			if (!(color == playerColor || color == Piece::Color::EMPTY)) {
				vec.push_back(diagRight);
			}
		}

		// handle en passant cases
		if (moveData.en_passant_position.IsValid()) {
			if (moveData.en_passant_position.GetRow() == position.GetRow() &&
					abs(moveData.en_passant_position.GetColumn() - position.GetColumn()) == 1) {

				vec.push_back({ moveData.en_passant_position.GetRow() + forward,
					moveData.en_passant_position.GetColumn() });
			}
		}
	}

	BoardPosition posForward = { position.GetRow() + forward, position.GetColumn() };
	Piece::Color forwardColor = GetPiece(posForward).GetColor();
	if (forwardColor == Piece::Color::EMPTY) {
		vec.push_back(posForward);

		bool allowDoubleForward = position.GetRow() == startingRow || position.GetRow() == startingRow + forward;
		if (allowDoubleForward) {
			BoardPosition posDoubleForward = { position.GetRow() + 2 * forward, position.GetColumn() };
			Piece::Color doubleForwardColor = GetPiece(posDoubleForward).GetColor();
			if (doubleForwardColor == Piece::Color::EMPTY) {
				vec.push_back(posDoubleForward);
			}
		}
	}
}

void Board::_AddKnightMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const {
	std::array<BoardPosition, 8> dps = { {
			{ -2, -1 }, { -2, 1 }, { -1, -2 }, { -1, 2 },
			{  2, -1 }, {  2, 1 }, {  1, -2 }, {  1, 2 }
	} };

	for (const auto& dp : dps) {
		int r = position.GetRow() + dp.GetRow();
		int c = position.GetColumn() + dp.GetColumn();

		if (r > 7 || c > 7 || r < 0 || c < 0) {
			continue;
		}

		BoardPosition pos { r, c };
		Piece::Color pieceColor = GetPiece(pos).GetColor();

		if (piece.GetColor() == Piece::Color::UNION && pieceColor != Piece::Color::EMPTY) {
			continue;
		}

		if (pieceColor != playerColor) {
			vec.push_back(pos);
		}
	}
}

void Board::_AddKingMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, const GameMoveData& moveData) const {
	for (int dc = -1; dc <= 1; dc++) {
		for (int dr = -1; dr <= 1; dr++) {
			int r = position.GetRow() + dr;
			int c = position.GetColumn() + dc;

			if (r > 7 || c > 7 || r < 0 || c < 0) {
				continue;
			}

			BoardPosition pos { r, c };
			Piece::Color pieceColor = GetPiece(pos).GetColor();

			if (pieceColor == Piece::Color::EMPTY) {
				vec.push_back(pos);
			}
		}
	}

	// TODO: check if king is under sako, or moves through or towards a
	// protected square
	const auto& moves = _GetAllPossibleMoves(false);

	switch (playerColor) {
		case Piece::Color::WHITE:
			if (moveData.can_white_castle_king_side &&
					GetPiece({ 0, 5 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 0, 6 }).GetColor() == Piece::Color::EMPTY) {
				vec.push_back({ 0, 6 });
			}

			if (moveData.can_white_castle_queen_side &&
					GetPiece({ 0, 3 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 0, 2 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 0, 1 }).GetColor() == Piece::Color::EMPTY) {
				vec.push_back({ 0, 2 });
			}

			break;
		case Piece::Color::BLACK:
			if (moveData.can_black_castle_king_side &&
					GetPiece({ 7, 5 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 7, 6 }).GetColor() == Piece::Color::EMPTY) {
				vec.push_back({ 7, 6 });
			}

			if (moveData.can_black_castle_queen_side &&
					GetPiece({ 7, 3 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 7, 2 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 7, 1 }).GetColor() == Piece::Color::EMPTY) {
				vec.push_back({ 7, 2 });
			}

			break;
		default:
			abort();
	}
}

void Board::_AddDiagonalMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const {
	std::array<BoardPosition, 4> dps = { { { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } } };
	_AddMoves(position, piece, playerColor, vec, dps);
}

void Board::_AddStraightMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const {
	std::array<BoardPosition, 4> dps = { { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } } };
	_AddMoves(position, piece, playerColor, vec, dps);
}

void Board::_AddMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, std::array<BoardPosition, 4> dps) const {
	for (const auto& dp : dps) {
		for (int i = 1; i < 8; i++) {
			if (i == 0) {
				continue;
			}

			int r = position.GetRow() + dp.GetRow() * i;
			int c = position.GetColumn() + dp.GetColumn() * i;

			if (r > 7 || c > 7 || r < 0 || c < 0) {
				break;
			}

			BoardPosition pos { r, c };
			Piece::Color pieceColor = GetPiece(pos).GetColor();

			if (piece.GetColor() == Piece::Color::UNION && pieceColor != Piece::Color::EMPTY) {
				break;
			}

			if (pieceColor == playerColor) {
				break;
			}

			vec.push_back(pos);

			if (pieceColor != Piece::Color::EMPTY) {
				break;
			}
		}
	}
}

std::vector<Move> Board::_GetAllPossibleMoves(bool checkSako) const {

}

void Board::_AddAllPossibleMoves(const BoardPosition& position, const Piece& piece, std::vector<Move>& moves) {

}

}
