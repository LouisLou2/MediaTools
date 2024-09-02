//
// Created by leo on 24-9-2.
//
#include<iostream>
#include<cstdio>
#include<cmath>
#include<cstring>
using namespace std;

#define NUM 8
#define PI 3.1415926

int around(double a)
{
    if(a >= 0)
    {
        return int(a+0.5);
    }
    else
    {
        return int(a-0.5);
    }

}
// DCT - Discrete Cosine Transform
void DCT(int data[NUM][NUM])
{
    int output[NUM][NUM];
    double alpha,beta;//C(k)  C(l)
    int m=0,n=0,k=0,l=0;
    for(k = 0;k < NUM;k++)
    {
        for(l = 0;l < NUM;l++)
        {
            if(k == 0)
            {
                alpha = sqrt(1.0/NUM);
            }
            else
            {
                alpha = sqrt(2.0/NUM);
            }
            if(l == 0)
            {
                beta = sqrt(1.0/NUM);
            }
            else
            {
                beta = sqrt(2.0/NUM);
            }
            double temp = 0.0;
            for(m = 0;m < NUM;m++)
            {
                for(n = 0;n < NUM;n++)
                {
                    temp += data[m][n] * cos((2*m+1)*k*PI/(2.0*NUM)) * cos((2*n+1)*l*PI/(2.0*NUM));
                }
            }
            output[k][l] = around(alpha * beta *temp);
        }
    }
    memset(data,0,sizeof(int)*NUM*NUM);
    memcpy(data,output,sizeof(int)*NUM*NUM);

}
//Inverse DCT
void IDCT(int data[NUM][NUM])
{
    int output[NUM][NUM];
    double alpha,beta;
    int m=0,n=0,k=0,l=0;
    for(m = 0;m < NUM;m++)
    {
        for(n = 0;n < NUM;n++)
        {
            double temp = 0.0;
            for(k = 0;k < NUM;k++)
            {
                for(l = 0;l < NUM;l++)
                {
                    if(k == 0)
                    {
                        alpha = sqrt(1.0/NUM);
                    }
                    else
                    {
                        alpha = sqrt(2.0/NUM);
                    }
                    if(l == 0)
                    {
                        beta = sqrt(1.0/NUM);
                    }
                    else
                    {
                        beta = sqrt(2.0/NUM);
                    }

                    temp += alpha * beta * data[k][l] * cos((2*m+1)*k*PI/(2*NUM)) * cos((2*n+1)*l*PI/(2*NUM));

                }
            }
            output[m][n] = around(temp);
        }
    }
    memset(data,0,sizeof(int)*NUM*NUM);
    memcpy(data,output,sizeof(int)*NUM*NUM);

}

int main()
{
    int input[NUM][NUM] =
         {

                   {89, 101, 114, 125, 126, 115, 105, 96},

                   {97, 115, 131, 147, 149, 135, 123, 113},

                   {114, 134, 159, 178, 175, 164, 149, 137},

                   {121, 143, 177, 196, 201, 189, 165, 150},

                   {119, 141, 175, 201, 207, 186, 162, 144},

                   {107, 130, 165, 189, 192, 171, 144, 125},

                   {97, 119, 149, 171, 172, 145, 117, 96},

                   {88, 107, 136, 156, 155, 129, 97, 75}

         };
    DCT(input);
    cout << "The result of DCT is:\n";
    for(int i = 0;i < NUM;i++)
    {
        for(int j = 0;j < NUM;j++)
        {
            cout << input[i][j] << '\t';
        }
        cout << endl;
    }
    IDCT(input);
    cout << "The result of IDCT is:\n";
    for(int i = 0;i < NUM;i++)
    {
        for(int j = 0;j < NUM;j++)
        {
            cout << input[i][j] << '\t';
        }
        cout << endl;
    }
    return 0;
}