/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef BOARD_H_
#define BOARD_H_

#include "GameMoveData.h"
#include "Piece.h"
#include "Move.h"

#include <array>
#include <vector>

namespace ps {

class Board {

public:
	Board();

	std::string GetPsFEN() const;
	void SetPsFEN(const std::string& fen);

	const Piece& GetPiece(const BoardPosition& position) const;
	Piece& GetPiece(const BoardPosition& position);

	const Piece& operator[](const BoardPosition& position) const;
	Piece& operator[](const BoardPosition& position);

	std::vector<Move> GetAllPossibleMoves(Piece::Color color, const GameMoveData& moveData) const;
	std::vector<BoardPosition> CalculatePossibleMoves(const BoardPosition& piece, Piece::Color playerColor, const GameMoveData& moveData, bool checkSako = true) const;
	std::vector<BoardPosition> CalculatePossibleMoves(const BoardPosition& origin, const Piece& piece, Piece::Color playerColor, const GameMoveData& moveData, bool checkSako = true) const;

private:
	void _AddPawnMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, const GameMoveData& moveData) const;
	void _AddKnightMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddKingMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, const GameMoveData& moveData, bool checkSako) const;
	void _AddDiagonalMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddStraightMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec) const;
	void _AddMoves(const BoardPosition& position, const Piece& piece, Piece::Color playerColor, std::vector<BoardPosition>& vec, std::array<BoardPosition, 4> dps) const;

	std::vector<Move> _GetAllPossibleMoves(bool checkSako, Piece::Color color, const GameMoveData& moveData) const;
	void _AddAllPossibleMoves(const BoardPosition& position, const Piece& piece, Piece::Color color, std::vector<Move>& moves, const GameMoveData& moveData) const;
	void _AddAllPossibleChainMoves(Move prefix, const Piece& piece, std::vector<Move>& moves, const GameMoveData& moveData) const;

	Piece _squares[8][8] {};

};

}

#endif
