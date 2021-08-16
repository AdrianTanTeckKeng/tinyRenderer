#include <vector>
#include <cmath>
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255); 
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width  = 200;
const int height = 200;


int main(int argc, char** argv) {
	TGAImage  image(width, height, TGAImage::RGB);

	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}