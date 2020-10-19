#include <iostream>
#include "tgaimage.h"
#include <cmath>

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 128, 0, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
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

int main(int argc, char** argv) {
	TGAImage image(500, 500, TGAImage::RGB);
	//circle(250, 250, 100, image, white);
	line(400, 350, 80, 40, image, white);
	image.write_tga_file("output.tga"); 
	return 0;
}
