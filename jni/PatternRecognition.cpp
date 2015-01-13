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
 * �������ƣ�
 *   ImgToGray()
 *
 * ����:
 *   jintArray buf    - ָ��Դͼ����������
 *   int w       - Դͼ���ȣ���������
 *   int h       - Դͼ��߶ȣ���������
 * ����ֵ:
 *   jintArray               - �����µ��������顣
 *
 * ˵��:
 * �ú������ڶ�256ɫͼ����лҶȻ�����
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
            // ������ص���ɫ
            int color = cbuf[w * i + j];
            int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
            int green = ((color & 0x0000FF00) >> 8);
            int blue = color & 0x000000FF;
            color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
            color = alpha | (color << 16) | (color << 8) | color;  //�����㣬��rgbת��֮���ֵ����r,g,b,ʹrgb��ͬ����ɵ�ɫ
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
 * �������ƣ�
 *   ImgToBinary()
 *
 * ����:
 *   jintArray buf    - ָ��Դͼ����������
 *   int w       - Դͼ���ȣ���������
 *   int h       - Դͼ��߶ȣ���������
 * ����ֵ:
 *   jintArray               - �����µ��������顣
 *
 * ˵��:
 * �ú������ڶԻҶ�ͼ����ж�ֵ����
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
			// ������ص���ɫ
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
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
			color = alpha | (color << 16) | (color << 8) | color;  //�����㣬��rgbת��֮���ֵ����r,g,b,ʹrgb��ͬ����ɵ�ɫ
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
	            // ������ص���ɫ
	            int color = cbuf[w * i + j];
	            int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
	            int green = ((color & 0x0000FF00) >> 8);
	            int blue = color & 0x000000FF;
	            color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
	            //color = alpha | (color << 16) | (color << 8) | color;  //�����㣬��rgbת��֮���ֵ����r,g,b,ʹrgb��ͬ����ɵ�ɫ
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
		   // ÿ��
		   for(int j = 0; j < w; j++)
		   {
			// ָ��DIB��i�У���j�����ص�ָ��
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
			// ������ص���ɫ
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
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
			color = alpha | (color << 16) | (color << 8) | color;  //�����㣬��rgbת��֮���ֵ����r,g,b,ʹrgb��ͬ����ɵ�ɫ
			cbuf[w * i + j] = color;
		}
	}

	int bThre=2;
	int bTemp;
	for(int i = 0; i < h; i++)
		{
	     // ÿ��
		 for(int j = 0; j < w; j++)
			{
			     bTemp=abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-2-i)+j])+abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-1-i)+j+1]);

			    // �ж��Ƿ�С����ֵ
			    if (bTemp < 255)
				{
			     // �ж��Ƿ������ֵ������С��������Ҷ�ֵ���䡣
				   if (bTemp >= bThre)
				   {
					   bTemp=alpha|(bTemp<<16)|(bTemp<<8)|bTemp;
					   cbuf[w*(h-1-i)+j]=bTemp;
				   }
				}
	           else
				{
					// ֱ�Ӹ�ֵΪ255
	        	   int color1=255;
	        	   color1=alpha|(color1<<16)|(color1<<8)|color1;
	        	   cbuf[w*(h-1-i)+j]=color1;
				}
		 }
	   }

	//���Ҫ����һ��ͼ��������������
	for(int j = 0; j < h; j++)
	{
	 //ָ�����±�һ�У���j�����ص�ָ��
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

	//���������������Ҫ��˵��������ɢ�㣬����
	if(m_lianXuShu>=lianXuShu)
		return true;
	//���ȼ�һ
	m_lianXuShu++;
	//�趨���ʱ�־
	lplab[lWidth * y +x] = true;

	//������ʵ�����
	lab[m_lianXuShu-1].x = x;
	lab[m_lianXuShu-1].y = y;

	//���صĻҶ�ֵ
	int gray;

	//�����ж�
    //���������������Ҫ��˵��������ɢ�㣬����
	if(m_lianXuShu>=lianXuShu)
		return true;

	//�������ݹ�
	else
	{
		//�������������Լ����ϡ����ϡ����¡����°˸�����
		//����Ǻ�ɫ�㣬����ú���������еݹ�

		//���������
		gray=cbuf[lWidth*(y-1)+x];
		//fprintf(stream,"%d %d %d\n",gray,y,x);
		//�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y-1 >=0 && gray == 0 && lplab[(y-1)*lWidth+x] == false)
		{
			//fprintf(stream,"%s\n","1 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y-1,lab,lianXuShu);
		}
		//�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//���µ�
		gray=cbuf[lWidth*(y-1)+x-1];

        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y-1 >=0 &&  x-1 >=0 && gray== 0 && lplab[(y-1)*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","2 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y-1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}


		//���

		gray=cbuf[lWidth*y+x-1];

        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(x-1 >=0 &&  gray== 0 && lplab[y*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","3 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����

		gray=cbuf[lWidth*(y+1)+x-1];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y+1 <lHeight && x-1 >= 0 && gray == 0 && lplab[(y+1)*lWidth+x-1] == false)
		{
		//���еݹ鴦��
		//fprintf(stream,"%s\n","4 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y+1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����
		gray=cbuf[lWidth*(y+1)+x];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y+1 < lHeight && gray == 0 && lplab[(y+1)*lWidth+x] == false)
		{
        //���еݹ鴦��
		//fprintf(stream,"%s\n","5 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y+1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����

		gray=cbuf[lWidth*(y+1)+x+1];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y+1 <lHeight && x+1 <lWidth &&  gray == 0 && lplab[(y+1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","6 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y+1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//�ұ�

		gray=cbuf[lWidth*y+x+1];
		//�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(x+1 <lWidth && gray==0 && lplab[y*lWidth+x+1] == false)
		{
		//fprintf(stream,"%s\n","7 ");
        //���еݹ鴦��
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y,lab,lianXuShu);
		}
        //�жϳ���

		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����
		gray=cbuf[lWidth*(y-1)+x+1];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y-1 >=0 && x+1 <lWidth && gray == 0 && lplab[(y-1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","8 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y-1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
	}

	//fprintf(stream,"%s\n","error");
	//����ݹ����������false��˵������ɢ��
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
			// ������ص���ɫ
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
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
	// ѭ������
	m_lianXuShu=0;
	CPoint lab[21];
	for (i=0;i<lHeight*lWidth;i++)
    {
    //�����еı�־λ����Ϊ��
		lplab[i] = false;
	}
	for(i =0;i<lHeight;i++)
	{

	   //����ɨ��
		for(j=0;j<lWidth;j++)
			{
				//�Ȱѱ�־λ��false
				for(k=0;k<m_lianXuShu;k++)
				lplab[lab[k].y * lWidth + lab[k].x] = false;

				//��������0
				m_lianXuShu =0;

				//������ɢ���ж�
				//lpTemp[i*lWidth+j] = DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,j,i,lab,length);
				lpTemp[i*lWidth+j] = DeleteScaterJudge(cbuf,(bool *)lplab,lWidth,lHeight,j,i,lab,length);
			}
	}
	for(i = 0;i<lHeight;i++)
	{

	  //����ɨ��
		for(j=0;j<lWidth;j++)
		{
				//�鿴��־λ,���Ϊ���򽫴˵���Ϊ�׵�
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
			// ������ص���ɫ
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
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
	//ͼ�����ߵ�ƽ���߶�
	double leftaver=0.0;
   //ͼ���Ұ�ߵ�ƽ���߶�
	double rightaver=0.0;
	//ͼ�����б��
	double slope;
	int counts;
	//ɨ�����ߵ�ͼ�����ɫ���ص�ƽ���߶�

	//��
	for (i=0;i<lHeight;i++)
	{
      //��
		for (j=0;j<lWidth/2;j++)
		{
		 //���Ϊ�ڵ�
		 if (cbuf[lWidth*i+j]==0)
		 {
          //����߶Ƚ���ͳ�Ƶ���
		  counts +=lWidth/2 -j;
		  leftaver += i*(lWidth/2 -j);
		 }
		}
	}
    //����ƽ���߶�
	leftaver /= counts;
	//��ͳ��ѭ���������¸�ֵ
	counts =0;
	//ɨ���Ұ�ߵ�ͼ�����ɫ���ص�ƽ���߶�
	//��
	for (i =0;i<lHeight;i++)
	{
       //��
		for (j=lWidth/2;j<lWidth;j++)
		{
			//ָ���i�е�j�����ص�ָ��
            //���Ϊ�ڵ�
			if (cbuf[lWidth*i+j]== 0)
				{
				  //����ͳ�Ƶ���
					counts +=lWidth -j;
					rightaver += i*(lWidth -j);
				}
		}
	}
	//�����Ұ�ߵ�ƽ���߶�
	rightaver /= counts;
	//����б��
	slope = (leftaver - rightaver) / (lWidth/2);
	//���ص�ĻҶ�ֵ
	int gray;
	//λ��ӳ��ֵ
	int i_src;
	//����б�ʣ��ѵ�ǰ��ͼ��ĵ�ӳ�䵽Դͼ��ĵ�
	//��
	for (i=0;i<lHeight;i++)
	{
		//��
   		for (j=0;j<lWidth;j++)
		{
		   //����ӳ��λ��
			i_src=int(i - (j-lWidth/2)*slope);

			//�������ͼ���⣬�����ð�ɫ
			if (i_src <0 || i_src >=lHeight )
				gray = 255;
			else
			{
				//����Դͼ�����ҵ㣬ȡ������ֵ
				//ָ���i_src�е�j�����ص�ָ��
				gray=cbuf[lWidth*i_src+j];
			}
			//����ͼ��ĵ��õõ�������ֵ���
            //ָ���i�е�j�����ص�ָ��
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
	// ѭ������
	int	i;
	int	j;
	int alpha=0xFF<<24;
	//��ȡ��߶ȷ����ϵ���������
	double wscale,hscale;
	//����ӳ��������������
	int i_src,j_src;
	//int *cbuftemp=new int[lWidth*lHeight];

	//����ַ�λ����Ϣ�Ľṹ��
	CRect rect;
	CRect rectnew;

	//�����һ���µľ������������Ա�洢��׼����ľ�����������
	m_charRectCopy.clear ();

	//��ͷ��β���ɨ��������
	while(!m_charRect.empty())
	{
		//�ӱ�ͷ�ϵõ�һ������
		rect= m_charRect.front();

		//������ͷ����ɾ��һ��
		m_charRect.pop_front();

		//������������

		//�����귽�����������
		wscale=(double)tarWidth/rect.Width ();

		//�����귽�����������
		hscale=(double)tarHeight/rect.Height ();

		//�����׼������

		//�ϱ߽�
		rectnew.top =rect.top ;

		//�±߽�
		rectnew.bottom =rect.top +tarHeight;

		//��߽�
		rectnew.left =rect.left ;

		//�ұ߽�
		rectnew.right =rectnew.left +tarWidth;

		//��ԭ���ο��ڵ�����ӳ�䵽�µľ��ο���
		for(i=rectnew.top ;i<rectnew.bottom ;i++)
		{
			for(j=rectnew.left ;j<rectnew.right ;j++)
			{

				//����ӳ������
				i_src=rectnew.top +int((i-rectnew.top )/hscale);
				j_src=rectnew.left +int((j-rectnew.left )/wscale);

				//�����Ӧ�����ص����ӳ�����
				int colortemp=cbuf[lWidth*i_src+j_src];
				colortemp=alpha|(colortemp<<16)|(colortemp<<8)|colortemp;
				cbufnew[lWidth*i+j]=colortemp;
				//cbuftemp[lWidth*i+j]=colortemp;
			}
		}
		//����׼����ľ�����������µ�����
		m_charRectCopy.push_back (rectnew);
	}

	//�洢��׼�����µ�rect����
    m_charRect=m_charRectCopy;
//    CRect recttest=m_charRect.front();
//    for(i=recttest.top;i<recttest.bottom;i++)
//    {
//    	for(j=recttest.left;j<recttest.right;j++)
//    	{
//    		cbufnew[lWidth*i+j]=cbuftemp[lWidth*i+j];
//    	}
//    }

	//�������������ݿ�����ͼ�����������
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
	//����ͳ��ͼ�����ַ������ļ�����
	digicount=0;
	cbuf = env->GetIntArrayElements(buf, false);
	jint *cbufnew=(jint*) malloc(sizeof(jint)*w*h);
	int alpha = 0xFF << 24;  //alpha=0xFF000000
	int color1=255;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// ������ص���ɫ
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
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
		 //��
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//���Ƿ�Ϊ�ڵ�
			if (gray == 0)
			{
			   //��Ϊ�ڵ㣬�Ѵ˵���Ϊ�ַ����µ���ߵ�
				top = i;
				//��iǿ�и�ֵ���ж�ѭ��
				i=height;
				//����ѭ��
				break;
			}
		//����õ㲻�Ǻڵ㣬����ѭ��
		}
	}
	//��������ɨ�裬���±߽�

		//��
	for (i = height-1;i>=0;i--)
	{
		//��
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//�ж��Ƿ�Ϊ�ڵ�
			if (gray == 0)
			{
				//��Ϊ�ڵ㣬�Ѵ˵���Ϊ�ַ����µ���͵�
				bottom = i;
				//��iǿ�и�ֵ���ж�ѭ��
				i=-1;
				//����ѭ��
				break;
			}
		  //����õ㲻�Ǻڵ㣬����ѭ��
		}
	}
	//lab �����Ƿ����һ���ַ��ָ�ı�־
	bool lab = false;
	//����ɨ��һ�����Ƿ��ֺ�ɫ��
	bool black = false;
	//����������
	digicount=0;
	CRect rect;
	//��
	for (i=0;i<width;i++)
	{
		//��ʼɨ��һ��
		black=false;

		for (j=0;j<height;j++)
			{
				gray=cbuf[width*j+i];
				//�ж��Ƿ�Ϊ�ڵ�
				if (gray == 0)
				{
					//������ֺڵ㣬���ñ�־λ
					black=true;

					//�����û�н���һ���ַ��ķָ�
					if(lab==false)
					{
						//�������߽�
						rect.left = i;
						//�ַ��ָʼ
						lab = true;
					}
					//����ַ��ָ��Ѿ���ʼ��
					else
					  //����ѭ��
						break;
				}
			}

		//����Ѿ�ɨ�������ұ����У�˵������ͼ��ɨ����ϡ��˳�
		   if(i==(width-1))

		 //�˳�����ѭ��
		   break;

		//�������black��Ϊfalse��˵��ɨ����һ�У���û�з��ֺڵ㡣������ǰ�ַ��ָ����
		if(lab==true&&black==false)
		{
		   //��λ����Ϣ����ṹ����
		   //�����ұ߽�
			rect.right =i-1;  //chenxupro
			//�����ϱ߽�
			rect.top =top;
			//�����±߽�
			rect.bottom =bottom;
			//��������һ�����أ�����ѹ���ַ�
			//rect.InflateRect (1,1);
			//������ṹ�������λ����Ϣ������1�ĺ���
			charRect1.push_back (rect);
			//���ñ�־λ����ʼ��һ�ε��ַ��ָ�
			lab=false;

			//�ַ�����ͳ�Ƽ�������1
			digicount++;

		}

		//������һ�е�ɨ��
	}
	 //�ٽ������������ε�top��bottom��ȷ��

	//������1��ֵ������2
	charRect2=charRect1;

	//������2���������
	charRect2.clear ();

	//����һ���µĴ��λ����Ϣ�Ľṹ��
	CRect rectnew;
	//��������1��ͷ��β�������ɨ��
	while(!charRect1.empty())
	{
		//������1ͷ�ϵõ�һ������
		rect= charRect1.front();

		//������1ͷ����ɾ��һ��
		charRect1.pop_front();

		//������Ӿ�ȷ�ľ�������

		//��þ�ȷ����߽�
		rectnew.left =rect.left-1 ;

		//��þ�ȷ���ұ߽�
		rectnew.right =rect.right+1 ;

		//ͨ����õľ�ȷ���ұ߽�����±߾����½��о�ȷ��λ

		// ���¶���ɨ������ϱ߽�

		//��
		for(i=rect.top ;i<rect.bottom ;i++)
		{
		  //��
			for(j=rect.left ;j<rect.right ;j++)
			{
				 // ָ��ͼ���i�У���j�����ص�ָ��
				//�����������Ǻڵ�
				if (cbuf[width*i+j] == 0)
				{
					//�����ϱ߽�
					rectnew.top = i-1;
					//��i����ǿ�ƶ���������ѭ��
					i=rect.bottom  ;
					//����ѭ��
					break;
				}
			}
		}
		//���¶���ɨ������±߽�
		//��
		for(i=rect.bottom-1 ;i>=rect.top  ;i--)
		{
			//��
			for(j=rect.left ;j<rect.right ;j++)
			{
				// ָ��ͼ���i�У���j�����ص�ָ��
				//�õ����Ϊ�ڵ�
				if (cbuf[width*i+j] == 0)
				{
					//�����±߽�
					rectnew.bottom = i+1;

					//��i����ǿ�ƶ���������ѭ��
					i=-1;
					//����ѭ��
					break;
				}
			}
		}
		//���õ����µ�׼ȷ��λ����Ϣ�Ӻ���嵽����2��β��
		charRect2.push_back (rectnew);
	}
	//������2 ���ݸ�����1
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
	     // ÿ��
		 for(int j = 0; j < w; j++)
			{
			     bTemp=abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-2-i)+j])+abs(cbuf[w*(h-1-i)+j]-cbuf[w*(h-1-i)+j+1]);

			    // �ж��Ƿ�С����ֵ
			    if (bTemp < 255)
				{
			     // �ж��Ƿ������ֵ������С��������Ҷ�ֵ���䡣
				   if (bTemp >= bThre)
				   {
					   cbuf[w*(h-1-i)+j]=bTemp;
				   }
				}
	           else
				{
					// ֱ�Ӹ�ֵΪ255
	        	   int color1=255;
	        	   cbuf[w*(h-1-i)+j]=color1;
				}
		 }
	   }

	//���Ҫ����һ��ͼ��������������
	for(int j = 0; j < h; j++)
	{
	 //ָ�����±�һ�У���j�����ص�ָ��
		int color1=255;
		cbuf[w*0+j]=color1;
	}
	return cbuf;
}
bool DeleteScaterJudgeForRecog(int *cbuf,bool *lplab, int lWidth, int lHeight, int x, int y, CPoint lab[], int lianXuShu)
{

	//���������������Ҫ��˵��������ɢ�㣬����
	if(m_lianXuShu>=lianXuShu)
		return true;
	//���ȼ�һ
	m_lianXuShu++;
	//�趨���ʱ�־
	lplab[lWidth * y +x] = true;

	//������ʵ�����
	lab[m_lianXuShu-1].x = x;
	lab[m_lianXuShu-1].y = y;

	//���صĻҶ�ֵ
	int gray;

	//�����ж�
    //���������������Ҫ��˵��������ɢ�㣬����
	if(m_lianXuShu>=lianXuShu)
		return true;

	//�������ݹ�
	else
	{
		//�������������Լ����ϡ����ϡ����¡����°˸�����
		//����Ǻ�ɫ�㣬����ú���������еݹ�

		//���������
		gray=cbuf[lWidth*(y-1)+x];
		//fprintf(stream,"%d %d %d\n",gray,y,x);
		//�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y-1 >=0 && gray == 0 && lplab[(y-1)*lWidth+x] == false)
		{
			//fprintf(stream,"%s\n","1 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y-1,lab,lianXuShu);
		}
		//�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//���µ�
		gray=cbuf[lWidth*(y-1)+x-1];

        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y-1 >=0 &&  x-1 >=0 && gray== 0 && lplab[(y-1)*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","2 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y-1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}


		//���

		gray=cbuf[lWidth*y+x-1];

        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(x-1 >=0 &&  gray== 0 && lplab[y*lWidth+x-1] == false)
		{
			//fprintf(stream,"%s\n","3 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����

		gray=cbuf[lWidth*(y+1)+x-1];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y+1 <lHeight && x-1 >= 0 && gray == 0 && lplab[(y+1)*lWidth+x-1] == false)
		{
		//���еݹ鴦��
		//fprintf(stream,"%s\n","4 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x-1,y+1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����
		gray=cbuf[lWidth*(y+1)+x];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y+1 < lHeight && gray == 0 && lplab[(y+1)*lWidth+x] == false)
		{
        //���еݹ鴦��
		//fprintf(stream,"%s\n","5 ");
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x,y+1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����

		gray=cbuf[lWidth*(y+1)+x+1];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y+1 <lHeight && x+1 <lWidth &&  gray == 0 && lplab[(y+1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","6 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y+1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//�ұ�

		gray=cbuf[lWidth*y+x+1];
		//�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(x+1 <lWidth && gray==0 && lplab[y*lWidth+x+1] == false)
		{
		//fprintf(stream,"%s\n","7 ");
        //���еݹ鴦��
		DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y,lab,lianXuShu);
		}
        //�жϳ���

		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
		//����
		gray=cbuf[lWidth*(y-1)+x+1];
        //�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
		if(y-1 >=0 && x+1 <lWidth && gray == 0 && lplab[(y-1)*lWidth+x+1] == false)
		{
			//fprintf(stream,"%s\n","8 ");
			//���еݹ鴦��
			DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,x+1,y-1,lab,lianXuShu);
		}
        //�жϳ���
		//���������������Ҫ��˵��������ɢ�㣬����
		if(m_lianXuShu>=lianXuShu)
		{
			return true;
		}
	}

	//fprintf(stream,"%s\n","error");
	//����ݹ����������false��˵������ɢ��
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
	// ѭ������
	m_lianXuShu=0;
	CPoint lab[21];
	for (i=0;i<lHeight*lWidth;i++)
    {
    //�����еı�־λ����Ϊ��
		lplab[i] = false;
	}
	for(i =0;i<lHeight;i++)
	{

	   //����ɨ��
		for(j=0;j<lWidth;j++)
			{
				//�Ȱѱ�־λ��false
				for(k=0;k<m_lianXuShu;k++)
				lplab[lab[k].y * lWidth + lab[k].x] = false;

				//��������0
				m_lianXuShu =0;

				//������ɢ���ж�
				lpTemp[i*lWidth+j] = DeleteScaterJudge(cbuf,lplab,lWidth,lHeight,j,i,lab,length);
			}
	}
	for(i = 0;i<lHeight;i++)
	{

	  //����ɨ��
		for(j=0;j<lWidth;j++)
		{
				//�鿴��־λ,���Ϊ���򽫴˵���Ϊ�׵�
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
	//ͼ�����ߵ�ƽ���߶�
	double leftaver=0.0;
   //ͼ���Ұ�ߵ�ƽ���߶�
	double rightaver=0.0;
	//ͼ�����б��
	double slope;
	int counts;
	//ɨ�����ߵ�ͼ�����ɫ���ص�ƽ���߶�

	//��
	for (i=0;i<lHeight;i++)
	{
      //��
		for (j=0;j<lWidth/2;j++)
		{
		 //���Ϊ�ڵ�
		 if (cbuf[lWidth*i+j]==0)
		 {
          //����߶Ƚ���ͳ�Ƶ���
		  counts +=lWidth/2 -j;
		  leftaver += i*(lWidth/2 -j);
		 }
		}
	}
    //����ƽ���߶�
	leftaver /= counts;
	//��ͳ��ѭ���������¸�ֵ
	counts =0;
	//ɨ���Ұ�ߵ�ͼ�����ɫ���ص�ƽ���߶�
	//��
	for (i =0;i<lHeight;i++)
	{
       //��
		for (j=lWidth/2;j<lWidth;j++)
		{
			//ָ���i�е�j�����ص�ָ��
            //���Ϊ�ڵ�
			if (cbuf[lWidth*i+j]== 0)
				{
				  //����ͳ�Ƶ���
					counts +=lWidth -j;
					rightaver += i*(lWidth -j);
				}
		}
	}
	//�����Ұ�ߵ�ƽ���߶�
	rightaver /= counts;
	//����б��
	slope = (leftaver - rightaver) / (lWidth/2);
	//���ص�ĻҶ�ֵ
	int gray;
	//λ��ӳ��ֵ
	int i_src;
	//����б�ʣ��ѵ�ǰ��ͼ��ĵ�ӳ�䵽Դͼ��ĵ�
	//��
	for (i=0;i<lHeight;i++)
	{
		//��
   		for (j=0;j<lWidth;j++)
		{
		   //����ӳ��λ��
			i_src=int(i - (j-lWidth/2)*slope);

			//�������ͼ���⣬�����ð�ɫ
			if (i_src <0 || i_src >=lHeight )
				gray = 255;
			else
			{
				//����Դͼ�����ҵ㣬ȡ������ֵ
				//ָ���i_src�е�j�����ص�ָ��
				gray=cbuf[lWidth*i_src+j];
			}
			//����ͼ��ĵ��õõ�������ֵ���
            //ָ���i�е�j�����ص�ָ��
			cbufnew[lWidth*i+j]=gray;
		}
	}
	return cbufnew;
}
void StdDIBbyRectForRecog(int* cbuf, int *cbufnew,int lWidth,int lHeight,int tarWidth, int tarHeight)
{
	// ѭ������
	int	i;
	int	j;
	//��ȡ��߶ȷ����ϵ���������
	double wscale,hscale;
	//����ӳ��������������
	int i_src,j_src;
	//int *cbuftemp=new int[lWidth*lHeight];

	//����ַ�λ����Ϣ�Ľṹ��
	CRect rect;
	CRect rectnew;

	//�����һ���µľ������������Ա�洢��׼����ľ�����������
	m_charRectCopy.clear ();

	//��ͷ��β���ɨ��������
	while(!m_charRect.empty())
	{
		//�ӱ�ͷ�ϵõ�һ������
		rect= m_charRect.front();

		//������ͷ����ɾ��һ��
		m_charRect.pop_front();

		//������������

		//�����귽�����������
		wscale=(double)tarWidth/rect.Width ();

		//�����귽�����������
		hscale=(double)tarHeight/rect.Height ();

		//�����׼������

		//�ϱ߽�
		rectnew.top =rect.top ;

		//�±߽�
		rectnew.bottom =rect.top +tarHeight;

		//��߽�
		rectnew.left =rect.left ;

		//�ұ߽�
		rectnew.right =rectnew.left +tarWidth;

		//��ԭ���ο��ڵ�����ӳ�䵽�µľ��ο���
		for(i=rectnew.top ;i<rectnew.bottom ;i++)
		{
			for(j=rectnew.left ;j<rectnew.right ;j++)
			{

				//����ӳ������
				i_src=rectnew.top +int((i-rectnew.top )/hscale);
				j_src=rectnew.left +int((j-rectnew.left )/wscale);

				//�����Ӧ�����ص����ӳ�����
				int colortemp=cbuf[lWidth*i_src+j_src];
				cbufnew[lWidth*i+j]=colortemp;
				//cbuftemp[lWidth*i+j]=colortemp;
			}
		}
		//����׼����ľ�����������µ�����
		m_charRectCopy.push_back (rectnew);
	}

	//�洢��׼�����µ�rect����
    m_charRect=m_charRectCopy;
//    CRect recttest=m_charRect.front();
//    for(i=recttest.top;i<recttest.bottom;i++)
//    {
//    	for(j=recttest.left;j<recttest.right;j++)
//    	{
//    		cbufnew[lWidth*i+j]=cbuftemp[lWidth*i+j];
//    	}
//    }

	//�������������ݿ�����ͼ�����������
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
	//����ͳ��ͼ�����ַ������ļ�����
	digicount=0;
	int *cbufnew=(int*) malloc(sizeof(int)*w*h);
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			cbufnew[w*i+j]=255;
		}
	}
	for (i=0;i<height;i++)
	{
		 //��
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//���Ƿ�Ϊ�ڵ�
			if (gray == 0)
			{
			   //��Ϊ�ڵ㣬�Ѵ˵���Ϊ�ַ����µ���ߵ�
				top = i;
				//��iǿ�и�ֵ���ж�ѭ��
				i=height;
				//����ѭ��
				break;
			}
		//����õ㲻�Ǻڵ㣬����ѭ��
		}
	}
	//��������ɨ�裬���±߽�

		//��
	for (i = height-1;i>=0;i--)
	{
		//��
		for (j=0;j<width;j++)
		{
			gray=cbuf[width*i+j];
			//�ж��Ƿ�Ϊ�ڵ�
			if (gray == 0)
			{
				//��Ϊ�ڵ㣬�Ѵ˵���Ϊ�ַ����µ���͵�
				bottom = i;
				//��iǿ�и�ֵ���ж�ѭ��
				i=-1;
				//����ѭ��
				break;
			}
		  //����õ㲻�Ǻڵ㣬����ѭ��
		}
	}
	//lab �����Ƿ����һ���ַ��ָ�ı�־
	bool lab = false;
	//����ɨ��һ�����Ƿ��ֺ�ɫ��
	bool black = false;
	//����������
	digicount=0;
	CRect rect;
	//��
	for (i=0;i<width;i++)
	{
		//��ʼɨ��һ��
		black=false;

		for (j=0;j<height;j++)
			{
				gray=cbuf[width*j+i];
				//�ж��Ƿ�Ϊ�ڵ�
				if (gray == 0)
				{
					//������ֺڵ㣬���ñ�־λ
					black=true;

					//�����û�н���һ���ַ��ķָ�
					if(lab==false)
					{
						//�������߽�
						rect.left = i;
						//�ַ��ָʼ
						lab = true;
					}
					//����ַ��ָ��Ѿ���ʼ��
					else
					  //����ѭ��
						break;
				}
			}

		//����Ѿ�ɨ�������ұ����У�˵������ͼ��ɨ����ϡ��˳�
		   if(i==(width-1))

		 //�˳�����ѭ��
		   break;

		//�������black��Ϊfalse��˵��ɨ����һ�У���û�з��ֺڵ㡣������ǰ�ַ��ָ����
		if(lab==true&&black==false)
		{
		   //��λ����Ϣ����ṹ����
		   //�����ұ߽�
			rect.right =i-1;  //chenxupro
			//�����ϱ߽�
			rect.top =top;
			//�����±߽�
			rect.bottom =bottom;
			//��������һ�����أ�����ѹ���ַ�
			//rect.InflateRect (1,1);
			//������ṹ�������λ����Ϣ������1�ĺ���
			charRect1.push_back (rect);
			//���ñ�־λ����ʼ��һ�ε��ַ��ָ�
			lab=false;

			//�ַ�����ͳ�Ƽ�������1
			digicount++;

		}

		//������һ�е�ɨ��
	}
	 //�ٽ������������ε�top��bottom��ȷ��

	//������1��ֵ������2
	charRect2=charRect1;

	//������2���������
	charRect2.clear ();

	//����һ���µĴ��λ����Ϣ�Ľṹ��
	CRect rectnew;
	//��������1��ͷ��β�������ɨ��
	while(!charRect1.empty())
	{
		//������1ͷ�ϵõ�һ������
		rect= charRect1.front();

		//������1ͷ����ɾ��һ��
		charRect1.pop_front();

		//������Ӿ�ȷ�ľ�������

		//��þ�ȷ����߽�
		rectnew.left =rect.left-1 ;

		//��þ�ȷ���ұ߽�
		rectnew.right =rect.right+1 ;

		//ͨ����õľ�ȷ���ұ߽�����±߾����½��о�ȷ��λ

		// ���¶���ɨ������ϱ߽�

		//��
		for(i=rect.top ;i<rect.bottom ;i++)
		{
		  //��
			for(j=rect.left ;j<rect.right ;j++)
			{
				 // ָ��ͼ���i�У���j�����ص�ָ��
				//�����������Ǻڵ�
				if (cbuf[width*i+j] == 0)
				{
					//�����ϱ߽�
					rectnew.top = i-1;
					//��i����ǿ�ƶ���������ѭ��
					i=rect.bottom  ;
					//����ѭ��
					break;
				}
			}
		}
		//���¶���ɨ������±߽�
		//��
		for(i=rect.bottom-1 ;i>=rect.top  ;i--)
		{
			//��
			for(j=rect.left ;j<rect.right ;j++)
			{
				// ָ��ͼ���i�У���j�����ص�ָ��
				//�õ����Ϊ�ڵ�
				if (cbuf[width*i+j] == 0)
				{
					//�����±߽�
					rectnew.bottom = i+1;

					//��i����ǿ�ƶ���������ѭ��
					i=-1;
					//����ѭ��
					break;
				}
			}
		}
		//���õ����µ�׼ȷ��λ����Ϣ�Ӻ���嵽����2��β��
		charRect2.push_back (rectnew);
	}
	//������2 ���ݸ�����1
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
			// ������ص���ɫ
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
			int green = ((color & 0x0000FF00) >> 8);
			int blue = color & 0x000000FF;
			color = red*0.299+green*0.587+blue*0.114;     //Convert to 8 bit
			//color = alpha | (color << 16) | (color << 8) | color;  //�����㣬��rgbת��֮���ֵ����r,g,b,ʹrgb��ͬ����ɵ�ɫ
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
	   // ÿ��
	   for(int j = 0; j < w; j++)
	   {
		// ָ��DIB��i�У���j�����ص�ָ��
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

#define TRAININGSNUM 60000    //ѵ��������  //max->60000  1000->500
#define HNNUM        35       //52  ->12
#define ETANUM       0.04     //0.015   0.04
#define FEADIM       64       //��ȡ������ά��  K-L�任 56->100  200?����13������ ����һЩ������ 200->12
#define EXNUM        0.02     //0.001

#define RAWDATADIM   400      //ÿ�����ֵ����ص��� 16*16 28*28 20*20->��һ����
#define MAXDIEDAINUM 5000
#define TESTINGSNUM	 1000     //����������  //max->10000
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

/*** ����0��1��˫��������� ***/
double drnd()
{
	return ((double) rand() / (double) BIGRND);
}

/*** ����-1.0��1.0֮���˫��������� ***/
double dpn1()
{
	return ((drnd() * 2.0) - 1.0);
}

double squash(double x)
{
	return (1.0 / (1.0 + exp(-x)));
}

/*** ����1ά˫����ʵ������ ***/
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

/*** ����2ά˫����ʵ������ ***/
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

/*** ������������� ***/
void bpnn_initialize(int seed)
{
	// printf("Random number generator seed: %d\n", seed);
	srand(seed);
}

/*** �����ʼ��Ȩֵ ***/
void bpnn_randomize_weights(double **w, int m, int n)
{
	int i, j;
	for (i = 0; i <= m; i++) {
		for (j = 0; j <= n; j++) {
			w[i][j] = dpn1();
		}
	}
}

/*** 0��ʼ��Ȩֵ ***/
void bpnn_zero_weights(double **w, int m, int n)
{
	int i, j;
	for (i = 0; i <= m; i++) {
		for (j = 0; j <= n; j++) {
			w[i][j] = 0.0;
		}
	}
}

/*********ǰ����*********/
void bpnn_layerforward(double *l1, double *l2, double **conn, int n1, int n2)
{
	double sum;
	int j, k;
	
	/*** ������ֵ ***/
	l1[0] = 1.0;
	
	/*** ���ڵڶ����ÿ����Ԫ ***/
	for (j = 1; j <= n2; j++) {
		
		/*** ��������ļ�Ȩ�ܺ� ***/
		sum = 0.0;
		for (k = 0; k <= n1; k++) {
			sum += conn[k][j] * l1[k];
		}
		l2[j] = squash(sum);
	}
}

/*********ǰ����*********/
void bpnn_layerforward_1(double *l1, double *l2, double **conn, int n1, int n2)
{
	double sum;
	int j, k;
	
	/*** ������ֵ ***/
	l1[0] = 1.0;
	
	/*** ���ڵڶ����ÿ����Ԫ ***/
	for (j = 1; j <= n2; j++) {
		
		/*** ��������ļ�Ȩ�ܺ� ***/
		sum = 0.0;
		for (k = 0; k <= n1; k++) {
			sum += conn[k][j] * l1[k];
		}
		//l2[j] = squash(sum);
		l2[j]=sum;
	}
}

/* ������ */
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

/* ��������� */
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

/* ����Ȩֵ */
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

/*******����Ȩֵ**********/
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

/************��ȡȨֵ*************/
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

/*****������������Ŀ******/
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

/********��ȡ��������Ŀ*********/
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

//��������:��������������ȡ�����������ѹ�һ��������ÿһ�����ض���Ϊ������ȡ����
double *code_OnlyOne (unsigned char *BinGuiYiImageDataArray)
{   
	//ѭ������
    int i,j;
	int CurrentColorValue=0;
	double *data=NULL;
	//Ϊ������������ά�洢�ռ�
	data = (double *)malloc(sizeof(double)*20*20);
	
	//����һ����������ÿ��������Ϊһ����������ȡ����
	//��ÿ����������ɨ��
	for(i=0;i<20;i++)
	{  
		//��ÿ����������ɨ��
		for(j=0;j<20;j++)
		{	
			//ָ��ͼ���i�е�j�и����ص�ָ��
			CurrentColorValue=BinGuiYiImageDataArray[i*20+j];
			if(CurrentColorValue==0)
			{
				//��������������Ӧλ����1
				data[i*20+j]=1;
			}
			//������������������   
			if(CurrentColorValue!=0)
			{
				//��������������Ӧλ����0
				data[i*20+j]=0;
			}
		}
	}
	return data;
}

/****************************************************
��������:����������������������������������BP�������ѵ��,ѵ��������Ȩֵ���沢��ѵ���Ľ����ʾ����
********************************************************/
//MaxDieDaiNum->ָ������������!!!
//num->������Ŀ
void BpTrain(double ** data_in, double** data_out,int n_in,int n_hidden,double min_ex,double momentum,double eta ,int num,int CurrentMaxDieDaiNum)
{
	//ѭ������   
	int i,k,l;
	
	//���������Ŀ
	int n_out=4;   
	//ָ����������ݵ�ָ��
	double* input_unites; 
	//ָ���������ݵ�ָ��
	double* hidden_unites;
	//ָ����������ݵ�ָ��
	double* output_unites; 
	//ָ������������ݵ�ָ��
	double* hidden_deltas;
	//ָ���������������ָ��
	double* output_deltas;  
	//ָ������Ŀ�������ָ��
	double* target;    
	//ָ�������������֮��Ȩֵ��ָ��
	double** input_weights;
	//ָ�������������֮���Ȩֵ��ָ��
	double** hidden_weights;
	//ָ����һ�������������֮��Ȩֵ��ָ��
	double** input_prev_weights ;
	//ָ����һ�������������֮���Ȩֵ��ָ��
	double** hidden_prev_weights;
	//ÿ��ѭ����ľ���������ֵ 
	double ex;
	
	//Ϊ�������ݽṹ�����ڴ�ռ�
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
	
	//Ϊ���������������
	time_t t; 
	bpnn_initialize((unsigned)time(&t));
	
	//�Ը���Ȩֵ���г�ʼ����ʼ��
	bpnn_randomize_weights( input_weights,n_in,n_hidden);
	bpnn_randomize_weights( hidden_weights,n_hidden,n_out);
	bpnn_zero_weights(input_prev_weights, n_in,n_hidden );
	bpnn_zero_weights(hidden_prev_weights,n_hidden,n_out );
	
	//��ʼ����BP����ѵ��
	//�����趨���ĵ�������Ϊ15000��
	//MaxDieDaiNum
	for(l=0;l<CurrentMaxDieDaiNum;l++)
	{
		//bpnn_initialize((unsigned)time(&t));
		//�Ծ����������
		ex=0;
		//���������������ɨ��
		for(k=0;k<num;k++)  
		{ 
			//����ȡ�������������������͵��������
			for(i=1;i<=n_in;i++)
			{
				input_unites[i] = data_in[k][i-1];
			}
			
			//��Ԥ��������������͵�BP��������������Ԫ
			for(i=1;i<=n_out;i++)
			{
				target[i]=data_out[k][i-1];
			}
			
			//ǰ���伤��
			//������������㴫������ 
			bpnn_layerforward(input_unites,hidden_unites,input_weights, n_in,n_hidden);
			//�������������������
			bpnn_layerforward(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
			//bpnn_layerforward_1(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
			
			//������
			//���������������������Ƚϼ��������ÿ������ϵ����
			bpnn_output_error(output_deltas,target,output_unites,n_out);
			//������������ϵ�����������ÿ���ڵ��ϵ����
			bpnn_hidden_error(hidden_deltas,n_hidden, output_deltas, n_out,hidden_weights, hidden_unites);
			
			//Ȩֵ����
			//���������ÿ���ڵ��ϵ���������������������֮���Ȩֵ    
			bpnn_adjust_weights(output_deltas,n_out, hidden_unites,n_hidden,hidden_weights, hidden_prev_weights, eta, momentum); 
			//��������ÿ���ڵ��ϵ���������������������֮���Ȩֵ    	
			bpnn_adjust_weights(hidden_deltas, n_hidden, input_unites, n_in,input_weights, input_prev_weights, eta, momentum);  
			
			//���ͳ��		
			for(i=1;i<=n_out;i++)
			{
				ex+=(output_unites[i]-data_out[k][i-1])*(output_unites[i]-data_out[k][i-1]);
			}
		}
		//����������
		ex=ex/double(num*n_out);
		printf("��%d�ε���,�������Ϊ:%lf\n",l,ex);
		//�����������Ѿ��㹻��С������ѭ����ѵ�����  
		if(ex<min_ex)
		{
			break;
		}
	}
	
	//��ر���
	//���������������֮���Ȩֵ
	w_weight(input_weights,n_in,n_hidden,"/mnt/sdcard/InToHidden.dat");
	//���������������֮���Ȩֵ
	w_weight(hidden_weights,n_hidden,n_out,"/mnt/sdcard/HiddenToOut.dat");
	//���������ĸ���
	w_num(n_in,n_hidden,n_out,"/mnt/sdcard/ThreeLevelNum.dat");
	
	//��ʾѵ�����
	if(ex<=min_ex)
	{
		printf("����%d��,ƽ�����%.4f\n",l,ex);
	}
	
	if(ex>min_ex)
	{
		printf("����%d��,ƽ�����%.4f\n�ﲻ��Ԥ��Ҫ��\n��Ҫ������������ѵ��",l,ex);
	}
	
	//�ͷ��ڴ�ռ�
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

//����ȫ�������Լ�
int GetTestSetData(double **testdata_in,int *testdata_trueclass,char *testdata_in_fname,char *testdata_out_cfname)
{   
	//ѭ������
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
	//ͼ���ļ��ѹ�һ��->���������ֽ�
	//�����ļ�
	fseek(fcf,8,SEEK_SET);

	for (i = 0; i < TESTINGSNUM; i++)   //ѵ����������������ĸ���
	{
		//RAWDATADIM->20*20��ȡ���������Լ�������
		for(j=0;j<20;j++)
		{
			for(k=0;k<20;k++)
			{
				fread(&CurrentColorValue, sizeof(unsigned char), 1, fdf);   //retrieve gray data
				testdata_in[i][j*20+k]=CurrentColorValue/(256.0);           //�ȳ���256.0
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
	for (i = 0; i < TESTINGSNUM; i++)   //ѵ����������������ĸ���
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

//num->��ʶ��������ĸ���
//��������:����������������������������ѵ�����õ�Ȩֵ,����ʶ�𣬽�ʶ��Ľ��д��BPRecogResult.txt 
int CodeRecognize(double *data_in,int n_in,int n_hidden,int n_out)
{
	//ѭ������
	int i;
	//ʶ����
	int recognize=-1;
	//ָ����������ݵ�ָ��
	double* input_unites; 
	//ָ���������ݵ�ָ��
	double* hidden_unites;
	//ָ����������ݵ�ָ��
	double* output_unites; 
	//ָ�������������֮��Ȩֵ��ָ��
	double** input_weights;
	//ָ�������������֮���Ȩֵ��ָ��
	double** hidden_weights;
	//Ϊ�������ݽṹ�����ڴ�ռ�
	input_unites= alloc_1d_dbl(n_in + 1);
	hidden_unites=alloc_1d_dbl(n_hidden + 1);
	output_unites=alloc_1d_dbl(n_out + 1);
	input_weights=alloc_2d_dbl(n_in + 1, n_hidden + 1);
	hidden_weights = alloc_2d_dbl(n_hidden + 1, n_out + 1);
	
	//��ȡȨֵ
	if( r_weight(input_weights,n_in,n_hidden,"/mnt/sdcard/InToHidden.dat")==false)
	{
		return FAIL;
	}
	if(r_weight(hidden_weights,n_hidden,n_out,"/mnt/sdcard/HiddenToOut.dat")==false)
	{
		return FAIL;
	}
	
	//����ȡ�������������������͵��������
	for(i=1;i<=n_in;i++)
	{
		input_unites[i]=data_in[i-1];
	}

	//ǰ�����뼤��
	bpnn_layerforward(input_unites,hidden_unites,input_weights, n_in,n_hidden);
	bpnn_layerforward(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
	//bpnn_layerforward_1(hidden_unites, output_unites,hidden_weights,n_hidden,n_out);
		
	//��������������ʶ��
	int result=0 ;
	//����ÿһλ�����
	for(i=1;i<=n_out;i++)
	{
		//�������0.5��Ϊ1
		if(output_unites[i]>0.5)  //0.5->0.9
		{
			//result+=(int)pow(2,double(4-i));
			result+=NearestInt(pow(2,double(4-i)));
		}
	}
		
	//����ж��Ľ��С�ڵ���9����Ϊ����
	if(result<=9)
	{
		recognize=result;
	}
	//����ж��Ľ������9����Ϊ�����������λΪһ������ֵ20
	if(result>9)
	{
		recognize=10;   //10->9
	}

	/*
	//��ʶ����д���ı���
	FILE *fp;
	fp=fopen("BPRecogResult.txt","w+");
	
	if(recognize==10)
	{
		fprintf(fp,"�޷�ʶ��!");
		printf("�޷�ʶ��!");
	}
	else
	{
		fprintf(fp,"\n%d ",recognize);
		printf("\n%d ",recognize);
	}
		
	fclose(fp);
	*/

	//�ͷŴ洢�ռ�
	free(input_unites);
	free(hidden_unites);
	free(output_unites);
	free(input_weights);
	free(hidden_weights);

	return recognize;
}

#define ALGO_DEBUG

/*
   �ú�˹�ɶ��£�Householder���任��n��ʵ�Գƾ���Լ��Ϊ�Գ����Խ���
   ��ʿ��. �����㷨���򼯣�C��������������3��. �廪��ѧ������. 2004

   double a[] --- ά��Ϊnxn�����n��ʵ�Գƾ���A
   int n      --- ʵ�Գƾ���A�Ľ���
   double q[] --- ά��nxn������Householder�任�ĳ˻�����Q������
                  Tri_Symmetry_Diagonal_Eigenvector()�������ã�����
				  Q������Ϊ�ú�����һ������ʱ���ɼ���ʵ�Գ���A������
				  ֵ����Ӧ����������
   double b[] --- ά��Ϊn�����ضԳ����Խ����е����Խ���Ԫ��
   double c[] --- ����n��ǰn��1��Ԫ�ط��ضԳ����Խ����еĴζԽ���Ԫ�ء�
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
   ����ʵ�Գ����Խ����ȫ������ֵ���Ӧ��������
   ��ʿ��. �����㷨���򼯣�C��������������3��. �廪��ѧ������. 2004

   int n    --- ʵ�Գ����Խ���Ľ���
   double b --- ����Ϊn�����n�׶Գ����Խ�������Խ����ϵĸ�Ԫ�أ�
                ����ʱ���ȫ������ֵ
   double c --- ����Ϊn��ǰn��1��Ԫ�ش��n�׶Գ����Խ���ĴζԽ�
                ���ϵ�Ԫ��
   double q --- ά��Ϊnxn������ŵ�λ�����򷵻�n��ʵ�Գ����Խ�
                ��T�����������飻�����Householder_Tri_Symetry_Diagonal()
				���������ص�һ��ʵ�Գƾ���A�ĺ�˹�ɶ��ñ任�ĳ�
				������Q���򷵻�ʵ�Գƾ���A�����������顣���У�q��
				�ĵ�j��Ϊ������b�е�j������ֵ��Ӧ����������
   double eps --- �������ڵ��������еĿ��ƾ���Ҫ��	   
   int l    --- Ϊ���һ������ֵ�����������������
   ����ֵ��
   �����ر��С��0����˵��������l�λ�δ���һ������ֵ������fail
   ��Ϣ����������ر�Ǵ���0����˵��������������ȫ������ֵ��һ
   ά����b�����������������ɶ�ά����q����
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

# define EPS          0.000001   /* ���㾫�� */
# define Iteration    60         /* ��ȡ������������������ */

/*
   ����ʵ�Գ����ȫ������ֵ���Ӧ��������
   int n              --- ʵ�Գ���Ľ���
   double * CovMatrix --- ά��Ϊnxn�����n�׶Գ���ĸ�Ԫ�أ�
   double * Eigen     --- ����Ϊn��Ϊn������ֵ
   double * EigenVector --- ά��Ϊnxn������n��ʵ�Գ�����������������У�
                            EigenVector�е�j��Ϊ������Eigen�е�j������ֵ
							��Ӧ����������
   ����ֵ��
   �����ر��С��0����˵��������Iteration�λ�δ���һ������ֵ��
   �����ر�Ǵ���0����˵��������������ȫ������ֵ��һ
   ά����Eigen�����������������ɶ�ά����EigenVector����
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
               ����Ϊԭʼ����ά����ÿ������Ϊһ������
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
  ͶӰ����Ԫ��������
  double * newdata --- ҪͶӰ����Ԫ�ط����ϵ����ݾ���
  int row, col     --- ���ݾ����ά��
                       row��������Ϊ��Ԫ����ά��������������
					   col��������Ϊ������
  double * PC      --- ��Ԫ������������������
  double * newsignals --- ͶӰ���õ��źŵ�ϵ����row*col��
  double * ShiftValue --- ���һ����row��Ԫ�ص�ƫ�����������ֵΪNULL
                          ������ֵ������ȥ��ֵ�������ȥ��ֵ
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

/*��һ���� 0 �� 1�Ϳ���*/

/*
��һ��PCA������->���Ƿ��������Ŀ����
��ͼ�����ݿ��ļ������ݶ������������飬������PCA
*/
int ReadImageDBToLieVector(double *ImageDataLieArray,char *data_in_fname)
{
	//TRAININGSNUM*RAWDATADIM
	//ѭ������
    int i,j;
	FILE *fdf;
	unsigned char CurrentImageData[RAWDATADIM];
	//open datafile
	if((fdf = fopen (data_in_fname, "rb")) == NULL)
	{
		printf("Can't open file %s\n", data_in_fname);
		return FAIL;
	}
	//ͼ���ļ��ѹ�һ��->���������ֽ�
	for (i = 0; i < TRAININGSNUM; i++)   //ѵ����������������ĸ���
	{
		//RAWDATADIM->20*20��ȡ���������Լ�������
		fread(CurrentImageData, sizeof(unsigned char), RAWDATADIM, fdf);  //retrieve gray data
		for(j=0;j<RAWDATADIM;j++)
		{
			//ImageDataLieArray[j*TRAININGSNUM+i]=CurrentImageData[j]+0.0;
			ImageDataLieArray[j*TRAININGSNUM+i]=CurrentImageData[j]/(256.0);   //�ȹ�һ��
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

/*��������ݿ��ļ�����ʵ������ݶ������������*/
int ReadClassDBToClassVector(double **data_out,char *data_out_cfname)
{   
	//ѭ������
    int i;
	FILE *fcf;
	unsigned char sclass;
	//open classfile
	if((fcf = fopen (data_out_cfname, "rb")) == NULL)
	{
		printf("Can't open file %s\n", data_out_cfname);
		return FAIL;
	}
	//ͼ���ļ��ѹ�һ��->���������ֽ�
	//�����ļ�
	fseek(fcf,8,SEEK_SET);
	for (i = 0; i < TRAININGSNUM; i++)   //ѵ����������������ĸ���
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

//������ά�ȵķ�����ȡ��FEADIM��������
void GetFeaturePCA(double *PC,double *FeaturePC)
{
	for(int i=0;i<RAWDATADIM;i++)    //RAWDATADIM��
	{
		for(int j=0;j<FEADIM;j++)
		{
			FeaturePC[i*FEADIM+j]=PC[i*RAWDATADIM+j];
		}
	}
}

//����ת�� 
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

//FeaturePC->RAWDATADIM*FEADIM  ��*��
//FeaturePCT->FEADIM*RAWDATADIM ����ת��
void GetFeaturePCAT(double *FeaturePC,double *FeaturePCT)
{
	MatrixInver(FeaturePC,FeaturePCT,RAWDATADIM,FEADIM);
}

//ImageDataLieArray->RAWDATADIM�� TRAININGSNUM��  MeanV->RAWDATADIM
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

//����������� ������A*B��C ->A[m,n],B[n,k],C[m,k]
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

//����ѵ��������ȥƽ��ֵ��ľ���
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

//����ѵ��������ȥƽ��ֵ��ľ���  TestData->RAWDATADIM������
void CalcTestDataMinusMeanV(double *TestData,double *TestDataMinusMeanV,double *MeanV)
{
	//subtract mean
	for(int i=0;i<RAWDATADIM;i++) 
	{
		TestDataMinusMeanV[i] = TestData[i]-MeanV[i];
	}
}

//����ѵ�������������������-> (56*400)*(400*TRAININGSNUM)=56*TRAININGSNUM  ->56�� TRAININGSNUM��
void CalcDataFeatureVector(double *FeaturePCT,double *ImageDataLieArrayMinusMeanV,double *DataFeatureVector)
{
	MatrixMul(FeaturePCT,ImageDataLieArrayMinusMeanV,FEADIM,RAWDATADIM,TRAININGSNUM,DataFeatureVector);
}

//������������������� FeaturePCT->56*400 TestDataMinusMeanV->1*400->400*1 56*1
void TestDataMinusMeanVToRecogFeatureVector(double *FeaturePCT,double *TestDataMinusMeanV,double *data_need_recogT)
{
	MatrixMul(FeaturePCT,TestDataMinusMeanV,FEADIM,RAWDATADIM,1,data_need_recogT);
}

//data_in->TRAININGSNUM��FEADIM�� DataFeatureVector->FEADIM*TRAININGSNUM
void EnterDataFeatureVectorToTrainInput(double **data_in,double *DataFeatureVector)
{
	for(int i=0;i<TRAININGSNUM;i++)   //ѵ����������������ĸ��� iΪ��
	{
		//RAWDATADIM->20*20��ȡ���������Լ�������! 1000��ѵ������Ӧ��Ҳ�㹻��.
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
			// ������ص���ɫ
			int color = cbuf[w * i + j];
			int red = ((color & 0x00FF0000) >> 16);  //�����㣬ȡredֵ������16λ��תΪ����ֵ
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
	
	//��ʼ������
	double BpPa_m_a=0;
	double BpPa_m_eta=ETANUM;   //0.015->0.1
	double BpPa_m_ex=EXNUM;
	int BpPa_m_hn=HNNUM;   //7 21~31
	//���ϵ��
	double momentum=BpPa_m_a; 
	//��С�������
	double min_ex=BpPa_m_ex; 
	//������Ŀ
	int n_hidden=BpPa_m_hn; 
	//ѵ������->ѧϰ����?
	double eta=BpPa_m_eta;
	int n_out=4;

	double *PC=(double *)malloc(sizeof(double)*RAWDATADIM*RAWDATADIM);         //400*400
	double *V=(double *)malloc(sizeof(double)*RAWDATADIM);

	double *MeanV=(double *)malloc(sizeof(double)*RAWDATADIM);   //400��1��
	double *FeaturePC=(double *)malloc(sizeof(double)*RAWDATADIM*FEADIM);      //���Ҫ�ǵ�free��
	double *FeaturePCT=(double *)malloc(sizeof(double)*FEADIM*RAWDATADIM); 

	double *TestDataMinusMeanV=(double *)malloc(sizeof(double)*RAWDATADIM);   //400��1��
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
				testdata_in[0][j*20+k]=Num1[j*20+k]/(256.0);           //�ȳ���256.0
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
	//��ȡ���� ����ȡ1200+100 ȫ�ֱ���!!!����ȫ�ֱ���!!!
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