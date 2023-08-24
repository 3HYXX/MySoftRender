#pragma once

#include <glm/glm.hpp>
#include "Math.h"

class V2F {
public:
	glm::vec4 worldPos;
	glm::vec4 windowPos;
	glm::vec4 color;
	glm::vec2 texcoord;
	glm::vec3 normal;
	float Z;

	V2F() = default;
	~V2F() = default;
	V2F(
		const glm::vec4& _wPos,
		const glm::vec4& _pPos,
		const glm::vec4& _color,
		const glm::vec2& _tex,
		const glm::vec3& _normal,
		float _Z
	) :
		worldPos(_wPos), windowPos(_pPos), color(_color), texcoord(_tex), normal(_normal), Z(_Z) {}
	V2F(const V2F& v) :
		worldPos(v.worldPos), windowPos(v.windowPos), color(v.color), texcoord(v.texcoord), normal(v.normal), Z(v.Z) {}

	static V2F lerp(const V2F& v1, const V2F& v2, const float& factor) {
		V2F result;
		result.windowPos = Lerp(v1.windowPos, v2.windowPos, factor);
		result.worldPos = Lerp(v1.worldPos, v2.worldPos, factor);
		result.color = Lerp(v1.color, v2.color, factor);
		result.normal = Lerp(v1.normal, v2.normal, factor);
		result.texcoord = Lerp(v1.texcoord, v2.texcoord, factor);
		result.Z = Lerp(v1.Z, v2.Z, factor);
		return result;
	}
};