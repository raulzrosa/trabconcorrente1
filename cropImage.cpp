#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>


//#define NUM_NODES 4
#define MASTER 0
//	g++ -o cropImage cropImage.cpp `pkg-config --cflags --libs opencv`
			//crop[i].release();
			/*printf("%d %d %d\n",i,crop[i].size().height,crop[i].size().width);
			namedWindow("in", CV_WINDOW_AUTOSIZE );
			imshow("in", crop[i]);
			waitKey(0);*/
using namespace cv;
using namespace std;


int main(int argc, char *argv[]) {
	int NUM_NODES;
	Mat in, crop, out;
	MPI_Init(&argc, &argv);
	int rank;
    MPI_Status status;
    //descobre o rank do nó
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//descobre quantos nós a maquina suporta
    MPI_Comm_size(MPI_COMM_WORLD, &NUM_NODES);
    printf("NUMERO DE NOS = %d\n",NUM_NODES);
    if(rank == 0)  {
	    in = imread("sm.png", CV_LOAD_IMAGE_GRAYSCALE);
		int divisao_h = in.size().height/(NUM_NODES - 1);
		int divisao_w = in.size().width;
		int dimensoes[2];
		for(int i = 1; i < NUM_NODES; i++) {
			//recorto um pedaço da imagem pra mandar para o nó i
			crop =in(Rect(0,divisao_h*(i - 1),divisao_w,divisao_h));
			//pego as dimensões do pedaço da imagem e salvo num tipo struct 'image' definido la em cima
			dimensoes[0] = crop.size().width;
			dimensoes[1] = crop.size().height;
			// envio as dimensões para o nó
   			MPI_Send(dimensoes, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
			// envio o pedaço da imagem'
			MPI_Send(crop.data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD);
		}
		//apago a imagem original para economizar memória
		in.release();

		//recebe os pedaços dos nós
		//for(int i = 0; i < NUM_NODES; i++) {
			//MPI_Recv(&recv, 1,custom_type, 0, 0, MPI_COMM_WORLD, &status);
		//}
		printf("MASTER SHOW DE BOLA\n");
	} else {
		//parte do código que roda dentro de cada nó
		int dimensoes[2];
		MPI_Recv(dimensoes, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		printf("%d %d\n", dimensoes[0],dimensoes[1]);
		Mat mat(Size(dimensoes[0], dimensoes[1]), CV_8U);	
		MPI_Recv(mat.data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, 0, 1	, MPI_COMM_WORLD, &status);
		namedWindow("in", CV_WINDOW_AUTOSIZE );
		imshow("in", mat);
		waitKey(0);
		//aplica_smooth();
		//MPI_Send(&recv, 1, custom_type, MASTER, 0, MPI_COMM_WORLD);
		//MPI_Send(mat.data, recv.w*recv.h, MPI_UNSIGNED_CHAR, MASTER, 0, MPI_COMM_WORLD);
		printf("NÓ SHOW DE BOLA\n");
		//imwrite("Lenna2.png", mat);
	}	

	MPI_Finalize();
	return 0;
}