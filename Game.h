/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef GAME_H_
#define GAME_H_

#include <wx/wx.h>

#include <memory>

#include "Board.h"
#include "GameMoveData.h"

namespace ps {

class Game : public wxApp {

public:
	Game();

	bool OnInit() override;

	const Board& GetBoard() const;

	const GameMoveData& GetMoveData() const;

	Piece::Color GetPlayerColor() const;

	void SwitchPlayerColor();

	void MakeMove(const Move& move);


private:
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(const Game&) = delete;
	Game& operator=(Game&&) = delete;

	wxFrame *_window = nullptr;
	std::unique_ptr<Board> _board;
	Piece::Color _player_color = Piece::Color::WHITE;
	GameMoveData _move_data;

};

}

#endif
