#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<semaphore.h>
#include<sys/time.h>
#include<pthread.h>

using namespace std;

#define Eliminator_lenth 1789
#define RowLenth 2958

#define worker_count 4

struct eliRow{
    bool is_new_eliminator;
    int FirstOne;
    bitset<Eliminator_lenth> bit;
};

typedef struct////线程函数定义
{
    int id;
} threadParam_t;

//消元子
bitset<Eliminator_lenth> eliminator[Eliminator_lenth]; 
//被消元行
eliRow eliminatedRow[RowLenth];

pthread_barrier_t barrier_1;
pthread_barrier_t barrier_2;
pthread_barrier_t barrier_3;


void read()
{
    ifstream file;
    file.open("/eliminator.txt",ios_base::in);//读取消元子

    if(!file.is_open())
    {
        cout<<"消元子文件打开失败"<<endl;
    }
    string s;
    while(getline(file,s))
    {
        bool rowIndex = true;
        int x;
        //cout<<s<<endl;
        stringstream st;
        int temp;
        st<<s;
        
        while(st>>temp)
        {
            if(rowIndex)
            {
                x = temp;
                rowIndex = false;
            }
            eliminator[x].set(temp);//第一个temp同时也为行号
        }      

    }
    
    file.close();
    file.clear(ios::goodbit);

    file.open("/eliRow.txt",ios_base::in);//读取被消元行

    if(!file.is_open())
    {
        cout<<"被消元行打开失败"<<endl;
    }
    int x = 0;
    while(getline(file,s))
    {
        bool Index = true;
        stringstream st;
        int temp;
        st<<s;
        eliminatedRow[x].is_new_eliminator=false;

        while(st>>temp)
        {
            if(Index)
            {
                Index = false;
                eliminatedRow[x].FirstOne=temp;
            }
            eliminatedRow[x].bit.set(temp);
        }  
        x++;    
    }
    file.close();
}

// 定义全局变量
int start;

// 线程函数
void* mythread(void* param)
{
    // 获取线程参数
    threadParam_t* pa = (threadParam_t*)param;
    int t_id = pa->id;

    // 从后往前遍历每一列
    for(int i = Eliminator_lenth-1;i>=0;i--)
    {
        // 如果该列有消元子
        if(eliminator[i].any())
        {
            // 遍历每一行
            for(int j=t_id;j<RowLenth;j+=worker_count)
            {
                // 如果该行的首个1的位置等于当前列
                if(eliminatedRow[j].FirstOne==i)
                {
                    // 该行异或消元子
                    eliminatedRow[j].bit^=eliminator[i];
                    // 如果该行全为0
                    if(eliminatedRow[j].bit.none())
                    {
                        // 标记该行为消元子
                        eliminatedRow[j].FirstOne=-1;
                        eliminatedRow[j].is_new_eliminator=true;
                    }
                    else
                    {
                        // 更新该行的首个1的位置
                        for (int l = eliminatedRow[j].FirstOne; l >= 0; l--)
                        {
                            if(eliminatedRow[j].bit[l] == 1)
                            {
                                eliminatedRow[j].FirstOne = l;
                                break;
                            }
                        }
                    } 
                }                                            
            }
        }
        else
        {
            // 等待所有线程到达同步点1
            pthread_barrier_wait(&barrier_1);
            // 如果是第一个线程
            if(t_id==0)
            {
                bool Index = true;
                // 遍历每一行
                for(int j =0;j<RowLenth;j++)
                {
                    // 如果该行的首个1的位置等于当前列
                    if(eliminatedRow[j].FirstOne==i)
                    {
                        // 将该行的消元子赋值给当前列的消元子
                        eliminator[i]=eliminatedRow[j].bit;
                        // 标记该行为消元子
                        eliminatedRow[j].is_new_eliminator=true;
                        // 记录该行的位置
                        start=j;  //标记
                        Index=false;
                        break;
                    }
                }
                // 如果没有找到消元行
                if(Index)
                    start=RowLenth;
            }
            // 等待所有线程到达同步点2
            pthread_barrier_wait(&barrier_2);
            // 计算每个线程需要遍历的行的起始位置
            int new_start = t_id;
            for(;new_start<=start;new_start+=worker_count);
            // 遍历每一行
            for(int j = new_start;j<RowLenth;j+=worker_count)
            {
                // 如果该行的首个1的位置等于当前列
                if(eliminatedRow[j].FirstOne==i)
                {
                    // 该行异或消元子
                    eliminatedRow[j].bit^=eliminator[i];
                    // 如果该行全为0
                    if(eliminatedRow[j].bit.none())
                    {
                        // 标记该行为消元子
                        eliminatedRow[j].FirstOne=-1;
                        eliminatedRow[j].is_new_eliminator=true;
                    }
                    else
                    {
                        // 更新该行的首个1的位置
                        for (int l = eliminatedRow[j].FirstOne; l >= 0; l--)
                        {
                            if(eliminatedRow[j].bit[l] == 1)
                            {
                                eliminatedRow[j].FirstOne = l;
                                break;
                            }
                        }
                    }
                }                                                       
            }
        }
    }
    pthread_exit(NULL);
}

//线程初始化
void func_thread()
{
     //初始化barrier
    pthread_barrier_init(&barrier_1,NULL,worker_count);
    pthread_barrier_init(&barrier_2,NULL,worker_count);
    pthread_barrier_init(&barrier_3,NULL,worker_count);
    //创建线程
    pthread_t* handles = new  pthread_t[worker_count];
    threadParam_t* param = new threadParam_t[worker_count];

    for(int t_id = 0;t_id<worker_count;t_id++)
    {
        param[t_id].id = t_id;
    }
    for(int t_id = 0;t_id<worker_count;t_id++)
    {
        pthread_create((&handles[t_id]),NULL,mythread,(&param[t_id]));
    }
    for(int t_id = 0;t_id<worker_count;t_id++)
    {
        pthread_join(handles[t_id],NULL);
    }
 //销毁所有barrier
    pthread_barrier_destroy(&barrier_1);
    pthread_barrier_destroy(&barrier_2);
    pthread_barrier_destroy(&barrier_3);   
}

int main()
{
    read();
    struct timeval begin1,end1;
    gettimeofday(&begin1,NULL);
    func_thread();

    gettimeofday(&end1,NULL);
    long long time_use = (end1.tv_sec-begin1.tv_sec)*1000000 + end1.tv_usec-begin1.tv_usec;
    cout<<time_use<<endl;
    return 0;
}


