/*-----cv_basic.h-----*/
#ifndef CV_BASIC_H
#define CV_BASIC_H

#include<iostream>
#include<cmath>
#define sqr(a) ((a)*(a))
#define max_(a,b) ((a)>(b)?(a):(b))
#define min_(a,b) ((a)<(b)?(a):(b))

const int space_x[9]={0,1,1,0,-1,-1,-1,0,1};//9 directions
const int space_y[9]={0,0,1,1,1,0,-1,-1,-1};
const int dirx[4]={1,0,-1,0};//4 directions up,down,left,right
const int diry[4]={0,1,0,-1};

typedef struct {//plane vector
	int x,y;
	int basic_direction()//consistent with dirx,y
	{
		//consider line y=x y=-x and the four parts they cut out
		if(x>=y&&x>-y)return 0;
		if(x>y&&x<=-y)return 3;
		if(x<=y&&x<-y)return 2;
		return 1;
	}
	double modulus()
	{
		return sqrt(sqr(x)+sqr(y));
	}
	int modulus_sqr()
	{
		return sqr(x)+sqr(y);
	}
	void set(int x_,int y_)
	{
		x=x_;
		y=y_;
	}
	void init()
	{
		x=y=0;
	}
}pl_vector;
pl_vector make_vec(int x,int y)
{
	pl_vector a;
	a.set(x,y);
	return a;
}
pl_vector operator+(pl_vector a,pl_vector b)
{
	pl_vector c;
	c.x=a.x+b.x;
	c.y=a.y+b.y;
	return c;
}
void operator+=(pl_vector &a,pl_vector b)
{
	a.x+=b.x;
	a.y+=b.y;
}
pl_vector operator-(pl_vector a,pl_vector b)
{
	pl_vector c;
	c.x=a.x-b.x;
	c.y=a.y-b.y;
	return c;
}
pl_vector operator-=(pl_vector &a,pl_vector b)
{
	a.x-=b.x;
	a.y-=b.y;
}
pl_vector operator*(int a,pl_vector b)
{
	b.x*=a;
	b.y*=a;
	return b;
}
pl_vector operator*(double a,pl_vector b)
{
	b.x=round(b.x*a);
	b.y=round(b.y*a);
	return b;
}
int operator*(pl_vector a,pl_vector b)
{
	return a.x*b.x+a.y*b.y;
}
bool operator==(pl_vector a,pl_vector b)
{
	return a.x==b.x&&a.y==b.y;
}
std::ostream& operator<<(std::ostream& out,pl_vector a)
{
	out<<"("<<a.x<<","<<a.y<<")";
	return out;
}

//define basic vector calculation
const pl_vector spacev[8]={{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
const pl_vector dirv[4]={{1,0},{0,1},{-1,0},{0,-1}};
const int sspace_size=60;
const int espace_size=12;
const pl_vector espacev[espace_size]=
{{0,-2},{-1,-1},{0,-1},{1,-1},{-2,0},{-1,0},{1,0},{2,0},{-1,1},{0,1},{1,1},{0,2}};
const pl_vector sspacev[sspace_size]=
{{0,-5},{-1,-4},{0,-4},{1,-4},{-2,-3},{-1,-3},{0,-3},{1,-3},{2,-3},{-3,-2},{-2,-2},{-1,-2},
{0,-2},{1,-2},{2,-2},{3,-2},{-4,-1},{-3,-1},{-2,-1},{-1,-1},{0,-1},{1,-1},{2,-1},{3,-1},
{4,-1},{-5,0},{-4,0},{-3,0},{-2,0},{-1,0},{1,0},{2,0},{3,0},{4,0},{5,0},{-4,1},{-3,1},
{-2,1},{-1,1},{0,1},{1,1},{2,1},{3,1},{4,1},{-3,2},{-2,2},{-1,2},{0,2},{1,2},{2,2},{3,2},
{-2,3},{-1,3},{0,3},{1,3},{2,3},{-1,4},{0,4},{1,4},{0,5}};

typedef struct{
	int R,G,B;
	void set(int R_,int G_,int B_)
	{
		R=R_;
		G=G_;
		B=B_;
	}
}Colour;
std::ostream& operator<<(std::ostream& out,Colour c)
{
	out<<"(R="<<c.R<<",G="<<c.G<<",B="<<c.B<<")";
	return out;
}
typedef struct{
	double H,S,V;
}HSV;
HSV get_HSV(Colour c)
{
	double mx=max_(c.R,max_(c.G,c.B));
	double mn=min_(c.R,min_(c.G,c.B));
	HSV f;
	f.V=mx;
	if(mx==0){
		f.S=0;
	}
	else f.S=(mx-mn)/mx;
	if(mx==mn){
		f.H=0;
		return f;
	}
	if(c.R==mx)f.H=(c.G-c.B)/(mx-mn)*60;
    if(c.G==mx)f.H=120+(c.B-c.R)/(mx-mn)*60;
    if(c.B==mx)f.H=240+(c.R-c.G)/(mx-mn)*60;
    if(f.H<0)f.H+=360;
    return f;
}
std::ostream& operator<<(std::ostream& out,HSV h)
{
	out<<"(H="<<h.H<<",S="<<h.S<<",V="<<h.V<<")";
	return out;
}

#endif
