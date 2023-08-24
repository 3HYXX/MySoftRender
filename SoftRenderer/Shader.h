#pragma once

#include <glm/glm.hpp>
#include "Vertex.h"
#include "V2F.h"
#include "Texture.h"

class Shader {

public:
	Shader() {
		ModelMatrix = glm::mat4(1.0f);
		ViewMatrix = glm::mat4(1.0f);
		ProjectMatrix = glm::mat4(1.0f);
	}
	~Shader() = default;
private:
	glm::mat4 ModelMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectMatrix;
	Texture* texture;

public:
	V2F VertexShader(const Vertex& a2v) {
		V2F o;
		//MVP
		o.worldPos = ModelMatrix * a2v.position;
		o.windowPos = ProjectMatrix * ViewMatrix * o.worldPos;

		o.Z = 1 / o.windowPos.z;
		o.worldPos *= o.Z;
		o.color = a2v.color * o.Z;
		o.normal = a2v.normal * o.Z;
		o.texcoord = a2v.texcoord * o.Z;
		return o;

		/*o.color = a2v.color;
		o.normal = a2v.normal;
		o.texcoord = a2v.texcoord;
		return o;*/
	}
	//现在直接输出点的颜色
	/*glm::vec4 FragmentShader(const V2F& v) {
		glm::vec4 color;
		color = v.color;
		return color;
	}*/

	glm::vec4 FragmentShader(const V2F& v) {
		glm::vec4 color = texture->Sample2D(v.texcoord);
		return color;
	}

	void setModelMatrix(const glm::mat4& model) {
		ModelMatrix = model;
	}
	void setViewMatrix(const glm::mat4& view) {
		ViewMatrix = view;
	}
	void setProjectMatrix(const glm::mat4& project) {
		ProjectMatrix = project;
	}

	void setTexture(Texture* texture) {
		this->texture = texture;
	}
};