#pragma once

enum class RotationDirection {
    None,  // 回転していない
    Left,  // 左回転
    Right  // 右回転
};

enum class PlayerState {
    Moving,          // 移動中
    Rotating,        // 回転中
    Falling,         // 落下中
    SlidingOnIce,    // 氷の上を滑っている
    MoveFailed,      // 移動失敗
    RotationFailed   // 回転失敗 (失敗理由は別に保持)
};

enum class RotationFailReason {
    None,                  // 失敗なし（初期状態）
    HitDiagonalFrontWall,   // 斜め前の壁にぶつかる
    HitSideWall,            // 隣の壁にぶつかる
    NextTileIsHole,         // 隣のマスに穴がある
    HitDiagonalBackWall,    // 斜め後ろの壁にぶつかる
    HitBackWall,            // 後ろの壁にぶつかる
    BackTileIsHole          // 後ろに穴が開いてる
};

enum class MoveType {
    Normal,         // 通常の移動
    ParentCarriesChild, // 親が子を持ち上げる
    ChildCarriesParent  // 子が親を持ち上げる
};