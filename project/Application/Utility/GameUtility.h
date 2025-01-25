#pragma once
#include <Library/Math/Vector3.h>

class GameUtility
{
public:
    static bool approximately_equal(const Vector3& a, const Vector3& b, float epsilon = 1e-5f);
    static Vector3 rotate_direction_90_left(const Vector3& direction);
    static Vector3 rotate_direction_90_right(const Vector3& direction);
};

