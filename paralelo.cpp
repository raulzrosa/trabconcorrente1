/*	g++ -o smooth smooth.cpp `pkg-config --cflags --libs opencv`
	g++ -o sequencial sequencial.cpp `pkg-config --cflags --libs opencv`
*/
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <time.h>
#define NTHREADS 4

using namespace cv;
using namespace std;

//matriz com a imagem de entrada
Mat in;
Mat *out_p;
//border = 1 pois tamanho da máscara = 5 => floor(5/2) = 2
int border = 2;

void aplica_smooth_grayscale(int ini_i, int ini_j, int end_i, int end_j) {
	float average;
	for(int i = ini_i; i < end_i; i++) {
		for(int j = ini_j; j < end_j; j++) {
			//nao sei se tenho que somar como uchar ou float
			average = in.at<uchar>(i, j) 
			+ in.at<uchar>(i + 1, j) 
			+ in.at<uchar>(i - 1, j) 
			+ in.at<uchar>(i, j + 1) 
			+ in.at<uchar>(i, j - 1) 
			+ in.at<uchar>(i + 2, j)
			+ in.at<uchar>(i - 2, j)
			+ in.at<uchar>(i, j + 2)
			+ in.at<uchar>(i, j - 2)
			+ in.at<uchar>(i + 1, j + 1) 
			+ in.at<uchar>(i + 1, j - 1) 
			+ in.at<uchar>(i + 1, j - 2) 
			+ in.at<uchar>(i + 1, j + 2)
			+ in.at<uchar>(i - 1, j + 1)
			+ in.at<uchar>(i - 1, j - 1)
			+ in.at<uchar>(i - 1, j + 2)
			+ in.at<uchar>(i - 1, j - 2)
			+ in.at<uchar>(i + 2, j + 1)
			+ in.at<uchar>(i + 2, j - 1)
			+ in.at<uchar>(i + 2, j + 2)
			+ in.at<uchar>(i + 2, j - 2)
			+ in.at<uchar>(i - 2, j + 1)
			+ in.at<uchar>(i - 2, j - 1)
			+ in.at<uchar>(i - 2, j + 2)
			+ in.at<uchar>(i - 2, j - 2);
			average = average/25;	
			out_p->at<uchar>(i - 2, j - 2) = (uchar)average;
		}
	}
	//printf("OLAR!\n");
}

void smooth_grayscale() {

	//copia imagem de entrada 
	out_p = new Mat(in.size(), CV_8U, 1);

	
	//achar a média aritmética e depois atualizar o pixel
	float average;
	omp_set_num_threads(NTHREADS);	
	//int ini_i = 0, ini_j = 0, end_i = 100, end_j = 100;
	int ini_i[4],ini_j[4],end_i[4],end_j[4];
	int meio_w, meio_h;
	//calcula o meio da imagem
	meio_h = in.size().height/2;
	meio_w = in.size().width/2;
	//calcula o inicio de cada pedaço da imagem
	ini_i[0] = 0 + border; 		ini_j[0] = 0 + border;
	ini_i[1] = 0 + border; 		ini_j[1] = meio_w;
	ini_i[2] = meio_h; 			ini_j[2] = 0 + border;
	ini_i[3] = meio_h; 			ini_j[3] = meio_w;
	//calcula o fim de cada pedaço da imagem
	end_i[0] = meio_h; 						end_j[0] = meio_w;
	end_i[1] = meio_h; 						end_j[1] = in.size().width - border;
	end_i[2] = in.size().height - border; 	end_j[2] = meio_w;
	end_i[3] = in.size().height - border;	end_j[3] = in.size().width - border;

	//replica a borda pra solucionar o problema dos pixels de borda
	copyMakeBorder(in, in, border, border, border, border, BORDER_REPLICATE);

	//chama cada iteração do for junto em paralelo passando como parametro o 
	//inicio e o fim de cada pedaço da imagem que cada thread deve rodar
	#pragma omp parallel for shared(in,out_p, end_i, end_j, ini_i, ini_j)
		for(int i = 0; i < NTHREADS; i ++) {
			aplica_smooth_grayscale(ini_i[i], ini_j[i], end_i[i], end_j[i]);
		}
}

//função que aplica o filtro em imagem colorida
//sera necessário utilizar função MPI para paralelizar cada cor (R, G e B) em diferentes nós
//esse tipo de composição é uma decomposição funcional pois cada cor da imagem é idenpendente, ou seja,
//nao precisam compartilhar o mesmo dado.
//ja para cada cor, vamor fazer uma decomposição por domínio: vamos dividir a imagem pelo número de nucleos
//que o processador possui. Para isso vamos utilizar o OpenMP
/*Mat *aplica_smooth_color() {

	//copia imagem de entrada 
	Mat *out = new Mat(in->size(), CV_8UC3, 1);
	Mat aux(in->size(), CV_8UC3, 1);

	//border = 1 pois tamanho da máscara = 5 => floor(5/2) = 2
	int border = 2;
	
	//replica a borda pra solucionar o problema dos pixels de borda
	copyMakeBorder(*in, aux, border, border, border, border, BORDER_REPLICATE);

	//achar a média aritmética e depois atualizar o pixel
	float average;

	for(int i = border; i < in->size().height - border; i++) {
		for(int j = border; j < in->size().width - border; j++) {
			//nao sei se tenho que somar como uchar ou float]
			for(int color = 0; color < 3; color++) {
				average = in->at<cv::Vec3b>(i, j)[color] 
				+ in->at<cv::Vec3b>(i + 1, j)[color] 
				+ in->at<cv::Vec3b>(i - 1, j)[color]
				+ in->at<cv::Vec3b>(i, j + 1)[color] 
				+ in->at<cv::Vec3b>(i, j - 1)[color]
				+ in->at<cv::Vec3b>(i - 2, j)[color] 
				+ in->at<cv::Vec3b>(i + 2, j)[color] 
				+ in->at<cv::Vec3b>(i, j + 2)[color]
				+ in->at<cv::Vec3b>(i, j - 2)[color]
				+ in->at<cv::Vec3b>(i + 1, j + 1)[color]
				+ in->at<cv::Vec3b>(i + 1, j + 2)[color] 
				+ in->at<cv::Vec3b>(i + 1, j - 1)[color] 
				+ in->at<cv::Vec3b>(i + 1, j - 2)[color]
				+ in->at<cv::Vec3b>(i - 1, j - 1)[color] 
				+ in->at<cv::Vec3b>(i - 1, j + 1)[color]
				+ in->at<cv::Vec3b>(i - 1, j + 2)[color]
				+ in->at<cv::Vec3b>(i - 1, j - 2)[color] 
				+ in->at<cv::Vec3b>(i - 2, j - 2)[color] 
				+ in->at<cv::Vec3b>(i - 2, j + 1)[color]
				+ in->at<cv::Vec3b>(i - 2, j - 1)[color]
				+ in->at<cv::Vec3b>(i - 2, j + 2)[color]
				+ in->at<cv::Vec3b>(i + 2, j + 2)[color]
				+ in->at<cv::Vec3b>(i + 2, j + 1)[color] 
				+ in->at<cv::Vec3b>(i + 2, j - 1)[color]
				+ in->at<cv::Vec3b>(i + 2, j - 2)[color];

				average = average/25;
				out->at<cv::Vec3b>(i, j)[color] = (uchar)average;
			}

		}
	}
	return out;
}
*/
int main(int argc, char *argv[]) {
	//diz se a imagem é grayscale or color
	int tipo_img = atoi(argv[2]);
	//arquivo de entrada
	const char *fileIn, *fileOut;
	clock_t itime, ftime;
	//matriz que receberá a imagem de saida
	//Mat *out;
	itime = clock();
	//le o nome da imagem
	fileIn = argv[1];
	fileOut = argv[3];
	//le e salva a imagem na matriz
	if(tipo_img == 0) {
		in = imread(fileIn, CV_LOAD_IMAGE_GRAYSCALE);
		printf("%d,%d\n",in.size().width,in.size().height);
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
		smooth_grayscale();
		//mostra imagem original
		//namedWindow("original", CV_WINDOW_AUTOSIZE);
	    //imshow("original", in);
		//mostra imagem na tela
		//namedWindow("smooth", CV_WINDOW_AUTOSIZE );
	    //imshow("smooth", *out);
	} else if(tipo_img == 1) {
		//aplica_smooth_color(&in);
		//mostra imagem original
		//namedWindow("original", CV_WINDOW_AUTOSIZE);
	    //imshow("original", in);

		//mostra imagem na tela
		//namedWindow("smooth", CV_WINDOW_AUTOSIZE );
	    //imshow("smooth", *out);
	} else {
		cout << "Tipo de imagem nao suportado" << endl;
		return -1;
	}
	imwrite(fileOut, *out_p);
    //waitKey(0);
    	ftime = clock();
    	printf("\nExecution time: %lf seconds\n",(ftime-itime) / (CLOCKS_PER_SEC * 1.0)); 
		printf("-----------------------------------------\n");
	in.release();
	out_p->release();
    return 0;
}
