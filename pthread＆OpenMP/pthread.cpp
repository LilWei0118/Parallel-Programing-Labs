
#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
using namespace std;
const int N=600;
const int thread_num=8;
float m[N][N];
void m_reset()
{
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<i;j++)
			m[i][j]=0;
		m[i][i]=1.0;
		for(int j=i+1;j<N;j++)
			m[i][j]=rand();
	}
	for(int k=0;k<N;k++)
		for(int i=k+1;i<N;i++)
			for(int j=0;j<N;j++)
				m[i][j]+=m[k][j];
}//======================================================静态块划分======================================

int main(){
	struct timeval start;
	struct timeval end;//clock
	float timecount;
	m_reset();
	gettimeofday(&start,NULL);

	int i,j,k;
	float tmp;
#pragma omp parallel if(1),num_threads(thread_num),private(i,j,k,tmp)
//	for(int cy=0;cy<20;cy++){
	for(k=0;k<N;k++)
	{
#pragma omp single
		{
		tmp=m[k][k];
		for(j=k+1;j<N;j++)
			m[k][j]=m[k][j]/tmp;
		m[k][k]=1.0;
		}

#pragma omp for
		for(i=k+1;i<N;i++)
		{
			tmp=m[i][k];
			for(j=k+1;j<N;j++)
				m[i][j]=m[i][j]-tmp*m[k][j];
			m[i][k]=0;
		}
	}
//	}
//	for(int i=0;i<N;i++)
//	{
//		for(int j=0;j<N;j++)
//			cout<<m[i][j]<<" ";
//		cout<<endl;
//	}
	gettimeofday(&end,NULL);
	timecount+=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<timecount<<endl;
	return 0;
}

//========================================================静态循环划分=================================

//
//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//	gettimeofday(&start,NULL);
//
//	int i,j,k;
//	float tmp;
//#pragma omp parallel if(1),num_threads(thread_num),private(i,j,k,tmp)
////	for(int cy=0;cy<20;cy++){
//	for(k=0;k<N;k++)
//	{
//#pragma omp single
//		{
//		tmp=m[k][k];
//		for(j=k+1;j<N;j++)
//			m[k][j]=m[k][j]/tmp;
//		m[k][k]=1.0;
//		}
//
//#pragma omp for schedule(static,chunksize)
//		for(i=k+1;i<N;i++)
//		{
//			tmp=m[i][k];
//			for(j=k+1;j<N;j++)
//				m[i][j]=m[i][j]-tmp*m[k][j];
//			m[i][k]=0;
//		}
//	}
////	}
////	for(int i=0;i<N;i++)
////	{
////		for(int j=0;j<N;j++)
////			cout<<m[i][j]<<" ";
////		cout<<endl;
////	}
//	gettimeofday(&end,NULL);
//	timecount+=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
//	cout<<timecount<<endl;
//	return 0;
//}
//
//==========================================动态循环划分=======================

//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//	gettimeofday(&start,NULL);
//
//	int i,j,k;
//	float tmp;
//#pragma omp parallel if(1),num_threads(thread_num),private(i,j,k,tmp)
////	for(int cy=0;cy<20;cy++){
//	for(k=0;k<N;k++)
//	{
//#pragma omp single
//		{
//		tmp=m[k][k];
//		for(j=k+1;j<N;j++)
//			m[k][j]=m[k][j]/tmp;
//		m[k][k]=1.0;
//		}
//
//#pragma omp for schedule(dynamic,chunksize)
//		for(i=k+1;i<N;i++)
//		{
//			tmp=m[i][k];
//			for(j=k+1;j<N;j++)
//				m[i][j]=m[i][j]-tmp*m[k][j];
//			m[i][k]=0;
//		}
//	}
////	}
////	for(int i=0;i<N;i++)
////	{
////		for(int j=0;j<N;j++)
////			cout<<m[i][j]<<" ";
////		cout<<endl;
////	}
//	gettimeofday(&end,NULL);
//	timecount+=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
//	cout<<timecount<<endl;
//	return 0;
//}


//=================================guided================================
//
//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//	gettimeofday(&start,NULL);
//
//	int i,j,k;
//	float tmp;
//#pragma omp parallel if(1),num_threads(thread_num),private(i,j,k,tmp)
////	for(int cy=0;cy<20;cy++){
//	for(k=0;k<N;k++)
//	{
//#pragma omp single
//		{
//		tmp=m[k][k];
//		for(j=k+1;j<N;j++)
//			m[k][j]=m[k][j]/tmp;
//		m[k][k]=1.0;
//		}
//
//#pragma omp for schedule(guided,chunksize)
//		for(i=k+1;i<N;i++)
//		{
//			tmp=m[i][k];
//			for(j=k+1;j<N;j++)
//				m[i][j]=m[i][j]-tmp*m[k][j];
//			m[i][k]=0;
//		}
//	}
////	}
////	for(int i=0;i<N;i++)
////	{
////		for(int j=0;j<N;j++)
////			cout<<m[i][j]<<" ";
////		cout<<endl;
////	}
//	gettimeofday(&end,NULL);
//	timecount+=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
//	cout<<timecount<<endl;
//	return 0;
//}

