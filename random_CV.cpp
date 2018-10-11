#include<iostream>
#include<fstream> 
#include<algorithm>
#include<cstring>
#include<time.h>
#include<cmath>
#include<windows.h>
#define sqr(a) (a)*(a)
#define size1 128
#define pi 3.1415926536
using namespace std;
int display[size1][size1];

const double percent_=0.10;
const double thick_=3;
const double inw=6;

const double v1x=-1;//-1;
const double v1y=1;//1;
const double v2x=-1;//-4;
const double v2y=1;//8;

const double centre_x=size1>>1;
const double centre_y=size1>>1;
const double theta_=2*pi/180;
const double distort_=1;//4;

void rotate_point(double &x1,double &y1,double x0,double y0,double theta)//counterclockwise
{
	double v0x=x1-x0;
	double v0y=y1-y0;
	double c=cos(theta),s=sin(theta);
	double v1x=c*v0x-s*v0y;
	double v1y=s*v0x+c*v0y;
	x1=x0+v1x;
	y1=y0+v1y;
} 
struct segment{
	double A,B,C;//Ax+By+C=0;
	double x1,y1;
	double x2,y2;
	void set(double x1_,double y1_,double x2_,double y2_)
	{
		x1=x1_;y1=y1_;x2=x2_;y2=y2_;
		if(x1==x2){
			B=0;
			A=1;
			C=-x1;
			return;
		}
		double k=(y1-y2)/(x1-x2);
		double b=y1-k*x1;
		B=1;
		A=-k;
		C=-b;
	}
	void add_vector(double x1_,double y1_,double x2_,double y2_)
	{
		set(x1+x1_,y1+y1_,x2+x2_,y2+y2_);
	}
	void add_rotate(double x0,double y0,double theta)//counterclockwise
	{
		rotate_point(x1,y1,x0,y0,theta);
		rotate_point(x2,y2,x0,y0,theta);
		set(x1,y1,x2,y2);
	}
}s[7];
bool d[7];
/*
 -
| |
 -
| |
 _
*/
int rand01()
{
	return rand()&1;
}
double rand_real(double x)
{
	return x*(rand()%20000-10000)/10000.0;
}
int rand_percent(double x)
{
	if(x==0)return 0;
	int t=int(1.0/x);
	return !(rand()%t);
}
void set_num(int x)
{
	memset(d,0,sizeof(d));
	switch(x){
		case 0:d[0]=d[1]=d[2]=d[4]=d[5]=d[6]=1;break;
		case 1:d[2]=d[5]=1;break;
		case 2:d[0]=d[2]=d[3]=d[4]=d[6]=1;break;
		case 3:d[0]=d[2]=d[3]=d[5]=d[6]=1;break;
		case 4:d[1]=d[2]=d[3]=d[5]=1;break;
		case 5:d[0]=d[1]=d[3]=d[5]=d[6]=1;break;
		case 6:d[0]=d[1]=d[3]=d[4]=d[5]=d[6]=1;break;
		case 7:d[0]=d[2]=d[5]=1;break;
		case 8:d[0]=d[1]=d[2]=d[3]=d[4]=d[5]=d[6]=1;break;
		case 9:d[0]=d[1]=d[2]=d[3]=d[5]=d[6]=1;break;
		default:break;
	}
}
double eu_dis(double x1,double y1,double x2,double y2)
{
	return sqrt(sqr(x1-x2)+sqr(y1-y2));
}
double distance_segment(segment a,double x,double y)
{
	if(a.B*x-a.A*y-(a.B*a.x1-a.A*a.y1)>0^a.B*x-a.A*y-(a.B*a.x2-a.A*a.y2)>0)
	    return abs(a.A*x+a.B*y+a.C)/sqrt(sqr(a.A)+sqr(a.B));
	else return min(eu_dis(a.x1,a.y1,x,y),eu_dis(a.x2,a.y2,x,y));
}
void init_segment()
{
	int L=size1>>2;
	int R=size1-L;
	int B=size1>>2;
	int T=size1-L;
	int M=(T+L)>>1;
	s[0].set(L+inw,T,R-inw,T);
	s[1].set(L,T-inw,L,M+inw);
	s[2].set(R,T-inw,R,M+inw);
	s[3].set(L+inw,M,R-inw,M);
	s[4].set(L,M-inw,L,B+inw);
	s[5].set(R,M-inw,R,B+inw);
	s[6].set(L+inw,B,R-inw,B);
}
void set_display(double thick)
{
	int i,j,k;
	for(i=0;i<size1;i++)
	    for(j=0;j<size1;j++){
	    	for(k=0;k<7;k++){
	    		if(d[k]&&distance_segment(s[k],i,j)<=thick)display[i][j]=1;
			}
		}
}
void distort_vector()
{
	s[0].add_vector(v1x,v1y,v2x,v2y);
	s[3].add_vector(v1x,v1y,v2x,v2y);
	s[6].add_vector(v1x,v1y,v2x,v2y);
	s[1].add_vector(v1x,v1y,v1x,v1y);
	s[4].add_vector(v1x,v1y,v1x,v1y);
	s[2].add_vector(v2x,v2y,v2x,v2y);
	s[5].add_vector(v2x,v2y,v2x,v2y);
}
void distort_rotate()
{
	for(int i=0;i<7;i++)s[i].add_rotate(centre_x,centre_y,theta_);
}
void distort_random()
{
	for(int i=0;i<7;i++)
	    s[i].add_vector(rand_real(distort_),rand_real(distort_),
		                rand_real(distort_),rand_real(distort_));
}
void set_centre_mass()
{
	int sx=0,sy=0,n=0;
	int i,j;
	for(i=0;i<size1;i++)
	    for(j=0;j<size1;j++)if(display[i][j]){
	    	sx+=i;
	    	sy+=j;
	    	++n;
		}
	display[sx/n][sy/n]=2;
}
void output_display()
{
	int x,y;
	for(y=size1-1;y>=0;y--){
		for(x=0;x<size1;x++)
	        if(display[x][y])cout<<"*";
			else cout<<" ";
		cout<<"\n";
	}
}
void output_display2()
{
	int x,y;
	for(y=size1-1;y>=0;y--){
		for(x=0;x<size1;x++)
	        if(display[x][y]==1)cout<<"*";
	        else if(display[x][y]==2)cout<<"+";
			else cout<<" ";
		cout<<"\n";
	}
}
void output_file()
{
	//ofstream fout("cv31.in");
	ofstream fout("test.in");
	fout<<size1<<" "<<size1<<endl;
	int x,y;
	for(y=size1-1;y>=0;y--){
		for(x=0;x<size1;x++)
	        if(display[x][y])fout<<1;
			else fout<<0;
		fout<<"\n";
	}
	fout.close();
}
void output_ans(int ans)
{
	ofstream gout("test_ans.in");
	gout<<ans<<endl;
	gout.clear();
}
int main()
{
	srand((unsigned)time(NULL));
	int i,j;
	
	//int tt=0;
	int tt=rand()%3+1;
	//while(1){
	
	memset(display,0,sizeof(display));
	init_segment();
	set_num(tt);
	distort_vector();
	distort_rotate();
	distort_random();
	
	set_display(thick_);
	for(i=0;i<size1;i++)
	    for(j=0;j<size1;j++)
	        if(rand_percent(percent_))display[i][j]=!display[i][j];
	
	//output_display();
	output_file();
	output_ans(tt);
	//set_centre_mass();
	//output_display2();
	
	//Sleep(1000);
	//system("cls");
	tt=(tt+1)%10;
    //}
    
	return 0;
}
