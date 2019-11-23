/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef APP_H_
#define APP_H_

#include "Window.h"
#include "Game.h"

namespace ps {

class App : public wxApp {

public:
	bool OnInit();

private:
	Window *_window = nullptr;

	std::unique_ptr<Game> _game;

};

}

#endif
