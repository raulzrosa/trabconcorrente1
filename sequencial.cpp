/*
	g++ -o smooth smooth.cpp `pkg-config --cflags --libs opencv`
	g++ -o sequencial sequencial.cpp `pkg-config --cflags --libs opencv`
*/
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>


using namespace cv;
using namespace std;

Mat *aplica_smooth(Mat *in) {

	//copia imagem de entrada 
	Mat *out = new Mat(in->size(), CV_8U, 1);
	Mat aux(in->size(), CV_8U, 1);

	//border = 1 pois tamanho da máscara = 3 => floor(3/2) = 1
	int border = 1;
	
	//replica a borda pra solucionar o problema dos pixels de borda
	copyMakeBorder(*in, aux, border, border, border, border, BORDER_REPLICATE);

	//achar a média aritmética e depois atualizar o pixel
	float average;

	for(int i = border; i < in->size().height - border; i++) {
		for(int j = border; j < in->size().width - border; j++) {
			//nao sei se tenho que somar como uchar ou float
			average = in->at<uchar>(i, j) + in->at<uchar>(i + 1, j) + in->at<uchar>(i - 1, j) 
			+ in->at<uchar>(i, j + 1) + in->at<uchar>(i, j - 1) + in->at<uchar>(i + 1, j + 1) + 
			in->at<uchar>(i - 1, j - 1) + in->at<uchar>(i + 1, j - 1) + in->at<uchar>(i - 1, j + 1);
			average = average/9;
		//	printf("%f ",average);	
			out->at<uchar>(i, j) = (uchar)average;
		}
	}
	return out;
}

int main(int argc, char *argv[]) {
	//arquivo de entrada
	const char *fileIn;
	//matriz com a imagem de entrada
	Mat in_grayscale;
	//matriz que receberá a imagem de saida
	Mat *out_grayscale;
	//le o nome da imagem
	//fileIn = argv[1];
	//le e salva a imagem na matriz
	in_grayscale = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	//caso nao consegui abrir a imagem
	if (in_grayscale.empty()) {
		cout << "Nao foi possivel abrir a  imagem: " << endl;
		return -1;
	}

  
	//aplica algoritmo smooth e recebe a nova imagem
	out_grayscale = aplica_smooth(&in_grayscale);
	//mostra imagem original
	namedWindow("original", CV_WINDOW_AUTOSIZE);
    imshow("original", in_grayscale);

	//mostra imagem na tela
	namedWindow("smooth", CV_WINDOW_AUTOSIZE );
    imshow("smooth", *out_grayscale);

    waitKey(0);
    return 0;
}
