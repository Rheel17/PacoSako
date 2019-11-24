/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "Window.h"

#include <wx/splitter.h>
#include <wx/image.h>
#include <wx/wfstream.h>
#include <wx/graphics.h>

#define wxDefault wxDefaultPosition, wxDefaultSize

namespace ps {

const wxString NewGameDialog::_PLAYER_CHOICES[2] = {
		"Human",
		"AI: Random"
};

#define ID_BUTTON_CANCEL 17001
#define ID_BUTTON_CREATE 17002

BEGIN_EVENT_TABLE(BoardView, wxWindow)
	EVT_PAINT(BoardView::PaintEvent)
	EVT_SIZE(BoardView::SizeEvent)
	EVT_LEFT_DOWN(BoardView::MouseLeftDownEvent)
	EVT_LEFT_DCLICK(BoardView::MouseLeftDownEvent)
	EVT_LEFT_UP(BoardView::MouseLeftUpEvent)
	EVT_LEAVE_WINDOW(BoardView::MouseLeaveWindowEvent)
	EVT_MOTION(BoardView::MouseMotionEvent)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(NewGameDialog, wxTopLevelWindow)
EVT_TEXT(wxID_ANY, NewGameDialog::TextEvent)
	EVT_CHECKBOX(wxID_ANY, NewGameDialog::CheckboxEvent)
	EVT_BUTTON(ID_BUTTON_CANCEL, NewGameDialog::CancelButtonEvent)
	EVT_BUTTON(ID_BUTTON_CREATE, NewGameDialog::CreateButtonEvent)
END_EVENT_TABLE()

BoardView::BoardView(wxWindow *parent, bool displayOnly) :
		wxWindow(parent, wxID_ANY, wxDefault, wxBORDER_SUNKEN),
		_display_only(displayOnly) {

	// set widget properties
	SetDoubleBuffered(true);
	SetMinClientSize({ 180, 180 });

	// some brush constants
	_brush_board_dark = std::make_unique<wxBrush>(wxColour(215, 128, 73));
	_brush_board_light = std::make_unique<wxBrush>(wxColour(241, 202, 163));
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
		_images[file] = wxImage(input, wxBITMAP_TYPE_PNG);
	}
}

void BoardView::SetGame(Game *game) {
	_game = game;
	_display = _game->GetBoard();
	Redraw();
}

void BoardView::PaintEvent(wxPaintEvent& evt) {
	auto size = GetClientSize();
	_tile_size = std::min(size.x, size.y) / 9;

	if (_bitmaps.empty()) {
		for (const auto& [file, image] : _images) {
			wxImage scaledImage(image);
			scaledImage.Rescale(_tile_size, _tile_size, wxIMAGE_QUALITY_HIGH);
			_bitmaps[file] = scaledImage;
		}
	}

	wxPaintDC dc(this);
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
	_Draw(gc);
}

void BoardView::SizeEvent(wxSizeEvent& evt) {
	_bitmaps.clear();
	Redraw();
}

void BoardView::MouseLeftDownEvent(wxMouseEvent& evt) {
	if (_moving_piece.GetColor() != Piece::Color::EMPTY) {
		// if we are currently holding a piece: drop it.
		_PutDown();
		Redraw();
		return;
	}

	auto size = GetClientSize();

	_mouse_down = true;
	_is_dragging = false;

	// transform the mouse position to board space
	_mouse_point = {
			(evt.m_x - (size.x - 8.0 * _tile_size) / 2.0) / _tile_size,
			(evt.m_y - (size.y - 8.0 * _tile_size) / 2.0) / _tile_size
	};

	// we are not currently holding a piece, so check if the user clicked a
	// square with a piece
	if (_game && !_display_only && _mouse_point.m_x > 0 && _mouse_point.m_x < 8) {
		// get the x, y position on the board
		int x = _Col(int(_mouse_point.m_x));
		int y = _Row(int(_mouse_point.m_y));

		// set the piece origin
		_moving_piece_origin = { y, x };

		// get the piece at the clicked square
		const Piece& draggingPiece = _game->GetBoard()[_moving_piece_origin];

		if (draggingPiece.GetColor() == _game->GetPlayerColor() || draggingPiece.GetColor() == Piece::Color::UNION) {
			// we can pick up the current piece; so pick it up

			_display[_moving_piece_origin] = Piece();
			_moving_piece = draggingPiece;
			_possible_moves = _game->GetBoard().CalculatePossibleMoves(_moving_piece_origin, _game->GetPlayerColor(), _game->GetMoveData());
			_current_move = ps::Move(_moving_piece_origin);

			Redraw();
		} else {
			_moving_piece_origin = { -1, -1 };
		}
	}
}

void BoardView::MouseLeftUpEvent(wxMouseEvent& evt) {
	_mouse_down = false;

	if (_is_dragging) {
		_PutDown();
		Redraw();
	}
}

void BoardView::MouseLeaveWindowEvent(wxMouseEvent& evt) {
	_PutDown();
	Redraw();
}

void BoardView::MouseMotionEvent(wxMouseEvent& evt) {
	auto size = GetClientSize();

	_mouse_point = {
			(evt.m_x - (size.x - 8.0 * _tile_size) / 2.0) / _tile_size,
			(evt.m_y - (size.y - 8.0 * _tile_size) / 2.0) / _tile_size
	};
	_mouse_position = {
			_Row(int(_mouse_point.m_y)),
			_Col(int(_mouse_point.m_x))
	};

	if (!_is_dragging && _mouse_down && _moving_piece.GetColor() != Piece::Color::EMPTY && _mouse_position != _moving_piece_origin) {
		_is_dragging = true;
	}

	Redraw();
}

void BoardView::Redraw() {
	RefreshRect(GetClientRect(), false);
}

int BoardView::_Row(int r) const {
	if (_rotated) {
		return r;
	} else {
		return 7 - r;
	}
}

int BoardView::_Col(int c) const {
	if (_rotated) {
		return 7 - c;
	} else {
		return c;
	}
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
			(size.x - 8 * _tile_size) / 2,
			(size.y - 8 * _tile_size) / 2
	);

	gc->SetTransform(transform);

	// draw the board
	gc->SetBrush(*_brush_board_light);
	gc->DrawRectangle(0.0, 0.0, 8.0 * _tile_size, 8.0 * _tile_size);

	gc->SetBrush(*_brush_board_dark);

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if ((r + c) % 2) {
				gc->DrawRectangle(c * _tile_size, r * _tile_size, _tile_size, _tile_size);
			}
		}
	}

	// draw the coordinates
	if (!_display_only) {
		gc->SetFont(GetFont(), { 0, 0, 0 });

		for (int i = 0; i < 8; i++) {
			if (_rotated) {
				gc->DrawText(wxString(char(i + '1'), 1), -10.0, (i + 0.5) * _tile_size - 5.0);
				gc->DrawText(wxString(char('h' - i), 1), (i + 0.5) * _tile_size - 2.0, 8.0 * _tile_size + 5.0);
			} else {
				gc->DrawText(wxString(char('8' - i), 1), -10.0, (i + 0.5) * _tile_size - 5.0);
				gc->DrawText(wxString(char(i + 'a'), 1), (i + 0.5) * _tile_size - 2.0, 8.0 * _tile_size + 5.0);
			}
		}
	}

	if (!_game) {
		return;
	}

	if (_display_only) {
		_display = _game->GetBoard();
	}

	// draw the origin position
	if (_moving_piece.GetColor() != Piece::Color::EMPTY) {
		gc->SetBrush(*_brush_tile_origin);
		gc->DrawRectangle(
						_tile_size * _Col(_moving_piece_origin.GetColumn()),
						_tile_size * _Row(_moving_piece_origin.GetRow()),
						_tile_size, _tile_size);
	}

	// draw the pieces
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			_DrawPiece(gc, _display[{ _Row(y), _Col(x) }], { wxDouble(x), wxDouble(y) });
		}
	}

	// draw the possible moves
	gc->SetBrush(*_brush_tile_mouseover);

	for (const auto& move : _possible_moves) {
		if (move == _mouse_position) {
			gc->DrawRectangle(
				_tile_size * _Col(_mouse_position.GetColumn()),
				_tile_size * _Row(_mouse_position.GetRow()),
				_tile_size, _tile_size);
		} else {
			gc->DrawEllipse(
				_tile_size * _Col(move.GetColumn()) + _tile_size / 2.0 - 11.0,
				_tile_size * _Row(move.GetRow()) + _tile_size / 2.0 - 11.0,
				22.0, 22.0);
		}
	}

	// draw the dragging piece
	_DrawPiece(gc, _moving_piece, _mouse_point - wxPoint2DDouble { 0.5, 0.5 });
}

void BoardView::_DrawPiece(wxGraphicsContext *gc, Piece piece, wxPoint2DDouble boardPosition) {
	std::string postString;
	std::string whitePart;
	std::string blackPart;
	int translate = 0;

	if (_rotated) {
		whitePart = "_white_left";
		blackPart = "_black_right";

		switch (piece.GetColor()) {
			case Piece::Color::EMPTY: return;
			case Piece::Color::WHITE: postString = whitePart; translate =  9; break;
			case Piece::Color::BLACK: postString = blackPart; translate = -9; break;
			case Piece::Color::UNION: postString = "_union_wb"; break;
		}
	} else {
		whitePart = "_white_right";
		blackPart = "_black_left";

		switch (piece.GetColor()) {
			case Piece::Color::EMPTY: return;
			case Piece::Color::WHITE: postString = whitePart; translate = -9; break;
			case Piece::Color::BLACK: postString = blackPart; translate =  9; break;
			case Piece::Color::UNION: postString = "_union_bw"; break;
		}
	}

	if (translate != 0) {
		translate = _tile_size / translate;
	}

	gc->DrawBitmap(_bitmaps["icon_under" + postString],
			int(_tile_size * boardPosition.m_x + translate),
			int(_tile_size * boardPosition.m_y),
			_tile_size, _tile_size);

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
					int(_tile_size * boardPosition.m_x + translate),
					int(_tile_size * boardPosition.m_y),
					_tile_size, _tile_size);
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
					int(_tile_size * boardPosition.m_x + translate),
					int(_tile_size * boardPosition.m_y),
					_tile_size, _tile_size);
		}
	}
}

void BoardView::_PutDown() {
	// we are no longer dragging the piece, because we just dropped it
	_is_dragging = false;

	if (!_game || _display_only) {
		return;
	}

	// check if the dropped square is a valid position for the current moving
	// piece
	if (_moving_piece_origin != _mouse_position && _mouse_position.IsValid() &&
			std::find(_possible_moves.begin(), _possible_moves.end(), _mouse_position) != _possible_moves.end()) {

		// we can drop the piece here; so do that.
		// append the current move chain
		_current_move.AddPosition(_mouse_position);

		// check for en passant
		if (_game->GetBoard()[_mouse_position].GetColor() == Piece::Color::EMPTY &&
				_moving_piece.GetTypeOfColor(_game->GetPlayerColor()) == Piece::Type::PAWN &&
				_moving_piece_origin.GetColumn() != _mouse_position.GetColumn()) {

			// this was an en passant move
			Piece original = _display[{ _moving_piece_origin.GetRow(), _mouse_position.GetColumn() }];
			_display[{ _moving_piece_origin.GetRow(), _mouse_position.GetColumn() }] = Piece();

			if (original.GetColor() == Piece::Color::UNION) {
				// the target was a union, so chain the move with the new piece.

				_display[_mouse_position] = original;
				_moving_piece = _display[_mouse_position].MakeUnionWith(_moving_piece);
				_moving_piece_origin = _mouse_position;
				_possible_moves = _display.CalculatePossibleMoves(_moving_piece_origin, _moving_piece, _game->GetPlayerColor(), _game->GetMoveData());
				return;
			} else {
				// the target was not a union, so finish the move.
				_game->MakeMove(_current_move);
			}
		} else {
			// this was not an en passant move
			if (_game->GetBoard()[_mouse_position].GetColor() == Piece::Color::UNION) {
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
				_possible_moves = _display.CalculatePossibleMoves(_moving_piece_origin, _moving_piece, _game->GetPlayerColor(), _game->GetMoveData());
				return;
			} else {
				// the target was not a union, so finish the move.
				// TODO: check if a pawn promotion happened and if so, let the
				// player choose a piece.
				_game->MakeMove(_current_move);
			}
		}
	}

	// we are done dropping the piece, and it was the last piece of the chain:
	// clear the moving piece and reset the displayed board to the actual board.
	_moving_piece_origin = { -1, -1 };
	_moving_piece = Piece();
	_possible_moves.clear();
	_current_move = ps::Move();
	_display = _game->GetBoard();
}

NewGameDialog::NewGameDialog(wxWindow *parent) :
		wxDialog(parent, wxID_ANY, "New Game") {

	auto panel = new wxBoxSizer(wxHORIZONTAL);
	auto labelPanel = new wxFlexGridSizer(5, 1, 0, 0);
	auto contentPanel = new wxBoxSizer(wxVERTICAL);
	auto topRightPanel = new wxFlexGridSizer(1, 2, 0, 0);
	auto colorPanel = new wxFlexGridSizer(3, 1, 0, 0);
	auto setupPanel = new wxFlexGridSizer(2, 1, 0, 0);
	auto buttonPanel = new wxFlexGridSizer(1, 2, 0, 0);

	topRightPanel->AddGrowableCol(1, 1);
	labelPanel->AddGrowableRow(2, 2);

	_combo_white = new wxComboBox(this, wxID_ANY, "Human", wxDefault, _PLAYER_CHOICES_COUNT, _PLAYER_CHOICES, wxCB_READONLY);
	_combo_black = new wxComboBox(this, wxID_ANY, "Human", wxDefault, _PLAYER_CHOICES_COUNT, _PLAYER_CHOICES, wxCB_READONLY);
	_text_game_setup = new wxTextCtrl(this, wxID_ANY, _DEFAULT_SETUP);
	_check_default_setup = new wxCheckBox(this, wxID_ANY, "Use default setup");
	_button_cancel = new wxButton(this, ID_BUTTON_CANCEL, "Cancel");
	_button_create = new wxButton(this, ID_BUTTON_CREATE, "Create");
	_board_view = new BoardView(this, true);

	_combo_white->SetMinSize(wxSize(172, _combo_white->GetMinHeight()));
	_combo_black->SetMinSize(wxSize(172, _combo_black->GetMinHeight()));
	_text_game_setup->SetMinSize(wxSize(521, _text_game_setup->GetMinHeight()));
	_text_game_setup->Enable(false);
	_check_default_setup->Set3StateValue(wxCheckBoxState::wxCHK_CHECKED);
	_board_view->SetGame(&_game);

	auto labelWhiteSizer = new wxBoxSizer(wxHORIZONTAL);
	auto labelBlackSizer = new wxBoxSizer(wxHORIZONTAL);
	auto labelGameSetupSizer = new wxBoxSizer(wxHORIZONTAL);

	auto labelWhite = new wxStaticText(this, wxID_ANY, "White:", wxDefault, wxALIGN_RIGHT);
	auto labelBlack = new wxStaticText(this, wxID_ANY, "Black:", wxDefault, wxALIGN_RIGHT);
	auto labelGameSetup = new wxStaticText(this, wxID_ANY, "Game setup:", wxDefault, wxALIGN_RIGHT);

	labelWhiteSizer->Add(labelWhite, 0, wxALIGN_CENTER_VERTICAL);
	labelBlackSizer->Add(labelBlack, 0, wxALIGN_CENTER_VERTICAL);
	labelGameSetupSizer->Add(labelGameSetup, 0, wxALIGN_CENTER_VERTICAL);

	labelWhiteSizer->Add(0, _combo_white->GetClientSize().y);
	labelBlackSizer->Add(0, _combo_black->GetClientSize().y);
	labelGameSetupSizer->Add(0, _text_game_setup->GetClientSize().y);

	wxSizerFlags textSizerFlags = wxSizerFlags().Right().Border(wxLEFT | wxTOP | wxBOTTOM, 10);
	wxSizerFlags borderSizerFlags = wxSizerFlags().Expand().Border(wxALL, 10);

	labelPanel->Add(labelWhiteSizer, textSizerFlags);
	labelPanel->Add(labelBlackSizer, textSizerFlags);
	labelPanel->Add(0, 100, 1);
	labelPanel->Add(labelGameSetupSizer, textSizerFlags);

	colorPanel->Add(_combo_white, borderSizerFlags);
	colorPanel->Add(_combo_black, borderSizerFlags);
	colorPanel->AddStretchSpacer(1);

	setupPanel->Add(_text_game_setup, borderSizerFlags);
	setupPanel->Add(_check_default_setup, wxSizerFlags().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));

	buttonPanel->Add(_button_cancel, borderSizerFlags);
	buttonPanel->Add(_button_create, borderSizerFlags);

	topRightPanel->Add(colorPanel, 1);
	topRightPanel->Add(_board_view, wxSizerFlags(1).Right().Border(wxLEFT | wxRIGHT, 10));
	contentPanel->Add(topRightPanel, wxSizerFlags(1).Expand());
	contentPanel->Add(setupPanel);
	contentPanel->Add(buttonPanel, wxSizerFlags(0).Right());

	panel->Add(labelPanel, 0);
	panel->Add(contentPanel, 1);

	SetSizerAndFit(panel);
	CenterOnParent();
}

void NewGameDialog::TextEvent(wxCommandEvent& evt) {
	if (_game.SetState(std::string(_text_game_setup->GetValue().c_str()))) {
		_button_create->Enable(true);
	} else {
		_game.SetState(_EMPTY_SETUP);
		_button_create->Enable(false);
	}

	_board_view->Redraw();
}

void NewGameDialog::CheckboxEvent(wxCommandEvent& evt) {
	bool checked = _check_default_setup->Get3StateValue() == wxCheckBoxState::wxCHK_CHECKED;

	if (checked) {
		_store_game_setup = _text_game_setup->GetValue();
		_has_store_game_setup = true;
		_text_game_setup->SetValue(_DEFAULT_SETUP);
		_text_game_setup->Enable(false);
	} else {
		if (_has_store_game_setup) {
			_text_game_setup->SetValue(_store_game_setup);
		}

		_text_game_setup->Enable(true);
	}
}

void NewGameDialog::CancelButtonEvent(wxCommandEvent& evt) {
	Close(true);
}

void NewGameDialog::CreateButtonEvent(wxCommandEvent& evt) {
	static_cast<Window *>(GetParent())->StartGame(_game);
	Close(true);
}

Window::Window() :
		wxFrame(nullptr, wxID_ANY, L"Paco Ŝako") {

	_menu_game = new wxMenu;
	_menu_game->Append(wxID_NEW, L"&New");

	_menu = new wxMenuBar;
	_menu->Append(_menu_game, L"Game");
	SetMenuBar(_menu);

	wxSplitterWindow *splitter = new wxSplitterWindow(this);
	splitter->SetMinimumPaneSize(100);
	splitter->SetWindowStyle(splitter->GetWindowStyle() | wxSP_LIVE_UPDATE);

	_board_view = new BoardView(splitter);
	wxPanel *right = new wxPanel(splitter, wxID_ANY, wxDefault, wxBORDER_SUNKEN);

	splitter->SplitVertically(_board_view, right);

	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(splitter, 1, wxEXPAND);
	SetSizerAndFit(topSizer);
}

void Window::NewGame() {
	NewGameDialog *newGameDialog = new NewGameDialog(this);
	newGameDialog->ShowModal();
}

void Window::StartGame(Game game) {
	_game = std::move(game);
	_board_view->SetGame(&game);
}

}
