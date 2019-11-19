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

	// set widget properties
	SetDoubleBuffered(true);
	SetMinSize({ _TILE_SIZE * 9, _TILE_SIZE * 9 });

	// some brush constants
	_brush_tile_origin = std::make_unique<wxBrush>(wxColour(249, 166, 45, 143));
	_brush_tile_mouseover = std::make_unique<wxBrush>(wxColour(67, 107, 166, 143));

	// Specify the png files used in the rendering
	std::vector<std::string> files = {
			"icon_under_white_left", "icon_under_black_left",
			"icon_under_white_right", "icon_under_black_right",
			"icon_under_union_wb", "icon_under_union_bw",
			"icon_head_pawn_white_left", "icon_head_pawn_black_left",
			"icon_head_pawn_white_right", "icon_head_pawn_black_right",
			"icon_head_rook_white_left", "icon_head_rook_black_left",
			"icon_head_rook_white_right", "icon_head_rook_black_right",
			"icon_head_knight_white_left", "icon_head_knight_black_left",
			"icon_head_knight_white_right", "icon_head_knight_black_right",
			"icon_head_bishop_white_left", "icon_head_bishop_black_left",
			"icon_head_bishop_white_right", "icon_head_bishop_black_right",
			"icon_head_queen_white_left", "icon_head_queen_black_left",
			"icon_head_queen_white_right", "icon_head_queen_black_right",
			"icon_head_king_white_left", "icon_head_king_black_left",
			"icon_head_king_white_right", "icon_head_king_black_right",
	};

	// load the png files
	for (const auto& file : files) {
		wxFileInputStream input("Resources/png/" + file + ".png");
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
		// if we are currently holding a piece: drop it.
		_PutDown();
		_Redraw();
		return;
	}

	auto size = GetClientSize();

	_mouse_down = true;
	_is_dragging = false;

	// transform the mouse position to board space
	_mouse_point = {
			(evt.m_x - (size.x - 8.0 * _TILE_SIZE) / 2.0) / _TILE_SIZE,
			(evt.m_y - (size.y - 8.0 * _TILE_SIZE) / 2.0) / _TILE_SIZE
	};

	// we are not currently holding a piece, so check if the user clicked a
	// square with a piece
	if (_mouse_point.m_x > 0 && _mouse_point.m_x < 8) {
		// get the x, y position on the board
		int x = int(_mouse_point.m_x);
		int y = _Row(int(_mouse_point.m_y));

		// set the piece origin
		_moving_piece_origin = { y, x };

		// get the piece at the clicked square
		const Piece& draggingPiece = _game.GetBoard()[_moving_piece_origin];

		if (draggingPiece.GetColor() == _game.GetPlayerColor() || draggingPiece.GetColor() == Piece::Color::UNION) {
			// we can pick up the current piece; so pick it up

			_display[_moving_piece_origin] = Piece();
			_moving_piece = draggingPiece;
			_possible_moves = _game.GetBoard().CalculatePossibleMoves(_moving_piece_origin, _game.GetPlayerColor(), _game.GetMoveData());
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
			_Row(int(_mouse_point.m_y)),
			int(_mouse_point.m_x)
	};

	if (!_is_dragging && _mouse_down && _moving_piece.GetColor() != Piece::Color::EMPTY && _mouse_position != _moving_piece_origin) {
		_is_dragging = true;
	}

	_Redraw();
}

int BoardView::_Row(int r) const {
	if (_rotated) {
		return r;
	} else {
		return 7 - r;
	}
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
//	if (_rotated) {
//		gc->DrawBitmap(_bitmaps["board_rotated"],
//				0.0, 0.0, 8.0 * _TILE_SIZE, 8.0 * _TILE_SIZE);
//	} else {
//		gc->DrawBitmap(_bitmaps["board_standard"],
//				0.0, 0.0, 8.0 * _TILE_SIZE, 8.0 * _TILE_SIZE);
//	}

	// draw the coordinates
	gc->SetFont(GetFont(), { 0, 0, 0 });

	for (int i = 0; i < 8; i++) {
		if (_rotated) {
			gc->DrawText(wxString(char(i + '1'), 1), -10.0, (i + 0.5) * _TILE_SIZE - 5.0);
			gc->DrawText(wxString(char('h' - i), 1), (i + 0.5) * _TILE_SIZE - 2.0, 8.0 * _TILE_SIZE + 5.0);
		} else {
			gc->DrawText(wxString(char('8' - i), 1), -10.0, (i + 0.5) * _TILE_SIZE - 5.0);
			gc->DrawText(wxString(char(i + 'a'), 1), (i + 0.5) * _TILE_SIZE - 2.0, 8.0 * _TILE_SIZE + 5.0);
		}
	}

	// draw the origin position
	if (_moving_piece.GetColor() != Piece::Color::EMPTY) {
		gc->SetBrush(*_brush_tile_origin);
		gc->DrawRectangle(
						_TILE_SIZE * _moving_piece_origin.GetColumn(),
						_TILE_SIZE * _Row(_moving_piece_origin.GetRow()),
						_TILE_SIZE, _TILE_SIZE);
	}

	// draw the pieces
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			_DrawPiece(gc, _display[{ _Row(y), x }], { wxDouble(x), wxDouble(y) });
		}
	}

	// draw the possible moves
	gc->SetBrush(*_brush_tile_mouseover);

	for (const auto& move : _possible_moves) {
		if (move == _mouse_position) {
			gc->DrawRectangle(
				_TILE_SIZE * _mouse_position.GetColumn(),
				_TILE_SIZE * _Row(_mouse_position.GetRow()),
				_TILE_SIZE, _TILE_SIZE);
		} else {
			gc->DrawEllipse(
				_TILE_SIZE * move.GetColumn() + _TILE_SIZE / 2.0 - 11.0,
				_TILE_SIZE * _Row(move.GetRow()) + _TILE_SIZE / 2.0 - 11.0,
				22.0, 22.0);
		}
	}

	// draw the dragging piece
	_DrawPiece(gc, _moving_piece, _mouse_point - wxPoint2DDouble { 0.5, 0.5 });
}

void BoardView::_DrawPiece(wxGraphicsContext *gc, Piece piece, wxPoint2DDouble boardPosition) {
	// TODO: on resize, redraw the piece images

	std::string postString;
	std::string whitePart;
	std::string blackPart;
	int translate = 0;

	if (_rotated) {
		whitePart = "_white_left";
		blackPart = "_black_right";

		switch (piece.GetColor()) {
			case Piece::Color::EMPTY: return;
			case Piece::Color::WHITE: postString = whitePart; translate =  8; break;
			case Piece::Color::BLACK: postString = blackPart; translate = -8; break;
			case Piece::Color::UNION: postString = "_union_wb"; break;
		}
	} else {
		whitePart = "_white_right";
		blackPart = "_black_left";

		switch (piece.GetColor()) {
			case Piece::Color::EMPTY: return;
			case Piece::Color::WHITE: postString = whitePart; translate = -8; break;
			case Piece::Color::BLACK: postString = blackPart; translate =  8; break;
			case Piece::Color::UNION: postString = "_union_bw"; break;
		}
	}

	gc->DrawBitmap(_bitmaps["icon_under" + postString],
			_TILE_SIZE * boardPosition.m_x + translate,
			_TILE_SIZE * boardPosition.m_y,
			_TILE_SIZE, _TILE_SIZE);

	std::string typeString;

	switch (piece.GetWhiteType()) {
		case Piece::Type::NONE: goto black; break;
		case Piece::Type::PAWN: typeString = "icon_head_pawn"; break;
		case Piece::Type::ROOK: typeString = "icon_head_rook"; break;
		case Piece::Type::KNIGHT: typeString = "icon_head_knight"; break;
		case Piece::Type::BISHOP: typeString = "icon_head_bishop"; break;
		case Piece::Type::QUEEN: typeString = "icon_head_queen"; break;
		case Piece::Type::KING: typeString = "icon_head_king"; break;
	}

	{
		std::string bitmapFile = typeString + whitePart;
		if (auto iter = _bitmaps.find(bitmapFile); iter != _bitmaps.end()) {
			gc->DrawBitmap(iter->second,
					_TILE_SIZE * boardPosition.m_x + translate,
					_TILE_SIZE * boardPosition.m_y,
					_TILE_SIZE, _TILE_SIZE);
		}
	}

	black:
	switch (piece.GetBlackType()) {
		case Piece::Type::NONE: return;
		case Piece::Type::PAWN: typeString = "icon_head_pawn"; break;
		case Piece::Type::ROOK: typeString = "icon_head_rook"; break;
		case Piece::Type::KNIGHT: typeString = "icon_head_knight"; break;
		case Piece::Type::BISHOP: typeString = "icon_head_bishop"; break;
		case Piece::Type::QUEEN: typeString = "icon_head_queen"; break;
		case Piece::Type::KING: typeString = "icon_head_king"; break;
	}

	{
		std::string bitmapFile = typeString + blackPart;
		if (auto iter = _bitmaps.find(bitmapFile); iter != _bitmaps.end()) {
			gc->DrawBitmap(iter->second,
					_TILE_SIZE * boardPosition.m_x + translate,
					_TILE_SIZE * boardPosition.m_y,
					_TILE_SIZE, _TILE_SIZE);
		}
	}
}

void BoardView::_PutDown() {
	// we are no longer dragging the piece, because we just dropped it
	_is_dragging = false;

	// check if the dropped square is a valid position for the current moving
	// piece
	if (_moving_piece_origin != _mouse_position && _mouse_position.IsValid() &&
			std::find(_possible_moves.begin(), _possible_moves.end(), _mouse_position) != _possible_moves.end()) {

		// we can drop the piece here; so do that.
		// append the current move chain
		_current_move.AddPosition(_mouse_position);

		// check for en passant
		if (_game.GetBoard()[_mouse_position].GetColor() == Piece::Color::EMPTY &&
				_moving_piece.GetTypeOfColor(_game.GetPlayerColor()) == Piece::Type::PAWN &&
				_moving_piece_origin.GetColumn() != _mouse_position.GetColumn()) {

			// this was an en passant move
			Piece original = _display[{ _moving_piece_origin.GetRow(), _mouse_position.GetColumn() }];
			_display[{ _moving_piece_origin.GetRow(), _mouse_position.GetColumn() }] = Piece();

			if (original.GetColor() == Piece::Color::UNION) {
				// the target was a union, so chain the move with the new piece.

				_display[_mouse_position] = original;
				_moving_piece = _display[_mouse_position].MakeUnionWith(_moving_piece);
				_moving_piece_origin = _mouse_position;
				_possible_moves = _display.CalculatePossibleMoves(_moving_piece_origin, _moving_piece, _game.GetPlayerColor(), _game.GetMoveData());
				return;
			} else {
				// the target was not a union, so finish the move.
				_game.MakeMove(_current_move);
			}
		} else {
			// this was not an en passant move
			if (_game.GetBoard()[_mouse_position].GetColor() == Piece::Color::UNION) {
				// the target was a union, so chain the move with the new piece.

				// check if there was a white pawn promotion
				if (_mouse_position.GetRow() == 7 && _moving_piece.GetWhiteType() == Piece::Type::PAWN) {
					// promote to queen
					// TODO: make the player choose the piece
					_moving_piece = Piece(Piece::Type::QUEEN, Piece::Type::NONE);
				}

				// check if there was a black pawn promotion
				if (_mouse_position.GetRow() == 0 && _moving_piece.GetBlackType() == Piece::Type::PAWN) {
					// promote to queen
					// TODO: make the player choose the piece
					_moving_piece = Piece(Piece::Type::NONE, Piece::Type::QUEEN);
				}

				_moving_piece = _display[_mouse_position].MakeUnionWith(_moving_piece);
				_moving_piece_origin = _mouse_position;
				_possible_moves = _display.CalculatePossibleMoves(_moving_piece_origin, _moving_piece, _game.GetPlayerColor(), _game.GetMoveData());
				return;
			} else {
				// the target was not a union, so finish the move.
				// TODO: check if a pawn promotion happened and if so, let the
				// player choose a piece.
				_game.MakeMove(_current_move);
			}
		}
	}

	// we are done dropping the piece, and it was the last piece of the chain:
	// clear the moving piece and reset the displayed board to the actual board.
	_moving_piece_origin = { -1, -1 };
	_moving_piece = Piece();
	_possible_moves.clear();
	_current_move = ps::Move();
	_display = _game.GetBoard();
}

Window::Window(Game& game) :
		wxFrame(nullptr, wxID_ANY, L"Paco Ŝako"),
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
