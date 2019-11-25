/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "Game.h"

#include <cassert>

#include "PlayerHuman.h"
#include "Window.h"

namespace ps {

Game::Game() {
	_board = std::make_unique<Board>();
}

Game::~Game() {
	if (!_game_thread) {
		return;
	}

	_game_thread_close.store(true);
	_game_thread->join();
}

Game::Game(const Game& game) noexcept :
	_board(std::make_unique<Board>(*game._board)),
	_current_player(std::move(game._current_player)),
	_move_data(std::move(game._move_data)),
	_fify_move_rule_count(std::move(game._fify_move_rule_count)),
	_current_move(std::move(game._current_move)) {
}

Game& Game::operator=(const Game& game) noexcept {
	_board = std::make_unique<Board>(*game._board);
	_current_player = std::move(game._current_player);
	_move_data = std::move(game._move_data);
	_fify_move_rule_count = std::move(game._fify_move_rule_count);
	_current_move = std::move(game._current_move);

	return *this;
}

void Game::SetPlayers(Player *white, Player *black) {
	_player_white = std::unique_ptr<Player>(white);
	_player_black = std::unique_ptr<Player>(black);
}

bool Game::SetState(const std::string &psFEN) {
	// Read the board
	if (!_board->SetPsFEN(psFEN)) {
		return false;
	}

	_move_data = GameMoveData();

	size_t boardEnd = psFEN.find(' ');
	const char *arr = psFEN.c_str() + boardEnd + 1;

	// read the current player
	if (*arr == 'w') {
		_current_player = Piece::Color::WHITE;
	} else {
		_current_player = Piece::Color::BLACK;
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

	return true;
}

void Game::StartThread(Window *window) {
	assert(_player_white && _player_black);

	_game_thread = std::unique_ptr<std::thread>(new std::thread([window](Game *game){
		game->_Loop(window);
	}, this));
}

const Board& Game::GetBoard() const {
	return *_board;
}

const GameMoveData& Game::GetMoveData() const {
	return _move_data;
}

Piece::Color Game::GetPlayerColor() const {
	return _current_player;
}

void Game::SwitchPlayerColor() {
	if (_current_player == Piece::Color::WHITE) {
		_current_player = Piece::Color::BLACK;
	} else {
		_current_player = Piece::Color::WHITE;
	}
}

void Game::MakeMove(const Move& move) {
	const auto& positions = move.GetPositions();
	if (positions.empty()) {
		return;
	}

	Piece movingPiece = _board->GetPiece(positions.front());

	// count the number of pawns before the move for pawn promotion detection
	int whitePawnCount = 0;
	int blackPawnCount = 0;
	for (const auto& position : positions) {
		whitePawnCount += _board->GetPiece(position).GetWhiteType() == Piece::Type::PAWN;
		blackPawnCount += _board->GetPiece(position).GetBlackType() == Piece::Type::PAWN;
	}

	_move_data.en_passant_position = { -1, -1 };
	move.PerformOn(*_board);

	if (positions.size() >= 2) {
		const BoardPosition& prev = *(positions.end() - 2);
		const BoardPosition& next = positions.back();

		if (abs(next.GetRow() - prev.GetRow()) == 2 &&
				_board->GetPiece(next).GetTypeOfColor(_current_player) == Piece::Type::PAWN) {

			_move_data.en_passant_position = next;
		}
	}

	if (positions.size() == 2) {
		if (_board->GetPiece(positions.back()).GetTypeOfColor(_current_player) == Piece::Type::KING) {
			// the player just moved the king
			switch (_current_player) {
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
	int row = _current_player == Piece::Color::WHITE ? 0 : 7;
	bool *kingSide = _current_player == Piece::Color::WHITE ?
			&_move_data.can_white_castle_king_side : &_move_data.can_black_castle_king_side;
	bool *queenSide = _current_player == Piece::Color::WHITE ?
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

	// check for pawn promotion
	for (const auto& position : positions) {
		whitePawnCount -= _board->GetPiece(position).GetWhiteType() == Piece::Type::PAWN;
		blackPawnCount -= _board->GetPiece(position).GetBlackType() == Piece::Type::PAWN;
	}

	// check if a non-reversible move was made (only creating a union or pawn
	// promotion are non-reversible moves in paco sako.
	if (whitePawnCount != 0 || blackPawnCount || (positions.size() == 2 && movingPiece.GetColor() != Piece::Color::UNION &&
			_board->GetPiece(positions.back()).GetColor() == Piece::Color::UNION)) {

		_fify_move_rule_count = 0;
	} else {
		_fify_move_rule_count++;
	}

	// next player
	SwitchPlayerColor();

	if (_current_player == Piece::Color::WHITE) {
		_current_move++;
	}
}

std::string Game::GetPsFEN() const {
	std::string boardFEN = _board->GetPsFEN();
	boardFEN += ' ';

	if (_current_player == Piece::Color::WHITE) {
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

void Game::_Loop(Window *window) {
	bool isWhitePlayerHuman = (bool) dynamic_cast<PlayerHuman *>(_player_white.get());
	bool isBlackPlayerHuman = (bool) dynamic_cast<PlayerHuman *>(_player_black.get());

	while (!_game_thread_close) {
		std::cout << "white: " << std::flush;
		Move whiteMove = _player_white->MakeMove(*_board, _move_data, _game_thread_close);

		if (_game_thread_close) {
			break;
		}

		std::cout << whiteMove << std::endl;
		MakeMove(whiteMove);

		window->GetEventHandler()->CallAfter([window, whiteMove, isWhitePlayerHuman]() {
			window->FinishMove(whiteMove, isWhitePlayerHuman);
		});

		std::cout << "black: " << std::flush;
		Move blackMove = _player_black->MakeMove(*_board, _move_data, _game_thread_close);

		if (_game_thread_close) {
			break;
		}

		std::cout << blackMove << std::endl;
		MakeMove(blackMove);

		window->GetEventHandler()->CallAfter([window, blackMove, isBlackPlayerHuman]() {
			window->FinishMove(blackMove, isBlackPlayerHuman);
		});
	}
}

}
