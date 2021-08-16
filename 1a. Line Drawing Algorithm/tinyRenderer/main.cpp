#include "tgaimage.h"
#include <iostream>
#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (float t = 0.; t < 1.; t += .01) {
        int x = x0 * (1. - t) + x1 * t;
        int y = y0 * (1. - t) + y1 * t;
        image.set(x, y, color);
    }
}

// This always assume x0 < x1, therefore it will not render if x1 < x0
void line2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    for (int x = x0; x <= x1; x++) {
        float t = float(x - x0) / (float)(x1 - x0);
        int y = y0 * (1 - t) + y1 * t;
        image.set(x, y, color);
    }
}

// Line drawing now check if x0 < x1 before proceeding to draw
void line3(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    // Check if x0 > x1
    if (x0 > x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x < x1; x++) {
        float t = float(x - x0) / float(x1 - x0);
        int y = (1 - t) * y0 + t * y1;
        image.set(x, y, color);
    }
}

// We notice that there is asymmetry in how the lines are drawn. So we perform transpose to make it symmetric

void line4(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {

    // First check if it is steep
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        // First swap x and y
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    // Now check if x0 < x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    // Now perform line drawing algorithm
    for (int x = x0; x < x1; x++) {
        float t = float(x - x0) / float(x1 - x0);
        int y = (1 - t) * y0 + t * y1;
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }
}

// We optimize the line drawing algorithm
// (i) First, we note that in the for loop, the divisor (x1 - x0) is the same for all loops
// (ii) More importantly, we can draw a best fit line which doesnt require division.

void line5(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    // First check if the line is steep
    bool steep = false;
    if (std::abs(x0-x1) < std::abs(y0-y1)) {
        steep = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    // Now check if x0 < x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    // Now draw
    int dx = x1 - x0;
    int dy = y1 - y0;
    float gradient = std::abs(dy/float(dx));
    float error = 0;
    int y = y0;
    for (int x = x0; x < x1; x++) {
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error += gradient;

        // Reduce the number of divisions required
        if (error > .5) {
            y += (y1 > y0 ? 1 : -1);
            error -= 1;
        }
    }
}

void line6(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
            steep = true;
            std::swap(x0, y0);
            std::swap(x1, y1);
    }

        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        int dx = x1 - x0;
        int dy = y1 - y0;
        int gradient = std::abs(dy)*2;
        int error = 0;
        int y = y0;
        for (int x = x0; x < x1; x++) {
            if (steep) {
                image.set(y, x, color);
            }
            else {
                image.set(x, y, color);
            }

            error += gradient;
            if (error > dx) {
                error -= 2 * dx;
                y += (y1 > y0 ? 1 : -1);
            }
        }
}

// Line drawing algorithm that checks if x0 < x1 and perform swap if the line is steep(this will lead to sparse pixels)
void test_line_unoptimized(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    // Check if line is steep
    bool steep = false;
    if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;

    }
    // First check if x0 < x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    // Then loop through and start drawing
    float gradient = std::abs((y1 - y0)/float(x1 - x0));
    float error = 0;
    int y = y0;
    for (int x = x0; x < x1; x++) {
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }

        error += gradient;
        if (error > 0.5) {
            error -= 1;
            y += (y1 > y0 ? 1 : -1);
        }
    }
}

void test_line_optimized(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    // Check if steep
    bool steep = false;
    if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
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
int main(int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGB);
    test_line_optimized(20, 13, 40, 80, image, red);
    test_line_optimized(13, 20, 80, 40, image, white);
    test_line_optimized(80, 40, 13, 20, image, red);
    test_line_optimized(80, 40, 13, 50, image, white);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}