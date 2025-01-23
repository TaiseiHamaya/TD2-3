#pragma once
#include <memory>


class SpriteInstance;

class ClearUIClass{
public:
    ClearUIClass();
    ~ClearUIClass();

    void init();
    void update();
    void begin_rendering();
    void darw();

    //アクセッサ
    void SetClearFlag(bool value){ clearFlag = value; }

private:
    std::unique_ptr<SpriteInstance> sprite;

    bool clearFlag;

};