#pragma once

#include <glm/glm.hpp>

class Vertex {
public:
	glm::vec4 position;
	glm::vec4 color;
	glm::vec2 texcoord;
	glm::vec3 normal;

	Vertex() = default;
	~Vertex() = default;

	Vertex(
		const glm::vec4& _pos,
		const glm::vec4& _color,
		const glm::vec2& _tex,
		const glm::vec3& _normal
	) :
		position(_pos), color(_color), texcoord(_tex), normal(_normal) {}
	Vertex(
		const glm::vec3& _pos,
		const glm::vec4& _color = glm::vec4(0, 0, 0, 0),
		const glm::vec2& _tex = glm::vec2(0, 0),
		const glm::vec3& _normal = glm::vec3(0, 0, 1)
	) :
		position(_pos, 1.0f), color(_color), texcoord(_tex), normal(_normal) {}

	Vertex(const Vertex& v) :position(v.position), color(v.color), texcoord(v.texcoord), normal(v.normal) {}
};