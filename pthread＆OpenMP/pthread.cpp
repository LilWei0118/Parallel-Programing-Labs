
#include <pthread.h>
#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <immintrin.h>
//============================================动态线程版本==============================================
using namespace std;
const int N=1000;
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
}
int main(){
	struct timeval start;
	struct timeval end;//clock
	float timecount;
	m_reset();
	gettimeofday(&start,NULL);
//	for(int cy=0;cy<20;cy++){
	for(int k=0;k<N;k++)
	{
		for(int j=k+1;j<N;j++)
			m[k][j]=m[k][j]/m[k][k];
		m[k][k]=1.0;
		for(int i=k+1;i<N;i++)
		{
			for(int j=k+1;j<N;j++)
				m[i][j]=m[i][j]-m[i][k]*m[k][j];
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
//
//
//

////=========================================静态线程与信号量同步版本==============================

//struct threadParam_t{
//	int k;//turn
//	int t_id;//id of thread
//	int count;
//};//thread
//
//void *threadFunc(void* param){
//	threadParam_t *p=(threadParam_t*)param;
//	int k=p->k;
//	int t_id=p->t_id;
//	int r=k+t_id+1;
//	int count=p->count;
//
//	for(int i=r;i<N;i+=count)
//	{
//		for(int j=k+1;j<N;++j)
//			m[i][j]=m[i][j]-m[i][k]*m[k][j];
//		m[i][k]=0;
//	}
//	//cout<<"finish "<<t_id<<endl;
//	pthread_exit(NULL);
//	return NULL;
//	//cout<<"finish "<<t_id<<endl;
//}
//
//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//	gettimeofday(&start,NULL);
////	int k=0;
//	int worker_count=16;
//        pthread_t* handles=new pthread_t[worker_count];
//        threadParam_t* param=new threadParam_t[worker_count];//init thread
//	for(int k=0;k<N;k++)
//	{
//		for(int j=k+1;j<N;j++)
//			m[k][j]=m[k][j]/m[k][k];
//		m[k][k]=1.0;
//
//		for(int t_id = 0;t_id < worker_count; t_id++)
//		{
//			param[t_id].k=k;
//			param[t_id].t_id=t_id;
//			param[t_id].count=worker_count;
//		}
//		for(int t_id = 0; t_id < worker_count; t_id++)
//			pthread_create(&handles[t_id],NULL,threadFunc,(&param[t_id]));
//		for(int t_id = 0; t_id < worker_count; t_id++)
//			pthread_join(handles[t_id],NULL);
//	}
//	delete[] handles;
//        delete[] param;
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

////===================================静态线程信号量同步版本与三重循环全部纳入线程函数======================

//sem_t sem_main;
//sem_t sem_workerstart[worker_count];
//sem_t sem_workerend[worker_count];
//void m_reset()
//{
//	for(int i=0;i<N;i++)
//	{
//		for(int j=0;j<i;j++)
//			m[i][j]=0;
//		m[i][i]=1.0;
//		for(int j=i+1;j<N;j++)
//			m[i][j]=rand();
//	}
//	for(int k=0;k<N;k++)
//		for(int i=k+1;i<N;i++)
//			for(int j=0;j<N;j++)
//				m[i][j]+=m[k][j];
//}
//struct threadParam_t{
////	int k;//turn
//	int t_id;//id of thread
//};//thread
//
//void *threadFunc(void* param){
//	threadParam_t *p=(threadParam_t*)param;
////	int k=p->k;
//	int t_id=p->t_id;
////	int r=k+t_id+1;
////	int k=0;
//	for(int k=0;k<N;++k)
//	{
//
//		sem_wait(&sem_workerstart[t_id]);
//
//		for(int i=k+1+t_id;i<N;i+=worker_count)
//		{
//			for(int j=k+1;j<N;++j)
//				m[i][j]=m[i][j]-m[i][k]*m[k][j];
//			m[i][k]=0;
//		}
//
//		sem_post(&sem_main);
//		sem_wait(&sem_workerend[t_id]);
//	}
//	//cout<<"finish "<<t_id<<endl;
//	pthread_exit(NULL);
//	return NULL;
//	//cout<<"finish "<<t_id<<endl;
//}
//
//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//
//	gettimeofday(&start,NULL);
//	sem_init(&sem_main,0,0);
//	for(int i=0;i<worker_count;++i){
//		sem_init(&sem_workerstart[i],0,0);
//		sem_init(&sem_workerend[i],0,0);
//	}//init thread
//
//	pthread_t* handles=new pthread_t[worker_count];
//	threadParam_t* param=new threadParam_t[worker_count];
//	for(int t_id = 0;t_id < worker_count; t_id++)
//	{
////		param[t_id].k=k;
//		param[t_id].t_id=t_id;
////		param[t_id].count=worker_count;
//		pthread_create(&handles[t_id],NULL,threadFunc,(&param[t_id]));
//	}//create thread
//
//
////	int k=0;
//	for(int k=0;k<N;k++)
//	{
//		for(int j=k+1;j<N;j++)
//			m[k][j]=m[k][j]/m[k][k];
//		m[k][k]=1.0;
//
//		for(int t_id=0;t_id<worker_count;t_id++)
//			sem_post(&sem_workerstart[t_id]);
//
//		for(int t_id = 0; t_id < worker_count; t_id++)
//			sem_wait(&sem_main);
//
//		for(int t_id=0;t_id<worker_count;t_id++)
//			sem_post(&sem_workerend[t_id]);
//
//	}
//
//	for(int t_id=0;t_id<worker_count;t_id++)
//		pthread_join(handles[t_id],NULL);
//	sem_destroy(&sem_main);
//	for(int t_id=0;t_id<worker_count;t_id++){
//		sem_destroy(&sem_workerend[t_id]);
//		sem_destroy(&sem_workerstart[t_id]);
//	}
//	delete[] handles;
//	delete[] param;
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

////====================================================== 静态线程与 barrier 同步=============================================

//sem_t sem_leader;
//sem_t sem_Divsion[worker_count-1];
//sem_t sem_Elimination[worker_count-1];
//void m_reset()
//{
//	for(int i=0;i<N;i++)
//	{
//		for(int j=0;j<i;j++)
//			m[i][j]=0;
//		m[i][i]=1.0;
//		for(int j=i+1;j<N;j++)
//			m[i][j]=rand();
//	}
//	for(int k=0;k<N;k++)
//		for(int i=k+1;i<N;i++)
//			for(int j=0;j<N;j++)
//				m[i][j]+=m[k][j];
//}
//struct threadParam_t{
////	int k;//turn
//	int t_id;//id of thread
//};//thread
//
//void *threadFunc(void* param){
//	threadParam_t *p=(threadParam_t*)param;
////	int k=p->k;
//	int t_id=p->t_id;
////	int r=k+t_id+1;
////	int k=0;
//	for(int k=0;k<N;++k)
//	{
//		if(t_id==0)
//		{
//			for(int j=k+1;j<N;j++)
//				m[k][j]=m[k][j]/m[k][k];
//			m[k][k]=1.0;
//		}
//		else
//			sem_wait(&sem_Divsion[t_id-1]);
//
//		if(t_id==0)
//			for(int i=0;i<worker_count-1;i++)
//				sem_post(&sem_Divsion[i]);
//
//		for(int i=k+1+t_id;i<N;i+=worker_count)
//		{
//			for(int j=k+1;j<N;++j)
//				m[i][j]=m[i][j]-m[i][k]*m[k][j];
//			m[i][k]=0;
//		}
//
//		if(t_id==0)
//		{
//			for(int i=0;i<worker_count-1;i++)
//				sem_wait(&sem_leader);
//			for(int i=0;i<worker_count-1;i++)
//				sem_post(&sem_Elimination[i]);
//		}
//		else{
//			sem_post(&sem_leader);
//			sem_wait(&sem_Elimination[t_id-1]);
//		}
//	}
//	//cout<<"finish "<<t_id<<endl;
//	pthread_exit(NULL);
//	return NULL;
//	//cout<<"finish "<<t_id<<endl;
//}
//
//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//
//	gettimeofday(&start,NULL);
//	sem_init(&sem_leader,0,0);
//	for(int i=0;i<worker_count-1;++i){
//		sem_init(&sem_Divsion[i],0,0);
//		sem_init(&sem_Elimination[i],0,0);
//	}//init thread
//
//	pthread_t* handles=new pthread_t[worker_count];
//	threadParam_t* param=new threadParam_t[worker_count];
//	for(int t_id = 0;t_id < worker_count; t_id++)
//	{
////		param[t_id].k=k;
//		param[t_id].t_id=t_id;
////		param[t_id].count=worker_count;
//		pthread_create(&handles[t_id],NULL,threadFunc,(&param[t_id]));
//	}//create thread
//
//	for(int t_id=0;t_id<worker_count;t_id++)
//		pthread_join(handles[t_id],NULL);
//
//	sem_destroy(&sem_leader);
//	for(int t_id=0;t_id<worker_count;t_id++){
//		sem_destroy(&sem_Divsion[t_id-1]);
//		sem_destroy(&sem_Elimination[t_id-1]);
//	}
//	delete[] handles;
//	delete[] param;
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
////============================================SSE 实现==========================================
//#include <pthread.h>
//#include <sys/time.h>
//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <semaphore.h>
//
//using namespace std;
//const int N=1500;
//const int worker_count=8;
//float m[N][N];
//
//pthread_barrier_t barrier_Divsion;
//pthread_barrier_t barrier_Elimination;
//
//void m_reset()
//{
//	for(int i=0;i<N;i++)
//	{
//		for(int j=0;j<i;j++)
//			m[i][j]=0;
//		m[i][i]=1.0;
//		for(int j=i+1;j<N;j++)
//			m[i][j]=rand();
//	}
//	for(int k=0;k<N;k++)
//		for(int i=k+1;i<N;i++)
//			for(int j=0;j<N;j++)
//				m[i][j]+=m[k][j];
//}
//struct threadParam_t{
////	int k;//turn
//	int t_id;//id of thread
//};//thread
//
//void *threadFunc(void* param){
//	threadParam_t *p=(threadParam_t*)param;
////	int k=p->k;
//	int t_id=p->t_id;
////	int r=k+t_id+1;
////	int k=0;
//	for(int k=0;k<N;++k)
//	{
//		if(t_id==0)
//		{
//			for(int j=k+1;j<N;j++)
//				m[k][j]=m[k][j]/m[k][k];
//			m[k][k]=1.0;
//		}
//		pthread_barrier_wait(&barrier_Divsion);
//
//		for(int i=k+1+t_id;i<N;i+=worker_count)
//		{
//			for(int j=k+1;j<N;++j)
//				m[i][j]=m[i][j]-m[i][k]*m[k][j];
//			m[i][k]=0;
//		}
//
//		pthread_barrier_wait(&barrier_Elimination);
//
//	}
//	//cout<<"finish "<<t_id<<endl;
//	pthread_exit(NULL);
//	return NULL;
//	//cout<<"finish "<<t_id<<endl;
//}
//
//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//
//	gettimeofday(&start,NULL);
//	pthread_barrier_init(&barrier_Divsion,NULL,worker_count);
//	pthread_barrier_init(&barrier_Elimination,NULL,worker_count);
//
//	pthread_t* handles=new pthread_t[worker_count];
//	threadParam_t* param=new threadParam_t[worker_count];
//	for(int t_id = 0;t_id < worker_count; t_id++)
//	{
////		param[t_id].k=k;
//		param[t_id].t_id=t_id;
////		param[t_id].count=worker_count;
//		pthread_create(&handles[t_id],NULL,threadFunc,(&param[t_id]));
//	}//create thread
//
//	for(int t_id=0;t_id<worker_count;t_id++)
//		pthread_join(handles[t_id],NULL);
//
//	pthread_barrier_destroy(&barrier_Divsion);
//	pthread_barrier_destroy(&barrier_Elimination);
//	delete[] handles;
//	delete[] param;
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


//////=============================================AVX 实现===============================================

//
//
//pthread_barrier_t barrier_Divsion;
//pthread_barrier_t barrier_Elimination;
//
//void m_reset()
//{
//	for(int i=0;i<N;i++)
//	{
//		for(int j=0;j<i;j++)
//			m[i][j]=0;
//		m[i][i]=1.0;
//		for(int j=i+1;j<N;j++)
//			m[i][j]=rand();
//	}
//	for(int k=0;k<N;k++)
//		for(int i=k+1;i<N;i++)
//			for(int j=0;j<N;j++)
//				m[i][j]+=m[k][j];
//}
//struct threadParam_t{
////	int k;//turn
//	int t_id;//id of thread
//};//thread
//
//void *threadFunc(void* param){
//	threadParam_t *p=(threadParam_t*)param;
////	int k=p->k;
//	int t_id=p->t_id;
////	int r=k+t_id+1;
////	int k=0;
//	for(int k=0;k<N;++k)
//	{
//		if(t_id==0)
//		{
//			__m256 vt= _mm256_set1_ps(m[k][k]);
//			for(int j=k+1;j<N;j+=8)
//				if(j+8>N)
//					for(;j<N;j++)
//						m[k][j]=m[k][j]/m[k][k];
//				else
//				{
//					__m256 va= _mm256_loadu_ps(m[k]+j);
//					va= _mm256_div_ps(va,vt);
//					_mm256_storeu_ps(m[k]+j,va);
//				}
//			m[k][k]=1.0;
//		}
//		pthread_barrier_wait(&barrier_Divsion);
//
//		for(int i=k+1+t_id;i<N;i+=worker_count)
//		{
//			for(int j=k+1;j<N;j+=8)
//			{
//				if(j+8>N)
//					for(;j<N;j++)
//						m[i][j]=m[i][j]-m[i][k]*m[k][j];//rest value
//				else//pal
//				{
//					__m256 temp1= _mm256_loadu_ps(m[i]+j);
//					__m256 temp2= _mm256_loadu_ps(m[k]+j);
//					__m256 temp3= _mm256_set1_ps(m[i][k]);
//					temp2= _mm256_mul_ps(temp3,temp2);
//					temp1= _mm256_sub_ps(temp1,temp2);
//					_mm256_storeu_ps(m[i]+j,temp1);
//				}
//			m[i][k]=0;
//			}
//		}
//
//		pthread_barrier_wait(&barrier_Elimination);
//
//	}
//	//cout<<"finish "<<t_id<<endl;
//	pthread_exit(NULL);
//	return NULL;
//	//cout<<"finish "<<t_id<<endl;
//}
//
//int main(){
//	struct timeval start;
//	struct timeval end;//clock
//	float timecount;
//	m_reset();
//
//	gettimeofday(&start,NULL);
//	pthread_barrier_init(&barrier_Divsion,NULL,worker_count);
//	pthread_barrier_init(&barrier_Elimination,NULL,worker_count);
//
//	pthread_t* handles=new pthread_t[worker_count];
//	threadParam_t* param=new threadParam_t[worker_count];
//	for(int t_id = 0;t_id < worker_count; t_id++)
//	{
////		param[t_id].k=k;
//		param[t_id].t_id=t_id;
////		param[t_id].count=worker_count;
//		pthread_create(&handles[t_id],NULL,threadFunc,(&param[t_id]));
//	}//create thread
//
//	for(int t_id=0;t_id<worker_count;t_id++)
//		pthread_join(handles[t_id],NULL);
//
//	pthread_barrier_destroy(&barrier_Divsion);
//	pthread_barrier_destroy(&barrier_Elimination);
//	delete[] handles;
//	delete[] param;
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
//////
//////=========================================================6============================================================
