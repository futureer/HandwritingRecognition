#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <deque>
using namespace std;
int m_lianXuShu;
int digicount;
FILE *stream;
class CPoint
{
public:
	int y;
	int x;
};
class CRect
{
public:
	int top;
	int bottom;
	int left;
	int right;
	int Width()
	{
		return right-left+1;
	}
	int Height()
	{
		return bottom-top+1;
	}
};
typedef deque<CRect> CRectLink;
CRectLink m_charRectCopy;
CRectLink m_charRect;
extern "C" {
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_ImgToGray(JNIEnv* env, jobject obj, jintArray buf, int w, int h);
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_ImgToBinary(JNIEnv* env,jobject, jintArray buf, int w, int h);
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_GradientSharp(JNIEnv* env,jobject, jintArray buf, int w, int h);
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_RemoveScatterNoise(JNIEnv* env,jobject, jintArray buf, int w, int h);
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_SlopeAdjust(JNIEnv* env, jobject obj,jintArray buf,int w,int h);
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_CharSegment(JNIEnv* env, jobject obj,jintArray buf,int w,int h);
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_ImgToBinaryOTSU(JNIEnv* env,jobject, jintArray buf, int w, int h);
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_PcaBpnnRecog(JNIEnv* env, jobject obj, jintArray buf, int w, int h);
};
/*************************************************************************
 *
 * 函数名称：
 *   ImgToGray()
 *
 * 参数:
 *   jintArray buf    - 指向源图像像素数组
 *   int w       - 源图像宽度（象素数）
 *   int h       - 源图像高度（象素数）
 * 返回值:
 *   jintArray               - 返回新的像素数组。
 *
 * 说明:
 * 该函数用于对256色图像进行灰度化处理。
 ************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_ImgToGray(JNIEnv* env, jobject obj, jintArray buf, int w, int h)
{
    jint *cbuf;
    cbuf = env->GetIntArrayElements(buf, false);
    if (cbuf == NULL) {
        return 0; /* exception occurred */
    }
    int alpha = 0xFF << 24;  //alpha=0xFF000000
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            // 获得像素的颜色
            int color = cbuf[w * i + j];
            int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
            int green = ((color & 0x0000FF00) >> 8);
            int blue = color & 0x000000FF;
            color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
            color = alpha | (color << 16) | (color << 8) | color;  //或运算，将rgb转化之后的值赋给r,g,b,使rgb相同，变成单色
            cbuf[w * i + j] = color;
        }
    }
    int size=w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, cbuf);
    env->ReleaseIntArrayElements(buf, cbuf, 0);
    return result;
}
/*************************************************************************
 *
 * 函数名称：
 *   ImgToBinary()
 *
 * 参数:
 *   jintArray buf    - 指向源图像像素数组
 *   int w       - 源图像宽度（象素数）
 *   int h       - 源图像高度（象素数）
 * 返回值:
 *   jintArray               - 返回新的像素数组。
 *
 * 说明:
 * 该函数用于对灰度图像进行二值化。
 ************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_ImgToBinary(JNIEnv* env,jobject, jintArray buf, int w, int h)
{
	jint *cbuf;
	cbuf = env->GetIntArrayElements(buf, false);
	if(cbuf==NULL)
	{
		return 0;
	}
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			// 获得像素的颜色
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
			if(color<90)
			{
				color=0;
			}
			else
			{
				color=255;
			}
			color = alpha | (color << 16) | (color << 8) | color;  //或运算，将rgb转化之后的值赋给r,g,b,使rgb相同，变成单色
			cbuf[w * i + j] = color;
		}
	}
	int size=w * h;
	jintArray result = env->NewIntArray(size);
	env->SetIntArrayRegion(result, 0, size, cbuf);
	env->ReleaseIntArrayElements(buf, cbuf, 0);
	return result;
}
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_ImgToBinaryOTSU(JNIEnv* env,jobject, jintArray buf, int w, int h)
{
	jint *cbuf;
	int pixelValue[256] ={0};
	long double p[256] ={0};
	    cbuf = env->GetIntArrayElements(buf, false);
	    if (cbuf == NULL) {
	        return 0; /* exception occurred */
	    }
	    int alpha = 0xFF << 24;  //alpha=0xFF000000
	    for (int i = 0; i < h; i++) {
	        for (int j = 0; j < w; j++) {
	            // 获得像素的颜色
	            int color = cbuf[w * i + j];
	            int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
	            int green = ((color & 0x0000FF00) >> 8);
	            int blue = color & 0x000000FF;
	            color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
	            //color = alpha | (color << 16) | (color << 8) | color;  //或运算，将rgb转化之后的值赋给r,g,b,使rgb相同，变成单色
	            cbuf[w * i + j] = color;
	            pixelValue[cbuf[w*i+j]]++;
	        }
	    }
	    for(int i=0;i<256;i++)
	    {
	    	p[i]=(long double)pixelValue[i]/(long double )(w*h);
	    }
	    int t=0;
	    int Threshold=0;
	    long double w_a=0;
	    long double w_b=0;
	    long double u_a=0;
	    long double u_b=0;
	    long double maxB=0;
	    for(t=0;t<256;t++)
	    {
	    	for(int i=0;i<t;i++)
	    	{
	    		w_a=w_a+p[i];
	    	}
	    	for (int i=t+1;i<256;i++)
		    {
		        w_b = w_b +p[i];
		    }
		//
			for (int i=0;i<=t;i++)
		    {
		        u_a += i*p[i]/w_a;
		    }

			for (int i=t+1;i<256;i++)
		    {
		        u_b += i *p[i]/w_b;
		    }

			if (w_a*w_b*(u_a-u_b)*(u_a-u_b)>maxB)
		    {
		        maxB = w_a*w_b*(u_a-u_b)*(u_a-u_b);
		        Threshold = t;
		    }
		    w_a = w_b = u_a = u_b = 0;
	    }
	    int colortemp;
	    for(int i = 0; i < h; i++)
		{
		   // 每列
		   for(int j = 0; j < w; j++)
		   {
			// 指向DIB第i行，第j个象素的指针
			//lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (cyDIB - 1 - i) + j;
			 colortemp=cbuf[w*i+j];
			if ( colortemp >Threshold)
			{
				colortemp=255;
				colortemp = alpha | (colortemp << 16) | (colortemp << 8) | colortemp;
				cbuf[w*i+j]=colortemp;
			}
			else
			{
				colortemp=0;
				colortemp = alpha | (colortemp << 16) | (colortemp << 8) | colortemp;
				cbuf[w*i+j]=colortemp;
			}
		   }
		}
	    int size=w * h;
	    jintArray result = env->NewIntArray(size);
	    env->SetIntArrayRegion(result, 0, size, cbuf);
	    env->ReleaseIntArrayElements(buf, cbuf, 0);
	    return result;
}
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_GradientSharp(JNIEnv* env,jobject, jintArray buf, int w, int h)
{

	jint *cbuf;
	cbuf = env->GetIntArrayElements(buf, false);
	if(cbuf==NULL)
	{
		return 0;
	}
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			// 获得像素的颜色
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
			if(color<90)
			{
				color=0;
			}
			else
			{
				color=255;
			}
			color = alpha | (color << 16) | (color << 8) | color;  //或运算，将rgb转化之后的值赋给r,g,b,使rgb相同，变成单色
			cbuf[w * i + j] = color;
		}
	}

	int bThre=2;
	int bTemp;
	for(int i = 0; i < h; i++)
		{
	     // 每列
		 for(int j = 0; j < w; j++)
			{
			     bTemp=abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-2-i)+j])+abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-1-i)+j+1]);

			    // 判断是否小于阈值
			    if (bTemp < 255)
				{
			     // 判断是否大于阈值，对于小于情况，灰度值不变。
				   if (bTemp >= bThre)
				   {
					   bTemp=alpha|(bTemp<<16)|(bTemp<<8)|bTemp;
					   cbuf[w*(h-1-i)+j]=bTemp;
				   }
				}
	           else
				{
					// 直接赋值为255
	        	   int color1=255;
	        	   color1=alpha|(color1<<16)|(color1<<8)|color1;
	        	   cbuf[w*(h-1-i)+j]=color1;
				}
		 }
	   }

	//最后还要处理一下图像中最下面那行
	for(int j = 0; j < h; j++)
	{
	 //指向最下边一行，第j个象素的指针
		int color1=255;
		color1=alpha|(color1<<16)|(color1<<8)|color1;
		cbuf[w*0+j]=color1;
	}
	int size=w * h;
	jintArray result = env->NewIntArray(size);
	env->SetIntArrayRegion(result, 0, size, cbuf);
	env->ReleaseIntArrayElements(buf, cbuf, 0);
	return result;
}
//bool DeleteScaterJudge(int *cbuf,jboolean *lplab, int lWidth, int lHeight, int x, int y, CPoint lab[], int lianXuShu)
bool DeleteScaterJudge(int *cbuf,bool *lplab, int lWidth, int lHeight, int x, int y, CPoint lab[], int lianXuShu)
{

	//如果连续长度满足要求，说明不是离散点，返回
	if(m_lianXuShu>=lianXuShu)
		return true;
	//长度加一
	m_lianXuShu++;
	//设定访问标志
	lplab[lWidth * y +x] = true;

	//保存访问点坐标
	lab[m_lianXuShu-1].x = x;
	lab[m_lianXuShu-1].y = y;

	//象素的灰度值
	int gray;

	//长度判定
    //如果连续长度满足要求，说明不是离散点，返回
	if(m_lianXuShu>=lianXuShu)
		return true;

	//下面进入递归
	else
	{
		//考察上下左右以及左上、右上、左下、右下八个方向
		//如果是黑色点，则调用函数自身进行递归

		//考察下面点
		gray=cbuf[lWidth*(y-1)+x];
		//fprintf(stream,"%d %d %d\n",gray,y,x);
		//如果点在图像内、颜色为黑色并且没有被访问过
		if(y-1 >=0 && gray == 0 && lplab[(y-1)*lWidth+x] == false)
		{
			//fprintf(stream,"%s\n","1 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y-1,lab,lianXuShu);
		}
		//判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//左下点
		gray=cbuf[lWidth*(y-1)+x-1];

        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y-1 >=0 &&  x-1 >=0 && gray== 0 && lplab[(y-1)*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","2 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y-1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}


		//左边

		gray=cbuf[lWidth*y+x-1];

        //如果点在图像内、颜色为黑色并且没有被访问过
		if(x-1 >=0 &&  gray== 0 && lplab[y*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","3 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//左上

		gray=cbuf[lWidth*(y+1)+x-1];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y+1 <lHeight && x-1 >= 0 && gray == 0 && lplab[(y+1)*lWidth+x-1] == false)
		{
		//进行递归处理
		//fprintf(stream,"%s\n","4 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y+1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//上面
		gray=cbuf[lWidth*(y+1)+x];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y+1 < lHeight && gray == 0 && lplab[(y+1)*lWidth+x] == false)
		{
        //进行递归处理
		//fprintf(stream,"%s\n","5 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y+1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//右上

		gray=cbuf[lWidth*(y+1)+x+1];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y+1 <lHeight && x+1 <lWidth &&  gray == 0 && lplab[(y+1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","6 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y+1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//右边

		gray=cbuf[lWidth*y+x+1];
		//如果点在图像内、颜色为黑色并且没有被访问过
		if(x+1 <lWidth && gray==0 && lplab[y*lWidth+x+1] == false)
		{
		//fprintf(stream,"%s\n","7 ");
        //进行递归处理
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y,lab,lianXuShu);
		}
        //判断长度

		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//右下
		gray=cbuf[lWidth*(y-1)+x+1];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y-1 >=0 && x+1 <lWidth && gray == 0 && lplab[(y-1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","8 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y-1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
	}

	//fprintf(stream,"%s\n","error");
	//如果递归结束，返回false，说明是离散点
	return false;

}
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_RemoveScatterNoise(JNIEnv* env,jobject, jintArray buf, int w, int h)
{
	jint *cbuf;
	//char buffer[] = {'x' , 'y' , 'z'};
	//stream=fopen("/sdcard/log.txt","wb");
	//fprintf(stream,"%s\n","hello world");
	//fwrite (buffer , sizeof(buffer), 1 , stream );
	int lHeight=h;
	int lWidth=w;
	int i;
	int j;
	int k;
	cbuf = env->GetIntArrayElements(buf, false);
	if(cbuf==NULL)
	{
		return 0;
	}
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// 获得像素的颜色
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
			if(color<90)
			{
				color=0;
			}
			else
			{
				color=255;
			}
			cbuf[w * i + j] = color;
		}
	}
	jboolean *lplab=(jboolean *)malloc(sizeof(jboolean)*w*h);
	jboolean *lpTemp=(jboolean *)malloc(sizeof(jboolean)*w*h);
	int length=15;
	// 循环变量
	m_lianXuShu=0;
	CPoint lab[21];
	for (i=0;i<lHeight*lWidth;i++)
    {
    //将所有的标志位设置为非
		lplab[i] = false;
	}
	for(i =0;i<lHeight;i++)
	{

	   //逐行扫描
		for(j=0;j<lWidth;j++)
			{
				//先把标志位置false
				for(k=0;k<m_lianXuShu;k++)
				lplab[lab[k].y * lWidth + lab[k].x] = false;

				//连续数置0
				m_lianXuShu =0;

				//进行离散性判断
				//lpTemp[i*lWidth+j] = DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,j,i,lab,length);
				lpTemp[i*lWidth+j] = DeleteScaterJudge(cbuf,(bool *)lplab,lWidth,lHeight,j,i,lab,length);
			}
	}
	for(i = 0;i<lHeight;i++)
	{

	  //逐列扫描
		for(j=0;j<lWidth;j++)
		{
				//查看标志位,如果为非则将此点设为白点
				if(lpTemp[i*lWidth+j] == false)
				{
					int color1=255;
					color1=alpha|(color1<<16)|(color1<<8)|color1;
					cbuf[lWidth*i+j]=color1;
				}
				else
				{
					int color1=0;
					color1=alpha|(color1<<16)|(color1<<8)|color1;
					cbuf[lWidth*i+j]=color1;
				}
			}
	}
	int size=w * h;
	jintArray result = env->NewIntArray(size);
	env->SetIntArrayRegion(result, 0, size, cbuf);
	env->ReleaseIntArrayElements(buf, cbuf, 0);
	free(lplab);
	free(lpTemp);
	//fclose(stream);
	return result;
}

JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_SlopeAdjust(JNIEnv* env, jobject obj,jintArray buf,int w,int h)
{
	jint *cbuf;
	int i;
	int j;
	int color1=255;
	cbuf = env->GetIntArrayElements(buf, false);
	jint *cbufnew=(jint*) malloc(sizeof(jint)*w*h);
	if(cbuf==NULL)
	{
		return 0;
	}
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// 获得像素的颜色
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
//			if(color<180)
//			{
//				color=0;
//			}
//			else
//			{
//				color=255;
//			}
			cbuf[w * i + j] = color;
			color1=alpha | (color1 << 16) | (color1 << 8) | color1;
			cbufnew[w*i+j]=color1;
		}
	}
	int lHeight=h;
	int lWidth=w;
	//图像左半边的平均高度
	double leftaver=0.0;
   //图像右半边的平均高度
	double rightaver=0.0;
	//图像的倾斜度
	double slope;
	int counts;
	//扫描左半边的图像，求黑色象素的平均高度

	//行
	for (i=0;i<lHeight;i++)
	{
      //列
		for (j=0;j<lWidth/2;j++)
		{
		 //如果为黑点
		 if (cbuf[lWidth*i+j]==0)
		 {
          //对其高度进行统计叠加
		  counts +=lWidth/2 -j;
		  leftaver += i*(lWidth/2 -j);
		 }
		}
	}
    //计算平均高度
	leftaver /= counts;
	//将统计循环变量重新赋值
	counts =0;
	//扫描右半边的图像，求黑色象素的平均高度
	//行
	for (i =0;i<lHeight;i++)
	{
       //列
		for (j=lWidth/2;j<lWidth;j++)
		{
			//指向第i行第j个象素的指针
            //如果为黑点
			if (cbuf[lWidth*i+j]== 0)
				{
				  //进行统计叠加
					counts +=lWidth -j;
					rightaver += i*(lWidth -j);
				}
		}
	}
	//计算右半边的平均高度
	rightaver /= counts;
	//计算斜率
	slope = (leftaver - rightaver) / (lWidth/2);
	//象素点的灰度值
	int gray;
	//位置映射值
	int i_src;
	//根据斜率，把当前新图像的点映射到源图像的点
	//行
	for (i=0;i<lHeight;i++)
	{
		//列
   		for (j=0;j<lWidth;j++)
		{
		   //计算映射位置
			i_src=int(i - (j-lWidth/2)*slope);

			//如果点在图像外，象素置白色
			if (i_src <0 || i_src >=lHeight )
				gray = 255;
			else
			{
				//否则到源图像中找点，取得象素值
				//指向第i_src行第j个象素的指针
				gray=cbuf[lWidth*i_src+j];
			}
			//把新图像的点用得到的象素值填充
            //指向第i行第j个象素的指针
			gray=alpha|(gray<<16)|(gray<<8)|gray;
			cbufnew[lWidth*i+j]=gray;
		}
	}
	int size=w * h;
	jintArray result = env->NewIntArray(size);
	env->SetIntArrayRegion(result, 0, size, cbufnew);
	env->ReleaseIntArrayElements(buf, cbuf, 0);
	free(cbufnew);
	return result;
}
void StdDIBbyRect(int* cbuf, int *cbufnew,int lWidth,int lHeight,int tarWidth, int tarHeight)
{
	// 循环变量
	int	i;
	int	j;
	int alpha=0xFF<<24;
	//宽度、高度方向上的缩放因子
	double wscale,hscale;
	//进行映射操作的坐标变量
	int i_src,j_src;
	//int *cbuftemp=new int[lWidth*lHeight];

	//存放字符位置信息的结构体
	CRect rect;
	CRect rectnew;

	//先清空一个新的矩形区域链表以便存储标准化后的矩形区域链表
	m_charRectCopy.clear ();

	//从头到尾逐个扫描各个结点
	while(!m_charRect.empty())
	{
		//从表头上得到一个矩形
		rect= m_charRect.front();

		//从链表头上面删掉一个
		m_charRect.pop_front();

		//计算缩放因子

		//横坐标方向的缩放因子
		wscale=(double)tarWidth/rect.Width ();

		//纵坐标方向的缩放因子
		hscale=(double)tarHeight/rect.Height ();

		//计算标准化矩形

		//上边界
		rectnew.top =rect.top ;

		//下边界
		rectnew.bottom =rect.top +tarHeight;

		//左边界
		rectnew.left =rect.left ;

		//右边界
		rectnew.right =rectnew.left +tarWidth;

		//将原矩形框内的象素映射到新的矩形框内
		for(i=rectnew.top ;i<rectnew.bottom ;i++)
		{
			for(j=rectnew.left ;j<rectnew.right ;j++)
			{

				//计算映射坐标
				i_src=rectnew.top +int((i-rectnew.top )/hscale);
				j_src=rectnew.left +int((j-rectnew.left )/wscale);

				//将相对应的象素点进行映射操作
				int colortemp=cbuf[lWidth*i_src+j_src];
				colortemp=alpha|(colortemp<<16)|(colortemp<<8)|colortemp;
				cbufnew[lWidth*i+j]=colortemp;
				//cbuftemp[lWidth*i+j]=colortemp;
			}
		}
		//将标准化后的矩形区域插入新的链表
		m_charRectCopy.push_back (rectnew);
	}

	//存储标准化后新的rect区域
    m_charRect=m_charRectCopy;
//    CRect recttest=m_charRect.front();
//    for(i=recttest.top;i<recttest.bottom;i++)
//    {
//    	for(j=recttest.left;j<recttest.right;j++)
//    	{
//    		cbufnew[lWidth*i+j]=cbuftemp[lWidth*i+j];
//    	}
//    }

	//将缓存区的内容拷贝到图像的数据区内
	//memcpy(lpDIBBits,lpNewDIBBits,lLineBytes*lHeight);
}
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_CharSegment(JNIEnv* env, jobject obj,jintArray buf,int w,int h)
{
	jint *cbuf;
	int height=h;
	int width=w;
	int top,bottom;
	int gray;
	int i,j;
	CRectLink charRect1,charRect2;
	charRect1.clear();
	charRect2.clear();
	//用来统计图像中字符个数的计数器
	digicount=0;
	cbuf = env->GetIntArrayElements(buf, false);
	jint *cbufnew=(jint*) malloc(sizeof(jint)*w*h);
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	int color1=255;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// 获得像素的颜色
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
			cbuf[w * i + j] = color;
			color1=alpha | (color1 << 16) | (color1 << 8) | color1;
			cbufnew[w*i+j]=color1;
		}
	}
	for (i=0;i<height;i++)
	{
		 //列
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//看是否为黑点
			if (gray == 0)
			{
			   //若为黑点，把此点作为字符大致的最高点
				top = i;
				//对i强行赋值以中断循环
				i=height;
				//跳出循环
				break;
			}
		//如果该点不是黑点，继续循环
		}
	}
	//从下往上扫描，找下边界

		//行
	for (i = height-1;i>=0;i--)
	{
		//列
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//判断是否为黑点
			if (gray == 0)
			{
				//若为黑点，把此点作为字符大致的最低点
				bottom = i;
				//对i强行赋值以中断循环
				i=-1;
				//跳出循环
				break;
			}
		  //如果该点不是黑点，继续循环
		}
	}
	//lab 用作是否进入一个字符分割的标志
	bool lab = false;
	//表明扫描一列中是否发现黑色点
	bool black = false;
	//计数器置零
	digicount=0;
	CRect rect;
	//行
	for (i=0;i<width;i++)
	{
		//开始扫描一列
		black=false;

		for (j=0;j<height;j++)
			{
				gray=cbuf[width*j+i];
				//判断是否为黑点
				if (gray == 0)
				{
					//如果发现黑点，设置标志位
					black=true;

					//如果还没有进入一个字符的分割
					if(lab==false)
					{
						//设置左侧边界
						rect.left = i;
						//字符分割开始
						lab = true;
					}
					//如果字符分割已经开始了
					else
					  //跳出循环
						break;
				}
			}

		//如果已经扫到了最右边那列，说明整副图像扫描完毕。退出
		   if(i==(width-1))

		 //退出整个循环
		   break;

		//如果到此black仍为false，说明扫描了一列，都没有发现黑点。表明当前字符分割结束
		if(lab==true&&black==false)
		{
		   //将位置信息存入结构体中
		   //设置右边界
			rect.right =i-1;  //chenxupro
			//设置上边界
			rect.top =top;
			//设置下边界
			rect.bottom =bottom;
			//将框外括一个象素，以免压到字符
			//rect.InflateRect (1,1);
			//将这个结构体插入存放位置信息的链表1的后面
			charRect1.push_back (rect);
			//设置标志位，开始下一次的字符分割
			lab=false;

			//字符个数统计计数器加1
			digicount++;

		}

		//进入下一列的扫描
	}
	 //再将矩形轮廓矩形的top和bottom精确化

	//将链表1赋值给链表2
	charRect2=charRect1;

	//将链表2的内容清空
	charRect2.clear ();

	//建立一个新的存放位置信息的结构体
	CRect rectnew;
	//对于链表1从头至尾逐个进行扫描
	while(!charRect1.empty())
	{
		//从链表1头上得到一个矩形
		rect= charRect1.front();

		//从链表1头上面删掉一个
		charRect1.pop_front();

		//计算更加精确的矩形区域

		//获得精确的左边界
		rectnew.left =rect.left-1 ;

		//获得精确的右边界
		rectnew.right =rect.right+1 ;

		//通过获得的精确左右边界对上下边境重新进行精确定位

		// 由下而上扫描计算上边界

		//行
		for(i=rect.top ;i<rect.bottom ;i++)
		{
		  //列
			for(j=rect.left ;j<rect.right ;j++)
			{
				 // 指向图像第i行，第j个象素的指针
				//如果这个象素是黑点
				if (cbuf[width*i+j] == 0)
				{
					//设置上边界
					rectnew.top = i-1;
					//对i进行强制定义以跳出循环
					i=rect.bottom  ;
					//跳出循环
					break;
				}
			}
		}
		//由下而上扫描计算下边界
		//行
		for(i=rect.bottom-1 ;i>=rect.top  ;i--)
		{
			//列
			for(j=rect.left ;j<rect.right ;j++)
			{
				// 指向图像第i行，第j个象素的指针
				//该点如果为黑点
				if (cbuf[width*i+j] == 0)
				{
					//设置下边界
					rectnew.bottom = i+1;

					//对i进行强制定义以跳出循环
					i=-1;
					//跳出循环
					break;
				}
			}
		}
		//将得到的新的准确的位置信息从后面插到链表2的尾上
		charRect2.push_back (rectnew);
	}
	//将链表2 传递给链表1
	//charRect1=charRect2;
    m_charRect=charRect2;
    StdDIBbyRect(cbuf,cbufnew,width,height,20,20);
    int size=w * h;
	jintArray result = env->NewIntArray(size);
	env->SetIntArrayRegion(result, 0, size, cbufnew);
	env->ReleaseIntArrayElements(buf, cbuf, 0);
	return result;
}

int *GradientSharpForRecog(int* cbuf, int w, int h)
{
	int bThre=2;
	int bTemp;
	for(int i = 0; i < h; i++)
		{
	     // 每列
		 for(int j = 0; j < w; j++)
			{
			     bTemp=abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-2-i)+j])+abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-1-i)+j+1]);

			    // 判断是否小于阈值
			    if (bTemp < 255)
				{
			     // 判断是否大于阈值，对于小于情况，灰度值不变。
				   if (bTemp >= bThre)
				   {
					   cbuf[w*(h-1-i)+j]=bTemp;
				   }
				}
	           else
				{
					// 直接赋值为255
	        	   int color1=255;
	        	   cbuf[w*(h-1-i)+j]=color1;
				}
		 }
	   }

	//最后还要处理一下图像中最下面那行
	for(int j = 0; j < h; j++)
	{
	 //指向最下边一行，第j个象素的指针
		int color1=255;
		cbuf[w*0+j]=color1;
	}
	return cbuf;
}
bool DeleteScaterJudgeForRecog(int *cbuf,bool *lplab, int lWidth, int lHeight, int x, int y, CPoint lab[], int lianXuShu)
{

	//如果连续长度满足要求，说明不是离散点，返回
	if(m_lianXuShu>=lianXuShu)
		return true;
	//长度加一
	m_lianXuShu++;
	//设定访问标志
	lplab[lWidth * y +x] = true;

	//保存访问点坐标
	lab[m_lianXuShu-1].x = x;
	lab[m_lianXuShu-1].y = y;

	//象素的灰度值
	int gray;

	//长度判定
    //如果连续长度满足要求，说明不是离散点，返回
	if(m_lianXuShu>=lianXuShu)
		return true;

	//下面进入递归
	else
	{
		//考察上下左右以及左上、右上、左下、右下八个方向
		//如果是黑色点，则调用函数自身进行递归

		//考察下面点
		gray=cbuf[lWidth*(y-1)+x];
		//fprintf(stream,"%d %d %d\n",gray,y,x);
		//如果点在图像内、颜色为黑色并且没有被访问过
		if(y-1 >=0 && gray == 0 && lplab[(y-1)*lWidth+x] == false)
		{
			//fprintf(stream,"%s\n","1 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y-1,lab,lianXuShu);
		}
		//判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//左下点
		gray=cbuf[lWidth*(y-1)+x-1];

        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y-1 >=0 &&  x-1 >=0 && gray== 0 && lplab[(y-1)*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","2 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y-1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}


		//左边

		gray=cbuf[lWidth*y+x-1];

        //如果点在图像内、颜色为黑色并且没有被访问过
		if(x-1 >=0 &&  gray== 0 && lplab[y*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","3 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//左上

		gray=cbuf[lWidth*(y+1)+x-1];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y+1 <lHeight && x-1 >= 0 && gray == 0 && lplab[(y+1)*lWidth+x-1] == false)
		{
		//进行递归处理
		//fprintf(stream,"%s\n","4 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y+1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//上面
		gray=cbuf[lWidth*(y+1)+x];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y+1 < lHeight && gray == 0 && lplab[(y+1)*lWidth+x] == false)
		{
        //进行递归处理
		//fprintf(stream,"%s\n","5 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y+1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//右上

		gray=cbuf[lWidth*(y+1)+x+1];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y+1 <lHeight && x+1 <lWidth &&  gray == 0 && lplab[(y+1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","6 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y+1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//右边

		gray=cbuf[lWidth*y+x+1];
		//如果点在图像内、颜色为黑色并且没有被访问过
		if(x+1 <lWidth && gray==0 && lplab[y*lWidth+x+1] == false)
		{
		//fprintf(stream,"%s\n","7 ");
        //进行递归处理
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y,lab,lianXuShu);
		}
        //判断长度

		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//右下
		gray=cbuf[lWidth*(y-1)+x+1];
        //如果点在图像内、颜色为黑色并且没有被访问过
		if(y-1 >=0 && x+1 <lWidth && gray == 0 && lplab[(y-1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","8 ");
			//进行递归处理
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y-1,lab,lianXuShu);
		}
        //判断长度
		//如果连续长度满足要求，说明不是离散点，返回
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
	}

	//fprintf(stream,"%s\n","error");
	//如果递归结束，返回false，说明是离散点
	return false;

}
int *RemoveScatterNoiseForRecog(int *cbuf, int w, int h)
{
	//char buffer[] = {'x' , 'y' , 'z'};
	//stream=fopen("/sdcard/log.txt","wb");
	//fprintf(stream,"%s\n","hello world");
	//fwrite (buffer , sizeof(buffer), 1 , stream );
	int lHeight=h;
	int lWidth=w;
	int i;
	int j;
	int k;
	bool *lplab=(bool *)malloc(sizeof(bool)*w*h);
	bool *lpTemp=(bool *)malloc(sizeof(bool)*w*h);
	int length=15;
	// 循环变量
	m_lianXuShu=0;
	CPoint lab[21];
	for (i=0;i<lHeight*lWidth;i++)
    {
    //将所有的标志位设置为非
		lplab[i] = false;
	}
	for(i =0;i<lHeight;i++)
	{

	   //逐行扫描
		for(j=0;j<lWidth;j++)
			{
				//先把标志位置false
				for(k=0;k<m_lianXuShu;k++)
				lplab[lab[k].y * lWidth + lab[k].x] = false;

				//连续数置0
				m_lianXuShu =0;

				//进行离散性判断
				lpTemp[i*lWidth+j] = DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,j,i,lab,length);
			}
	}
	for(i = 0;i<lHeight;i++)
	{

	  //逐列扫描
		for(j=0;j<lWidth;j++)
		{
				//查看标志位,如果为非则将此点设为白点
				if(lpTemp[i*lWidth+j] == false)
				{
					int color1=255;
					cbuf[lWidth*i+j]=color1;
				}
				else
				{
					int color1=0;
					cbuf[lWidth*i+j]=color1;
				}
			}
	}
	free(lplab);
	free(lpTemp);
	return cbuf;
}
int *SlopeAdjustForRecog(int* cbuf,int w,int h)
{
	int i;
	int j;
	int *cbufnew=(int*) malloc(sizeof(int)*w*h);
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			cbufnew[w*i+j]=255;
		}
	}
	int lHeight=h;
	int lWidth=w;
	//图像左半边的平均高度
	double leftaver=0.0;
   //图像右半边的平均高度
	double rightaver=0.0;
	//图像的倾斜度
	double slope;
	int counts;
	//扫描左半边的图像，求黑色象素的平均高度

	//行
	for (i=0;i<lHeight;i++)
	{
      //列
		for (j=0;j<lWidth/2;j++)
		{
		 //如果为黑点
		 if (cbuf[lWidth*i+j]==0)
		 {
          //对其高度进行统计叠加
		  counts +=lWidth/2 -j;
		  leftaver += i*(lWidth/2 -j);
		 }
		}
	}
    //计算平均高度
	leftaver /= counts;
	//将统计循环变量重新赋值
	counts =0;
	//扫描右半边的图像，求黑色象素的平均高度
	//行
	for (i =0;i<lHeight;i++)
	{
       //列
		for (j=lWidth/2;j<lWidth;j++)
		{
			//指向第i行第j个象素的指针
            //如果为黑点
			if (cbuf[lWidth*i+j]== 0)
				{
				  //进行统计叠加
					counts +=lWidth -j;
					rightaver += i*(lWidth -j);
				}
		}
	}
	//计算右半边的平均高度
	rightaver /= counts;
	//计算斜率
	slope = (leftaver - rightaver) / (lWidth/2);
	//象素点的灰度值
	int gray;
	//位置映射值
	int i_src;
	//根据斜率，把当前新图像的点映射到源图像的点
	//行
	for (i=0;i<lHeight;i++)
	{
		//列
   		for (j=0;j<lWidth;j++)
		{
		   //计算映射位置
			i_src=int(i - (j-lWidth/2)*slope);

			//如果点在图像外，象素置白色
			if (i_src <0 || i_src >=lHeight )
				gray = 255;
			else
			{
				//否则到源图像中找点，取得象素值
				//指向第i_src行第j个象素的指针
				gray=cbuf[lWidth*i_src+j];
			}
			//把新图像的点用得到的象素值填充
            //指向第i行第j个象素的指针
			cbufnew[lWidth*i+j]=gray;
		}
	}
	return cbufnew;
}
void StdDIBbyRectForRecog(int* cbuf, int *cbufnew,int lWidth,int lHeight,int tarWidth, int tarHeight)
{
	// 循环变量
	int	i;
	int	j;
	//宽度、高度方向上的缩放因子
	double wscale,hscale;
	//进行映射操作的坐标变量
	int i_src,j_src;
	//int *cbuftemp=new int[lWidth*lHeight];

	//存放字符位置信息的结构体
	CRect rect;
	CRect rectnew;

	//先清空一个新的矩形区域链表以便存储标准化后的矩形区域链表
	m_charRectCopy.clear ();

	//从头到尾逐个扫描各个结点
	while(!m_charRect.empty())
	{
		//从表头上得到一个矩形
		rect= m_charRect.front();

		//从链表头上面删掉一个
		m_charRect.pop_front();

		//计算缩放因子

		//横坐标方向的缩放因子
		wscale=(double)tarWidth/rect.Width ();

		//纵坐标方向的缩放因子
		hscale=(double)tarHeight/rect.Height ();

		//计算标准化矩形

		//上边界
		rectnew.top =rect.top ;

		//下边界
		rectnew.bottom =rect.top +tarHeight;

		//左边界
		rectnew.left =rect.left ;

		//右边界
		rectnew.right =rectnew.left +tarWidth;

		//将原矩形框内的象素映射到新的矩形框内
		for(i=rectnew.top ;i<rectnew.bottom ;i++)
		{
			for(j=rectnew.left ;j<rectnew.right ;j++)
			{

				//计算映射坐标
				i_src=rectnew.top +int((i-rectnew.top )/hscale);
				j_src=rectnew.left +int((j-rectnew.left )/wscale);

				//将相对应的象素点进行映射操作
				int colortemp=cbuf[lWidth*i_src+j_src];
				cbufnew[lWidth*i+j]=colortemp;
				//cbuftemp[lWidth*i+j]=colortemp;
			}
		}
		//将标准化后的矩形区域插入新的链表
		m_charRectCopy.push_back (rectnew);
	}

	//存储标准化后新的rect区域
    m_charRect=m_charRectCopy;
//    CRect recttest=m_charRect.front();
//    for(i=recttest.top;i<recttest.bottom;i++)
//    {
//    	for(j=recttest.left;j<recttest.right;j++)
//    	{
//    		cbufnew[lWidth*i+j]=cbuftemp[lWidth*i+j];
//    	}
//    }

	//将缓存区的内容拷贝到图像的数据区内
	//memcpy(lpDIBBits,lpNewDIBBits,lLineBytes*lHeight);
}
int* CharSegmentForRecog(int* cbuf,int w,int h)
{
	int height=h;
	int width=w;
	int top,bottom;
	int gray;
	int i,j;
	CRectLink charRect1,charRect2;
	charRect1.clear();
	charRect2.clear();
	//用来统计图像中字符个数的计数器
	digicount=0;
	int *cbufnew=(int*) malloc(sizeof(int)*w*h);
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			cbufnew[w*i+j]=255;
		}
	}
	for (i=0;i<height;i++)
	{
		 //列
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//看是否为黑点
			if (gray == 0)
			{
			   //若为黑点，把此点作为字符大致的最高点
				top = i;
				//对i强行赋值以中断循环
				i=height;
				//跳出循环
				break;
			}
		//如果该点不是黑点，继续循环
		}
	}
	//从下往上扫描，找下边界

		//行
	for (i = height-1;i>=0;i--)
	{
		//列
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//判断是否为黑点
			if (gray == 0)
			{
				//若为黑点，把此点作为字符大致的最低点
				bottom = i;
				//对i强行赋值以中断循环
				i=-1;
				//跳出循环
				break;
			}
		  //如果该点不是黑点，继续循环
		}
	}
	//lab 用作是否进入一个字符分割的标志
	bool lab = false;
	//表明扫描一列中是否发现黑色点
	bool black = false;
	//计数器置零
	digicount=0;
	CRect rect;
	//行
	for (i=0;i<width;i++)
	{
		//开始扫描一列
		black=false;

		for (j=0;j<height;j++)
			{
				gray=cbuf[width*j+i];
				//判断是否为黑点
				if (gray == 0)
				{
					//如果发现黑点，设置标志位
					black=true;

					//如果还没有进入一个字符的分割
					if(lab==false)
					{
						//设置左侧边界
						rect.left = i;
						//字符分割开始
						lab = true;
					}
					//如果字符分割已经开始了
					else
					  //跳出循环
						break;
				}
			}

		//如果已经扫到了最右边那列，说明整副图像扫描完毕。退出
		   if(i==(width-1))

		 //退出整个循环
		   break;

		//如果到此black仍为false，说明扫描了一列，都没有发现黑点。表明当前字符分割结束
		if(lab==true&&black==false)
		{
		   //将位置信息存入结构体中
		   //设置右边界
			rect.right =i-1;  //chenxupro
			//设置上边界
			rect.top =top;
			//设置下边界
			rect.bottom =bottom;
			//将框外括一个象素，以免压到字符
			//rect.InflateRect (1,1);
			//将这个结构体插入存放位置信息的链表1的后面
			charRect1.push_back (rect);
			//设置标志位，开始下一次的字符分割
			lab=false;

			//字符个数统计计数器加1
			digicount++;

		}

		//进入下一列的扫描
	}
	 //再将矩形轮廓矩形的top和bottom精确化

	//将链表1赋值给链表2
	charRect2=charRect1;

	//将链表2的内容清空
	charRect2.clear ();

	//建立一个新的存放位置信息的结构体
	CRect rectnew;
	//对于链表1从头至尾逐个进行扫描
	while(!charRect1.empty())
	{
		//从链表1头上得到一个矩形
		rect= charRect1.front();

		//从链表1头上面删掉一个
		charRect1.pop_front();

		//计算更加精确的矩形区域

		//获得精确的左边界
		rectnew.left =rect.left-1 ;

		//获得精确的右边界
		rectnew.right =rect.right+1 ;

		//通过获得的精确左右边界对上下边境重新进行精确定位

		// 由下而上扫描计算上边界

		//行
		for(i=rect.top ;i<rect.bottom ;i++)
		{
		  //列
			for(j=rect.left ;j<rect.right ;j++)
			{
				 // 指向图像第i行，第j个象素的指针
				//如果这个象素是黑点
				if (cbuf[width*i+j] == 0)
				{
					//设置上边界
					rectnew.top = i-1;
					//对i进行强制定义以跳出循环
					i=rect.bottom  ;
					//跳出循环
					break;
				}
			}
		}
		//由下而上扫描计算下边界
		//行
		for(i=rect.bottom-1 ;i>=rect.top  ;i--)
		{
			//列
			for(j=rect.left ;j<rect.right ;j++)
			{
				// 指向图像第i行，第j个象素的指针
				//该点如果为黑点
				if (cbuf[width*i+j] == 0)
				{
					//设置下边界
					rectnew.bottom = i+1;

					//对i进行强制定义以跳出循环
					i=-1;
					//跳出循环
					break;
				}
			}
		}
		//将得到的新的准确的位置信息从后面插到链表2的尾上
		charRect2.push_back (rectnew);
	}
	//将链表2 传递给链表1
	//charRect1=charRect2;
    m_charRect=charRect2;
    StdDIBbyRect(cbuf,cbufnew,width,height,20,20);
	return cbufnew;
}
int* ImgToBinaryOTSUForRecog(int *cbuf,int w,int h)
{
	int pixelValue[256] ={0};
	long double p[256] ={0};
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			// 获得像素的颜色
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
			//color = alpha | (color << 16) | (color << 8) | color;  //或运算，将rgb转化之后的值赋给r,g,b,使rgb相同，变成单色
			cbuf[w * i + j] = color;
			pixelValue[cbuf[w*i+j]]++;
		}
	}
	for(int i=0;i<256;i++)
	{
		p[i]=(long double)pixelValue[i]/(long double )(w*h);
	}
	int t=0;
	int Threshold=0;
	long double w_a=0;
	long double w_b=0;
	long double u_a=0;
	long double u_b=0;
	long double maxB=0;
	for(t=0;t<256;t++)
	{
		for(int i=0;i<t;i++)
		{
			w_a=w_a+p[i];
		}
		for (int i=t+1;i<256;i++)
		{
			w_b = w_b +p[i];
		}
	//
		for (int i=0;i<=t;i++)
		{
			u_a += i*p[i]/w_a;
		}

		for (int i=t+1;i<256;i++)
		{
			u_b += i *p[i]/w_b;
		}

		if (w_a*w_b*(u_a-u_b)*(u_a-u_b)>maxB)
		{
			maxB = w_a*w_b*(u_a-u_b)*(u_a-u_b);
			Threshold = t;
		}
		w_a = w_b = u_a = u_b = 0;
	}
	int colortemp;
	for(int i = 0; i < h; i++)
	{
	   // 每列
	   for(int j = 0; j < w; j++)
	   {
		// 指向DIB第i行，第j个象素的指针
		//lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (cyDIB - 1 - i) + j;
		 colortemp=cbuf[w*i+j];
		if ( colortemp >Threshold)
		{
			colortemp=255;
//			colortemp = alpha | (colortemp << 16) | (colortemp << 8) | colortemp;
			cbuf[w*i+j]=colortemp;
		}
		else
		{
			colortemp=0;
//			colortemp = alpha | (colortemp << 16) | (colortemp << 8) | colortemp;
			cbuf[w*i+j]=colortemp;
		}
	   }
	}
	return cbuf;
}

#include <stdio.h>
#include <math.h> 
#include <time.h>
#include <stdlib.h>

#define TRAININGSNUM 60000    //训练样本数  //max->60000  1000->500
#define HNNUM        35       //52  ->12
#define ETANUM       0.04     //0.015   0.04
#define FEADIM       64       //提取的特征维数  K-L变换 56->100  200?加入13特征点 增加一些特征点 200->12
#define EXNUM        0.02     //0.001

#define RAWDATADIM   400      //每个数字的像素点数 16*16 28*28 20*20->归一化后
#define MAXDIEDAINUM 5000
#define TESTINGSNUM	 1000     //测试样本数  //max->10000
#define SUCCESSFUL   0
#define FAIL         -1
#define BIGRND 32767.0        //32767->32767.0

double out[][4]=
{      
	0.01,0.01,0.01,0.01,
	0.01,0.01,0.01,0.99,
	0.01,0.01,0.99,0.01,
	0.01,0.01,0.99,0.99,
	0.01,0.99,0.01,0.01,
	0.01,0.99,0.01,0.99,
	0.01,0.99,0.99,0.01,
	0.01,0.99,0.99,0.99,
	0.99,0.01,0.01,0.01,
	0.99,0.01,0.01,0.99
};

/*** 返回0－1的双精度随机数 ***/
double drnd()
{
	return ((double) rand() / (double) BIGRND);
}

/*** 返回-1.0到1.0之间的双精度随机数 ***/
double dpn1()
{
	return ((drnd() * 2.0) - 1.0);
}

double squash(double x)
{
	return (1.0 / (1.0 + exp(-x)));
}

/*** 申请1维双精度实数数组 ***/
double *alloc_1d_dbl(int n)
{
	double *new1;
	new1 = (double *) malloc ((unsigned) (n * sizeof (double)));
	if (new1 == NULL) {
		printf("ALLOC_1D_DBL: Couldn't allocate array of doubles\n");
		return (NULL);
	}
	return (new1);
}

/*** 申请2维双精度实数数组 ***/
double **alloc_2d_dbl(int m, int n)
{
	int i;
	double **new1;
	new1 = (double **) malloc ((unsigned) (m * sizeof (double *)));
	if (new1 == NULL) {
		// printf("ALLOC_2D_DBL: Couldn't allocate array of dbl ptrs\n");
		return (NULL);
	}
	for (i = 0; i < m; i++) {
		new1[i] = alloc_1d_dbl(n);
	}
	return (new1);
}

unsigned char *alloc_1d_uch(int n)
{
	unsigned char *new1;
	new1 = (unsigned char *) malloc ((unsigned) (n * sizeof (unsigned char)));
	if (new1 == NULL) {
		printf("ALLOC_1D_UCH: Couldn't allocate array of unsigned chars\n");
		return (NULL);
	}
	return (new1);
}

unsigned char **alloc_2d_uch(int m,int n)
{
	int i;
	unsigned char **new1;
	new1 = (unsigned char **) malloc ((unsigned) (m * sizeof (unsigned char *)));
	if (new1 == NULL) 
	{
		return (NULL);
	}
	for (i = 0; i < m; i++) {
		new1[i] = alloc_1d_uch(n);
	}
	return (new1);
}

/*** 设置随机数种子 ***/
void bpnn_initialize(int seed)
{
	// printf("Random number generator seed: %d\n", seed);
	srand(seed);
}

/*** 随机初始化权值 ***/
void bpnn_randomize_weights(double **w, int m, int n)
{
	int i, j;
	for (i = 0; i <= m; i++) {
		for (j = 0; j <= n; j++) {
			w[i][j] = dpn1();
		}
	}
}

/*** 0初始化权值 ***/
void bpnn_zero_weights(double **w, int m, int n)
{
	int i, j;
	for (i = 0; i <= m; i++) {
		for (j = 0; j <= n; j++) {
			w[i][j] = 0.0;
		}
	}
}

/*********前向传输*********/
void bpnn_layerforward(double *l1, double *l2, double **conn, int n1, int n2)
{
	double sum;
	int j, k;
	
	/*** 设置阈值 ***/
	l1[0] = 1.0;
	
	/*** 对于第二层的每个神经元 ***/
	for (j = 1; j <= n2; j++) {
		
		/*** 计算输入的加权总和 ***/
		sum = 0.0;
		for (k = 0; k <= n1; k++) {
			sum += conn[k][j] * l1[k];
		}
		l2[j] = squash(sum);
	}
}

/*********前向传输*********/
void bpnn_layerforward_1(double *l1, double *l2, double **conn, int n1, int n2)
{
	double sum;
	int j, k;
	
	/*** 设置阈值 ***/
	l1[0] = 1.0;
	
	/*** 对于第二层的每个神经元 ***/
	for (j = 1; j <= n2; j++) {
		
		/*** 计算输入的加权总和 ***/
		sum = 0.0;
		for (k = 0; k <= n1; k++) {
			sum += conn[k][j] * l1[k];
		}
		//l2[j] = squash(sum);
		l2[j]=sum;
	}
}

/* 输出误差 */
void bpnn_output_error(double *delta, double *target, double *output, int nj)
{
	int j;
	double o, t, errsum;
	
	errsum = 0.0;
	for (j = 1; j <= nj; j++) {
		o = output[j];
		t = target[j];
		delta[j] = o * (1.0 - o) * (t - o);
	}
}

/* 隐含层误差 */
void bpnn_hidden_error(double* delta_h, int nh, double *delta_o, int no, double **who, double *hidden)
{
	int j, k;
	double h, sum, errsum;
	
	errsum = 0.0;
	for (j = 1; j <= nh; j++) {
		h = hidden[j];
		sum = 0.0;
		for (k = 1; k <= no; k++) {
			sum += delta_o[k] * who[j][k];
		}
		delta_h[j] = h * (1.0 - h) * sum;
	}
}

/* 调整权值 */
void bpnn_adjust_weights(double *delta, int ndelta, double *ly, int nly, double** w, double **oldw, double eta, double momentum)
{
	double new_dw;
	int k, j;
	
	ly[0] = 1.0;
	for (j = 1; j <= ndelta; j++) {
		for (k = 0; k <= nly; k++) {
			new_dw = ((eta * delta[j] * ly[k]) + (momentum * oldw[k][j]));
			w[k][j] += new_dw;
			oldw[k][j] = new_dw;
		}
	}
}

/*******保存权值**********/
void w_weight(double **w,int n1,int n2,char*name)
{
	int i,j;
	double *buffer;
	FILE *fp;
	fp=fopen(name,"wb+");
	buffer=(double*)malloc((n1+1)*(n2+1)*sizeof(double));
	for(i=0;i<=n1;i++)
	{
		for(j=0;j<=n2;j++)
			buffer[i*(n2+1)+j]=w[i][j];
	}
	fwrite((char*)buffer,sizeof(double),(n1+1)*(n2+1),fp);
	fclose(fp);
	free(buffer);
}

/************读取权值*************/
bool r_weight(double **w,int n1,int n2,char *name)
{
	int i,j;
	double *buffer;
	FILE *fp;
	if((fp=fopen(name,"rb"))==NULL)
	{
		return false;
	}
	buffer=(double*)malloc((n1+1)*(n2+1)*sizeof(double));
	fread((char*)buffer,sizeof(double),(n1+1)*(n2+1),fp);
	
	for(i=0;i<=n1;i++)
	{
		for(j=0;j<=n2;j++)
			w[i][j]=buffer[i*(n2+1)+j];
	}
	fclose(fp);
	free(buffer);
	return true;
}

/*****保存各层结点的数目******/
void w_num(int n1,int n2,int n3,char*name)
{
	FILE *fp;
	fp=fopen(name,"wb+");
	int *buffer;
	buffer=(int*)malloc(3*sizeof(int));
	buffer[0]=n1;
	buffer[1]=n2;
	buffer[2]=n3;
	fwrite((char*)buffer,sizeof(int),3,fp);
	fclose(fp);
	free(buffer);
}

/********读取各层结点数目*********/
bool r_num(int *n,char *name)
{
	int *buffer;
	FILE *fp;
	buffer=(int *)malloc(3*sizeof(int));
	if((fp=fopen(name,"rb"))==NULL)
	{
		return false;
	}
	fread((char*)buffer,sizeof(int),3,fp);
	n[0]=buffer[0];
	n[1]=buffer[1];
	n[2]=buffer[2];
	fclose(fp);
	free(buffer);
	return true;
}

//函数功能:对于输入样本提取特征向量，把归一化样本的每一个像素都作为特征提取出来
double *code_OnlyOne (unsigned char *BinGuiYiImageDataArray)
{   
	//循环变量
    int i,j;
	int CurrentColorValue=0;
	double *data=NULL;
	//为这个数组申请二维存储空间
	data = (double *)malloc(sizeof(double)*20*20);
	
	//将归一化的样本的每个象素作为一个特征点提取出来
	//对每个数据逐行扫描
	for(i=0;i<20;i++)
	{  
		//对每个数据逐列扫描
		for(j=0;j<20;j++)
		{	
			//指向图像第i行第j列个象素的指针
			CurrentColorValue=BinGuiYiImageDataArray[i*20+j];
			if(CurrentColorValue==0)
			{
				//将特征向量的相应位置填1
				data[i*20+j]=1;
			}
			//如果这个象素是其他的   
			if(CurrentColorValue!=0)
			{
				//将特征向量的相应位置填0
				data[i*20+j]=0;
			}
		}
	}
	return data;
}

/****************************************************
函数功能:根据输入的特征向量和期望的理想输出对BP网络进行训练,训练结束后将权值保存并将训练的结果显示出来
********************************************************/
//MaxDieDaiNum->指定最大迭代次数!!!
//num->样本数目
void BpTrain(double ** data_in, double** data_out,int n_in,int n_hidden,double min_ex,double momentum,double eta ,int num,int CurrentMaxDieDaiNum)
{
	//循环变量   
	int i,k,l;
	
	//输出层结点数目
	int n_out=4;   
	//指向输入层数据的指针
	double* input_unites; 
	//指向隐层数据的指针
	double* hidden_unites;
	//指向输出层数据的指针
	double* output_unites; 
	//指向隐层误差数据的指针
	double* hidden_deltas;
	//指向输出层误差数剧的指针
	double* output_deltas;  
	//指向理想目标输出的指针
	double* target;    
	//指向输入层于隐层之间权值的指针
	double** input_weights;
	//指向隐层与输出层之间的权值的指针
	double** hidden_weights;
	//指向上一次输入层于隐层之间权值的指针
	double** input_prev_weights ;
	//指向上一次隐层与输出层之间的权值的指针
	double** hidden_prev_weights;
	//每次循环后的均方误差误差值 
	double ex;
	
	//为各个数据结构申请内存空间
	input_unites= alloc_1d_dbl(n_in + 1);
	hidden_unites=alloc_1d_dbl(n_hidden + 1);
	output_unites=alloc_1d_dbl(n_out + 1);
	hidden_deltas = alloc_1d_dbl(n_hidden + 1);
	output_deltas = alloc_1d_dbl(n_out + 1);
	target = alloc_1d_dbl(n_out + 1);
	input_weights=alloc_2d_dbl(n_in + 1, n_hidden + 1);
	input_prev_weights = alloc_2d_dbl(n_in + 1, n_hidden + 1);
	hidden_prev_weights = alloc_2d_dbl(n_hidden + 1, n_out + 1);
	hidden_weights = alloc_2d_dbl(n_hidden + 1, n_out + 1);
	
	//为产生随机序列撒种
	time_t t; 
	bpnn_initialize((unsigned)time(&t));
	
	//对各种权值进行初始化初始化
	bpnn_randomize_weights( input_weights,n_in,n_hidden);
	bpnn_randomize_weights( hidden_weights,n_hidden,n_out);
	bpnn_zero_weights(input_prev_weights, n_in,n_hidden );
	bpnn_zero_weights(hidden_prev_weights,n_hidden,n_out );
	
	//开始进行BP网络训练
	//这里设定最大的迭代次数为15000次
	//MaxDieDaiNum
	for(l=0;l<CurrentMaxDieDaiNum;l++)
	{
		//bpnn_initialize((unsigned)time(&t));
		//对均方误差置零
		ex=0;
		//对样本进行逐个的扫描
		for(k=0;k<num;k++)  
		{ 
			//将提取的样本的特征向量输送到输入层上
			for(i=1;i<=n_in;i++)
			{
				input_unites[i] = data_in[k][i-1];
			}
			
			//将预定的理想输出输送到BP网络的理想输出单元
			for(i=1;i<=n_out;i++)
			{
				target[i]=data_out[k][i-1];
			}
			
			//前向传输激活
			//将数据由输入层传到隐层 
			bpnn_layerforward(input_unites,hidden_unites,input_weights, n_in,n_hidden);
			//将隐层的输出传到输出层
			bpnn_layerforward(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
			//bpnn_layerforward_1(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
			
			//误差计算
			//将输出层的输出与理想输出比较计算输出层每个结点上的误差
			bpnn_output_error(output_deltas,target,output_unites,n_out);
			//根据输出层结点上的误差计算隐层每个节点上的误差
			bpnn_hidden_error(hidden_deltas,n_hidden, output_deltas, n_out,hidden_weights, hidden_unites);
			
			//权值调整
			//根据输出层每个节点上的误差来调整隐层与输出层之间的权值    
			bpnn_adjust_weights(output_deltas,n_out, hidden_unites,n_hidden,hidden_weights, hidden_prev_weights, eta, momentum); 
			//根据隐层每个节点上的误差来调整隐层与输入层之间的权值    	
			bpnn_adjust_weights(hidden_deltas, n_hidden, input_unites, n_in,input_weights, input_prev_weights, eta, momentum);  
			
			//误差统计		
			for(i=1;i<=n_out;i++)
			{
				ex+=(output_unites[i]-data_out[k][i-1])*(output_unites[i]-data_out[k][i-1]);
			}
		}
		//计算均方误差
		ex=ex/double(num*n_out);
		printf("第%d次迭代,均方误差为:%lf\n",l,ex);
		//如果均方误差已经足够的小，跳出循环，训练完毕  
		if(ex<min_ex)
		{
			break;
		}
	}
	
	//相关保存
	//保存输入层与隐层之间的权值
	w_weight(input_weights,n_in,n_hidden,"/mnt/sdcard/InToHidden.dat");
	//保存隐层与输出层之间的权值
	w_weight(hidden_weights,n_hidden,n_out,"/mnt/sdcard/HiddenToOut.dat");
	//保存各层结点的个数
	w_num(n_in,n_hidden,n_out,"/mnt/sdcard/ThreeLevelNum.dat");
	
	//显示训练结果
	if(ex<=min_ex)
	{
		printf("迭代%d次,平均误差%.4f\n",l,ex);
	}
	
	if(ex>min_ex)
	{
		printf("迭代%d次,平均误差%.4f\n达不到预定要求\n需要调整参数重新训练",l,ex);
	}
	
	//释放内存空间
	free(input_unites);
	free(hidden_unites);
	free(output_unites);
	free(hidden_deltas);
	free(output_deltas);
	free(target);
	free(input_weights);
	free(hidden_weights);
	free(input_prev_weights);
	free(hidden_prev_weights);
}

//先完全读出测试集
int GetTestSetData(double **testdata_in,int *testdata_trueclass,char *testdata_in_fname,char *testdata_out_cfname)
{   
	//循环变量
    int i,j,k;
	FILE *fdf, *fcf;

	unsigned char CurrentColorValue=0;
	unsigned char sclass;

	//open datafile
	if((fdf = fopen (testdata_in_fname, "rb")) == NULL)
	{
		printf("Can't open file %s\n", testdata_in_fname);
		return FAIL;
	}
	//open classfile
	if((fcf = fopen (testdata_out_cfname, "rb")) == NULL)
	{
		printf("Can't open file %s\n", testdata_out_cfname);
		return FAIL;
	}
	//图像文件已归一化->无需跳过字节
	//分类文件
	fseek(fcf,8,SEEK_SET);

	for (i = 0; i < TESTINGSNUM; i++)   //训练样本或测试样本的个数
	{
		//RAWDATADIM->20*20读取所有属于自己的像素
		for(j=0;j<20;j++)
		{
			for(k=0;k<20;k++)
			{
				fread(&CurrentColorValue, sizeof(unsigned char), 1, fdf);   //retrieve gray data
				testdata_in[i][j*20+k]=CurrentColorValue/(256.0);           //先除以256.0
				//testdata_in[i][j*20+k]=CurrentColorValue+0.0;
				//testdata_in[i][j*20+k]=CurrentColorValue/(255.0);
				/*
				if(CurrentColorValue==255)
				{
					testdata_in[i][j*20+k]=1;
				}
				else
				{
					testdata_in[i][j*20+k]=0;
				}
				*/
			}
		}
	}
	for (i = 0; i < TESTINGSNUM; i++)   //训练样本或测试样本的个数
	{
		fread (&sclass, sizeof(unsigned char), 1, fcf);
		testdata_trueclass[i]=sclass;
	}

	fclose (fdf);
	fclose (fcf);

	return SUCCESSFUL;
}


int NearestInt(double F)
{
	return (int)(F)+(F>=0?(int)(10*F)%10<5?0:1:-(int)(10*F)%10<5?0:-1);
}

//num->需识别的样本的个数
//函数功能:读入输入样本的特征相量并根据训练所得的权值,进行识别，将识别的结果写入BPRecogResult.txt 
int CodeRecognize(double *data_in,int n_in,int n_hidden,int n_out)
{
	//循环变量
	int i;
	//识别结果
	int recognize=-1;
	//指向输入层数据的指针
	double* input_unites; 
	//指向隐层数据的指针
	double* hidden_unites;
	//指向输出层数据的指针
	double* output_unites; 
	//指向输入层于隐层之间权值的指针
	double** input_weights;
	//指向隐层与输出层之间的权值的指针
	double** hidden_weights;
	//为各个数据结构申请内存空间
	input_unites= alloc_1d_dbl(n_in + 1);
	hidden_unites=alloc_1d_dbl(n_hidden + 1);
	output_unites=alloc_1d_dbl(n_out + 1);
	input_weights=alloc_2d_dbl(n_in + 1, n_hidden + 1);
	hidden_weights = alloc_2d_dbl(n_hidden + 1, n_out + 1);
	
	//读取权值
	if( r_weight(input_weights,n_in,n_hidden,"/mnt/sdcard/InToHidden.dat")==false)
	{
		return FAIL;
	}
	if(r_weight(hidden_weights,n_hidden,n_out,"/mnt/sdcard/HiddenToOut.dat")==false)
	{
		return FAIL;
	}
	
	//将提取的样本的特征向量输送到输入层上
	for(i=1;i<=n_in;i++)
	{
		input_unites[i]=data_in[i-1];
	}

	//前向输入激活
	bpnn_layerforward(input_unites,hidden_unites,input_weights, n_in,n_hidden);
	bpnn_layerforward(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
	//bpnn_layerforward_1(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
		
	//根据输出结果进行识别
	int result=0 ;
	//考察每一位的输出
	for(i=1;i<=n_out;i++)
	{
		//如果大于0.5判为1
		if(output_unites[i]>0.5)  //0.5->0.9
		{
			//result+=(int)pow(2,double(4-i));
			result+=NearestInt(pow(2,double(4-i)));
		}
	}
		
	//如果判定的结果小于等于9，认为合理
	if(result<=9)
	{
		recognize=result;
	}
	//如果判定的结果大于9，认为不合理将结果定位为一个特殊值20
	if(result>9)
	{
		recognize=10;   //10->9
	}

	/*
	//将识别结果写到文本中
	FILE *fp;
	fp=fopen("BPRecogResult.txt","w+");
	
	if(recognize==10)
	{
		fprintf(fp,"无法识别!");
		printf("无法识别!");
	}
	else
	{
		fprintf(fp,"\n%d ",recognize);
		printf("\n%d ",recognize);
	}
		
	fclose(fp);
	*/

	//释放存储空间
	free(input_unites);
	free(hidden_unites);
	free(output_unites);
	free(input_weights);
	free(hidden_weights);

	return recognize;
}

#define ALGO_DEBUG

/*
   用豪斯荷尔德（Householder）变换将n阶实对称矩阵约化为对称三对角阵
   徐士良. 常用算法程序集（C语言描述），第3版. 清华大学出版社. 2004

   double a[] --- 维数为nxn。存放n阶实对称矩阵A
   int n      --- 实对称矩阵A的阶数
   double q[] --- 维数nxn。返回Householder变换的乘积矩阵Q。当与
                  Tri_Symmetry_Diagonal_Eigenvector()函数联用，若将
				  Q矩阵作为该函数的一个参数时，可计算实对称阵A的特征
				  值与相应的特征向量
   double b[] --- 维数为n。返回对称三对角阵中的主对角线元素
   double c[] --- 长度n。前n－1个元素返回对称三对角阵中的次对角线元素。
*/
void Householder_Tri_Symetry_Diagonal( double a[], int n, double q[], double b[], double c[] )
{ 
	int i, j, k, u;
	double h, f, g, h2;

	for ( i = 0; i <= n-1; i++ )
		for ( j = 0; j <= n-1; j++ )
		{
			u = i * n + j; 
			q[u] = a[u];
		}
	for ( i = n-1; i >= 1; i-- )
	{ 
		h = 0.0;
		if ( i > 1 )
			for ( k = 0; k <= i-1; k++ )
			{ 
				u = i * n + k;
				h = h + q[u] * q[u];
			}
		if ( h + 1.0 == 1.0 )
		{ 
			c[i] = 0.0;
			if ( i == 1 ) c[i] = q[i*n+i-1];
			b[i] = 0.0;
		}
		else
		{ 
			c[i] = sqrt( h );
			u = i * n + i - 1;
			if ( q[u] > 0.0 ) c[i] = -c[i];
			h = h - q[u] * c[i];
			q[u] = q[u] - c[i];
			f = 0.0;
			for ( j = 0; j <= i - 1; j++ )
			{ 
				q[j*n+i] = q[i*n+j] / h;
				g = 0.0;
				for ( k = 0; k <= j; k++ )
					g = g + q[j*n+k] * q[i*n+k];
				if ( j + 1 <= i-1 )
					for ( k = j+1; k <= i-1; k++ )
						g = g + q[k*n+j] * q[i*n+k];
					c[j] = g / h;
					f = f + g * q[j*n+i];
			}
			h2 = f / ( h + h );
			for ( j = 0; j <= i-1; j++ )
			{ 
				f = q[i*n+j];
				g = c[j] - h2 * f;
				c[j] = g;
				for ( k = 0; k <= j; k++ )
				{ 
					u = j * n + k;
					q[u] = q[u] - f * c[k] - g * q[i*n+k];
				}
			}
			b[i] = h;
		}
	}
	for ( i = 0; i <= n-2; i++ ) c[i] = c[i+1];
    c[n-1] = 0.0;
	b[0] = 0.0;
	for ( i = 0; i <= n-1; i++ )
	{ 
		if ( ( b[i] != 0.0 ) && ( i-1 >= 0 ) )
		{
			for ( j = 0; j <= i-1; j++ )
			{ 
				g = 0.0;
				for ( k = 0; k <= i-1; k++ )
					g = g + q[i*n+k] * q[k*n+j];
				for ( k = 0; k <= i-1; k++ )
				{ 
					u = k * n + j;
					q[u] = q[u] - g * q[k*n+i];
				}
			}
		}
		u = i * n + i;
		b[i] = q[u]; q[u] = 1.0;
		if ( i - 1 >= 0 )
		{
			for ( j = 0; j <= i - 1; j++ )
			{ 
				q[i*n+j] = 0.0; q[j*n+i] = 0.0;
			}
		}
	}
}

/*
   计算实对称三对角阵的全部特征值与对应特征向量
   徐士良. 常用算法程序集（C语言描述），第3版. 清华大学出版社. 2004

   int n    --- 实对称三对角阵的阶数
   double b --- 长度为n，存放n阶对称三对角阵的主对角线上的各元素；
                返回时存放全部特征值
   double c --- 长度为n，前n－1个元素存放n阶对称三对角阵的次对角
                线上的元素
   double q --- 维数为nxn，若存放单位矩阵，则返回n阶实对称三对角
                阵T的特征向量组；若存放Householder_Tri_Symetry_Diagonal()
				函数所返回的一般实对称矩阵A的豪斯荷尔得变换的乘
				积矩阵Q，则返回实对称矩阵A的特征向量组。其中，q中
				的的j列为与数组b中第j个特征值对应的特征向量
   double eps --- 本函数在迭代过程中的控制精度要求	   
   int l    --- 为求得一个特征值所允许的最大迭代次数
   返回值：
   若返回标记小于0，则说明迭代了l次还未求得一个特征值，并有fail
   信息输出；若返回标记大于0，则说明程序工作正常，全部特征值由一
   维数组b给出，特征向量组由二维数组q给出
*/
int Tri_Symmetry_Diagonal_Eigenvector( int n, double b[], double c[], double q[], double eps, int l )
{ 
	int i, j, k, m, it, u, v;
	double d, f, h, g, p, r, e, s;

	c[n-1] = 0.0; d = 0.0; f = 0.0;
	for ( j = 0; j <= n-1; j++ )
	{ 
		it = 0;
		h = eps * ( fabs( b[j] ) + fabs( c[j] ) );
		if ( h > d ) d = h;
		m = j;
		while ( ( m <= n-1 ) && ( fabs( c[m] ) > d ) ) m = m+1;
		if ( m != j )
		{ 
			do
			{ 
				if ( it == l )
				{ 
					#ifdef ALGO_DEBUG
					printf( "fail\n" );
                    #endif
					return( -1 );
				}
				it = it + 1;
				g = b[j];
				p = ( b[j+1] - g ) / ( 2.0 * c[j] );
				r = sqrt( p * p + 1.0 );
				if ( p >= 0.0 ) 
					b[j] = c[j] / ( p + r );
				else 
					b[j] = c[j] / ( p - r );
				h = g - b[j];
				for ( i = j+1; i <= n-1; i++ )
					b[i] = b[i] - h;
				f = f + h; p = b[m]; e = 1.0; s = 0.0;
				for ( i = m-1; i >= j; i-- )
				{ 
					g = e * c[i]; h = e * p;
					if ( fabs( p ) >= fabs( c[i] ) )
					{ 
						e = c[i] / p; r = sqrt( e * e + 1.0 );
						c[i+1] = s * p * r; s = e / r; e = 1.0 / r;
					}
					else
					{ 
						e = p / c[i]; r = sqrt( e * e + 1.0 );
						c[i+1] = s * c[i] * r;
						s = 1.0 / r; e = e / r;
					}
					p = e * b[i] - s * g;
					b[i+1] = h + s * ( e * g + s * b[i] );
					for ( k = 0; k <= n-1; k++ )
					{ 
						u = k * n + i + 1; v = u - 1;
						h = q[u]; q[u] = s * q[v] + e * h;
						q[v] = e * q[v] - s * h;
					}
				}
				c[j] = s * p; b[j] = e * p;
			}
			while ( fabs( c[j] ) > d );
		}
		b[j] = b[j] + f;
	}
	for ( i = 0; i <= n-1; i++ )
	{ 
		k = i; p = b[i];
		if ( i+1 <= n-1 )
		{ 
			j = i+1;
			while ( ( j <= n-1 ) && ( b[j] <= p ) )
			{ 
				k = j; p = b[j]; j = j+1;
			}
		}
		if ( k != i )
		{ 
			b[k] = b[i]; b[i] = p;
			for ( j = 0; j <= n-1; j++ )
			{ 
				u = j * n + i; v = j * n + k;
				p = q[u]; q[u] = q[v]; q[v] = p;
			}
		}
	}

	return 1;
}

# define EPS          0.000001   /* 计算精度 */
# define Iteration    60         /* 求取特征量的最多迭代次数 */

/*
   计算实对称阵的全部特征值与对应特征向量
   int n              --- 实对称阵的阶数
   double * CovMatrix --- 维数为nxn，存放n阶对称阵的各元素；
   double * Eigen     --- 长度为n，为n个特征值
   double * EigenVector --- 维数为nxn，返回n阶实对称阵的特征向量组其中，
                            EigenVector中的j列为与数组Eigen中第j个特征值
							对应的特征向量
   返回值：
   若返回标记小于0，则说明迭代了Iteration次还未求得一个特征值；
   若返回标记大于0，则说明程序工作正常，全部特征值由一
   维数组Eigen给出，特征向量组由二维数组EigenVector给出
*/
int SymmetricRealMatrix_Eigen( double CovMatrix[], int n, double Eigen[], double EigenVector[] )
{
	int k;
	double * subDiagonal;

	subDiagonal = ( double * )malloc( sizeof( double ) * n );
	Householder_Tri_Symetry_Diagonal( CovMatrix, n, EigenVector, Eigen, subDiagonal );
	k = Tri_Symmetry_Diagonal_Eigenvector( n, Eigen, subDiagonal, EigenVector, EPS, Iteration );
	free(subDiagonal);

	return k;
}

/*
  PCA1: Perform PCA using covariance.
  data     --- MxN matrix of input data ( M dimensions, N trials )
               行数为原始数据维数；每列数据为一个样本
  signals  --- MxN matrix of projected data 
  PC       --- each column is a PC
  V        --- Mx1 matrix of variances
  row = M dimensions, col = N trials 
*/
int pca( double * data, int row, int col,double * signals, double * PC, double * V )
{
	int x, y, k;
	double * mean;
	double * data1, * tPC, * tV;
	double * covariance, temp;
	int rvalue, * no, tp;

	if ( row <= 1 || col <= 1 ) return( -1 );
	/* subtract off the mean for each dimension */
	mean = ( double * )malloc( sizeof( double ) * row );
	data1 = ( double * )malloc( sizeof( double ) * row * col );
	for ( y = 0; y < row; y++ ) /* calculate mean */
	{
		mean[y] = 0;
		for ( x = 0; x < col; x++ )
			mean[y] += data[y*col+x];
	}
	for ( y = 0; y < row; y++ ) mean[y] = mean[y]/col;
	for ( y = 0; y < row; y++ ) /* subtract mean */
		for ( x = 0; x < col; x++ )
		{
			data1[y*col+x] = data[y*col+x] - mean[y];
		}
	free( mean );
	/* calculate the covariance matrix */
	covariance = ( double * )malloc( sizeof( double ) * row * row );
	for ( y = 0; y < row; y++ )
		for ( x = 0; x < row; x++ )
		{
			temp = 0;
			for ( k = 0; k < col; k++ ) 
				temp += data1[y*col+k] * data1[x*col+k];
			temp = temp / ( col-1 );
			covariance[x*row+y] = temp;
		}
	/* find the eigenvectors and eigenvalues */
	rvalue = SymmetricRealMatrix_Eigen( covariance, row, V, PC );
	free( covariance );
	/* sort the variances in decreasing order */
	no = ( int * )malloc( sizeof( int ) * row );
	for ( x = 0; x < row; x++ ) no[x] = x;
	for ( x = 0; x < row-1; x++ )
	{
		temp = V[x];
		for ( k = x; k < row; k++ )
			if ( temp < V[k] )
			{
				tp = no[k];
				no[k] = no[x];
				no[x] = tp;
				temp = V[k];
			}
	}
	/* exchange eigen value and vector in decreasing order */
	tV = ( double * )malloc( sizeof( double ) * row );
	tPC = ( double * )malloc( sizeof( double ) * row * row );
	for ( x = 0; x < row; x++ ) tV[x] = V[x];
	for ( x = 0; x < row; x++ )
		for ( y = 0; y < row; y++ )
			tPC[x*row+y] = PC[x*row+y];
	for ( x = 0; x < row; x++ )
	{
		if ( no[x] != x )
		{
			for ( y = 0; y < row; y++ )
				PC[y*row+x] = tPC[y*row+no[x]];
			V[x] = tV[no[x]];
		}
	}
	free( no );
	free( tV );
	free( tPC );

	/* project the original data: signals = PC' * data; */
	for ( y = 0; y < row; y++ )
		for ( x = 0; x < col; x++ )
		{
			signals[y*col+x] = 0;
			for ( k = 0; k < row; k++ )
				signals[y*col+x] += PC[k*row+y] * data1[k*col+x];
		}
	free(data1);

	return rvalue;
}

/*
  投影到主元素向量上
  double * newdata --- 要投影到主元素分量上的数据矩阵
  int row, col     --- 数据矩阵的维数
                       row（行数）为主元向量维数（即特征数）
					   col（列数）为采样数
  double * PC      --- 主元特征向量（列向量）
  double * newsignals --- 投影后获得的信号的系数（row*col）
  double * ShiftValue --- 如果一个有row个元素的偏移量，如果改值为NULL
                          则计算均值，并减去均值，否则减去改值
*/
int project2PCA( double * newdata, int row, int col,double * PC, double * newsignals, double * ShiftValue )
{
	int x, y, k;
	double * mean, * data1;

	/* subtract off the mean for each dimension */
	data1 = ( double * )malloc( sizeof( double ) * row * col );
	/* if ShiftValue <> NULL */
	if ( ShiftValue != NULL )
	{
		for ( y = 0; y < row; y++ ) /* subtract ShiftValue */
			for ( x = 0; x < col; x++ )
			{
				data1[y*col+x] = newdata[y*col+x] - ShiftValue[y];
			}
	}
	else
	{
		mean = ( double * )malloc( sizeof( double ) * row );
		for ( y = 0; y < row; y++ ) /* calculate mean */
		{
			mean[y] = 0;
			for ( x = 0; x < col; x++ )
				mean[y] += newdata[y*col+x];
		}
		if ( col <= 1 ) 
			for ( y = 0; y < row; y++ ) mean[y] = 0;
		else
			for ( y = 0; y < row; y++ ) mean[y] = mean[y]/col;
		for ( y = 0; y < row; y++ ) /* subtract mean */
			for ( x = 0; x < col; x++ )
			{
				data1[y*col+x] = newdata[y*col+x] - mean[y];
			}
		free( mean );		
	}
	/* project the original data: signals = PC' * data; */
	for ( y = 0; y < row; y++ )
	{
		for ( x = 0; x < col; x++ )
		{
			newsignals[y*col+x] = 0;
			for ( k = 0; k < row; k++ )
				newsignals[y*col+x] += PC[k*row+y] * data1[k*col+x];
		}
	}
	free(data1);

	return 1;
}

/*归一化用 0 和 1就可以*/

/*
算一下PCA主分量->还是放在这个项目里做
将图像数据库文件中数据读出成列向量组，并计算PCA
*/
int ReadImageDBToLieVector(double *ImageDataLieArray,char *data_in_fname)
{
	//TRAININGSNUM*RAWDATADIM
	//循环变量
    int i,j;
	FILE *fdf;
	unsigned char CurrentImageData[RAWDATADIM];
	//open datafile
	if((fdf = fopen (data_in_fname, "rb")) == NULL)
	{
		printf("Can't open file %s\n", data_in_fname);
		return FAIL;
	}
	//图像文件已归一化->无需跳过字节
	for (i = 0; i < TRAININGSNUM; i++)   //训练样本或测试样本的个数
	{
		//RAWDATADIM->20*20读取所有属于自己的像素
		fread(CurrentImageData, sizeof(unsigned char), RAWDATADIM, fdf);  //retrieve gray data
		for(j=0;j<RAWDATADIM;j++)
		{
			//ImageDataLieArray[j*TRAININGSNUM+i]=CurrentImageData[j]+0.0;
			ImageDataLieArray[j*TRAININGSNUM+i]=CurrentImageData[j]/(256.0);   //先归一化
			//ImageDataLieArray[j*TRAININGSNUM+i]=CurrentImageData[j]/(255.0); 
			/*
			if(CurrentImageData[j]==255)
			{
				ImageDataLieArray[j*TRAININGSNUM+i]=1;
			}
			else
			{
				ImageDataLieArray[j*TRAININGSNUM+i]=0;
			}
			*/
		}
	}
	fclose (fdf);
	return SUCCESSFUL;
}

/*将类别数据库文件中真实类别数据读出至类别数组*/
int ReadClassDBToClassVector(double **data_out,char *data_out_cfname)
{   
	//循环变量
    int i;
	FILE *fcf;
	unsigned char sclass;
	//open classfile
	if((fcf = fopen (data_out_cfname, "rb")) == NULL)
	{
		printf("Can't open file %s\n", data_out_cfname);
		return FAIL;
	}
	//图像文件已归一化->无需跳过字节
	//分类文件
	fseek(fcf,8,SEEK_SET);
	for (i = 0; i < TRAININGSNUM; i++)   //训练样本或测试样本的个数
	{
		fread (&sclass, sizeof(unsigned char), 1, fcf);
		//printf("sclass=%d .\n",sclass);
		for(int j=0;j<4;j++)
		{
			data_out[i][j]=out[sclass][j];
		}
	}
	fclose (fcf);
	return SUCCESSFUL;
}

//将所有维度的分量提取成FEADIM个主分量
void GetFeaturePCA(double *PC,double *FeaturePC)
{
	for(int i=0;i<RAWDATADIM;i++)    //RAWDATADIM行
	{
		for(int j=0;j<FEADIM;j++)
		{
			FeaturePC[i*FEADIM+j]=PC[i*RAWDATADIM+j];
		}
	}
}

//矩阵转置 
double *MatrixInver(double *A,double *AT,int m,int n)
{ 
	int i,j;
	for(i=0;i<n;i++) 
	{
		for(j=0;j<m;j++)
		{
			AT[i*m+j]=A[j*n+i];  
		}
	}
	return AT; 
}

//FeaturePC->RAWDATADIM*FEADIM  行*列
//FeaturePCT->FEADIM*RAWDATADIM 矩阵转置
void GetFeaturePCAT(double *FeaturePC,double *FeaturePCT)
{
	MatrixInver(FeaturePC,FeaturePCT,RAWDATADIM,FEADIM);
}

//ImageDataLieArray->RAWDATADIM行 TRAININGSNUM列  MeanV->RAWDATADIM
void CalcMeanVector(double *ImageDataLieArray,double *MeanV)
{
	//subtract off the mean for each dimension
	for(int i=0;i<RAWDATADIM;i++) /* calculate mean */
	{
		MeanV[i]=0.0;
		for(int j=0;j<TRAININGSNUM;j++)
		{
			MeanV[i]+=ImageDataLieArray[i*TRAININGSNUM+j];
		}
	}
	for(int k=0;k<RAWDATADIM;k++) 
	{
		MeanV[k]=MeanV[k]/(TRAININGSNUM+0.0);
	}
}

//两个矩阵相乘 参数：A*B＝C ->A[m,n],B[n,k],C[m,k]
void MatrixMul(double *a,double *b,int m,int n,int k,double *c)
{
	int i, j, l, u;
	for (i=0; i<=m-1; i++)
	{
		for (j=0; j<=k-1; j++)
		{ 
			u = i*k+j; 
			c[u] = 0.0;
			for (l=0; l<=n-1; l++)
			{
				c[u] = c[u] + a[i*n+l]*b[l*k+j];
			}
		}
	}
}

//计算训练样本减去平均值后的矩阵
void CalcImageDataLieArrayMinusMeanV(double *ImageDataLieArray,double *ImageDataLieArrayMinusMeanV,double *MeanV)
{
	//subtract mean
	for(int i=0;i<RAWDATADIM;i++) 
	{
		for(int j=0;j<TRAININGSNUM;j++)
		{
			ImageDataLieArrayMinusMeanV[i*TRAININGSNUM+j] = ImageDataLieArray[i*TRAININGSNUM+j]-MeanV[i];
		}
	}
}

//计算训练样本减去平均值后的矩阵  TestData->RAWDATADIM个数据
void CalcTestDataMinusMeanV(double *TestData,double *TestDataMinusMeanV,double *MeanV)
{
	//subtract mean
	for(int i=0;i<RAWDATADIM;i++) 
	{
		TestDataMinusMeanV[i] = TestData[i]-MeanV[i];
	}
}

//计算训练样本整体的特征矩阵-> (56*400)*(400*TRAININGSNUM)=56*TRAININGSNUM  ->56行 TRAININGSNUM列
void CalcDataFeatureVector(double *FeaturePCT,double *ImageDataLieArrayMinusMeanV,double *DataFeatureVector)
{
	MatrixMul(FeaturePCT,ImageDataLieArrayMinusMeanV,FEADIM,RAWDATADIM,TRAININGSNUM,DataFeatureVector);
}

//计算测试样本特征向量 FeaturePCT->56*400 TestDataMinusMeanV->1*400->400*1 56*1
void TestDataMinusMeanVToRecogFeatureVector(double *FeaturePCT,double *TestDataMinusMeanV,double *data_need_recogT)
{
	MatrixMul(FeaturePCT,TestDataMinusMeanV,FEADIM,RAWDATADIM,1,data_need_recogT);
}

//data_in->TRAININGSNUM行FEADIM列 DataFeatureVector->FEADIM*TRAININGSNUM
void EnterDataFeatureVectorToTrainInput(double **data_in,double *DataFeatureVector)
{
	for(int i=0;i<TRAININGSNUM;i++)   //训练样本或测试样本的个数 i为行
	{
		//RAWDATADIM->20*20读取所有属于自己的像素! 1000个训练样本应该也足够了.
		for(int j=0;j<FEADIM;j++)
		{
			data_in[i][j]=DataFeatureVector[j*TRAININGSNUM+i];
		}
	}
}

char Num1[400];
/*
ImgToGray
ImgToBinary
ImgToBinaryOTSU
GradientSharp
DeleteScaterJudge
RemoveScatterNoise
SlopeAdjust
StdDIBbyRect
CharSegment
GradientSharpForRecog
DeleteScaterJudgeForRecog
RemoveScatterNoiseForRecog
SlopeAdjustForRecog
StdDIBbyRectForRecog
CharSegmentForRecog
ImgToBinaryOTSUForRecog
*/
JNIEXPORT jintArray JNICALL Java_org_bigroom_pcabpnnhandwrittendigitsrecog_PatternRecognition_PcaBpnnRecog(JNIEnv* env, jobject obj, jintArray buf, int w, int h)
{
	jint *cbuf;
	int i,j;
	int count;
	int m,n;
	stream=fopen("/sdcard/log.txt","wb");
	jint *cbufnew=(jint*) malloc(sizeof(jint)*w*h);
	cbuf = env->GetIntArrayElements(buf, false);
	if (cbuf == NULL) {
		return 0;
	}
	//cbufnew=ImgToBinaryOTSUForRecog(cbuf,w,h);
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// 获得像素的颜色
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //与运算，取red值，右移16位，转为整数值
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
			if(color<90)
			{
				color=0;
			}
			else
			{
				color=255;
			}
			cbuf[w * i + j] = color;
		}
	}
	cbufnew=GradientSharpForRecog(cbuf,w,h);
	cbufnew=RemoveScatterNoiseForRecog(cbufnew,w,h);
	//cbufnew=SlopeAdjustForRecog(cbufnew,w,h);
	cbufnew=CharSegmentForRecog(cbufnew,w,h);

	int RecogResult=-1;

	//------------------------------------------------------------------------------------------
	double **testdata_in=alloc_2d_dbl(1,RAWDATADIM);   //RAWDATADIM->20*20=400
	int *testdata_trueclass=(int *)malloc(sizeof(double)*1);
	double *data_need_recog=(double *)malloc(sizeof(double)*FEADIM);
	
	//初始化变量
	double BpPa_m_a=0;
	double BpPa_m_eta=ETANUM;   //0.015->0.1
	double BpPa_m_ex=EXNUM;
	int BpPa_m_hn=HNNUM;   //7 21~31
	//相关系数
	double momentum=BpPa_m_a; 
	//最小均方误差
	double min_ex=BpPa_m_ex; 
	//隐层数目
	int n_hidden=BpPa_m_hn; 
	//训练步长->学习速率?
	double eta=BpPa_m_eta;
	int n_out=4;

	double *PC=(double *)malloc(sizeof(double)*RAWDATADIM*RAWDATADIM);         //400*400
	double *V=(double *)malloc(sizeof(double)*RAWDATADIM);

	double *MeanV=(double *)malloc(sizeof(double)*RAWDATADIM);   //400行1列
	double *FeaturePC=(double *)malloc(sizeof(double)*RAWDATADIM*FEADIM);      //最后要记得free掉
	double *FeaturePCT=(double *)malloc(sizeof(double)*FEADIM*RAWDATADIM); 

	double *TestDataMinusMeanV=(double *)malloc(sizeof(double)*RAWDATADIM);   //400行1列
	double *data_need_recogT=(double *)malloc(sizeof(double)*FEADIM);

	FILE *fp_MeanV;
	fp_MeanV=fopen("/mnt/sdcard/MeanV.dat","rb");
	fread(MeanV,sizeof(double),RAWDATADIM,fp_MeanV);
	fclose(fp_MeanV);

	FILE *fp_FeaturePCT;
	fp_FeaturePCT=fopen("/mnt/sdcard/FeaturePCT.dat","rb");
	fread(FeaturePCT,sizeof(double),FEADIM*RAWDATADIM,fp_FeaturePCT);
	fclose(fp_FeaturePCT);
	//------------------------------------------------------------------------------------------

	//char *dfname1 = "/sdcard/android_imageprocess/train-images.idx3-ubyte_bin_guiyi";
	//char *cfname1 = "/sdcard/android_imageprocess/train-labels.idx1-ubyte";
	jint *returnResult=(jint*) malloc(sizeof(jint)*digicount);
	int flag;
	for(count=0;count<digicount;count++)
	{
		CRect recttest=m_charRect.front();
		m_charRect.pop_front();
		//unsigned char *Num1=(unsigned char *) malloc(sizeof(unsigned char)*400);
		memset(Num1,0,400);
		for(i=recttest.top,m=0;i<recttest.bottom;i++,m++)
		{
			for(j=recttest.left,n=0;j<recttest.right;j++,n++)
			{
				Num1[20*m+n]=255-(unsigned char)cbufnew[w*i+j];
			}
		}

		for(int j=0;j<20;j++)
		{
			for(int k=0;k<20;k++)
			{
				testdata_in[0][j*20+k]=Num1[j*20+k]/(256.0);           //先除以256.0
			}
		}

		CalcTestDataMinusMeanV(testdata_in[0],TestDataMinusMeanV,MeanV);
		TestDataMinusMeanVToRecogFeatureVector(FeaturePCT,TestDataMinusMeanV,data_need_recogT);
		MatrixInver(data_need_recogT,data_need_recog,FEADIM,1);
		RecogResult=CodeRecognize(data_need_recog,FEADIM,n_hidden,n_out);

		//RecogResult=0;//RecogNumber(Num1,trainingSample,TRAININGSNUM);
		returnResult[count]=RecogResult;
		fprintf(stream,"%d",RecogResult);
	}
	//提取数据 先提取1200+100 全局变量!!!先用全局变量!!!
	fprintf(stream,"\n%d",digicount);
	fclose(stream);
	jintArray result=env->NewIntArray(digicount);
	env->SetIntArrayRegion(result, 0, digicount, returnResult);
	env->ReleaseIntArrayElements(buf, cbuf, 0);
	free(cbufnew);

	//----------------------------------------------------------------------------------------------------------------
	//free(data_out);
	//free(data_in);
	free(testdata_in);
	free(testdata_trueclass);
	free(data_need_recog);
	free(data_need_recogT);
	//free(ImageDataLieArray);
	//free(ImageDataLieArrayMinusMeanV);
	//free(signals);
	free(PC);
	free(V);
	free(MeanV);
	free(TestDataMinusMeanV);
	free(FeaturePC);
	free(FeaturePCT);
	//free(DataFeatureVector);

	return result;
}