#pragma once
#include <Library/Math/Vector3.h>
#include <Library/Math/Quaternion.h>

class GameUtility
{
public:
    static bool approximately_equal(const Vector3& a, const Vector3& b, float epsilon = 1e-5f);
    static Vector3 rotate_direction_90_left(const Vector3& direction);
    static Vector3 rotate_direction_90_right(const Vector3& direction);


    // 角度の判別(90度であればtrue)
    static bool IsRotation90Degrees(const Quaternion& q1, const Quaternion& q2);

    static bool IsRotation180Degrees(const Quaternion& q1, const Quaternion& q2);
};

