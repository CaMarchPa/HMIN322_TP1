#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include "ImageBase.h"


int setColor(int r, int g, int b);
double computeEcludianDist(int* centroid, int r, int g, int b);
ImageBase imgGenBy2Means(int* centroid_1, int* centroid_2, ImageBase img);
ImageBase imgGenBy2MeansFromAVG(int* centroid_1, int* centroid_2, ImageBase img);
ImageBase imgGenByKMeans(ImageBase img, int ** palette);
int ** getPalette(ImageBase img);
bool isInPalette(int ** palette, int * color);
double psnr(ImageBase img_original, ImageBase img_edited);


int main (int argc, char ** argv) {
	char inImgName[250];
	// char outImgName[250];

	if (argc != 2) {
		printf("ERROR : arguments not complete\n");
		return 1;
	}

	sscanf (argv[1],"%s",inImgName);

	ImageBase img;
	img.load(inImgName);
	int centroid_1[3] = {255, 0, 0};
	int centroid_2[3] = {0, 255, 0};

	// ImageBase out_1 = imgGenBy2Means(centroid_1, centroid_2, img);
	// out_1.save("../img/2meansGenImg.ppm");

	// ImageBase out_2 = imgGenBy2MeansFromAVG(centroid_1, centroid_2, img);
	// out_2.save("../img/2meansGenImgAVG.ppm");
	int ** palette;
	palette = (int **) malloc(sizeof(int*) * 256);
	for (int i = 0; i < 256; i++) {
		palette[i] = (int *) malloc(sizeof(int) * 3);
	}
	palette = getPalette(img);
	for (int i = 0; i < 256; i++) {
		std::cout << "palette " << i << " : " << palette[i][0] << ", " << palette[i][1] << ", " << palette[i][2] << '\n';
	}
	ImageBase out_3 = imgGenByKMeans(img, palette);
	out_3.save("../img/kmeansGenImg.ppm");
}

ImageBase imgGenBy2Means(int* centroid_1, int* centroid_2, ImageBase img) {
	int height = img.getHeight();
	int width = img.getWidth();

	ImageBase imOut(width, height, img.getColor());

	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++) {
			int r = img[j*3][i*3+0]; //Red
			int g = img[j*3][i*3+1]; //Green
			int b = img[j*3][i*3+2]; //Blue

			int dist_c1 = (int)computeEcludianDist(centroid_1, r, g, b);
			int dist_c2 = (int)computeEcludianDist(centroid_2, r, g, b);
			if (dist_c1 < dist_c2){
				imOut[j*3][i*3+0] = centroid_1[0];
				imOut[j*3][i*3+1] = centroid_1[1];
				imOut[j*3][i*3+2] = centroid_1[2];
			} else {
				imOut[j*3][i*3+0] = centroid_2[0];
				imOut[j*3][i*3+1] = centroid_2[1];
				imOut[j*3][i*3+2] = centroid_2[2];
			}
		}
	}

	return imOut;
}

int setColor(int r, int g, int b) {
		return (255<<24) | (r  << 16) | (g << 8) | b;
}

ImageBase imgGenBy2MeansFromAVG(int* centroid_1, int* centroid_2, ImageBase img) {
	int height = img.getHeight();
	int width = img.getWidth();

	int centroid1 = setColor(centroid_1[0], centroid_1[1], centroid_1[2]);
	int centroid2 = setColor(centroid_2[0], centroid_2[1], centroid_2[2]);

	ImageBase imOut(width, height, img.getColor());
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			imOut[j*3][i*3+0] = img[j*3][i*3+0];
			imOut[j*3][i*3+1] = img[j*3][i*3+1];
			imOut[j*3][i*3+2] = img[j*3][i*3+2];
		}
	}
	int avg_c1[3] = {0, 0, 0}; // coueleur moyenne de la classe 1
	int avg_c2[3] = {0, 0, 0}; // coueleur moyenne de la classe 2
	int counter_c1 = 0; // compte le nombre d'éléments de la classe 1
	int counter_c2 = 0; // compte le nombre d'éléments de la classe 2

	do {
		for (int j = 0; j < height; j++){
			for (int i = 0; i < width; i++) {
				int r = imOut[j*3][i*3+0]; //Red
				int g = imOut[j*3][i*3+1]; //Green
				int b = imOut[j*3][i*3+2]; //Blue

				int dist_c1 = (int)computeEcludianDist(centroid_1, r, g, b);
				int dist_c2 = (int)computeEcludianDist(centroid_2, r, g, b);
				if (dist_c1 < dist_c2){
					avg_c1[0] += r;
					avg_c1[1] += g;
					avg_c1[2] += b;
					counter_c1++;
				} else {
					avg_c2[0] += r;
					avg_c2[1] += g;
					avg_c2[2] += b;
					counter_c2++;
				}
			}
		}
		// couleur moyenne de la classe 1
		avg_c1[0] /= counter_c1;
		avg_c1[1] /= counter_c1;
		avg_c1[2] /= counter_c1;

		// couleur moyenne de la classe 2
		avg_c2[0] /= counter_c2;
		avg_c2[1] /= counter_c2;
		avg_c2[2] /= counter_c2;

		for (int j = 0; j < height; j++){
			for (int i = 0; i < width; i++) {
				int r = imOut[j*3][i*3+0]; //Red
				int g = imOut[j*3][i*3+1]; //Green
				int b = imOut[j*3][i*3+2]; //Blue

				int dist_c1 = (int)computeEcludianDist(centroid_1, r, g, b);
				int dist_c2 = (int)computeEcludianDist(centroid_2, r, g, b);
				if (dist_c1 < dist_c2){
					imOut[j*3][i*3+0] = avg_c1[0];
					imOut[j*3][i*3+1] = avg_c1[1];
					imOut[j*3][i*3+2] = avg_c1[2];
				} else {
					imOut[j*3][i*3+0] = avg_c2[0];
					imOut[j*3][i*3+1] = avg_c2[1];
					imOut[j*3][i*3+2] = avg_c2[2];
				}
			}
		}

		centroid_1 = avg_c1;
		centroid_2 = avg_c2;
		centroid1 = setColor(centroid_1[0], centroid_1[1], centroid_1[2]);
		centroid2 = setColor(centroid_2[0], centroid_2[1], centroid_2[2]);
	} while (centroid1 != centroid2);

	return imOut;
}


double computeEcludianDist(int* centroid, int r, int g, int b) {
	double square_sum = pow(r - centroid[0], 2) + pow(g - centroid[1], 2) + pow(b - centroid[2], 2);
	double dist = sqrt(square_sum);
	return dist;
}

ImageBase imgGenByKMeans(ImageBase img, int ** palette) {
	int height = img.getHeight();
	int width = img.getWidth();

	ImageBase imOut(width, height, img.getColor());

	for (int j = 0; j < img.getHeight(); i++) {
		for (int i = 0; i < img.getWidth(); i++) {
			double dist_temp = 99999.0f;
			int index = 0;
			int r = img[j*3][i*3+0];
			int g = img[j*3][i*3+1];
			int b = img[j*3][i*3+2];
			for (int k = 0; k < 256; k++) {
				double current_dist = computeEcludianDist(palette[k], r, g, b);
				if ( current_dist < dist_temp ) {
					dist_temp = current_dist;
					index = k;
				}
			}
			imOut[j*3][i*3+0] = palette[index][0];
			imOut[j*3][i*3+1] = palette[index][1];
			imOut[j*3][i*3+2] = palette[index][2];
		}
	}

	return imOut;
}

int ** getPalette(ImageBase img) {
	int height = img.getHeight();
	int width = img.getWidth();

	int ** palette;
	palette = (int **) malloc(sizeof(int*) * 256);
	for (int i = 0; i < 256; i++) {
		palette[i] = (int *) malloc(sizeof(int) * 3);
		palette[i][0] = -1;
		palette[i][1] = -1;
		palette[i][2] = -1;

	}

	int counter = 0;
	// while (counter < 256) {
		// int i = rand() % (width);
		// usleep(3500);
		// int j = rand() % (height);
		// std::cout << "i = " << i << ", j = " << j << '\n';
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int r = img[j*3][i*3+0];
			int g = img[j*3][i*3+1];
			int b = img[j*3][i*3+2];
			int *color = (int *) malloc (sizeof(int)*3);
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

		// usleep(5000);
	// }
	return palette;
}

bool isInPalette(int ** palette, int * color) {
	for (int i = 0; i < 256; i++) {
		bool test = (palette[i][0] == color[0]) && (palette[i][1] == color[1]) && (palette[i][2] == color[2]);
		if(test)
			return true;
	}
	return false;
}

double psnr(ImageBase img_original, ImageBase img_edited) {
	int height = img_original.getHeight();
	int width = img_original.getWidth();

	// Erreur quadratique moyenne
	double eqm = 0.0;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int Io_ij = setColor(img_original[j*3][i*3+0], img_original[j*3][i*3+1], img_original[j*3][i*3+2]);
			int Ie_ij = setColor(img_edited[j*3][i*3+0], img_edited[j*3][i*3+1], img_edited[j*3][i*3+2]);
			eqm += pow(Io_ij - Ie_ij, 2);
		}
	}
	int d = 255;
	eqm /= height*width;
	double psnr = 10 * log10(pow(d, 2) / eqm);
	return psnr;
}
