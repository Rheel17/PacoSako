/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "Game.h"

#include <windows.h>

#include "Window.h"

namespace ps {

wxIMPLEMENT_APP(Game);

Game::Game() {
	_board = std::make_unique<Board>();
}

bool Game::OnInit() {
	wxInitAllImageHandlers();

	_window = new Window(*this);
	_window->Show();
	_window->Maximize(true);
	return true;
}

const Board& Game::GetBoard() const {
	return *_board;
}

const GameMoveData& Game::GetMoveData() const {
	return _move_data;
}

Piece::Color Game::GetPlayerColor() const {
	return _player_color;
}

void Game::SwitchPlayerColor() {
	if (_player_color == Piece::Color::WHITE) {
		_player_color = Piece::Color::BLACK;
	} else {
		_player_color = Piece::Color::WHITE;
	}
}

void Game::MakeMove(const Move& move) {
	_move_data.en_passant_position = { -1, -1 };
	move.PerformOn(*_board);

	const auto& positions = move.GetPositions();
	if (positions.size() >= 2) {
		const BoardPosition& prev = *(positions.end() - 2);
		const BoardPosition& next = positions.back();

		if (abs(next.GetRow() - prev.GetRow()) == 2 &&
				_board->GetPiece(next).GetTypeOfColor(_player_color) == Piece::Type::PAWN) {

			_move_data.en_passant_position = next;
		}
	}

	if (positions.size() == 2) {
		if (_board->GetPiece(positions.back()).GetTypeOfColor(_player_color) == Piece::Type::KING) {
			// the player just moved the king
			switch (_player_color) {
				case Piece::Color::WHITE:
					_move_data.can_white_castle_king_side = false;
					_move_data.can_white_castle_queen_side = false;
					break;
				case Piece::Color::BLACK:
					_move_data.can_black_castle_king_side = false;
					_move_data.can_black_castle_queen_side = false;
					break;
				default:
					abort();
			}
		}
	}

	// check if the rooks moved
	int row = _player_color == Piece::Color::WHITE ? 0 : 7;
	bool *kingSide = _player_color == Piece::Color::WHITE ?
			&_move_data.can_white_castle_king_side : &_move_data.can_black_castle_king_side;
	bool *queenSide = _player_color == Piece::Color::WHITE ?
			&_move_data.can_white_castle_queen_side : &_move_data.can_black_castle_queen_side;

	if (*kingSide || *queenSide) {
		for (const auto& position : positions) {
			if (position == BoardPosition { row, 7 }) {
				*kingSide = false;
			} else if (position == BoardPosition { row, 0 }) {
				*queenSide = false;
			}
		}
	}

	// next player
	SwitchPlayerColor();
}

}
