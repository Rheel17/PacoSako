/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef WINDOW_H_
#define WINDOW_H_

#include <wx/wx.h>

#include <memory>
#include <unordered_map>
#include <vector>
#include <future>

#include "Player.h"
#include "Game.h"

namespace ps {

class BoardView : public wxWindow {
	wxDECLARE_EVENT_TABLE();

public:
	BoardView(wxWindow *parent, bool displayOnly = false);

	void SetGame(Game *game);
	void SetPlayerColor(Piece::Color playerColor);

    void PaintEvent(wxPaintEvent& evt);
    void SizeEvent(wxSizeEvent& evt);
    void MouseLeftDownEvent(wxMouseEvent& evt);
    void MouseLeftUpEvent(wxMouseEvent& evt);
    void MouseLeaveWindowEvent(wxMouseEvent& evt);
    void MouseMotionEvent(wxMouseEvent& evt);

    void ResetDisplay();
	void Redraw();
	void SetLastMove(const ps::Move& move);
	void Animate(const ps::Move& move);

private:
    int _Row(int r) const;
    int _Col(int c) const;

	void _Draw(wxGraphicsContext *gc);
	void _DrawPiece(wxGraphicsContext *gc, Piece piece, wxPoint2DDouble boardPosition);

	void _PutDown();
	void _FinishMove();

	Board _display;
	bool _display_only;
	bool _rotated = false;
	int _tile_size = _MINIMUM_TILE_SIZE;

	Game *_game = nullptr;
	Piece::Color _player_color = Piece::Color::EMPTY;

	bool _mouse_down = false;
	bool _is_dragging = false;
	wxPoint2DDouble _mouse_point;
	BoardPosition _mouse_position = { -1, -1 };

	BoardPosition _moving_piece_origin { -1, -1 };
	Piece _moving_piece;
	std::vector<BoardPosition> _possible_moves;
	ps::Move _current_move;
	ps::Move _last_move;

	std::vector<BoardPosition> _last_move_postfix;
	Piece _animating_piece;
	BoardPosition _animation_start;
	BoardPosition _animation_end;
	float _animation_time = 0.0f;
	float _animation_total = 0.0f;

	std::unique_ptr<wxBrush> _brush_board_dark;
	std::unique_ptr<wxBrush> _brush_board_light;
	std::unique_ptr<wxBrush> _brush_tile_origin;
	std::unique_ptr<wxBrush> _brush_tile_move;
	std::unique_ptr<wxBrush> _brush_tile_mouseover;
	std::unordered_map<std::string, wxImage> _images;
	std::unordered_map<std::string, wxBitmap> _bitmaps;

private:
	constexpr static int _MINIMUM_TILE_SIZE = 72;

};

class Window;

class NewGameDialog : public wxDialog {
	wxDECLARE_EVENT_TABLE();

public:
	NewGameDialog(Window *parent);

	void TextEvent(wxCommandEvent& evt);
	void CheckboxEvent(wxCommandEvent& evt);
	void CancelButtonEvent(wxCommandEvent& evt);
	void CreateButtonEvent(wxCommandEvent& evt);

private:
	Player *_CreatePlayer(wxComboBox *comboBox, Piece::Color color);

	Window *_parent;

	wxComboBox *_combo_white;
	wxComboBox *_combo_black;
	wxTextCtrl *_text_game_setup;
	wxCheckBox *_check_default_setup;
	wxButton *_button_cancel;
	wxButton *_button_create;
	BoardView *_board_view;

	bool _has_store_game_setup = false;
	wxString _store_game_setup;

	Game _game;

private:
	static constexpr auto _DEFAULT_SETUP = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	static constexpr auto _EMPTY_SETUP = "8/8/8/8/8/8/8/8 w - - 0 1";
	static constexpr int _PLAYER_CHOICES_COUNT = 2;
	const static wxString _PLAYER_CHOICES[_PLAYER_CHOICES_COUNT];

};

class Window : public wxFrame {
	friend class BoardView;

public:
	Window();
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	void NewGame();
	void NewGame(wxCommandEvent& evt);
	void StartGame(Game game, Player *white, Player *black);

	std::future<ps::Move> StartMove(Piece::Color playerColor);
	void FinishMove(const ps::Move& move, bool fromHuman);

	void Mate();
	void Stalemate();

private:
	void _MakeMove(const ps::Move& move);

	wxMenuBar *_menu;
	wxMenu *_menu_game;
	BoardView *_board_view;

	std::promise<ps::Move> *_move;
	Game *_game = nullptr;

};

}

#endif
