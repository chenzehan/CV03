#include<iostream>
#include<fstream>
#include<string>
#include<queue>
#include<windows.h>
#include<gdiplus.h>
#include"identify_cv03class.h"
#pragma comment(lib, "gdiplus.lib")
 
using namespace std;
using namespace Gdiplus;

const int sdirx[12]={1,0,-1,0,2,1,0,-1,-2,-1,0,1};
const int sdiry[12]={0,1,0,-1,0,-1,-2,-1,0,1,2,1};
const int thres_min_R=200;
const int thres_max_G=150;
const int thres_max_B=150;
const int dis_max_R=100;
const int dis_max_G=100;
const int dis_max_B=100;
const int const_catch=64;
const int const_sample=128;
struct Colour{
	int R,G,B;
}sp[const_catch][const_catch];//sample
bool vis[const_catch][const_catch];

int dsize,h,w;
bool in_sp(int x,int y)
{
	return x>=0&&x<w&&y>=0&&y<h;
}
bool Get_sample(int px,int py,Bitmap* bmp,Img_segment7* s,int &cx,int &cy)
{
	int T,B,L,R;
	int sum=0;
	T=B=py;
	L=R=px;
	queue<int>qx;
	queue<int>qy;
	vis[px][py]=1;
	qx.push(px);
	qy.push(py);
	while(!qx.empty()){
		int ux=qx.front();
		int uy=qy.front();
		qx.pop();
		qy.pop();
		++sum;
		for(int k=0;k<12;k++){
			int vx=ux+sdirx[k];
			int vy=uy+sdiry[k];
			if(in_sp(vx,vy)&&!vis[vx][vy]&&sp[vx][vy].R>thres_min_R&&
			sp[vx][vy].G<thres_max_G&&sp[vx][vy].B<thres_max_B){
				vis[vx][vy]=1;
				if(vx<L)L=vx;
				if(vx>R)R=vx;
				if(vy<T)T=vy;
				if(vy>B)B=vy;
				qx.push(vx);
				qy.push(vy);
			}
		}
	}
	if(sum<5)return false;
	int i,j;
	int tot=0;
	int Rtot=0,Gtot=0,Btot=0;
	for(j=T;j<=B;j++){
		for(i=L;i<=R;i++)if(!vis[i][j]){
			++tot;
			Rtot+=sp[i][j].R;
			Gtot+=sp[i][j].G;
			Btot+=sp[i][j].B;
		}
	}
	if(Rtot/tot>=dis_max_R||Gtot/tot>=dis_max_G||Btot/tot>=dis_max_B)return false;
	if(L>0)--L;
	if(T>0)--T;
	int sT,sB,sL,sR;
	int height=bmp->GetHeight();
	int width=bmp->GetWidth();
	sT=T*dsize;
	sL=L*dsize;
	sB=min((B+1)*dsize,height);
	sR=min((R+1)*dsize,width);
	int smH=sB-sT;
	int smW=sR-sL;
	int smsize=max(smH,smW)/const_sample+1;
	unsigned x,y;
	Color c;
	int cprH=smH/smsize;
	int cprW=smW/smsize;
	int** cpr=new int*[cprW];//array compression
	for(i=0;i<cprW;i++){
		cpr[i]=new int[cprH];
	}
	for(y=sT,j=0;j<cprH;y+=smsize,j++)
	    for(x=sL,i=0;i<cprW;x+=smsize,i++){
	    	bmp->GetPixel(x,y,&c);
	    	int R=(int)c.GetRed();
            int G=(int)c.GetGreen();
            int B=(int)c.GetBlue();
            if(R>thres_min_R&&G<thres_max_G&&B<thres_max_B)cpr[i][j]=1;
            else cpr[i][j]=0;
		}
	s->input_image(cpr,cprH,cprW);
	for(i=0;i<cprW;i++){
		delete[] cpr[i];
	}
	delete []cpr;
	cx=(sL+sR)>>1;
	cy=(sT+sB)>>1;
	return true;
}

int main()
{
    GdiplusStartupInput gdiplusstartupinput;
    ULONG_PTR gdiplustoken;
    GdiplusStartup(&gdiplustoken,&gdiplusstartupinput,NULL);
 
    wstring infilename(L"display05.jpg");
    string outfilename("catch.in");
 
    Bitmap* bmp=new Bitmap(infilename.c_str());
    UINT height=bmp->GetHeight();//UINT = unsigned int
    UINT width=bmp->GetWidth();
    
    memset(vis,0,sizeof(vis));
    dsize=max(height,width)/const_catch+1;
    h=height/dsize;
    w=width/dsize;
    int i,j;
    
    Color color;
    ofstream fout(outfilename.c_str());
    
    for(UINT y=0,j=0;y<height;y+=dsize,j++){
    	for(UINT x=0,i=0;x<width;x+=dsize,i++){
            bmp->GetPixel(x,y,&color);
            int R=(int)color.GetRed();
            int G=(int)color.GetGreen();
            int B=(int)color.GetBlue();
            sp[i][j].R=R;
            sp[i][j].G=G;
            sp[i][j].B=B;
        }
    }
    /*
    for(j=0;j<h;j++){
    	for(i=0;i<w;i++)if(sp[i][j].R>thres_min_R&&sp[i][j].G<thres_max_G
			&&sp[i][j].B<thres_max_B)fout<<1;
			else fout<<0;
		fout<<endl;
	}
    */
	char chname[]="catch.out";
    Img_segment7 s;
	s.init();
	for(j=0;j<h;j++)
	    for(i=0;i<w;i++)
		    if(!vis[i][j]&&sp[i][j].R>thres_min_R&&sp[i][j].G<thres_max_G
			&&sp[i][j].B<thres_max_B){
				int x,y;
				if(Get_sample(i,j,bmp,&s,x,y)){
					fout<<y*1.0/height<<","<<x*1.0/width<<endl;
					s.reduce_noise();
					s.cv_identify();
					fout<<s.get_result()<<endl;
					//s.output_img_file(chname);
					s.init();
				}
			}
    
    fout.close();
    
    delete bmp;
    GdiplusShutdown(gdiplustoken);
    return 0;
}
