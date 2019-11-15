/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef WINDOW_H_
#define WINDOW_H_

#include <wx/wx.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "Game.h"

namespace ps {

class BoardView : public wxWindow {
	wxDECLARE_EVENT_TABLE();

public:
	BoardView(wxWindow *parent, Game& game);

    void PaintEvent(wxPaintEvent& evt);
    void SizeEvent(wxSizeEvent& evt);
    void MouseLeftDownEvent(wxMouseEvent& evt);
    void MouseLeftUpEvent(wxMouseEvent& evt);
    void MouseLeaveWindowEvent(wxMouseEvent& evt);
    void MouseMotionEvent(wxMouseEvent& evt);

private:
	void _Redraw();
	void _Draw(wxGraphicsContext *gc);
	void _DrawPiece(wxGraphicsContext *gc, Piece piece, wxPoint2DDouble boardPosition);

	void _PutDown();

	Game& _game;
	Board _display;
	bool _rotated = false;

	bool _mouse_down = false;
	bool _is_dragging = false;
	wxPoint2DDouble _mouse_point;
	BoardPosition _mouse_position = { -1, -1 };

	BoardPosition _moving_piece_origin { -1, -1 };
	Piece _moving_piece;
	std::vector<BoardPosition> _possible_moves;
	ps::Move _current_move;

	std::unique_ptr<wxBrush> _brush_tile_origin;
	std::unique_ptr<wxBrush> _brush_tile_mouseover;
	std::unordered_map<std::string, wxBitmap> _bitmaps;

private:
	constexpr static auto _TILE_SIZE = 72;

};

class Window : public wxFrame {

public:
	Window(Game& game);

private:
	Game& _game;

};

}

#endif
