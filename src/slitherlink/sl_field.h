#pragma once

#include <iostream>

#include "../common/grid_loop.h"
#include "sl_database.h"

namespace penciloid
{
namespace slitherlink
{
class Field : public GridLoop<Field>
{
public:
	typedef int Clue;
	static const Clue kNoClue = -1;

	Field();
	Field(Y height, X width);

	Field(const Field &other);
	Field(Field &&other);
	Field &operator=(const Field &other);
	Field &operator=(Field &&other);

	~Field();

	void AddClue(Position cell, Clue clue);
	inline Clue GetClue(Position cell) { return field_clue_[CellId(cell)]; }

	void Inspect(Position pos);

	void SetDatabase(Database* database) { database_ = database; }
	Database* GetDatabase() { return database_; }

private:
	Clue *field_clue_;
	Database *database_;

	unsigned int CellId(Position pos) { return pos.y * width() + pos.x; }

	void ApplyTheorem(Position pos);
};

std::ostream& operator<<(std::ostream &stream, Field &field);

}
}
