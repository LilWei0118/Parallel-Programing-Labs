#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<vector>
#include<sys/time.h>
#include<mpi.h>


#define Eliminator_lenth 1789
#define RowLenth 2958

using namespace std;

struct eliRow{
    bool is_new_eliminator;
    int FirstOne;
    bitset<Eliminator_lenth> bit;
};

bitset<Eliminator_lenth> eliminator[Eliminator_lenth]; 
eliRow eline[RowLenth];


bool Finish()
{
    for(int i = 0;i<RowLenth;i++)
    {
        if(eline[i].is_new_eliminator==false)
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
    int rt = 0;

    int p = 25;

        //mpi
        int size;
        int rank;
        MPI_Init(NULL,NULL);
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        int range=RowLenth/size;
        int r1,r2;
        r1=rank*range;
        if(rank!=size-1)r2=r1+range-1;
        else r2=RowLenth-1;

        cout<<"start"<<endl;
        struct timeval begin1,end1;
        gettimeofday(&begin1,NULL);
        for(int i = Eliminator_lenth-1;i>=0;i-=p)
        {
            if(i-p<0)
            {
                cout<<"p=i+1"<<endl;
                p=i+1;
            }

            // 遍历每个进程负责的行
            for(int j=r1;j<=r2;j++)
            {
                if(eline[j].is_new_eliminator==0)
                {
                    // 消元过程
                    for(int k = 0;k<p;k++)
                    {
                        if(eline[j].bit[i-k]==1)
                        {
                            //消去过程
                            eline[j].bit^=eliminator[i-k];
                        }
                        if(eline[j].bit.none())
                        {
                            eline[j].is_new_eliminator=true;
                            cout<<"null"<<rt++<<endl;
                            break;
                        }
                        else
                        {
                            for(int l = eline[j].FirstOne;l>=0;l--)
                            {
                                if(eline[j].bit[l]==1)
                                {
                                    eline[j].FirstOne = l;
                                    break;
                                }
                            }
                            if(eliminator[eline[j].FirstOne].none())
                            {
                                eline[j].is_new_eliminator=true;
                                cout<<rt++<<endl;
                                break;                
                            }
                        }                
                    }
                    
                }       
            }

            // 进行进程间通信
            if(rank!=0)
                MPI_Send(&eline[r1],(r2-r1+1)*sizeof(eliRow),MPI_BYTE,0,1,MPI_COMM_WORLD);
            else for(int q=1;q<size;q++){
                if(q!=size-1)
                    MPI_Recv(&eline[q*range],range*sizeof(eliRow),MPI_BYTE,q,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                else MPI_Recv(&eline[q*range],sizeof(eliRow)*(range+RowLenth%size),MPI_BYTE,q,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
            
            // 进行进程间通信
            if(rank==0){
                for(int q=1;q<size;q++){
                    if(q!=size-1)
                        MPI_Send(&eliminator[q*range],range*sizeof(bitset<Eliminator_lenth>),MPI_BYTE,q,2,MPI_COMM_WORLD);
                    else MPI_Send(&eliminator[q*range],sizeof(bitset<Eliminator_lenth>)*(range+RowLenth%size),MPI_BYTE,q,2,MPI_COMM_WORLD);
                }
            }

            if(rank!=0){
                MPI_Recv(&eliminator[r1],(r2-r1+1)*sizeof(bitset<Eliminator_lenth>),MPI_BYTE,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }

            // 判断是否已经消元完成
            if(Finish())
            {
                cout<<"null"<<endl;
                break;
            }      
        } 

        gettimeofday(&end1,NULL);
        // 输出时间
        long long time_use = ((end1.tv_sec-begin1.tv_sec)*1000000 + end1.tv_usec-begin1.tv_usec);
        cout<<"时间："<<time_use<<endl;
        MPI_Finalize();
        return 0;
    }
    long long time_use = ((end1.tv_sec-begin1.tv_sec)*1000000 + end1.tv_usec-begin1.tv_usec);
    res();
    //cout1();
    cout<<"时间："<<time_use<<endl;
    MPI_Finalize();
    return 0;
}
