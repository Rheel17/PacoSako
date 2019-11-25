/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef GAME_H_
#define GAME_H_

#include <thread>
#include <memory>
#include <mutex>

#include "Player.h"
#include "Board.h"
#include "GameMoveData.h"

namespace ps {

class Game {

public:
	Game();

	Game(const Game& game) noexcept;
	Game& operator=(const Game& game) noexcept;

	Game(Game&&) = default;
	Game& operator=(Game&&) = default;

	void SetPlayers();

	void Loop();

	bool SetState(const std::string& psFEN);

	const Board& GetBoard() const;

	const GameMoveData& GetMoveData() const;

	Piece::Color GetPlayerColor() const;

	void SwitchPlayerColor();

	void MakeMove(const Move& move);

	std::string GetPsFEN() const;

private:
	std::unique_ptr<Board> _board;

	std::unique_ptr<Player> _player_white;
	std::unique_ptr<Player> _player_black;
	Piece::Color _current_player = Piece::Color::WHITE;

	GameMoveData _move_data;
	int _fify_move_rule_count = 0;
	int _current_move = 1;

	std::unique_ptr<std::thread> _game_thread;

};

}

#endif
