/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "Window.h"

#include <wx/splitter.h>
#include <wx/image.h>
#include <wx/wfstream.h>
#include <wx/graphics.h>

namespace ps {

BEGIN_EVENT_TABLE(BoardView, wxPanel)
	EVT_PAINT(BoardView::PaintEvent)
	EVT_SIZE(BoardView::SizeEvent)
	EVT_LEFT_DOWN(BoardView::MouseLeftDownEvent)
	EVT_LEFT_DCLICK(BoardView::MouseLeftDownEvent)
	EVT_LEFT_UP(BoardView::MouseLeftUpEvent)
	EVT_LEAVE_WINDOW(BoardView::MouseLeaveWindowEvent)
	EVT_MOTION(BoardView::MouseMotionEvent)
END_EVENT_TABLE()

BoardView::BoardView(wxWindow *parent, Game& game) :
		wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN),
		_game(game) {

	_display = game.GetBoard();

	SetDoubleBuffered(true);
	SetMinSize({ _TILE_SIZE * 9, _TILE_SIZE * 9 });

	_brush_tile_origin = std::make_unique<wxBrush>(wxColour(249, 166, 45, 143));
	_brush_tile_mouseover = std::make_unique<wxBrush>(wxColour(67, 107, 166, 143));

	std::vector<std::string> files = {
			"board_standard", "board_rotated",
			"icon_under_left_white", "icon_under_left_black",
			"icon_under_right_white", "icon_under_right_black",
			"icon_under_union_wb", "icon_under_union_bw",
			"icon_head_pawn"
	};

	for (const auto& file : files) {
		wxFileInputStream input("Resources/" + file + ".png");
		wxImage i(input, wxBITMAP_TYPE_PNG);
		_bitmaps[file] = wxBitmap(i);
	}
}

void BoardView::PaintEvent(wxPaintEvent& evt) {
	wxPaintDC dc(this);
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
	_Draw(gc);
}

void BoardView::SizeEvent(wxSizeEvent& evt) {
	_Redraw();
}

void BoardView::MouseLeftDownEvent(wxMouseEvent& evt) {
	if (_moving_piece.GetColor() != Piece::Color::EMPTY) {
		_PutDown();
		_Redraw();
		return;
	}

	auto size = GetClientSize();

	_mouse_down = true;
	_is_dragging = false;

	_mouse_point = {
			(evt.m_x - (size.x - 8.0 * _TILE_SIZE) / 2.0) / _TILE_SIZE,
			(evt.m_y - (size.y - 8.0 * _TILE_SIZE) / 2.0) / _TILE_SIZE
	};

	if (_mouse_point.m_x > 0 && _mouse_point.m_x < 8) {
		int x = int(_mouse_point.m_x);
		int y = 7 - int(_mouse_point.m_y);

		_moving_piece_origin = { y, x };
		const Piece& draggingPiece = _game.GetBoard()[_moving_piece_origin];

		if (draggingPiece.GetColor() == _game.GetPlayerColor() || draggingPiece.GetColor() == Piece::Color::UNION) {
			_display[_moving_piece_origin] = Piece();
			_moving_piece = draggingPiece;
			_possible_moves = _game.GetBoard().CalculatePossibleMoves(_moving_piece_origin, _game.GetPlayerColor());
			_current_move = ps::Move(_moving_piece_origin);

			_Redraw();
		} else {
			_moving_piece_origin = { -1, -1 };
		}
	}
}

void BoardView::MouseLeftUpEvent(wxMouseEvent& evt) {
	_mouse_down = false;

	if (_is_dragging) {
		_PutDown();
		_Redraw();
	}
}

void BoardView::MouseLeaveWindowEvent(wxMouseEvent& evt) {
	_PutDown();
	_Redraw();
}

void BoardView::MouseMotionEvent(wxMouseEvent& evt) {
	auto size = GetClientSize();

	_mouse_point = {
			(evt.m_x - (size.x - 8.0 * _TILE_SIZE) / 2.0) / _TILE_SIZE,
			(evt.m_y - (size.y - 8.0 * _TILE_SIZE) / 2.0) / _TILE_SIZE
	};
	_mouse_position = {
			7 - int(_mouse_point.m_y),
			int(_mouse_point.m_x)
	};

	if (!_is_dragging && _mouse_down && _moving_piece.GetColor() != Piece::Color::EMPTY && _mouse_position != _moving_piece_origin) {
		_is_dragging = true;
	}

	_Redraw();
}

void BoardView::_Redraw() {
	RefreshRect(GetClientRect(), false);
}

void BoardView::_Draw(wxGraphicsContext *gc) {
	auto size = GetClientSize();

	// draw the background
	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(GetBackgroundColour()));
	gc->DrawRectangle(0.0, 0.0, size.x, size.y);

	// move the board to the center
	wxGraphicsMatrix transform = gc->CreateMatrix(
			1.0, 0.0, 0.0, 1.0,
			(size.x - 8 * _TILE_SIZE) / 2,
			(size.y - 8 * _TILE_SIZE) / 2
	);

	gc->SetTransform(transform);

	// draw the board
	if (_rotated) {
		gc->DrawBitmap(_bitmaps["board_rotated"],
				0.0, 0.0, 8.0 * _TILE_SIZE, 8.0 * _TILE_SIZE);
	} else {
		gc->DrawBitmap(_bitmaps["board_standard"],
				0.0, 0.0, 8.0 * _TILE_SIZE, 8.0 * _TILE_SIZE);
	}

	// draw the origin position
	if (_moving_piece.GetColor() != Piece::Color::EMPTY) {
		gc->SetBrush(*_brush_tile_origin);
		gc->DrawRectangle(
						_TILE_SIZE * _moving_piece_origin.GetColumn(),
						_TILE_SIZE * (7 - _moving_piece_origin.GetRow()),
						_TILE_SIZE, _TILE_SIZE);
	}

	// draw the possible moves
	gc->SetBrush(*_brush_tile_mouseover);

	for (const auto& move : _possible_moves) {
		if (move == _mouse_position) {
			gc->DrawRectangle(
				_TILE_SIZE * _mouse_position.GetColumn(),
				_TILE_SIZE * (7 - _mouse_position.GetRow()),
				_TILE_SIZE, _TILE_SIZE);
		} else {
			gc->DrawEllipse(
				_TILE_SIZE * move.GetColumn() + _TILE_SIZE / 2.0 - 11.0,
				_TILE_SIZE * (7 - move.GetRow()) + _TILE_SIZE / 2.0 - 11.0,
				22.0, 22.0);
		}
	}

	// draw the pieces
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			_DrawPiece(gc, _display[{ 7 - y, x }], { wxDouble(x), wxDouble(y) });
		}
	}

	// draw the dragging piece
	_DrawPiece(gc, _moving_piece, _mouse_point - wxPoint2DDouble { 0.5, 0.5 });
}

void BoardView::_DrawPiece(wxGraphicsContext *gc, Piece piece, wxPoint2DDouble boardPosition) {
	std::string typeString;
	int translate = 0;

	if (_rotated) {
		switch (piece.GetColor()) {
			case Piece::Color::EMPTY: return;
			case Piece::Color::WHITE: typeString = "_left_white"; break;
			case Piece::Color::BLACK: typeString = "_right_black"; break;
			case Piece::Color::UNION: typeString = "_union_wb"; break;
		}
	} else {
		switch (piece.GetColor()) {
			case Piece::Color::EMPTY: return;
			case Piece::Color::WHITE: typeString = "_right_white"; break;
			case Piece::Color::BLACK: typeString = "_left_black"; break;
			case Piece::Color::UNION: typeString = "_union_bw"; break;
		}
	}

	gc->DrawBitmap(_bitmaps["icon_under" + typeString],
			_TILE_SIZE * boardPosition.m_x + translate,
			_TILE_SIZE * boardPosition.m_y,
			_TILE_SIZE, _TILE_SIZE);
}

void BoardView::_PutDown() {
	_is_dragging = false;

	if (_moving_piece_origin != _mouse_position && _mouse_position.IsValid() &&
			std::find(_possible_moves.begin(), _possible_moves.end(), _mouse_position) != _possible_moves.end()) {

		_current_move.AddPosition(_mouse_position);

		if (_game.GetBoard()[_mouse_position].GetColor() == Piece::Color::UNION) {
			_moving_piece = _display[_mouse_position].MakeUnionWith(_moving_piece);
			_moving_piece_origin = _mouse_position;
			_possible_moves = _display.CalculatePossibleMoves(_moving_piece_origin, _moving_piece, _game.GetPlayerColor());
			return;
		} else {
			_game.MakeMove(_current_move);
		}
	}

	_moving_piece_origin = { -1, -1 };
	_moving_piece = Piece();
	_possible_moves.clear();
	_current_move = ps::Move();
	_display = _game.GetBoard();
}

Window::Window(Game& game) :
		wxFrame(nullptr, wxID_ANY, L"Paco Åœako"),
		_game(game) {

	wxSplitterWindow* splitter = new wxSplitterWindow(this);
	splitter->SetMinimumPaneSize(100);
	splitter->SetWindowStyle(splitter->GetWindowStyle() | wxSP_LIVE_UPDATE);

	BoardView* left = new BoardView(splitter, _game);
	wxPanel* right = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

	splitter->SplitVertically(left, right);

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(splitter, 1, wxEXPAND);
	SetSizerAndFit(topSizer);
}

}
