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

Mat *aplica_smooth_grayscale(Mat *in) {

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

Mat *aplica_smooth_color(Mat *in) {

	//copia imagem de entrada 
	Mat *out = new Mat(in->size(), CV_8UC3, 1);
	Mat aux(in->size(), CV_8UC3, 1);

	//border = 1 pois tamanho da máscara = 3 => floor(3/2) = 1
	int border = 1;
	
	//replica a borda pra solucionar o problema dos pixels de borda
	copyMakeBorder(*in, aux, border, border, border, border, BORDER_REPLICATE);

	//achar a média aritmética e depois atualizar o pixel
	float average;

	for(int i = border; i < in->size().height - border; i++) {
		for(int j = border; j < in->size().width - border; j++) {
			//nao sei se tenho que somar como uchar ou float]
			for(int color = 0; color < 3; color++) {
				average = in->at<cv::Vec3b>(i, j)[color] + in->at<cv::Vec3b>(i + 1, j)[color] + in->at<cv::Vec3b>(i - 1, j)[color]
				+ in->at<cv::Vec3b>(i, j + 1)[color] + in->at<cv::Vec3b>(i, j - 1)[color] + in->at<cv::Vec3b>(i + 1, j + 1)[color] + 
				in->at<cv::Vec3b>(i - 1, j - 1)[color] + in->at<cv::Vec3b>(i + 1, j - 1)[color] + in->at<cv::Vec3b>(i - 1, j + 1)[color];
				average = average/9;
				out->at<cv::Vec3b>(i, j)[color] = (uchar)average;
			}

		}
	}
	return out;
}

int main(int argc, char *argv[]) {
	//diz se a imagem é grayscale or color
	int tipo_img = atoi(argv[2]);
	//arquivo de entrada
	const char *fileIn;
	//matriz com a imagem de entrada
	Mat in;
	//matriz que receberá a imagem de saida
	Mat *out;

	//le o nome da imagem
	fileIn = argv[1];

	//le e salva a imagem na matriz
	if(tipo_img == 0) {
		in = imread(fileIn, CV_LOAD_IMAGE_GRAYSCALE);
	} else if(tipo_img == 1) {
		in = imread(fileIn, CV_LOAD_IMAGE_COLOR);
	} else {
		cout << "Tipo de imagem nao suportado" << endl;
		return -1;
	}
	//caso nao consegui abrir a imagem
	if (in.empty()) {
		cout << "Nao foi possivel abrir a  imagem: " << endl;
		return -1;
	}

	//aplica algoritmo smooth e recebe a nova imagem
	if(tipo_img == 0) {
		out = aplica_smooth_grayscale(&in);
		//mostra imagem original
		namedWindow("original", CV_WINDOW_AUTOSIZE);
	    imshow("original", in);

		//mostra imagem na tela
		namedWindow("smooth", CV_WINDOW_AUTOSIZE );
	    imshow("smooth", *out);
	} else if(tipo_img == 1) {
		out = aplica_smooth_color(&in);
		//mostra imagem original
		namedWindow("original", CV_WINDOW_AUTOSIZE);
	    imshow("original", in);

		//mostra imagem na tela
		namedWindow("smooth", CV_WINDOW_AUTOSIZE );
	    imshow("smooth", *out);
	} else {
		cout << "Tipo de imagem nao suportado" << endl;
		return -1;
	}
    waitKey(0);
    return 0;
}
