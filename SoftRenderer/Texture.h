#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Texture {

public:
	int width;
	int height;
	int channels;
	unsigned char* data;
	Texture() = default;
	Texture(const std::string& Path) {
		data = stbi_load(Path.c_str(), &width, &height, &channels, 0);
	}
	~Texture() {
		if (data)
			free(data);
	}

	void LoadTexture(const std::string& Path) {
		if (data)
			free(data);
		data = stbi_load(Path.c_str(), &width, &height, &channels, 0);
	}
	glm::vec4 Sample2D(const glm::vec2& texcoord) {
		float x = texcoord.x - (int)texcoord.x;
		float y = texcoord.y - (int)texcoord.y;
		//支持负数坐标
		x = x < 0 ? -x : x;
		y = y < 0 ? -y : y;
		return GetColor(x * width, y * height) / 255.0f;
	}
	glm::vec4 GetColor(int x, int y) {
		if (x <0 || x > width || y < 0 || y> height)
			return glm::vec4(0, 0, 0, 0);
		int xy = channels * (y * width + x);
		return glm::vec4(*(data + xy), *(data + xy + 1), *(data + xy + 2), *(data + xy + 3));
	}
};