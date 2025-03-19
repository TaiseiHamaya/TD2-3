#pragma once

#include <list>

#include <Library/Math/Quaternion.h>

class MoveLogger {
public:
	struct MoveLog {
		struct CharacterInfo {
			float x;
			float y;
			Quaternion rotation;
		};
		CharacterInfo player;
		CharacterInfo child;
		bool isSticking;
	};

public:
	void initialize();
	void update();

public:
	bool can_undo() const;
	void emplace(MoveLog&& moveLog);
	MoveLog pop();

private:
	std::list<MoveLog> moveLogs;
};
