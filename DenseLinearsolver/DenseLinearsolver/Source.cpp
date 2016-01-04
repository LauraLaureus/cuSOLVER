#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>

#include <mkl.h>

#include <cuda_runtime.h>
#include <cusolverDn.h>

#include "eTimer.h"


#define N 3*512

using namespace std;

int main(int argc, char*argv[]){
	random_device gen; 
	normal_distribution<double> dis(0.0, 1.0);

	double *A1, *A2, *B1,*B2;


	A1 = (double*)mkl_malloc(N*N*sizeof(double), 64);
	A2 = (double*)mkl_malloc(N*N*sizeof(double), 64);
	B1 = (double*)mkl_malloc(N*sizeof(double), 64);
	B2 = (double*)mkl_malloc(N*sizeof(double), 64);

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			A1[i*N + j] = dis(gen);
			A2[i*N + j] = A1[i*N + j];
		}
		B1[i] = dis(gen);
		B2[i] = B1[i];
		A1[i*N + i] += 10;
		A2[i*N + i] = A1[i*N + i];
	}


	int info;
	int *ipiv = (int*)mkl_malloc(N*sizeof(double), 64);

	eTimer *Tcpu = new eTimer();
	eTimer *Tgpu = new eTimer();

	Tcpu->start();
	info = LAPACKE_dgesv(LAPACK_ROW_MAJOR, N, 1, A1, N, ipiv, B1, 1);
	Tcpu->stop();
	Tcpu->report("CPU");
	for (int  i = 0; i < 5; i++)
	{
		printf("%g ", B1[i]);
	}
	printf("\n");

	/*----------------------------gpu-----------------------------------------*/
	
	cudaError cudaStatus;
	cusolverStatus_t cusolverStatus;
	cusolverDnHandle_t handle;

	double *dev_A, *dev_B, *Work;
	int *dev_pivot, *dev_info, Lwork;

	cudaStatus = cudaGetDevice(0);
	cusolverStatus = cusolverDnCreate(&handle);


	cudaStatus = cudaMalloc((void**)&dev_A, N*N*sizeof(double));
	cudaStatus = cudaMalloc((void**)&dev_B, N*sizeof(double));
	cudaStatus = cudaMalloc((void**)&dev_pivot, N*sizeof(int));
	cudaStatus = cudaMalloc((void**)&dev_info, sizeof(int));
	cudaStatus = cudaMemcpy(dev_A, A2, N*N*sizeof(double), cudaMemcpyHostToDevice);
	cudaStatus = cudaMemcpy(dev_B, B2, N*sizeof(double), cudaMemcpyHostToDevice);

	Tgpu->start();
	cusolverStatus = cusolverDnDgetrf_bufferSize(handle, N, N, dev_A, N, &Lwork);
	cudaStatus = cudaMalloc((void**)&Work, Lwork*sizeof(double));
	cusolverStatus = cusolverDnDgetrf(handle,N,N,dev_A, N,Work,dev_pivot, dev_info);
	cusolverStatus = cusolverDnDgetrs(handle, CUBLAS_OP_T, N, 1, dev_A, N, dev_pivot, dev_B,N, dev_info);
	cudaStatus = cudaDeviceSynchronize();
	Tgpu ->stop();
	Tgpu->report("GPU");

	cudaStatus = cudaMemcpy(B2, dev_B, N*sizeof(double), cudaMemcpyDeviceToHost);

	for (int i = 0; i < 5; i++)
	{
		printf("%g ", B2[i]);
	}
	printf("\n");


	cudaStatus = cudaFree(dev_A);
	cudaStatus = cudaFree(dev_B);
	cudaStatus = cudaFree(dev_pivot);
	cudaStatus = cudaFree(dev_info);
	cudaStatus = cudaFree(Work);

	cusolverStatus = cusolverDnDestroy(handle);
	cudaStatus = cudaDeviceReset();

	mkl_free(A1);
	mkl_free(A2);
	mkl_free(B1);
	mkl_free(B2);
	mkl_free(ipiv);

	delete Tcpu;
	delete Tgpu;

	getchar();

	return 0;
}
