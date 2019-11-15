/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "Board.h"

namespace ps {

BoardPosition::BoardPosition(int row, int column) :
		_row(row), _column(column) {}

BoardPosition::BoardPosition(const char* name) :
		_row(name[1] - '1'), _column(name[0] - 'a') {}

BoardPosition::BoardPosition(std::string name) :
		_row(name[1] - '1'), _column(name[0] - 'a') {}

int BoardPosition::GetRow() const {
	return _row;
}

int BoardPosition::GetColumn() const {
	return _column;
}

std::string BoardPosition::GetName() const {
	if (IsValid()) {
		const char name[3] = { char(_column + 'a'), char(_row + '1'), 0 };
		return std::string(name);
	} else {
		return "invalid";
	}
}

bool BoardPosition::IsValid() const {
	return _row >= 0 && _row < 8 && _column >= 0 && _column < 8;
}

bool BoardPosition::operator==(const BoardPosition& bp) const {
	return _row == bp._row && _column == bp._column;
}

bool BoardPosition::operator!=(const BoardPosition& bp) const {
	return _row != bp._row || _column != bp._column;
}

Board::Board() {
	Board& b = *this;

	// white pieces
	b["a1"] = Piece(Piece::Type::ROOK, Piece::Type::NONE);
	b["b1"] = Piece(Piece::Type::KNIGHT, Piece::Type::NONE);
	b["c1"] = Piece(Piece::Type::BISHOP, Piece::Type::NONE);
	b["d1"] = Piece(Piece::Type::QUEEN, Piece::Type::NONE);
	b["e1"] = Piece(Piece::Type::KING, Piece::Type::NONE);
	b["f1"] = Piece(Piece::Type::BISHOP, Piece::Type::NONE);
	b["g1"] = Piece(Piece::Type::KNIGHT, Piece::Type::NONE);
	b["h1"] = Piece(Piece::Type::ROOK, Piece::Type::NONE);

	// black pieces
	b["a8"] = Piece(Piece::Type::NONE, Piece::Type::ROOK);
	b["b8"] = Piece(Piece::Type::NONE, Piece::Type::KNIGHT);
	b["c8"] = Piece(Piece::Type::NONE, Piece::Type::BISHOP);
	b["d8"] = Piece(Piece::Type::NONE, Piece::Type::QUEEN);
	b["e8"] = Piece(Piece::Type::NONE, Piece::Type::KING);
	b["f8"] = Piece(Piece::Type::NONE, Piece::Type::BISHOP);
	b["g8"] = Piece(Piece::Type::NONE, Piece::Type::KNIGHT);
	b["h8"] = Piece(Piece::Type::NONE, Piece::Type::ROOK);

	for (unsigned i = 0; i < 8; i++) {
		// a2-z2 (white pawns)
		_squares[1][i] = Piece(Piece::Type::PAWN, Piece::Type::NONE);

		// a7-z7 (black pawns)
		_squares[6][i] = Piece(Piece::Type::NONE, Piece::Type::PAWN);
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

std::vector<BoardPosition> Board::CalculatePossibleMoves(const BoardPosition &piece, Piece::Color playerColor) const {
	std::vector<BoardPosition> vec;
	Piece::Type type = GetPiece(piece).GetTypeOfColor(playerColor);

	switch (type) {
		case Piece::Type::NONE: break;
		case Piece::Type::PAWN:
			_AddPawnMoves(piece, playerColor, vec);
			break;
		case Piece::Type::ROOK:
			_AddStraightMoves(piece, playerColor, vec);
			break;
		case Piece::Type::KNIGHT:
			_AddKnightMoves(piece, playerColor, vec);
			break;
		case Piece::Type::BISHOP:
			_AddDiagonalMoves(piece, playerColor, vec);
			break;
		case Piece::Type::QUEEN:
			_AddStraightMoves(piece, playerColor, vec);
			_AddDiagonalMoves(piece, playerColor, vec);
			break;
		case Piece::Type::KING:
			break;
	}

	return vec;
}

void Board::_AddPawnMoves(const BoardPosition &position, Piece::Color playerColor, std::vector<BoardPosition> &vec) const {
	int startingRow = playerColor == Piece::Color::WHITE ? 0 : 7;
	int forward = playerColor == Piece::Color::WHITE ? 1 : -1;

	// cannot make a union if we are a union
	if (GetPiece(position).GetColor() != Piece::Color::UNION) {
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

void Board::_AddKnightMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec) const {
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

		// TODO: cannot make a union if we are a union

		if (pieceColor != playerColor) {
			vec.push_back(pos);
		}
	}
}

void Board::_AddDiagonalMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec) const {
	std::array<BoardPosition, 4> dps = { { { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } } };
	_AddMoves(position, playerColor, vec, dps);
}

void Board::_AddStraightMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec) const {
	std::array<BoardPosition, 4> dps = { { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } } };
	_AddMoves(position, playerColor, vec, dps);
}

void Board::_AddMoves(const BoardPosition& position, Piece::Color playerColor, std::vector<BoardPosition>& vec, std::array<BoardPosition, 4> dps) const {
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


			// TODO: cannot make a union if we are a union

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

}
