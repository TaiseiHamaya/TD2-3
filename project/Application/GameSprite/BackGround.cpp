#include "BackGround.h"
#include "Engine/Module/World/Sprite/SpriteInstance.h"

BackGround::BackGround() {
	init();
}

BackGround::~BackGround() {}

void BackGround::init() {
	curScroll = 0;
	backGroundSprite = std::make_unique<SpriteInstance>("backGround2.png");
	backGroundSprite2 = std::make_unique<SpriteInstance>("backGround.png");
}

void BackGround::update() {}

void BackGround::debugUpdate() {}

void BackGround::begin_rendering() {

	backGroundSprite->begin_rendering();
	backGroundSprite2->begin_rendering();
}

void BackGround::darw() {
	curScroll += scrollSpeed * WorldClock::DeltaSeconds();
	curScroll2 += scrollSpeed2 * WorldClock::DeltaSeconds();
	backGroundSprite->get_uv_transform().set_translate_x(curScroll);
	backGroundSprite->draw();
	backGroundSprite2->get_uv_transform().set_translate_x(curScroll2);
	backGroundSprite2->draw();
}
