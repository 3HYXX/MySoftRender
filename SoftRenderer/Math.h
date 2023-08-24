#pragma once

#include <glm/glm.hpp>


float Lerp(const float& f1, const float& f2, float factor) {
	return (1.0f - factor) * f1 + factor * f2;
}

glm::vec4 Lerp(const glm::vec4& v1, const glm::vec4& v2, float factor) {
	return (1.0f - factor) * v1 + factor * v2;
}
glm::vec3 Lerp(const glm::vec3& v1, const glm::vec3& v2, float factor) {
	return (1.0f - factor) * v1 + factor * v2;
}
glm::vec2 Lerp(const glm::vec2& v1, const glm::vec2& v2, float factor) {
	return (1.0f - factor) * v1 + factor * v2;
}

// glm 的矩阵是行矩阵，而一般我们用的都是列矩阵，所以存放的时候要转置
// 行矩阵做变换是  v * M  ，列矩阵是 M * v
// ox oy 是原点的坐标
// x坐标乘以w/2 ，y坐标乘以h/2，整体向左下平移
// Vp = [  w/2 ,   0  ,  0  , ox+w/2 ,
//	    0  ,  h/2 ,  0  , oy+h/2 ,
//          0  ,   0  ,  1  ,   0    ,
//          0  ,   0  ,  0  ,   1   ]
glm::mat4 GetViewPortMatrix(int ox, int oy, int width, int height) {

	glm::mat4 result = glm::mat4(1.0f);
	result[0][0] = width / 2.0f;
	result[3][0] = ox + (width / 2.0f);
	result[1][1] = height / 2.0f;
	result[3][1] = oy + (height / 2.0f);
	return result;
}

//观察矩阵
// V = R*T
// T = [  1 , 0 , 0 , -eyex          R = [  Right , 0 
//        0 , 1 , 0 , -eyey                   UP  , 0
//        0 , 0 , 1 , -eyez               - Front , 0
//        0 , 0 , 0 ,   1   ]                 0   , 1 ]
//V =  [  Right  ,  - Right·eye
//          UP   ,  - UP·eye
//       -Front  ,   Front·eye
//         0     ,       1        ]
// 世界空间一般用右手系，靠近屏幕为+Z，从摄像机视角看过去的话+Z是朝向屏幕里的
// 为了让朝向保持不变，对Front向量取反
glm::mat4 GetViewMatrix(glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up) {
	/*glm::mat4 result = glm::mat4(1.0f);
	result[0][0] = right.x;
	result[1][0] = right.y;
	result[2][0] = right.z;
	result[3][0] = -glm::dot(right, pos);
	result[0][1] = up.x;
	result[1][1] = up.y;
	result[2][1] = up.z;
	result[3][1] = -glm::dot(up, pos);
	result[0][2] = -front.x;
	result[1][2] = -front.y;
	result[2][2] = -front.z;
	result[3][2] = glm::dot(front, pos);
	return result;*/
	glm::mat4 Tview = glm::mat4(1.0f);
	Tview[3][0] = -pos.x;
	Tview[3][1] = -pos.y;
	Tview[3][2] = -pos.z;

	glm::mat4 Rview = glm::mat4(1.0f);
	Rview[0][0] = right.x;
	Rview[1][0] = right.y;
	Rview[2][0] = right.z;
	Rview[0][1] = up.x;
	Rview[1][1] = up.y;
	Rview[2][1] = up.z;
	Rview[0][2] = -front.x;
	Rview[1][2] = -front.y;
	Rview[2][2] = -front.z;

	return Rview * Tview;

}

//透视投影 参数 fov(弧度) aspect near far
//M = [   1/aspect*tan(fov/2),       0      ,         0      ,       0
//               0  ,         1/tan(fov/2)  ,         0      ,       0 
//               0  ,                0      ,  - (f+n)/(f-n) ,  -2fn/(f-n)
//               0  ,                0      ,         -1     ,       0     ]
// 投影之后从右手系变成了左手系，Z靠近屏幕的是负
glm::mat4 GetPerspectiveMatrix(const float& fovy, const float& aspect, const float& n, const float& f)
{
	/*glm::mat4 result = glm::mat4(0.0f);
	const float tanHalfFov = tan(fovy * 0.5f);
	result[0][0] = 1.0f / (aspect * tanHalfFov);
	result[1][1] = 1.0f / (tanHalfFov);
	result[2][2] = -(f + n) / (f - n);
	result[2][3] = -1.0f;
	result[3][2] = (-2.0f * n * f) / (f - n);
	return result;*/

	float t = tan(fovy * 0.5f) * abs(n);
	float b = -t;
	float r = t * aspect;
	float l = -r;

	glm::mat4 Mortho = glm::mat4(1.0f);
	Mortho[0][0] = 2 / (r - l);
	Mortho[1][1] = 2 / (t - b);
	Mortho[2][2] = 2 / (n - f);
	Mortho[3][0] = (r + l) / -2.0f;
	Mortho[3][1] = (t + b) / -2.0f;
	Mortho[3][2] = (n + f) / -2.0f;

	glm::mat4 Mpersp2ortho = glm::mat4(0);
	Mpersp2ortho[0][0] = n;
	Mpersp2ortho[1][1] = n;
	Mpersp2ortho[2][2] = n+f;
	Mpersp2ortho[3][2] = -n*f;
	Mpersp2ortho[2][3] = 1;

	return Mortho * Mpersp2ortho;

}

float saturate(const float& v) {
	if (v > 1)
		return 1;
	if (v < 0)
		return 0;
	return v;
}