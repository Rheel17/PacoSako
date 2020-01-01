/*
 * Copyright © 2020 Levi van Rheenen. All rights reserved.
 */
#include "App.h"

#include "Window.h"

namespace ps {

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	wxInitAllImageHandlers();

	_window = new Window();
	_window->Show();
	_window->Maximize(true);

	_window->NewGame();

	return true;
}

}
