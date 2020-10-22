#include <iostream>
#include <cmath>
#include "model.h"
#include "tgaimage.h"
#include "lodepng.h"

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
int main(int argc, char** argv) 
{
	int width = 800;
	int height = 800;

	//TGAImage image(width, height, TGAImage::RGB);
	
	Vec2i a(200, 250);
	Vec2i b(700, 470);
	Vec2i c(550, 680);
	//triangle(a, b, c, image, red);

	//image.flip_vertically();
	//image.write_tga_file("output.tga"); 
	return 0;
}*/


void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) 
{
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

int main(int argc, char** argv)
{
	const char* filename = argc > 1 ? argv[1] : "test.png";

	unsigned width = 1024, height = 1024;

	std::vector<unsigned char> image;

	image.resize(width * height * 4);

	for (unsigned y = 0; y < height; y++)
	
		for (unsigned x = 0; x < width; x++) {
		image[4 * width * y + 4 * x + 0] = 255 * !(x & y);
		image[4 * width * y + 4 * x + 1] = x ^ y;
		image[4 * width * y + 4 * x + 2] = x | y;
		image[4 * width * y + 4 * x + 3] = 255;
		}
	encodeOneStep(filename, image, width, height);
}
