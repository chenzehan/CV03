#include<iostream>
#include<fstream>
#include<string>
#include<queue>
#include<windows.h>
#include<gdiplus.h>
#include"cv_basic.h"
#include"identify_cv05.h"
#pragma comment(lib, "gdiplus.lib")
 
using namespace std;
using namespace Gdiplus;

const int const_s=60;
const int const_catch=128;
const int const_sample=128;
bool visit[const_sample][const_sample];//visit
Colour sample[const_sample][const_sample];
const int min_catch_size=3;

const int min_V=200;
const int Hred01=60;
const int Hred02=300;
const double min_S=0.2;

const int Diff=70;

int dsize,height,width;
bool in_sp(int x,int y)
{
	return x>=0&&x<width&&y>=0&&y<height;
}
bool in_sp(pl_vector a)
{
	return a.x>=0&&a.x<width&&a.y>=0&&a.y<height;
}

bool different_colour(Colour A,Colour B)
{
	return A.R>B.R+Diff||A.R<B.R-Diff||A.G>B.G+Diff||A.G<B.G-Diff||A.B>B.B+Diff||A.B<B.B-Diff;
}
bool similar_colour(Color c,Colour A)
{
	int R=(int)c.GetRed();
    int G=(int)c.GetGreen();
    int B=(int)c.GetBlue();
    return A.R<=R+Diff&&A.R>=R-Diff&&A.G<=G+Diff&&A.G>=G-Diff&&A.B<=B+Diff&&A.B>=B-Diff;
}
void simple_add(Colour &A,Colour B)
{
	A.R+=B.R;
	A.G+=B.G;
	A.B+=B.B; 
}
void get_average_colour(Colour &A,Colour total,int n)
{
	A.R=total.R/n;
	A.G=total.G/n;
	A.B=total.B/n;
}
bool is_red(HSV A)
{
	return (A.H<60||A.H>300)&&A.V>min_V&&A.S>min_S;
}
void cover_colour(int x,int y,Colour &average_colour,pl_vector &top_left,pl_vector &bottom_right,int &catch_size)
{
	Colour total_colour;
	int total=1;
	//average colour = total colour / total
	pl_vector u,v;
	visit[x][y]=true;
	queue<pl_vector>q;
	u=top_left=bottom_right=make_vec(x,y);
	q.push(u);
	total_colour=average_colour=sample[x][y];
	catch_size=0;
	while(!q.empty()){
		u=q.front();
		q.pop();
		++catch_size;
		if(u.x<top_left.x)top_left.x=u.x;
		if(u.y<top_left.y)top_left.y=u.y;
		if(u.x>bottom_right.x)bottom_right.x=u.x;
		if(u.y>bottom_right.y)bottom_right.y=u.y;
		for(int k=0;k<sspace_size;k++){
			v=u+sspacev[k];
			if(!in_sp(v)||visit[v.x][v.y]||different_colour(average_colour,sample[v.x][v.y]))continue;
			q.push(v);
			visit[v.x][v.y]=true;
			simple_add(total_colour,sample[v.x][v.y]);
			++total;
			get_average_colour(average_colour,total_colour,total);
		}
	}
}
void get_display(Colour object_colour,pl_vector top_left,pl_vector bottom_right,
Bitmap* bmp,Img_segment7 *s,pl_vector &centre)
{
	if(top_left.x>0)--top_left.x;
	if(top_left.y>0)--top_left.y;
	int sT,sB,sL,sR;
	int height_bmp=bmp->GetHeight();
	int width_bmp=bmp->GetWidth();
	sT=top_left.y*dsize;
	sL=top_left.x*dsize;
	sR=min((bottom_right.x+1)*dsize,width_bmp);
	sB=min((bottom_right.y+1)*dsize,height_bmp);
	int smH=sB-sT;
	int smW=sR-sL;
	int smsize=max(smH,smW)/const_sample+1;
	unsigned x,y;
	Color c;
	int cprH=smH/smsize;
	int cprW=smW/smsize;
	int** cpr=new int*[cprW];//array compression
	int i,j;
	for(i=0;i<cprW;i++){
		cpr[i]=new int[cprH];
	}
	for(y=sT,j=0;j<cprH;y+=smsize,j++)
	    for(x=sL,i=0;i<cprW;x+=smsize,i++){
	    	bmp->GetPixel(x,y,&c);
            if(similar_colour(c,object_colour))cpr[i][j]=1;
            else cpr[i][j]=0;
		}
	s->input_image(cpr,cprH,cprW);
	for(i=0;i<cprW;i++){
		delete[] cpr[i];
	}
	delete []cpr;
	centre.x=(sL+sR)>>1;
	centre.y=(sT+sB)>>1;
}

int main()
{
    GdiplusStartupInput gdiplusstartupinput;
    ULONG_PTR gdiplustoken;
    GdiplusStartup(&gdiplustoken,&gdiplusstartupinput,NULL);
    
    wstring infilename(L"display12.jpg");
    string outfilename("catch_cv05.in");
    
    Bitmap* bmp=new Bitmap(infilename.c_str());
    UINT height_bmp=bmp->GetHeight();//UINT = unsigned int
    UINT width_bmp=bmp->GetWidth();
    
    memset(visit,0,sizeof(visit));
    dsize=max(height_bmp,width_bmp)/const_sample+1;
    height=height_bmp/dsize;
    width=width_bmp/dsize;
    int i,j;
    
    Color color;
    ofstream fout(outfilename.c_str());
    
    for(UINT y=0,j=0;y<height_bmp;y+=dsize,j++){
    	for(UINT x=0,i=0;x<width_bmp;x+=dsize,i++){
            bmp->GetPixel(x,y,&color);
            sample[i][j].set((int)color.GetRed(),(int)color.GetGreen(),(int)color.GetBlue());
        }
    }
    //memset(visit,0,sizeof(visit));
    
    for(j=0;j<height;j++){
    	for(i=0;i<width;i++){
    		if(is_red(get_HSV(sample[i][j])))fout<<1;
    		else fout<<0;
    		//HSV H=get_HSV(sample[i][j]);
    		//fout<<"("<<(int)H.H<<","<<H.V<<","<<H.S<<")";
		}
		fout<<endl;
	}
	
	Colour average_colour;
	HSV average_HSV,point_HSV;
	Img_segment7* s=new Img_segment7;
	char ch[]="cv05_catch0.out";
	int No=0;
	
	fout<<width<<" "<<height<<endl;
    for(j=0;j<height;j++){
    	for(i=0;i<width;i++)if(!visit[i][j]){
    		point_HSV=get_HSV(sample[i][j]);
    		if(!is_red(point_HSV))continue;
    		pl_vector top_left,bottom_right;
    		
    		int catch_size;
    		cover_colour(i,j,average_colour,top_left,bottom_right,catch_size);
    		average_HSV=get_HSV(average_colour);
    		fout<<"HSV:"<<No<<":"<<catch_size<<":"<<average_HSV<<endl;
    		//fout<<top_left<<" "<<bottom_right<<endl;
    		if(!is_red(average_HSV)||catch_size<min_catch_size)continue;
    		
    		pl_vector centre;
    		s->init();
    		get_display(average_colour,top_left,bottom_right,bmp,s,centre);
    		s->cv05_identify();
    		ch[10]='0'+(++No);
    		fout<<centre.x*1.0/width<<","<<centre.y*1.0/height<<endl;
    		fout<<s->get_result()<<endl;
			s->output_img_file(ch);
		}
	}
	delete s;
    //fout<<No<<endl;
    
    fout.close();
    delete bmp;
    GdiplusShutdown(gdiplustoken);
    return 0;
}
