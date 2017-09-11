#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../../library/ImageBase.h"

ImageBase 2_meansGen(int centroid_1, int centroid_2, ImageBase);
double computeEcludianDist(int centroid, int r, int g, int b);

int main (int argc, char ** argv) {
	char cNomImgLue[250];
	
	if (argc != 3) {
		printf("ERROR : arguments not complete");
		return 1;
	}
	
	int k = argv[2] - 48;
	sscanf (argv[1],"%s",cNomImgLue) ;
	
	ImageBase img;
	img.load(cNomImgLue);
	
	
	ImageBase out = 
}

ImageBase 2_meansGen(int centroid_1, int centroid_2, ImageBase img) {
	int height = img.getHeight();
	int width = img.getWidth();
	
	ImageBase imOut(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++) {
			int r = img[j*3][x*3+0]; //Red
			int g = img[j*3][x*3+1]; //Green
			int b = img[j*3][x*3+2]; //Blue
			
			int dist_c1 = (int)computeEcludianDist(centroid_1, r, g, b);
			int dist_c2 = (int)computeEcludianDist(centroid_2, r, g, b);
			if (dist_c1 < dist_c2)
				imOut[j][i] = centroid_1;
			else
				imOut[j][i] = centroid_2;
		}
	}
	
	return imOut;
}


double computeEcludianDist(int centroid, int r, int g, int b) {
	double square_sum = pow(r-centroid, 2) + pow(g-centroid, 2) + pow(g-centroid, 2);
	double dist = sqrt(square_sum);
	return dist; 
}
