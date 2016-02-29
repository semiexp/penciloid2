#pragma once

#include <iostream>

#include "../common/grid_loop.h"
#include "../common/grid.h"
#include "sl_type.h"
#include "sl_problem.h"
#include "sl_database.h"

namespace penciloid
{
namespace slitherlink
{
class Field : public GridLoop<Field>
{
public:
	Field();
	Field(Y height, X width, Database *database = nullptr);
	Field(const Problem& problem, Database *database = nullptr);

	Field(const Field &other);
	Field(Field &&other);
	Field &operator=(const Field &other);
	Field &operator=(Field &&other);

	~Field();

	void AddClue(CellPosition cell, Clue clue);
	inline Clue GetClue(CellPosition cell) { return field_clue_.at(cell); }

	void Inspect(LoopPosition pos);

	void SetDatabase(Database* database) { database_ = database; }
	Database* GetDatabase() { return database_; }

private:
	Grid<Clue> field_clue_;
	Database *database_;

	unsigned int CellId(CellPosition pos) { return field_clue_.GetIndex(pos); }

	void ApplyTheorem(LoopPosition pos);
	void CheckDiagonalChain(LoopPosition pos);
};

std::ostream& operator<<(std::ostream &stream, Field &field);

}
}
