/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#ifndef BOARDPOSITION_H_
#define BOARDPOSITION_H_

#include <string>

class BoardPosition {

public:
	BoardPosition(int row, int column);
	BoardPosition(const char *name);
	BoardPosition(std::string name);

	int GetRow() const;
	int GetColumn() const;
	std::string GetName() const;

	bool IsValid() const;

	bool operator==(const BoardPosition& bp) const;
	bool operator!=(const BoardPosition& bp) const;

private:
	int _row;
	int _column;

};

#endif
