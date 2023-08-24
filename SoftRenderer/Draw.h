#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "V2F.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Texture.h"

class Draw {

private:
	int Width;
	int Height;
	FrameBuffer* FrontBuffer;
	Shader* shader;
	glm::mat4 ViewPortMatrix;
public:
	Draw(const int& w, const int& h) :
		Width(w), Height(h), FrontBuffer(nullptr), shader(nullptr) {}
	~Draw() {
		if (FrontBuffer)
			delete FrontBuffer;
		if (shader)
			delete shader;
		FrontBuffer = nullptr;
		shader = nullptr;
	}
	void setModelMatrix(const glm::mat4& model) {
		shader->setModelMatrix(model);
	}
	void setViewMatrix(const glm::mat4& view) {
		shader->setViewMatrix(view);
	}
	void setProjectMatrix(const glm::mat4& project) {
		shader->setProjectMatrix(project);
	}

	void setTexture(Texture* texture) {
		shader->setTexture(texture);
	}
	void Init() {
		if (FrontBuffer)
			delete FrontBuffer;
		if (shader)
			delete shader;
		ViewPortMatrix = GetViewPortMatrix(0, 0, Width, Height);
		FrontBuffer = new FrameBuffer(Width, Height);
		shader = new Shader();
	}
	void Resize(const int& w, const int& h) {
		Width = w;
		Height = h;
		FrontBuffer->Resize(w, h);
		ViewPortMatrix = GetViewPortMatrix(0, 0, w, h);
	}
	void ClearBuffer(const glm::vec4& color) {
		FrontBuffer->ClearColorBuffer(color);
	}
	void Show() {
		glDrawPixels(Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, FrontBuffer->colorBuffer.data());
	}

	void PerspectiveDivision(V2F& v) {
		v.windowPos /= v.windowPos.w;
		v.windowPos.w = 1.0f;
		// OpenGL的Z-Buffer是 [0,1]，而透视除法之后Z除了深度测试已经没用了
		v.windowPos.z = (v.windowPos.z + 1.0) * 0.5;
	}

	void DrawTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
		V2F o1 = shader->VertexShader(v1);
		V2F o2 = shader->VertexShader(v2);
		V2F o3 = shader->VertexShader(v3);

		PerspectiveDivision(o1);
		PerspectiveDivision(o2);
		PerspectiveDivision(o3);

		o1.windowPos = ViewPortMatrix * o1.windowPos;
		o2.windowPos = ViewPortMatrix * o2.windowPos;
		o3.windowPos = ViewPortMatrix * o3.windowPos;
		ScanLineTriangle(o1, o2, o3);
	}

	bool FaceCulling(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3) {

		glm::vec3 tmp1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
		glm::vec3 tmp2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);

		glm::vec3 normal = glm::normalize(glm::cross(tmp1, tmp2));
		//在NDC下，观察方向 是0,0,1
		glm::vec3 view = glm::vec3(0, 0, -1);
		return glm::dot(normal, view) < 0;
	}

	void DrawMesh(const Mesh& mesh) {
		if (mesh.EBO.empty()) {
			return;
		}
		for (int i = 0; i < mesh.EBO.size(); i += 3) {
			Vertex p1, p2, p3;
			p1 = mesh.VBO[mesh.EBO[i]];
			p2 = mesh.VBO[mesh.EBO[i + 1]];
			p3 = mesh.VBO[mesh.EBO[i + 2]];

			V2F v1, v2, v3;
			v1 = shader->VertexShader(p1);
			v2 = shader->VertexShader(p2);
			v3 = shader->VertexShader(p3);

			PerspectiveDivision(v1);
			PerspectiveDivision(v2);
			PerspectiveDivision(v3);

			if (!FaceCulling(v1.windowPos, v2.windowPos, v3.windowPos)) {
				continue;
			}

			v1.windowPos = ViewPortMatrix * v1.windowPos;
			v2.windowPos = ViewPortMatrix * v2.windowPos;
			v3.windowPos = ViewPortMatrix * v3.windowPos;
			ScanLineTriangle(v1, v2, v3);
		}
	}

	//光栅化的函数也放在这里，把里面对FrameBuffer的操作修改一下
	//扫描线填充算法
   //对任意三角形，分为上下两个平底三角形填充
	void ScanLineTriangle(const V2F& v1, const V2F& v2, const V2F& v3) {
		std::vector<V2F> arr = { v1,v2,v3 };
		if (arr[0].windowPos.y > arr[1].windowPos.y) {
			V2F tmp = arr[0];
			arr[0] = arr[1];
			arr[1] = tmp;
		}
		if (arr[1].windowPos.y > arr[2].windowPos.y) {
			V2F tmp = arr[1];
			arr[1] = arr[2];
			arr[2] = tmp;
		}
		if (arr[0].windowPos.y > arr[1].windowPos.y) {
			V2F tmp = arr[0];
			arr[0] = arr[1];
			arr[1] = tmp;
		}
		//arr[0] 在最下面  arr[2]在最上面
		//中间跟上面的相等，是底三角形
		if (arr[1].windowPos.y == arr[2].windowPos.y) {
			DownTriangle(arr[1], arr[2], arr[0]);
		}//顶三角形
		else if (arr[1].windowPos.y == arr[0].windowPos.y) {
			UpTriangle(arr[1], arr[0], arr[2]);
		}
		else {
			//插值求出中间点对面的那个点，划分为两个新的三角形
			float weight = (arr[2].windowPos.y - arr[1].windowPos.y) / (arr[2].windowPos.y - arr[0].windowPos.y);
			V2F newEdge = V2F::lerp(arr[2], arr[0], weight);
			UpTriangle(arr[1], newEdge, arr[2]);
			DownTriangle(arr[1], newEdge, arr[0]);
		}
	}

	void UpTriangle(const V2F& v1, const V2F& v2, const V2F& v3) {
		V2F left, right, top;
		left = v1.windowPos.x > v2.windowPos.x ? v2 : v1;
		right = v1.windowPos.x > v2.windowPos.x ? v1 : v2;
		top = v3;
		left.windowPos.x = int(left.windowPos.x);
		int dy = top.windowPos.y - left.windowPos.y;
		int nowY = top.windowPos.y;
		//从上往下插值
		for (int i = dy; i >= 0; i--) {
			float weight = 0;
			if (dy != 0) {
				weight = (float)i / dy;
			}
			V2F newLeft = V2F::lerp(left, top, weight);
			V2F newRight = V2F::lerp(right, top, weight);
			newLeft.windowPos.x = int(newLeft.windowPos.x);
			newRight.windowPos.x = int(newRight.windowPos.x + 0.5);
			newLeft.windowPos.y = newRight.windowPos.y = nowY;
			ScanLine(newLeft, newRight);
			nowY--;
		}
	}
	void DownTriangle(const V2F& v1, const V2F& v2, const V2F& v3) {
		V2F left, right, bottom;
		left = v1.windowPos.x > v2.windowPos.x ? v2 : v1;
		right = v1.windowPos.x > v2.windowPos.x ? v1 : v2;
		bottom = v3;
		int dy = left.windowPos.y - bottom.windowPos.y;
		int nowY = left.windowPos.y;
		//从上往下插值
		for (int i = 0; i < dy; i++) {
			float weight = 0;
			if (dy != 0) {
				weight = (float)i / dy;
			}
			V2F newLeft = V2F::lerp(left, bottom, weight);
			V2F newRight = V2F::lerp(right, bottom, weight);
			newLeft.windowPos.x = int(newLeft.windowPos.x);
			newRight.windowPos.x = int(newRight.windowPos.x + 0.5);
			newLeft.windowPos.y = newRight.windowPos.y = nowY;
			ScanLine(newLeft, newRight);
			nowY--;
		}
	}
	void ScanLine(const V2F& left, const V2F& right) {
		int length = right.windowPos.x - left.windowPos.x;
		for (int i = 0; i < length; i++) {
			V2F v = V2F::lerp(left, right, (float)i / length);
			v.windowPos.x = left.windowPos.x + i;
			v.windowPos.y = left.windowPos.y;

			//FrontBuffer->WritePoint(v.windowPos.x, v.windowPos.y, shader->FragmentShader(v));
			//深度测试[0,1],越大约靠近屏幕
			float depth = FrontBuffer->GetDepth(v.windowPos.x, v.windowPos.y);
			if (v.windowPos.z > depth) {

				float z = v.Z;
				v.worldPos /= z;
				v.texcoord /= z;
				v.normal /= z;
				v.color /= z;

				FrontBuffer->WritePoint(v.windowPos.x, v.windowPos.y, shader->FragmentShader(v));
				FrontBuffer->WriteDepth(v.windowPos.x, v.windowPos.y, v.windowPos.z);
			}
			
		}
	}

};

Mesh CreatePlane(const glm::vec3& leftTop, const glm::vec3& leftBottom, const glm::vec3& rightBottom, const glm::vec3& rightTop, const glm::vec3& normal) {
	Mesh result(4, 6);
	result.VBO[0].position = glm::vec4(leftTop, 1.0f);
	result.VBO[0].normal = normal;
	result.VBO[0].color = glm::vec4(255, 0, 0, 255);
	result.VBO[0].texcoord = glm::vec2(0.0f, 1.0f);
	result.VBO[1].position = glm::vec4(rightTop, 1.0f);
	result.VBO[1].normal = normal;
	result.VBO[1].color = glm::vec4(0, 255, 0, 255);
	result.VBO[1].texcoord = glm::vec2(1.0f, 1.0f);
	result.VBO[2].position = glm::vec4(rightBottom, 1.0f);
	result.VBO[2].normal = normal;
	result.VBO[2].color = glm::vec4(0, 0, 255, 0);
	result.VBO[2].texcoord = glm::vec2(1.0f, 0.0f);
	result.VBO[3].position = glm::vec4(leftBottom, 1.0f);
	result.VBO[3].normal = normal;
	result.VBO[3].color = glm::vec4(255, 0, 255, 255);
	result.VBO[3].texcoord = glm::vec2(0.0f, 0.0f);
	//绘制三角形的顺序是 左上->右下->右下 左上->左下->右上 都是逆时针方向 
	result.EBO[0] = 0;
	result.EBO[1] = 2;
	result.EBO[2] = 1;
	result.EBO[3] = 0;
	result.EBO[4] = 3;
	result.EBO[5] = 2;
	return result;
}

Mesh CreateBox(const glm::vec3& center, float radius) {
	Mesh result;
	Mesh front = CreatePlane(
		center + glm::vec3(-radius, radius, radius),
		center + glm::vec3(-radius, -radius, radius),
		center + glm::vec3(radius, -radius, radius),
		center + glm::vec3(radius, radius, radius),
		glm::vec3(0, 0, 1)
	);
	result.AddMesh(front);

	Mesh left = CreatePlane(
		center + glm::vec3(-radius, radius, -radius),
		center + glm::vec3(-radius, -radius, -radius),
		center + glm::vec3(-radius, -radius, radius),
		center + glm::vec3(-radius, radius, radius),
		glm::vec3(-1, 0, 0)
	);
	result.AddMesh(left);

	Mesh right = CreatePlane(
		center + glm::vec3(radius, radius, radius),
		center + glm::vec3(radius, -radius, radius),
		center + glm::vec3(radius, -radius, -radius),
		center + glm::vec3(radius, radius, -radius),
		glm::vec3(1, 0, 0)
	);
	result.AddMesh(right);

	Mesh back = CreatePlane(
		center + glm::vec3(radius, radius, -radius),
		center + glm::vec3(radius, -radius, -radius),
		center + glm::vec3(-radius, -radius, -radius),
		center + glm::vec3(-radius, radius, -radius),
		glm::vec3(0, 0, -1)
	);
	result.AddMesh(back);

	Mesh up = CreatePlane(
		center + glm::vec3(-radius, radius, -radius),
		center + glm::vec3(-radius, radius, radius),
		center + glm::vec3(radius, radius, radius),
		center + glm::vec3(radius, radius, -radius),
		glm::vec3(0, 1, 0)
	);
	result.AddMesh(up);

	Mesh down = CreatePlane(
		center + glm::vec3(-radius, -radius, radius),
		center + glm::vec3(-radius, -radius, -radius),
		center + glm::vec3(radius, -radius, -radius),
		center + glm::vec3(radius, -radius, radius),
		glm::vec3(0, -1, 0)
	);
	result.AddMesh(down);

	return result;
}