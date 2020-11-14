#include <iostream>
#include <vector>
#include "model.h"
#include "lodepng.h"
#include "geometry.h"

using namespace std;
Model *model = NULL;

//输出图像的大小
unsigned width = 1024, height = 1024;
int particle_size = 10000;

struct color {
	int R;
	int G;
	int B;
	int A;
};

struct position {
	int x;
	int y;
	int z;
};

/*********************************/
/************定义颜色*************/
/*********************************/
color white = { 255,255,255,255 };
color red = { 255,0,0,255 };
color green = { 0,128,0,255 };
color yellow = { 255,255,0,255 };

Vec3f cr(Vec3f a, Vec3f b)
{
	Vec3f result;

	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

//设置像素点的着色
void set_point(vector<unsigned char>& image, int x_i, int y_i, color C)
{
	int x = x_i;
	int y = height - y_i;
	image[4 * width * y + 4 * x + 0] = C.R;
	image[4 * width * y + 4 * x + 1] = C.G;
	image[4 * width * y + 4 * x + 2] = C.B;
	//image[4 * width * y + 4 * x + 3] = C.A;
}

void initial(vector<unsigned char>& image)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			image[4 * y*width + 4 * x + 3] = 255;
		}
	}
}

//画直线
void line(Vec2i t0, Vec2i t1, vector<unsigned char> &image, color color) 
{
	int x0 = t0.x;
	int y0 = t0.y;
	int x1 = t1.x;
	int y1 = t1.y;

	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			//image.set(y, x, color);
			set_point(image, y, x, color);
		}
		else {
			//image.set(x, y, color);
			set_point(image, x, y, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

//画圆形
/*void circle(int orient_x, int orient_y, int radius, vector<unsigned char> &image, color color)
{
	int t = 1;

	for (int x = (orient_x - radius - 1<0 ? 0:orient_x-radius-1 ); x < (orient_x + radius + 1>width ? width:orient_x+radius+1) ; x++)
	{
		for (int y = (orient_y - radius - 1<0 ? 0: orient_y-radius-1); y < (orient_y + radius + 1>height ? height: orient_y+radius+1); y++)
		{
			double distance = sqrt((x-orient_x)*(x-orient_x) + (y-orient_y)*(y-orient_y));
			double res = (distance - radius)*(distance - radius);
			//color = TGAColor(0, 0, 0, 255);
			//set_point(image, x, y, color);
			
			if (distance < radius)
			{
				set_point(image, x, y, color);
			}
			if (res < t)
			{
				set_point(image, x, y, color);
			}
		}
	}
}*/

Vec3f barycentric(Vec2i* pts, Vec2i P)
{
	Vec3f u = cr(Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x), Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

//画三角形
void triangle(Vec3f* pts, float* zbuffer, vector<unsigned char> &image, color color)
{
//	line(a, b, image, color);
//	line(b, c, image, color);
//	line(c, a, image, color);

	Vec2f bboxmin(width - 1, height - 1);
	Vec2f bboxmax(0, 0);
	Vec2f clamp(width - 1, height - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0)
			{
				bboxmin.x = std::max(0.f, std::min(bboxmin.x, pts[i].x));
				bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
			}
			else
			{
				bboxmin.y = std::max(0.f, std::min(bboxmin.y, pts[i].y));
				bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
			}
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec2i PP;
			PP.x = P.x;
			PP.y = P.y;
			Vec2i p[2];
			p[0].x = pts[0].x;
			p[0].y = pts[0].y;
			p[1].x = pts[1].x;
			p[1].y = pts[1].y;
			p[2].x = pts[2].x;
			p[2].y = pts[2].y;

			Vec3f bc_screen = barycentric(p, PP);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++)
			{
				if (i == 0)
					P.z += pts[i].z * bc_screen.x;
				else if (i == 1)
					P.z += pts[i].z * bc_screen.y;
				else 				
					P.z += pts[i].z * bc_screen.z;
			}
			if (zbuffer[int(P.x + P.y * width)] < P.z) {
				zbuffer[int(P.x + P.y * width)] = P.z;
				//image.set(P.x, P.y, color);
				set_point(image, PP.x, PP.y, color);
			}
		}
	}
}

//编码PNG格式的图片
void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) 
{
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

//画model
int main(int argc, char** argv)
{
	const char* filename = argc > 1 ? argv[1] : "test.png";
	
	float* zbuffer = new float[width * height];

	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("obj/african_head.obj");
	}

	//这是一张画布
	std::vector<unsigned char> image;

	image.resize(width * height * 4);
	//把画布变黑色
	initial(image);

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		Vec3f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			Vec3f world_coords = model->vert(face[j]);
			screen_coords[j] = Vec3f((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2., world_coords.z);
		}

		color c = { rand()%255, rand()%255, rand()%255, 255 };
		triangle(screen_coords, zbuffer, image, c);
	}

	//circle(500, 500, 200, image, red);
	encodeOneStep(filename, image, width, height);

	return 0;
}