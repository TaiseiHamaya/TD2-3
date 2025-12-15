#include "InputUiType.h"

#include <Engine/Runtime/Input/Input.h>

void InputUiType::update() {
	if (
		Input::IsPressPad(PadID(0xffff)) || 
		Input::StickL().length() != 0.0f || 
		Input::StickR().length() != 0.0f
) {
		type = InputType::Pad;
	}
	else if (
		InputAdvanced::PressArrow().length() != 0 ||
		InputAdvanced::PressWASD().length() != 0 ||
		Input::IsPressKey(KeyID::R) ||
		Input::IsPressKey(KeyID::Z) ||
		Input::IsPressKey(KeyID::Escape)
) {
		type = InputType::Keyboard;
	}
}
