#pragma once

enum class InputType {
	Pad = 0,
	Keyboard = 1,
};

class InputUiType {
public:
	void update();

	InputType get_type() const { return type; }

private:
	InputType type{ InputType::Pad };
};
