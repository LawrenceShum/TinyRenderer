#include <iostream>
#include <cmath>
#include "model.h"
#include "tgaimage.h"
#include "lodepng.h"

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
void circle(int orient_x, int orient_y, int radius, vector<unsigned char> &image, color color)
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
			
			/*if (distance < radius)
			{
				set_point(image, x, y, color);
			}*/
			if (res < t)
			{
				set_point(image, x, y, color);
			}
		}
	}
}

//画三角形
void triangle(Vec2i a, Vec2i b, Vec2i c, vector<unsigned char> &image, color color)
{
	line(a, b, image, color);
	line(b, c, image, color);
	line(c, a, image, color);
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
/*
int main(int argc, char** argv)
{
	const char* filename = argc > 1 ? argv[1] : "test.png";
	
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

	//画obj模型
	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++)
		{
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.)*width / 2.;
			int y0 = (v0.y + 1.)*height / 2.;
			int x1 = (v1.x + 1.)*width / 2.;
			int y1 = (v1.y + 1.)*height / 2.;
			Vec2i p0(x0, y0);
			Vec2i p1(x1, y1);
			line(p0, p1, image, green);
		}
	}	

	//circle(500, 500, 200, image, red);
	encodeOneStep(filename, image, width, height);

	return 0;
}*/

//计算粒子运动后更新的位置
void dynamic(vector<Vec2i>& particles, double dt)
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].x += 10;
	//	particles[i].y += 10;
	}
}

//画流体
int main(int argc, char** argv)
{
	double dt = 1;

	vector<Vec2i> particles;
	particles.resize(particle_size);

	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			particles[j*100+i].x = 200 + j;
			particles[j*100+i].y = 200 + i;
		}
	}

	for (int t = 0; t < 1000; t++)
	{
		char p[] = "0000.png";
		unsigned thousand = (unsigned)(t / 1000);
		p[0] = (char)('0' + thousand);
		unsigned hundred = (unsigned)((t - thousand * 1000) / 100);
		p[1] = (char)('0' + hundred);
		unsigned ten = (unsigned)((t-thousand*1000-hundred*100)/10);
		p[2] = (char)('0' + ten);
		unsigned unit = (unsigned)(t - thousand * 1000 - hundred * 100 - ten * 10);
		p[3] = (char)('0' + unit);
 		//char c = (char)('0' + t);
		//p[5] = c;

		const char* filename = p;
		std::vector<unsigned char> image;
		image.resize(width * height * 4);
		//把画布变黑色
		initial(image);

		for (int i = 0; i < particle_size; i++)
		{
			set_point(image, particles[i].x, particles[i].y, red);
		}

		dynamic(particles, dt);
		
		encodeOneStep(filename, image, width, height);
	}
}

