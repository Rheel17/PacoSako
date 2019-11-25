/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef WINDOW_H_
#define WINDOW_H_

#include <wx/wx.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "Player.h"
#include "Game.h"

namespace ps {

class BoardView : public wxWindow {
	wxDECLARE_EVENT_TABLE();

public:
	BoardView(wxWindow *parent, bool displayOnly = false);

	void SetGame(Game *game);

    void PaintEvent(wxPaintEvent& evt);
    void SizeEvent(wxSizeEvent& evt);
    void MouseLeftDownEvent(wxMouseEvent& evt);
    void MouseLeftUpEvent(wxMouseEvent& evt);
    void MouseLeaveWindowEvent(wxMouseEvent& evt);
    void MouseMotionEvent(wxMouseEvent& evt);

	void Redraw();

private:
    int _Row(int r) const;
    int _Col(int c) const;

	void _Draw(wxGraphicsContext *gc);
	void _DrawPiece(wxGraphicsContext *gc, Piece piece, wxPoint2DDouble boardPosition);

	void _PutDown();

	Board _display;
	bool _display_only;
	bool _rotated = false;
	int _tile_size = _MINIMUM_TILE_SIZE;

	Game *_game = nullptr;

	bool _mouse_down = false;
	bool _is_dragging = false;
	wxPoint2DDouble _mouse_point;
	BoardPosition _mouse_position = { -1, -1 };

	BoardPosition _moving_piece_origin { -1, -1 };
	Piece _moving_piece;
	std::vector<BoardPosition> _possible_moves;
	ps::Move _current_move;

	std::unique_ptr<wxBrush> _brush_board_dark;
	std::unique_ptr<wxBrush> _brush_board_light;
	std::unique_ptr<wxBrush> _brush_tile_origin;
	std::unique_ptr<wxBrush> _brush_tile_mouseover;
	std::unordered_map<std::string, wxImage> _images;
	std::unordered_map<std::string, wxBitmap> _bitmaps;

private:
	constexpr static int _MINIMUM_TILE_SIZE = 72;

};

class NewGameDialog : public wxDialog {
	wxDECLARE_EVENT_TABLE();

public:
	NewGameDialog(wxWindow *parent);

	void TextEvent(wxCommandEvent& evt);
	void CheckboxEvent(wxCommandEvent& evt);
	void CancelButtonEvent(wxCommandEvent& evt);
	void CreateButtonEvent(wxCommandEvent& evt);

private:
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
	static constexpr auto _DEFAULT_SETUP = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkw - 0 1";
	static constexpr auto _EMPTY_SETUP = "8/8/8/8/8/8/8/8 w - - 0 1";
	static constexpr int _PLAYER_CHOICES_COUNT = 2;
	const static wxString _PLAYER_CHOICES[_PLAYER_CHOICES_COUNT];

};

class Window : public wxFrame {

public:
	Window();

	void NewGame();
	void NewGame(wxCommandEvent& evt);
	void StartGame(Game game);

private:
	wxMenuBar *_menu;
	wxMenu *_menu_game;
	BoardView *_board_view;

	Game _game;

};

}

#endif
