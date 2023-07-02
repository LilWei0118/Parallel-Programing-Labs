#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<vector>
#include<sys/time.h>
#include<stdlib.h>
#include <xmmintrin.h> //SSE
#include <emmintrin.h> //SSE2
#include <pmmintrin.h> //SSE3
#include <tmmintrin.h> //SSSE3
#include <smmintrin.h> //SSE4.1
#include <nmmintrin.h> //SSSE4.2
#include <immintrin.h> //AVX、AVX2

#define Eliminator_lenth 1789
#define RowLenth 2958

using namespace std;

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
        bool rowIndex = true;
        int x;
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
    int p = 100;
    cout<<"start"<<endl;
    struct timeval begin1,end1;
    gettimeofday(&begin1,NULL);
    for(int i = Eliminator_lenth-1;i>=0;i-=p)
    {
        if(i-p<0)
            p=i+1;
        for(int j = 0;j<RowLenth;j++)
        {
            if(eliminatedRow[j].is_new_eliminator==0)
            {
                for(int k = 0;k<p;k++)
                {
                    if(eliminatedRow[j].bit[i-k]==1)
                    {
                        //消去过程normal
                        //eliminatedRow[j].bit^=eliminator[i-k];			
                        __m128i byte_e = _mm_loadu_si128((__m128i*)(&eliminatedRow[j]));
			            __m128i byte_r = _mm_loadu_si128((__m128i*)(&eliminator[i-k]));
			            byte_e = _mm_xor_si128(byte_e, byte_r);
			            _mm_storeu_si128((__m128i*)(&eliminatedRow[j].bit), byte_e);
                    }
                    //if(eliminatedRow[j].bit[i-k]==1)
                    // {
                    //     //消去过程AVX
                    //     __m256i byte_e = _mm256_loadu_si256((__m256i*)(&eliminatedRow[j]));
                    //     __m256i byte_r = _mm256_loadu_si256((__m256i*)(&eliminator[i-k]));
                    //     byte_e = _mm256_xor_si256(byte_e, byte_r);
                    //     _mm256_storeu_si256((__m256i*)(&eliminatedRow[j].bit), byte_e);
                    // }
                    // if(eliminatedRow[j].bit[i-k]==1)
                    // {
                    //     消去过程AVX512
                    //     __m512i byte_e = _mm512_loadu_si512((__m512i*)(&eliminatedRow[j]));
                    //     __m512i byte_r = _mm512_loadu_si512((__m512i*)(&eliminator[i-k]));
                    //     byte_e = _mm512_xor_si512(byte_e, byte_r);
                    //     _mm512_storeu_si512((__m512i*)(&eliminatedRow[j].bit), byte_e);
                    // }
                    if(eliminatedRow[j].bit.none())
                    {
                        eliminatedRow[j].is_new_eliminator=true;
                        break;
                    }
                    else
                    {
                        for(int l = eliminatedRow[j].FirstOne;l>=0;l--)
                        {
                            if(eliminatedRow[j].bit[l]==1)
                            {
                                eliminatedRow[j].FirstOne = l;
                                break;
                            }
                        }
                        if(eliminator[eliminatedRow[j].FirstOne].none())
                        {
                            eliminator[eliminatedRow[j].FirstOne]^=eliminatedRow[j].bit;
                            eliminatedRow[j].is_new_eliminator=true;
                            break;                
                        }
                    }                
                }
                
            }       
        }
        if(Finish())
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
