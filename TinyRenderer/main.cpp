#include <iostream>
#include "tgaimage.h"
#include <cmath>
#include "model.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 128, 0, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);

Model *model = NULL;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) 
{
	int x0 = t0.x;
	int x1 = t1.x;
	int y0 = t0.y;
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
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void circle(int orient_x, int orient_y, int radius, TGAImage &image, TGAColor initialColor)
{
	int t = 1;
	TGAColor color;
	for (int x = (orient_x - radius - 1<0 ? 0:orient_x-radius-1 ); x < (orient_x + radius + 1>500 ? 500:orient_x+radius+1) ; x++)
	{
		for (int y = (orient_y - radius - 1<0 ? 0: orient_y-radius-1); y < (orient_y + radius + 1>500 ? 500: orient_y+radius+1); y++)
		{
			double distance = sqrt((x-orient_x)*(x-orient_x) + (y-orient_y)*(y-orient_y));
			double res = (distance - radius)*(distance - radius);
			//color = TGAColor(0, 0, 0, 255);


			if (distance < radius)
			{
				color = TGAColor(int((radius-distance)*(255 / radius)), 0, 0, 255);
				image.set(x, y, color);
			}
		}
	}
}

void triangle(Vec2i a, Vec2i b, Vec2i c, TGAImage &image, TGAColor color)
{
	line(a, b, image, color);
	line(b, c, image, color);
	line(c, a, image, color);
}
/*
void fillTriangle(Vec2i a, Vec2i b, Vec2i c, TGAImage &image, TGAColor color)
{
	int left = min(a.x, b.x, c.x);
	int right = max(a.x, b.x, c.x);
	int top = max(a.y, b.y, c.y);
	int button = min(a.y, b.y, c.y);

	for (int j = button - 1; j <= top + 1; j++)
	{
		for (int i = left - 1; i <= right + 1; i++)
		{

		}
	}
}*/

int main(int argc, char** argv) 
{
	int width = 800;
	int height = 800;
	/*
	if (2 == argc) 
	{
		model = new Model(argv[1]);
	}
	else {
		model = new Model("obj/african_head.obj");
	}*/

	TGAImage image(width, height, TGAImage::RGB);
	//circle(250, 250, 100, image, white);
	//line(400, 350, 80, 40, image, white);
	/*for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.)*width / 2.;
			int y0 = (v0.y + 1.)*height / 2.;
			int x1 = (v1.x + 1.)*width / 2.;
			int y1 = (v1.y + 1.)*height / 2.;
			line(x0, y0, x1, y1, image, green);
		}
	}*/
	Vec2i a(200, 250);
	Vec2i b(700, 470);
	Vec2i c(550, 680);
	triangle(a, b, c, image, red);

	//image.flip_vertically();
	image.write_tga_file("output.tga"); 
	return 0;
}
