#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<vector>
#include<sys/time.h>
#include<omp.h>
#define Eliminator_lenth 1789
#define RowLenth 2958

using namespace std;
const int thread_num=4;
const int chunksize=1;
struct eliRow{
    bool is_new_eliminator;
    int FirstOne;
    bitset<Eliminator_lenth> bit;
};

//消元子
bitset<Eliminator_lenth> eliminator[Eliminator_lenth]; 
//被消元行
eliRow eliminatedRow[RowLenth];

bool Finish()
{
    for(int i = 0;i<RowLenth;i++)
    {
        if(eliminatedRow[i].is_new_eliminator==false)
            return false;
    }
    return true;
}

int main()
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
        bool RowIndex = true;
        int x;
        //cout<<s<<endl;
        stringstream st;
        int temp;
        st<<s;
        
        while(st>>temp)
        {
            if(RowIndex)
            {
                x = temp;
                RowIndex = false;
            }

            eliminator[x].set(temp);
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
        //cout<<s<<endl;
        bool Rowindex = true;
        stringstream st;
        int temp;
        st<<s;
        eliminatedRow[x].is_new_eliminator=false;

        while(st>>temp)
        {
            if(Rowindex)
            {
                Rowindex = false;
                eliminatedRow[x].FirstOne=temp;
            }
            eliminatedRow[x].bit.set(temp);
            //eliminator[x].set(temp);
        }  
        x++;    
    }
    file.close();
    int rt = 0;
    int p = 20;
    struct timeval begin1,end1;
    gettimeofday(&begin1,NULL);

    int i;//一层循环
    int j;//二层循环
    int k;//三层循环
    int l;

//静态循环划分
    #pragma omp parallel num_threads(thread_num),private(i,j,k,l,rt)
        for(i = Eliminator_lenth-1;i>=0;i-=p) // 从最后一行开始往上遍历，每次遍历p行
        {
            if(i-p<0) // 如果剩余行数小于p，则只遍历剩余行数
            {
                cout<<p<<endl;
                p=i+1;
            }

    #pragma omp for schedule(static,chunksize) // 并行执行for循环，每个线程处理chunksize个元素
            for(j = 0;j<RowLenth;j++) // 遍历每个消元行
            {
                if(eliminatedRow[j].is_new_eliminator==0) // 如果该消元行还没有被消元
                {
                    for(k = 0;k<p;k++) // 遍历当前p行
                    {
                        if(eliminatedRow[j].bit[i-k]==1) // 如果该消元行在当前p行中有1
                        {
                            // 消去过程
                            eliminatedRow[j].bit^=eliminator[i-k];
                        }
                        if(eliminatedRow[j].bit.none()) // 如果该消元行已经被消元为0
                        {
                            eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                            break;
                        }
                        else // 如果该消元行还没有被消元为0
                        {
                            for(l = eliminatedRow[j].FirstOne;l>=0;l--) // 从该消元行的第一个1开始往前遍历
                            {
                                if(eliminatedRow[j].bit[l]==1) // 如果找到了1
                                {
                                    eliminatedRow[j].FirstOne = l; // 更新该消元行的第一个1的位置
    				break;
                                }
                            }
                            if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                            {
                                eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                                break;
                            }
                        }                
                    }
                    
                }       
    	    }

    #pragma omp single // 单线程执行以下代码块
        for(j=0;j<RowLenth;j++) // 遍历每个消元行
            {
                if(eliminatedRow[j].is_new_eliminator==1) // 如果该消元行已经被消元
                {
                    if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                        eliminator[eliminatedRow[j].FirstOne]^=eliminatedRow[j].bit; // 消元该列
                    else // 如果该消元行的第一个1所在的列已经被消元
                    {
                            eliminatedRow[j].is_new_eliminator=0; // 标记该消元行还没有被消元
                        for(k = 0;k<p;k++) // 遍历当前p行
                        {
                            if(eliminatedRow[j].bit[i-k]==1) // 如果该消元行在当前p行中有1
                                    {//消去过程
                            eliminatedRow[j].bit^=eliminator[i-k];} // 消元该行
                        if(eliminatedRow[j].bit.none()) // 如果该消元行已经被消元为0
                                    {eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                            cout<<"null"<<rt++<<endl;
                            break;}
                        else // 如果该消元行还没有被消元为0
                        {for(l = eliminatedRow[j].FirstOne;l>=0;l--) // 从该消元行的第一个1开始往前遍历
                            {
                                if(eliminatedRow[j].bit[l]==1) // 如果找到了1
                                {eliminatedRow[j].FirstOne = l; // 更新该消元行的第一个1的位置
                                    break;}
                            }
                            if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                            {eliminator[eliminatedRow[j].FirstOne]^=eliminatedRow[j].bit; // 消元该列
                                eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                                //eliminatedRow[j].num;
                                //cout<<rt++<<endl;
                                break;} }
                        }
                    }
                }
            }
        if(Finish()) // 如果矩阵已经被消元为0
                {
                    cout<<"null"<<endl;
                    break;
                }
        } 

//动态循环划分
    #pragma omp parallel num_threads(thread_num),private(i,j,k,l,rt)
        for(i = Eliminator_lenth-1;i>=0;i-=p) // 从最后一行开始往上遍历，每次遍历p行
        {
            if(i-p<0) // 如果剩余行数小于p，则只遍历剩余行数
            {
                cout<<p<<endl;
                p=i+1;
            }

    #pragma omp for schedule(dynamic,chunksize) // 并行执行for循环，每个线程处理chunksize个元素
            for(j = 0;j<RowLenth;j++) // 遍历每个消元行
            {
                if(eliminatedRow[j].is_new_eliminator==0) // 如果该消元行还没有被消元
                {
                    for(k = 0;k<p;k++) // 遍历当前p行
                    {
                        if(eliminatedRow[j].bit[i-k]==1) // 如果该消元行在当前p行中有1
                        {
                            // 消去过程
                            eliminatedRow[j].bit^=eliminator[i-k];
                        }
                        if(eliminatedRow[j].bit.none()) // 如果该消元行已经被消元为0
                        {
                            eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                            break;
                        }
                        else // 如果该消元行还没有被消元为0
                        {
                            for(l = eliminatedRow[j].FirstOne;l>=0;l--) // 从该消元行的第一个1开始往前遍历
                            {
                                if(eliminatedRow[j].bit[l]==1) // 如果找到了1
                                {
                                    eliminatedRow[j].FirstOne = l; // 更新该消元行的第一个1的位置
    				break;
                                }
                            }
                            if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                            {
                                eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                                break;
                            }
                        }                
                    }
                    
                }       
    	    }

    #pragma omp single // 单线程执行以下代码块
        for(j=0;j<RowLenth;j++) // 遍历每个消元行
            {
                if(eliminatedRow[j].is_new_eliminator==1) // 如果该消元行已经被消元
                {
                    if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                        eliminator[eliminatedRow[j].FirstOne]^=eliminatedRow[j].bit; // 消元该列
                    else // 如果该消元行的第一个1所在的列已经被消元
                    {
                            eliminatedRow[j].is_new_eliminator=0; // 标记该消元行还没有被消元
                        for(k = 0;k<p;k++) // 遍历当前p行
                        {
                            if(eliminatedRow[j].bit[i-k]==1) // 如果该消元行在当前p行中有1
                                    {//消去过程
                            eliminatedRow[j].bit^=eliminator[i-k];} // 消元该行
                        if(eliminatedRow[j].bit.none()) // 如果该消元行已经被消元为0
                                    {eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                            cout<<"null"<<rt++<<endl;
                            break;}
                        else // 如果该消元行还没有被消元为0
                        {for(l = eliminatedRow[j].FirstOne;l>=0;l--) // 从该消元行的第一个1开始往前遍历
                            {
                                if(eliminatedRow[j].bit[l]==1) // 如果找到了1
                                {eliminatedRow[j].FirstOne = l; // 更新该消元行的第一个1的位置
                                    break;}
                            }
                            if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                            {eliminator[eliminatedRow[j].FirstOne]^=eliminatedRow[j].bit; // 消元该列
                                eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                                //eliminatedRow[j].num;
                                //cout<<rt++<<endl;
                                break;} }
                        }
                    }
                }
            }
        if(Finish()) // 如果矩阵已经被消元为0
                {
                    cout<<"null"<<endl;
                    break;
                }

        } 
        
//guided划分
    #pragma omp parallel num_threads(thread_num),private(i,j,k,l,rt)
        for(i = Eliminator_lenth-1;i>=0;i-=p) // 从最后一行开始往上遍历，每次遍历p行
        {
            if(i-p<0) // 如果剩余行数小于p，则只遍历剩余行数
            {
                cout<<p<<endl;
                p=i+1;
            }

    #pragma omp for schedule(guided,chunksize) // 并行执行for循环，每个线程处理chunksize个元素
            for(j = 0;j<RowLenth;j++) // 遍历每个消元行
            {
                if(eliminatedRow[j].is_new_eliminator==0) // 如果该消元行还没有被消元
                {
                    for(k = 0;k<p;k++) // 遍历当前p行
                    {
                        if(eliminatedRow[j].bit[i-k]==1) // 如果该消元行在当前p行中有1
                        {
                            // 消去过程
                            eliminatedRow[j].bit^=eliminator[i-k];
                        }
                        if(eliminatedRow[j].bit.none()) // 如果该消元行已经被消元为0
                        {
                            eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                            break;
                        }
                        else // 如果该消元行还没有被消元为0
                        {
                            for(l = eliminatedRow[j].FirstOne;l>=0;l--) // 从该消元行的第一个1开始往前遍历
                            {
                                if(eliminatedRow[j].bit[l]==1) // 如果找到了1
                                {
                                    eliminatedRow[j].FirstOne = l; // 更新该消元行的第一个1的位置
    				break;
                                }
                            }
                            if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                            {
                                eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                                break;
                            }
                        }                
                    }
                    
                }       
    	    }

    #pragma omp single // 单线程执行以下代码块
        for(j=0;j<RowLenth;j++) // 遍历每个消元行
            {
                if(eliminatedRow[j].is_new_eliminator==1) // 如果该消元行已经被消元
                {
                    if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                        eliminator[eliminatedRow[j].FirstOne]^=eliminatedRow[j].bit; // 消元该列
                    else // 如果该消元行的第一个1所在的列已经被消元
                    {
                            eliminatedRow[j].is_new_eliminator=0; // 标记该消元行还没有被消元
                        for(k = 0;k<p;k++) // 遍历当前p行
                        {
                            if(eliminatedRow[j].bit[i-k]==1) // 如果该消元行在当前p行中有1
                                    {//消去过程
                            eliminatedRow[j].bit^=eliminator[i-k];} // 消元该行
                        if(eliminatedRow[j].bit.none()) // 如果该消元行已经被消元为0
                                    {eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                            cout<<"null"<<rt++<<endl;
                            break;}
                        else // 如果该消元行还没有被消元为0
                        {for(l = eliminatedRow[j].FirstOne;l>=0;l--) // 从该消元行的第一个1开始往前遍历
                            {
                                if(eliminatedRow[j].bit[l]==1) // 如果找到了1
                                {eliminatedRow[j].FirstOne = l; // 更新该消元行的第一个1的位置
                                    break;}
                            }
                            if(eliminator[eliminatedRow[j].FirstOne].none()) // 如果该消元行的第一个1所在的列还没有被消元
                            {eliminator[eliminatedRow[j].FirstOne]^=eliminatedRow[j].bit; // 消元该列
                                eliminatedRow[j].is_new_eliminator=true; // 标记该消元行已经被消元
                                //eliminatedRow[j].num;
                                //cout<<rt++<<endl;
                                break;} }
                        }
                    }
                }
            }
        if(Finish()) // 如果矩阵已经被消元为0
                {
                    cout<<"null"<<endl;
                    break;
                }

        } 


    
    
    gettimeofday(&end1,NULL);
    long long time_use = ((end1.tv_sec-begin1.tv_sec)*1000000 + end1.tv_usec-begin1.tv_usec);
 
    cout<<"时间："<<time_use<<endl;
    
    return 0;
}
