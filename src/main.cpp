#include "CImg.h"
#include <iostream>

extern float const EPSILON = 0.0001;

using namespace cimg_library;

float ** getPalette(CImg<float> img);
bool isInPalette(float ** palette, float * color);
CImg<float> imgGenByKMeans(CImg<float> img, float ** palette);
float computeEcludianDist(float* centroid, float r, float g, float b);
float * getAVG(CImg<float> img, float * color);

int main(int argc, char const *argv[]) {
    CImg<float> img_read;

    if (argc != 2) {
		printf("ERROR : arguments not complete\n");
		return 1;
	}
    img_read.load(argv[1]);
    float ** palette = getPalette(img_read);

    CImg<float> out = imgGenByKMeans(img_read, palette);
    double psnr = img_read.PSNR(out);
    std::cout << "Le psnr avec est de : " << psnr << '\n';

    out.save_png("../img/kmeansResult.png");
    return 0;
}

float ** getPalette(CImg<float> img) {
    int height = img.height();
	int width = img.width();

	float ** palette;
	palette = (float **) malloc(sizeof(float*) * 256);
	for (int i = 0; i < 256; i++) {
		palette[i] = (float *) malloc(sizeof(float) * 3);
		palette[i][0] = -1;
		palette[i][1] = -1;
		palette[i][2] = -1;
	}

    int counter = 0;
    for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float r = img(i, j, 0, 0);
			float g = img(i, j, 0, 1);
			float b = img(i, j, 0, 2);
			float *color = (float *) malloc (sizeof(float)*3);
			color[0] = r;
			color[1] = g;
			color[2] = b;
			if (!isInPalette(palette, color) && counter < 256) {
				palette[counter][0] = r;
				palette[counter][1] = g;
				palette[counter][2] = b;
				counter++;
			}
		}
	}
    return palette;
}

bool isInPalette(float ** palette, float * color) {
    for (int i = 0; i < 256; i++) {
        bool test = (palette[i][0] == color[0]) && (palette[i][1] == color[1]) && (palette[i][2] == color[2]);
        if (test) {
            return true;
        }
    }
    return false;
}

CImg<float> imgGenByKMeans(CImg<float> img, float ** palette) {
    int width = img.width();
    int height = img.height();
    CImg<float> imOut(height, width, 1, 3);
    CImg<float> imgAVG = img;

    std::cout << "test 1" << '\n';
    bool is_convergent = false;
    int number_of_loop = 0;
    float ** avg_list;
    avg_list = (float **) malloc(sizeof(float *) * 256);
    for (int i = 0; i < 256; i++) {
        avg_list[i] = (float *) malloc(sizeof(float) * 3);
    }
    int * nb_of_pixel;
    nb_of_pixel = (int *) malloc(sizeof(int) * 256);
    do {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                float current_dist = 999999.0f;
                int index = 0;
                //
                float r = imgAVG(i, j, 0, 0);
                float g = imgAVG(i, j, 0, 1);
                float b = imgAVG(i, j, 0, 2);
                //
                for (int k = 0; k < 256; k++) {
                    float dist_temp = computeEcludianDist(palette[k], r, g, b);
                    if (dist_temp < current_dist) {
                        current_dist = dist_temp;
                        index = k;
                    }
                }
                //
                avg_list[index][0] += imgAVG(i, j, 0, 0);
                avg_list[index][1] += imgAVG(i, j, 0, 1);
                avg_list[index][2] += imgAVG(i, j, 0, 2);
                //
                nb_of_pixel[index]++;
                //
                imOut(i, j, 0, 0) = palette[index][0];
                imOut(i, j, 0, 1) = palette[index][1];
                imOut(i, j, 0, 2) = palette[index][2];
            }
        }

        for (int i = 0; i < 256; i++) {
            avg_list[i][0] /= nb_of_pixel[i];
            avg_list[i][1] /= nb_of_pixel[i];
            avg_list[i][2] /= nb_of_pixel[i];
        }

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                float dist_temp = 999999.0f;
                int index = 0;
                //
                float r = imOut(i, j, 0, 0);
                float g = imOut(i, j, 0, 1);
                float b = imOut(i, j, 0, 2);
                //
                for (int k = 0; k < 256; k++) {
                    float current_dist = computeEcludianDist(palette[k], r, g, b);
                    if (current_dist < dist_temp) {
                        dist_temp = current_dist;
                        index = k;
                    }
                }
                //
                imgAVG(i, j, 0, 0) = avg_list[index][0];
                imgAVG(i, j, 0, 1) = avg_list[index][1];
                imgAVG(i, j, 0, 2) = avg_list[index][2];
            }
        }

        is_convergent = true;
        for (int i = 0; i < 256; i++) {
            float dist = computeEcludianDist(palette[i], avg_list[i][0], avg_list[i][1], avg_list[i][2]);
            is_convergent = is_convergent && (dist <= EPSILON);
        }
        delete palette;
        palette = getPalette(imgAVG);
        number_of_loop++;
        std::cout << "number of loop : " << number_of_loop << '\n';
    } while (!is_convergent);
    std::cout << "test final" << '\n';
    imgAVG.save_png("../img/avg.png");
    return imOut;
}

float computeEcludianDist(float* centroid, float r, float g, float b) {
    float square_sum = pow(r - centroid[0], 2) + pow(g - centroid[1], 2) + pow(b - centroid[2], 2);
    return sqrt(square_sum);
}

float * getAVG(CImg<float> img, float * color) {
    int width = img.width();
    int height = img.height();
    float * avg;
    avg = (float *) malloc(sizeof(float) * 3);
    int counter = 0;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            bool test = (img(i, j, 0, 0) == color[0]) && (img(i, j, 0, 1) == color[1]) && (img(i, j, 0, 2) == color[2]);
            if (test) {
                avg[0] += img(i, j, 0, 0);
                avg[1] += img(i, j, 0, 1);
                avg[2] += img(i, j, 0, 2);
                counter++;
            }
        }
    }
    avg[0] /= counter;
    avg[1] /= counter;
    avg[2] /= counter;

    return avg;
}
