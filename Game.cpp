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
	SetState("r3k2r/ppp2ppp/8/1B1UPqpbB1/2UNpUPn4/3Q1N1n/PPP2UPbPP/3R1R1K b kw - 5 12");
	std::cout << GetPsFEN() << std::endl;
}

void Game::SetState(const std::string &psFEN) {
	// Read the board
	_board->SetPsFEN(psFEN);
	_move_data = GameMoveData();

	size_t boardEnd = psFEN.find(' ');
	const char *arr = psFEN.c_str() + boardEnd + 1;

	// read the current player
	if (*arr == 'w') {
		_player_color = Piece::Color::WHITE;
	} else {
		_player_color = Piece::Color::BLACK;
	}

	// read the castling possibilities
	arr += 2;

	if (*arr == '-') {
		arr += 2;
	} else {
		if (*arr == 'K') {
			_move_data.can_white_castle_king_side = true;
			arr++;
		}

		if (*arr == 'Q') {
			_move_data.can_white_castle_queen_side = true;
			arr++;
		}

		if (*arr == 'k') {
			_move_data.can_black_castle_king_side = true;
			arr++;
		}

		if (*arr == 'q') {
			_move_data.can_black_castle_queen_side = true;
			arr++;
		}

		arr++;
	}

	// read the en passant position
	if (*arr == '-') {
		arr += 2;
	} else {
		int ep[2] = { *arr, *(arr + 1) };
		ep[0] -= 'a';
		ep[1] -= '0';

		if (ep[1] <= 3) {
			ep[1]++;
		} else {
			ep[1]--;
		}

		_move_data.en_passant_position = { ep[0], ep[1] };
		arr += 3;
	}

	// read the move counts
	char *end;
	_fify_move_rule_count = strtol(arr, &end, 10);
	arr = end + 1;
	_current_move = strtol(arr, &end, 10);
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
	const auto& positions = move.GetPositions();
	if (positions.empty()) {
		return;
	}

	Piece movingPiece = _board->GetPiece(positions.front());

	_move_data.en_passant_position = { -1, -1 };
	move.PerformOn(*_board);

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

	// check if a non-reversible move was made (only creating a union or pawn
	// promotion are non-reversible moves in paco sako.
	// TODO: check for pawn promotion
	if (positions.size() == 2 && movingPiece.GetColor() != Piece::Color::UNION &&
			_board->GetPiece(positions.back()).GetColor() == Piece::Color::UNION) {

		_fify_move_rule_count = 0;
	} else {
		_fify_move_rule_count++;
	}

	// next player
	SwitchPlayerColor();

	if (_player_color == Piece::Color::WHITE) {
		_current_move++;
	}

	std::cout << GetPsFEN() << std::endl;
}

std::string Game::GetPsFEN() const {
	std::string boardFEN = _board->GetPsFEN();
	boardFEN += ' ';

	if (_player_color == Piece::Color::WHITE) {
		boardFEN += 'w';
	} else {
		boardFEN += 'b';
	}

	boardFEN += ' ';

	if (_move_data.can_white_castle_king_side || _move_data.can_white_castle_queen_side ||
			_move_data.can_black_castle_king_side || _move_data.can_black_castle_queen_side) {

		if (_move_data.can_white_castle_king_side) boardFEN += 'K';
		if (_move_data.can_white_castle_queen_side) boardFEN += 'Q';
		if (_move_data.can_black_castle_king_side) boardFEN += 'k';
		if (_move_data.can_black_castle_queen_side) boardFEN += 'w';

		boardFEN += ' ';
	} else {
		boardFEN += "- ";
	}

	BoardPosition ep = _move_data.en_passant_position;

	if (ep.IsValid()) {
		if (ep.GetRow() <= 3) {
			ep = { ep.GetRow() - 1, ep.GetColumn() };
		} else {
			ep = { ep.GetRow() + 1, ep.GetColumn() };
		}

		boardFEN += ep.GetName();
		boardFEN += ' ';
	} else {
		boardFEN += "- ";
	}

	boardFEN += std::to_string(_fify_move_rule_count);
	boardFEN += ' ';
	boardFEN += std::to_string(_current_move);

	return boardFEN;
}

}
