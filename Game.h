/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef GAME_H_
#define GAME_H_

#include <thread>
#include <memory>
#include <mutex>

#include "Board.h"
#include "GameMoveData.h"

namespace ps {

class Game {

public:
	Game();

	void Loop();

	bool SetState(const std::string& psFEN);

	const Board& GetBoard() const;

	const GameMoveData& GetMoveData() const;

	Piece::Color GetPlayerColor() const;

	void SwitchPlayerColor();

	void MakeMove(const Move& move);

	std::string GetPsFEN() const;

private:
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(const Game&) = delete;
	Game& operator=(Game&&) = delete;

	std::unique_ptr<Board> _board;
	Piece::Color _player_color = Piece::Color::WHITE;
	GameMoveData _move_data;
	int _fify_move_rule_count = 0;
	int _current_move = 1;

	std::unique_ptr<std::thread> _game_thread;

};

}

#endif
