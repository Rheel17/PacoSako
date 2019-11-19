/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "Board.h"

#include <algorithm>
#include <sstream>

namespace ps {

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

std::string Board::GetPsFEN() const {
	std::stringstream ss;

	for (int r = 7; r >= 0; r--) {
		if (r < 7) {
			ss << '/';
		}

		int empty = 0;

		for (int c = 0; c < 8; c++) {
			const Piece& piece = GetPiece({ r, c });

			if (piece.GetColor() == Piece::Color::EMPTY) {
				empty++;
				continue;
			} else if (empty > 0) {
				ss << empty;
				empty = 0;
			}

			switch (piece.GetColor()) {
				case Piece::Color::WHITE:
					ss << getTypeCharWhite(piece.GetWhiteType());
					break;
				case Piece::Color::BLACK:
					ss << getTypeCharBlack(piece.GetBlackType());
					break;
				case Piece::Color::UNION:
					ss << 'U';
					ss << getTypeCharWhite(piece.GetWhiteType());
					ss << getTypeCharBlack(piece.GetBlackType());
					break;
				case Piece::Color::EMPTY:
					break;
			}
		}

		if (empty > 0) {
			ss << empty;
		}
	}

	return ss.str();
}

void Board::SetPsFEN(const std::string& fen) {
	const char *arr = fen.c_str();

	for (int r = 7; r >= 0; r--) {
		int c = 0;

		while (c < 8) {
			if ('1' <= *arr && *arr <= '8') {
				int count = *arr - '0';

				for (int i = 0; i < count; i++) {
					_squares[r][c] = Piece();
					c++;
				}

				arr++;
				continue;
			}

			if (*arr == 'U') {
				arr++; auto whiteType = getTypeWhite(*arr);
				arr++; auto blackType = getTypeBlack(*arr);
				arr++;

				_squares[r][c] = Piece(whiteType, blackType);
				c++;
				continue;
			}

			auto whiteType = getTypeWhite(*arr);
			auto blackType = getTypeBlack(*arr);

			_squares[r][c] = Piece(whiteType, blackType);
			c++;
			arr++;
		}

		arr++;
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

std::vector<Move> Board::GetAllPossibleMoves(Piece::Color color, const GameMoveData& moveData) const {
	return _GetAllPossibleMoves(true, color, moveData);
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

	bool hasDestinations = false;
	std::vector<BoardPosition> destinations;

	const auto checkNotProtected = [&hasDestinations, &destinations, playerColor, moveData, this]
									(const BoardPosition& bp) {
		Piece::Color otherColor = opposite(playerColor);
		auto moves = _GetAllPossibleMoves(false, otherColor, moveData);

		if (!hasDestinations) {
			for (const auto& move : moves) {
				const auto& positions = move.GetPositions();

				if (GetPiece(positions.front()).GetColor() == Piece::Color::UNION) {
					continue;
				}

				destinations.push_back(positions.back());
			}

			hasDestinations = true;
		}

		return std::find(destinations.begin(), destinations.end(), bp) == destinations.end();
	};

	switch (playerColor) {
		case Piece::Color::WHITE:
			if (moveData.can_white_castle_king_side &&
					GetPiece({ 0, 5 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 0, 6 }).GetColor() == Piece::Color::EMPTY &&
					checkNotProtected({ 0, 4 }) &&
					checkNotProtected({ 0, 5 }) &&
					checkNotProtected({ 0, 6 })) {
				vec.push_back({ 0, 6 });
			}

			if (moveData.can_white_castle_queen_side &&
					GetPiece({ 0, 3 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 0, 2 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 0, 1 }).GetColor() == Piece::Color::EMPTY &&
					checkNotProtected({ 0, 4 }) &&
					checkNotProtected({ 0, 3 }) &&
					checkNotProtected({ 0, 2 })) {
				vec.push_back({ 0, 2 });
			}

			break;
		case Piece::Color::BLACK:
			if (moveData.can_black_castle_king_side &&
					GetPiece({ 7, 5 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 7, 6 }).GetColor() == Piece::Color::EMPTY &&
					checkNotProtected({ 7, 4 }) &&
					checkNotProtected({ 7, 5 }) &&
					checkNotProtected({ 7, 6 })) {
				vec.push_back({ 7, 6 });
			}

			if (moveData.can_black_castle_queen_side &&
					GetPiece({ 7, 3 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 7, 2 }).GetColor() == Piece::Color::EMPTY &&
					GetPiece({ 7, 1 }).GetColor() == Piece::Color::EMPTY &&
					checkNotProtected({ 7, 4 }) &&
					checkNotProtected({ 7, 3 }) &&
					checkNotProtected({ 7, 2 })) {
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

std::vector<Move> Board::_GetAllPossibleMoves(bool checkSako, Piece::Color color, const GameMoveData& moveData) const {
	std::vector<Move> moves;
	std::vector<Move> temp;
	Board dummy;
	Piece::Color otherColor = opposite(color);

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			BoardPosition position { r, c };
			const Piece& piece = GetPiece(position);

			if (piece.GetTypeOfColor(color) != Piece::Type::NONE) {
				if (checkSako) {
					temp.clear();
					_AddAllPossibleMoves(position, piece, color, temp, moveData);

					for (const Move& move : temp) {
						dummy = *this;
						move.PerformOn(dummy);

						auto dummyMoves = _GetAllPossibleMoves(false, otherColor, moveData);
						bool sako = false;

						for (const auto& dummyMove : dummyMoves) {
							const auto& positions = dummyMove.GetPositions();
							if (GetPiece(positions.back()).GetTypeOfColor(color) == Piece::Type::KING) {
								sako = true;
								break;
							}
						}

						if (!sako) {
							moves.push_back(std::move(move));
						}
					}
				} else {
					_AddAllPossibleMoves(position, piece, color, moves, moveData);
				}
			}
		}
	}

	return moves;
}

void Board::_AddAllPossibleMoves(const BoardPosition& position, const Piece& piece, Piece::Color color, std::vector<Move>& moves, const GameMoveData& moveData) const {
	auto initialMoves = CalculatePossibleMoves(position, piece, color, moveData);
	for (const auto& move : initialMoves) {
		if (piece.GetColor() == Piece::Color::UNION ||
				GetPiece(move).GetColor() == Piece::Color::EMPTY) {
			Move& m = moves.emplace_back(position);
			m.AddPosition(move);
		} else {
			Move prefix(position);
			prefix.AddPosition(move);
			_AddAllPossibleChainMoves(std::move(prefix), piece, moves, moveData);
		}
	}
}

void Board::_AddAllPossibleChainMoves(Move prefix, const Piece& piece, std::vector<Move>& moves, const GameMoveData& moveData) const {
	const BoardPosition& position = prefix.GetPositions().back();

	Board dummy(*this);
	Piece movingPiece = dummy[position].MakeUnionWith(piece);
	auto pieceMoves = dummy.CalculatePossibleMoves(position, movingPiece, movingPiece.GetColor(), moveData);

	for (const auto& move : pieceMoves) {
		if (dummy[move].GetColor() != Piece::Color::UNION) {
			Move& m = moves.emplace_back(prefix);
			m.AddPosition(move);
		} else {
			Move newPrefix(prefix);
			newPrefix.AddPosition(move);
			dummy._AddAllPossibleChainMoves(newPrefix, movingPiece, moves, moveData);
		}
	}
}

}
