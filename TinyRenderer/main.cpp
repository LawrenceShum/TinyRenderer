#include <iostream>
#include "tgaimage.h"
#include <cmath>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 128, 0, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	for (float t = 0.; t < 1.; t += .001) {
		int x = x0 + (x1 - x0)*t;
		int y = y0 + (y1 - y0)*t;
		image.set(x, y, color);
	}
}

void circle(int orient_x, int orient_y, int radius, TGAImage &image, TGAColor color)
{
	int t = 1;
	for (int x = (orient_x - radius - 1<0 ? 0:orient_x-radius-1 ); x < (orient_x + radius + 1>500 ? 500:orient_x+radius+1) ; x++)
	{
		for (int y = (orient_y - radius - 1<0 ? 0: orient_y-radius-1); y < (orient_y + radius + 1>500 ? 500: orient_y+radius+1); y++)
		{
			double distance = sqrt((x-orient_x)*(x-orient_x) + (y-orient_y)*(y-orient_y));
			double res = (distance - radius)*(distance - radius);

			if (res < t)
				image.set(x, y, color);
		}
	}
}

int main(int argc, char** argv) {
	TGAImage image(500, 500, TGAImage::RGB);
	circle(250, 250, 100, image, green);
	//line(13, 20, 80, 40, image, red);
	image.write_tga_file("output.tga"); 
	return 0;
}
