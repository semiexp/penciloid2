#pragma once

#include <iostream>

#include "../common/grid_loop.h"
#include "../common/grid.h"
#include "sl_type.h"
#include "sl_problem.h"
#include "sl_dictionary.h"
#include "sl_method.h"

namespace penciloid
{
namespace slitherlink
{
class Field : public GridLoop<Field>
{
public:
	Field();
	Field(Y height, X width, Dictionary *database = nullptr, const Method &met = Method());
	Field(const Problem& problem, Dictionary *database = nullptr, const Method &met = Method());

	Field(const Field &other);
	Field(Field &&other);
	Field &operator=(const Field &other);
	Field &operator=(Field &&other);

	~Field();

	void AddClue(CellPosition cell, Clue clue);
	inline Clue GetClue(CellPosition cell) { return field_clue_(cell); }

	void Inspect(LoopPosition pos);

	void SetDatabase(Dictionary* database) { database_ = database; }
	Dictionary* GetDatabase() { return database_; }

	Method GetMethod() const { return method_; }
	void SetMethod(const Method &m) {
		method_ = m;
		GridLoop<Field>::SetMethod(m.grid_loop_method);
	}
	void SetMethod(const GridLoopMethod &) = delete;

private:
	Grid<Clue> field_clue_;
	Dictionary *database_;
	Method method_;

	unsigned int CellId(CellPosition pos) { return field_clue_.GetIndex(pos); }

	void ApplyTheorem(LoopPosition pos);
	void CheckDiagonalChain(LoopPosition pos);
};

std::ostream& operator<<(std::ostream &stream, Field &field);

}
}
