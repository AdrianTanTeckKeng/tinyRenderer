#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255); 
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const int width  = 200;
const int height = 200;

// We will create a wire-frame renderer from our line drawing algorithm

void line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color) {

	// Unpack all variables
	int x0 = t0.x;
	int y0 = t0.y;
	int x1 = t1.x;
	int y1 = t1.y;


	// Check if steep
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y1 - y0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	// Check if x0 < x1
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int gradient = std::abs(dy);
	int error = 0;
	int y = y0;

	for (int x = x0; x < x1; x++) {
		if (steep) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}

		error += gradient * 2;
		if (error > dx) {
			error -= 2 * dx;
			y += (y1 > y0 ? 1 : -1);
		}
	}
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	// Sort the indices
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);

	int totalheight = t2.y - t0.y;
	int segmentheight = t1.y - t0.y + 1;

	for (int y = t0.y; y <= t1.y; y++) {
		// Compute alpha, beta(alpha will range from 0 to 1)
		float alpha = (float)(y - t0.y) / totalheight;
		float beta = (float)(y - t0.y) / segmentheight;

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = t0 + (t1 - t0) * beta;
		if (A.x > B.x) std::swap(A, B);
		//std::cout << A.y << " " << B.y << " " << y << std::endl;
		//line(A, B, image, red);
		for (int j = A.x; j < B.x; j++) {
			image.set(j, y, color);
		}
	}

	segmentheight = t2.y - t1.y + 1;
	for (int y = t1.y; y <= t2.y; y++) {
		// Compute alpha, beta(alpha will range from 0 to 1)
		float alpha = (float)(y - t0.y) / totalheight;
		float beta = (float)(y - t1.y) / segmentheight;

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = t1 + (t2 - t1) * beta;
		if (A.x > B.x) std::swap(A, B);
		//std::cout << A.y << " " << B.y << " " << y << std::endl;
		//line(A, B, image, red);
		for (int j = A.x; j < B.x; j++) {
			image.set(j, y, color);
		}
	}
}

void triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	// First rearrange all the vertices in ascending order of y
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);

	// Now raster the triangle. We draw it in two stages.
	int totalheight = t2.y - t0.y;
	int segmentheight = t1.y - t0.y;
	// We check if the y pixels are the same
	bool sameheight = false;
	if (segmentheight == 0) sameheight = true;
	if (sameheight) {
		int A = t0.x;
		int B = t1.x;
		if (A > B) std::swap(A, B);
		for (int j = A; j <= B; j++) {
			image.set(j, t0.y, color);
		}
	}
	else {
		for (int y = t0.y; y < t1.y; y++) {
			float alpha = float(y - t0.y) / float(totalheight);
			float beta = float(y - t0.y) / float(segmentheight);

			Vec2i A = t0 + (t2 - t0) * alpha;
			Vec2i B = t0 + (t1 - t0) * beta;
			if (A.x > B.x) std::swap(A, B);
			for (int j = A.x; j <= B.x; j++) {
				image.set(j, y, color);
			}
		}

	}
	
	segmentheight = t2.y - t1.y; // Added +1 to avoid dividing by zero
	sameheight = false;
	if (segmentheight == 0) sameheight = true;

	if (sameheight) {
		int A = t1.x;
		int B = t2.x;
		if (A > B) std::swap(A, B);
		for (int j = A; j <= B; j++) {
			image.set(j, t1.y, color);
		}
	}
	else {
		for (int y = t1.y; y < t2.y; y++) {
			float alpha = float(y - t0.y) / float(totalheight);
			float beta = float(y - t1.y) / float(segmentheight);

			Vec2i A = t0 + (t2 - t0) * alpha;
			Vec2i B = t1 + (t2 - t1) * beta;
			if (A.x > B.x) std::swap(A, B);
			for (int j = A.x; j <= B.x; j++) {
				image.set(j, y, color);
			}
		}
	}
	
}

void triangle_condensed(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	if (t0.y == t1.y && t0.y == t2.y) return; // No need to draw degenerate triangles
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	int total_height = t2.y - t0.y;
	for (int i = 0; i < total_height; i++) {
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			image.set(j, t0.y + i, color);
		}
	}
}

int main(int argc, char** argv) {
	TGAImage  image(width, height, TGAImage::RGB);
	
	Vec2i to[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
	Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
	Vec2i t2[3] = {Vec2i(180, 160), Vec2i(120, 160), Vec2i(130, 180)};
	Vec2i t3[3] = { Vec2i(180, 160), Vec2i(120, 160), Vec2i(130, 100) };
	triangle_condensed(to[0], to[1], to[2], image, red);
	triangle_condensed(t1[0], t1[1], t1[2], image, white);
	triangle_condensed(t2[0], t2[1], t2[2], image, green);
	//triangle2(t3[0], t3[1], t3[2], image, blue);
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}