#pragma once

#include <vector>

#include "Vector2.h"

/// <summary>
///	線形補間
/// </summary>
/// <typeparam name="type">型名</typeparam>
/// <param name="from">t=0のときの値</param>
/// <param name="to">t=1のときの値</param>
/// <param name="t">媒介変数</param>
/// <returns></returns>
template<typename type>
type Lerp(const type from, const type to, const float t);

Vector2 Bezier(std::vector<Vector2> node, const float t);

namespace Easing {

namespace In {

float Sine(float t);
float Quad(float t);
float Cubic(float t);
float Quart(float t);
float Quint(float t);
float Expo(float t);
float Circ(float t);
float Back(float t);
float Elastic(float t);
float Bounce(float t);

};

namespace Out {

float Sine(float t);
float Quad(float t);
float Cubic(float t);
float Quart(float t);
float Quint(float t);
float Expo(float t);
float Circ(float t);
float Back(float t);
float Elastic(float t);
float Bounce(float t);

};

namespace InOut {

float Sine(float t);
float Quad(float t);
float Cubic(float t);
float Quart(float t);
float Quint(float t);
float Expo(float t);
float Circ(float t);
float Back(float t);
float Elastic(float t);
float Bounce(float t);

};

};

template<typename type>
inline type Lerp(const type from, const type to, const float t) {
	return type(from * (1 - t)) + type(to * t);
}
