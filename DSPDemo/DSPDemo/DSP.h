#include "math.h"
//用混合同余法产生（a，b）区间上的随机数yi.
double uniform(double a,double b,long *seed)
{
	double t;
	*seed=2045*(*seed)+1;
	*seed=*seed-(*seed/1048576)*1048576;
	t=(*seed)/1048576.0;
	t=a+(b-a)*t;
	return(t);
}

double gauss(double mean,double sigma,long *s)
{
	int i;double x,y;
    for(x=0,i=0;i<12;i++)
    {
		x+=uniform(0.0,1.0,s);
	}
    x=x-6.0;
	y=mean+x*sigma;
	return(y);
}

void sinwn(double a[],double f[],double ph[],int m,double fs,double snr,long seed,double x[],int n)
{
	int i,k;
	double z,pi,nsr;
	pi=4.0*atan(1.0);
	z=snr/10.0;
	z=pow(10.0,z);
	z=1.0/(2*z);
	nsr=sqrt(z);
	for(i=0;i<m;i++)
	{
		f[i]=2*pi*f[i]/fs;
		ph[i]=ph[i]*pi/180.0;
	}
	for(k=0;k<n;k++)
	{
		x[k]=0.0;
		for(i=0;i<m;i++)
		{
			x[k]=x[k]+a[i]*sin(k*f[i]+ph[i]);
		}
		x[k]=x[k]+nsr*gauss(0.0,1.0,&seed);
	}
}

//LMS自适应滤波器
//x--双精度实型一维数组，长度n。输入信号
//d--双精度实型一维数组，长度n。理想输出信号
//y--双精度实型一维数组，长度n。实际输出信号
//n--整形变量。输入信号的长度
//w--双精度实型一维数组，长度为m。自适应滤波器的加权系数
//m--整形变量。自适应滤波器的长度（阶数-1）
//mu--双精度实型变量。收敛因子
void lms(double x[],double d[],double y[],int n,double w[],int m,double mu)
{
	int i,k;
	double e;
    for(i=0;i<m;i++)
	{
		w[i]=0.0;
	}
	for(k=0;k<m;k++)
	{
		y[k]=0.0;
		for(i=0;i<=k;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e=d[k]-y[k];
		for(i=0;i<=k;i++)
		{
			w[i]+=2.0*mu*e*x[k-i];
		}
	}
	for(k=m;k<n;k++)
	{
		y[k]=0.0;
		for(i=0;i<m;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e=d[k]-y[k];
		for(i=0;i<m;i++)
		{
			w[i]+=2.0*mu*e*x[k-i];
		}
	}
}

void lms(double x[],double d[],double y[],double e[],int n,double w[],int m,double mu)
{
	int i,k;
    for(i=0;i<m;i++)
	{
		w[i]=0.0;
	}
	for(k=0;k<m;k++)
	{
		y[k]=0.0;
		for(i=0;i<=k;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<=k;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
		}
	}
	for(k=m;k<n;k++)
	{
		y[k]=0.0;
		for(i=0;i<m;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<m;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
		}
	}
}

////////////////THIS LMS FOR CALWAV4 ////////////////////
void lms2(double x[],double d[],double e[],int n,double* v,int m,double mu)
{
	int i,k;
	double w[501],y[501];
    for(i=0;i<m;i++)
	{
		w[i]=0.0;
	}
	for(k=0;k<m;k++)
	{
		y[k]=0.0;
		for(i=0;i<=k;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<=k;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
			*(v+i*501+k)=w[i];
		}
	}
	for(k=m;k<n;k++)
	{
		y[k]=0.0;
		for(i=0;i<m;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<m;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
			*(v+i*501+k)=w[i];
		}
	}
}

//计算H(Z)的频率响应和相频响应
void gain(double b[],double a[],int m,int n,double x[],double y[],int len,int sign)
//len--频率响应的长度
                 //当sign=0,计算实部Re[H(w)]和虚部Im[H(w)]
                 //当sign=1,计算幅频响应|H(w)|和相频响应
                 //当sign=2,计算幅频响应|H(w)|用db表示和相频响应
{
	int i,k;
	double ar,ai,br,bi,zr,zi,im,re;
	double den,numr,numi,freq,temp;
	for(k=0;k<len;k++)
	{
		freq=k*0.5/(len-1);
		zr=cos(-8.0*atan(1.0)*freq);
		zi=sin(-8.0*atan(1.0)*freq);
		br=0.0;
		bi=0.0;
		for(i=m;i>0;i--)
		{
			re=br;
			im=bi;
			br=(re+b[i])*zr-im*zi;
			bi=(re+b[i])*zi+im*zr;
		}
		ar=0.0;
		ai=0.0;
		for(i=n;i>0;i--)
		{
			re=ar;
			im=ai;
			ar=(re+a[i])*zr-im*zi;
			ai=(re+a[i])*zi+im*zr;
		}
		br=br+b[0];
		ar=ar+1.0;
		numr=ar*br+ai*bi;
		numi=ar*bi-ai*br;
		den=ar*br+ai*br;
		x[k]=numr/den;
		y[k]=numi/den;
		switch(sign)
		{
		case 1:
			{
				temp=sqrt(x[k]*x[k]+y[k]*y[k]);
				y[k]=atan2(y[k],x[k]);
				x[k]=temp;
				break;
			}
		case 2:
			{
				temp=x[k]*x[k]+y[k]*y[k];
				y[k]=atan2(y[k],x[k]);
				x[k]=10.0*log10(temp);
			}
		}
	}
}