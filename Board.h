/*
 * Copyright © 2020 Levi van Rheenen. All rights reserved.
 */
#ifndef BOARD_H_
#define BOARD_H_

#include "GameMoveData.h"
#include "Piece.h"
#include "Move.h"

#include <array>
#include <vector>
#include <unordered_set>

namespace ps {

struct ChainHashKey;

class Board {

public:
	Board();

	std::string GetPsFEN() const;
	bool SetPsFEN(const std::string& fen);

	const Piece& GetPiece(const BoardPosition& position) const;
	Piece& GetPiece(const BoardPosition& position);

	const Piece& operator[](const BoardPosition& position) const;
	Piece& operator[](const BoardPosition& position);

	bool operator==(const Board& board) const;

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
	void _AddAllPossibleMoves(const BoardPosition& position, const Piece& piece, Piece::Color color, std::vector<Move>& moves, const GameMoveData& moveData, bool checkSako) const;
	void _AddAllPossibleChainMoves(Move prefix, bool lastEnPassant, const Piece& piece, std::vector<Move>& moves, const GameMoveData& moveData, std::unordered_set<ChainHashKey>& seenConfigs) const;

	std::array<std::array<Piece, 8>, 8> _squares {};

};

struct ChainHashKey {
	const Board board;
	const Piece moving_piece;
	const BoardPosition piece_origin;
	const BoardPosition ep_dest;

	// not included in the hash; only for backtracking purposes.
	const Move prefix;

	bool operator==(const ChainHashKey& key) const;
};

}

namespace std {

template<>
struct hash<ps::Board> {
	size_t operator()(const ps::Board& board) const {
		size_t result = 29;
		for (int r = 0; r < 8; r++) {
			for (int c = 0; c < 8; c++) {
				result = result * 31 + hash<ps::Piece>()(board[{ r, c }]);
			}
		}
		return result;
	}
};

template<>
struct hash<ps::ChainHashKey> {
	size_t operator()(const ps::ChainHashKey& key) const {
		size_t result = 29;
		result = 17 * result + hash<ps::Board>()(key.board);
		result = 17 * result + hash<ps::Piece>()(key.moving_piece);
		result = 17 * result + hash<ps::BoardPosition>()(key.piece_origin);
		result = 17 * result + hash<ps::BoardPosition>()(key.ep_dest);
		return result;
	}
};

}

#endif
