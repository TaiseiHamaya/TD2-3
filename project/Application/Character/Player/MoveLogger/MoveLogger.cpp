#include "MoveLogger.h"

void MoveLogger::initialize() {
}

void MoveLogger::update() {
}

bool MoveLogger::can_undo() const {
	return !moveLogs.empty();
}

void MoveLogger::emplace(MoveLog&& moveLog) {
	moveLogs.emplace_back(std::move(moveLog));
}

MoveLogger::MoveLog MoveLogger::pop() {
	MoveLog back = moveLogs.back();
	moveLogs.pop_back();
	return back;
}
