#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Math.h"

class FrameBuffer {
public:
	int Width, Height;
	std::vector<unsigned char> colorBuffer;
	std::vector<float> depthBuffer;
	~FrameBuffer() = default;

	FrameBuffer(const int& w = 800, const int& h = 600) {
		Width = w;
		Height = h;
		//RGBA
		colorBuffer.resize(w * h * 4, 0);
		depthBuffer.resize(w * h, 0.0f);
	}
	void Resize(const int& w, const int& h) {
		Width = w;
		Height = h;
		colorBuffer.resize(w * h * 4, 0);
		depthBuffer.resize(w * h, 0.0f);
	}

	float GetDepth(const int& x, const int& y) {
		if (x < 0 || x >= Width || y < 0 || y >= Height)
			return 1.0;
		return *(depthBuffer.data() + y * Width + x);
	}
	void WriteDepth(const int& x, const int& y, const float& depth) {
		if (x < 0 || x >= Width || y < 0 || y >= Height)
			return;
		float* p = depthBuffer.data();
		*(p + y * Width + x) = depth;
	}

	void ClearColorBuffer(const glm::vec4& color) {
		unsigned char* p = colorBuffer.data();
		for (int i = 0; i < Width * Height * 4; i += 4) {
			//
			*(p + i) = color.r;
			*(p + i + 1) = color.g;
			*(p + i + 2) = color.b;
			*(p + i + 3) = color.a;
		}
		depthBuffer.assign(Width * Height, 0.0f);
		
	}
	/*void WritePoint(const int& x, const int& y, const glm::vec4& color) {
		if (x < 0 || x >= Width || y < 0 || y >= Height)
			return;
		int xy = (y * Width + x);
		colorBuffer[xy * 4] = color.r;
		colorBuffer[xy * 4 + 1] = color.g;
		colorBuffer[xy * 4 + 2] = color.b;
		colorBuffer[xy * 4 + 3] = color.a;
	}*/

	void WritePoint(const int& x, const int& y, const glm::vec4& color) {
		if (x < 0 || x >= Width || y < 0 || y >= Height)
			return;
		int xy = (y * Width + x);
		unsigned char* p = colorBuffer.data();
		*(p + xy * 4) = saturate(color.r) * 255;
		*(p + xy * 4 + 1) = saturate(color.g) * 255;
		*(p + xy * 4 + 2) = saturate(color.b) * 255;
		*(p + xy * 4 + 3) = saturate(color.a) * 255;
	}
};