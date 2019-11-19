/*
 * Copyright © 2019 Levi van Rheenen. All rights reserved.
 */
#include "BoardPosition.h"

namespace ps {

BoardPosition::BoardPosition(int row, int column) :
		_row(row), _column(column) {}

BoardPosition::BoardPosition(const char* name) :
		_row(name[1] - '1'), _column(name[0] - 'a') {}

BoardPosition::BoardPosition(std::string name) :
		_row(name[1] - '1'), _column(name[0] - 'a') {}

int BoardPosition::GetRow() const {
	return _row;
}

int BoardPosition::GetColumn() const {
	return _column;
}

std::string BoardPosition::GetName() const {
	if (IsValid()) {
		const char name[3] = { char(_column + 'a'), char(_row + '1'), 0 };
		return std::string(name);
	} else {
		return "invalid";
	}
}

bool BoardPosition::IsValid() const {
	return _row >= 0 && _row < 8 && _column >= 0 && _column < 8;
}

bool BoardPosition::operator==(const BoardPosition& bp) const {
	return _row == bp._row && _column == bp._column;
}

bool BoardPosition::operator!=(const BoardPosition& bp) const {
	return _row != bp._row || _column != bp._column;
}

}
