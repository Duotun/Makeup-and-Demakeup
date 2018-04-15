#include"stdafx.h"
#include <opencv2/photo.hpp>

#include "opencv2/imgcodecs.hpp"

#include <opencv2/highgui.hpp>
#include<opencv2/core/core.hpp>

#include <vector>

#include <iostream>

#include <fstream>
#include<stdlib.h>
#include<stdio.h>
using namespace cv;
//bilateral value about the edge need to be carefully edited.
//de-makeup can be edited by inverse algorithm ad change some parameters and filter after compose with face parts.
using namespace std;

int n;
const double I1 = 0.0f;
const double E1 = 1.0f;
const double cp = 0.80f;
void grb2labandsplit(Mat rgb, Mat & lab, vector<Mat>&splitlab)   // RGB to LAB and split channels
{
	cvtColor(rgb, lab, COLOR_BGR2Lab);
	split(lab, splitlab);
	//tmp for original
	//cvtColor(rgb, rgb, COLOR_BGR2GRAY);
	//imshow("L", splitlab[0]);
}
void lab2grb(Mat &result)
{
	cvtColor(result, result, COLOR_Lab2BGR);
}
void bilateral_filter(Mat l, Mat &result)
{
	bilateralFilter(l, result, 9, 60.0f, 60.0f);
	imshow("filter", result);
}
void bilateral_filter_original(Mat inputimg, Mat &outputimg, double sigmas, double sigmar, int length)
{

	int i, j, x, y;
	int radius = (int)length / 2; // length is the diameter of filter   
	int img_width = inputimg.rows;	//sigmas is for colorspace // sigmar is for space(further pixels) 
	int img_height = inputimg.cols;    // sigmar is for space(further pixels) 
	vector<double>mask(length*length);  //define mask
										//compute sigmas
	for (i = 0; i < length; i++)
	{
		for (j = 0; j < length; j++)
		{
			mask[i*length + j] = exp(-(i*i + j*j) / (2 * sigmas*sigmas));
		}
	}
	double sum = 0.0f, k = 0.0f;
	printf("%d\n", inputimg.at<uchar>(0, 0));
	//compute spatial weight
	for (x = 0; x < img_width; x++)
	{
		unsigned char *pin = inputimg.ptr<uchar>(x);// һ���˲����Ǵ���Ҷ�ͼ���ҵ�ʵ���൱��8u ����Ϊ֮ǰ�ȷָ�������û����
		unsigned char *pout = outputimg.ptr<uchar>(x);   //edit output
		for (y = 0; y < img_height; y++)
		{
			int centerpix = y;
			for (i = -radius; i <= radius; i++)
			{
				for (j = -radius; j <= radius; j++)
				{
					int m = x + i, n = y + j;
					if (m > -1 && n> -1 && m < img_width &&n < img_height)
					{
						uchar value = inputimg.at<uchar>(m, n);   //spatial computing
						double eukliddiff = mask[(i + radius)*length + (j + radius)];
						double intens = pin[centerpix] - value;  // mod  + - both ok
						double factor = (double)exp(-0.5*intens / (2 * sigmar*sigmar))*eukliddiff;
						sum += factor*value;
						k += factor;
					}
				}
			}
			if (k != 0.0f)
			{
				pout[y] = sum / k;
			}
			sum = 0.0f;
			k = 0.0f;    // return initial value
		}
	}
}
void copyforab(vector<Mat>split, Mat &a, Mat &b)
{
	a = split.at(1);
	b = split.at(2);
}
void getdetail(Mat l, Mat bfi, Mat &result)
{
	//result = l - bfi;
	addWeighted(l, 1.0f, bfi, -1.0f, 0.0f, result);

	//imshow("structure", l);
}
void calcurd(Mat I, Mat R, Mat &result)
{
	addWeighted(I, I1, R, E1, 0.0f, result);
	imshow("11", result);
}
void calcurs(Mat I, Mat &result)
{
	result = I;
}
void calcura(Mat I, Mat R, Mat &result)
{
	addWeighted(I, (1 - cp), R, cp, 0.0f, result);
}
void calcurb(Mat I, Mat R, Mat &result)
{
	addWeighted(I, (1 - cp), R, cp, 0.0f, result);
}
void compose(Mat R1, Mat R2, Mat R3, Mat R4, Mat &Result, Mat *ptr)
{
	Mat tmp;
	add(R1, R2, tmp);
	ptr[0] = tmp;
	ptr[1] = R3;
	//imshow("r3", R3);
	ptr[2] = R4;
	//imshow("r4", R4);
	merge(ptr, 3, Result);
}
void regioninverse(Mat up, Mat division, Mat &result, Mat original)   //special for input1 upward 10
{
	for (int i = 0; i < division.rows; i++)
	{
		for (int j = 0; j < division.cols; j++)
		{
			if (division.at<uchar>(i, j) != 0)
			{
				result.at<Vec3b>(i, j)[0] = up.at<Vec3b>(i, j)[0];
				result.at<Vec3b>(i, j)[1] = up.at<Vec3b>(i, j)[1];
				result.at<Vec3b>(i, j)[2] = up.at<Vec3b>(i, j)[2];
				//result.at<Vec3b>(i, j)[0] = original.at<Vec3b>(i, j)[0];
				//result.at<Vec3b>(i, j)[1] = original.at<Vec3b>(i, j)[1];
				//result.at<Vec3b>(i, j)[2] = original.at<Vec3b>(i, j)[2];
			}
			else
			{
				result.at<Vec3b>(i, j)[0] = original.at<Vec3b>(i, j)[0];
				result.at<Vec3b>(i, j)[1] = original.at<Vec3b>(i, j)[1];
				result.at<Vec3b>(i, j)[2] = original.at<Vec3b>(i, j)[2];

			}

		}
	}
}
void revise(Mat reference, Mat & result)
{
	for (int i = 0; i < reference.rows; i++)
	{
		for (int j = 0; j < reference.cols; i++)
		{
			if (result.at<Vec3b>(i, j)[0] == 0 && result.at<Vec3b>(i, j)[1] == 0 && result.at<Vec3b>(i, j)[2] == 0)
			{
				result.at<Vec3b>(i, j)[0] = reference.at<Vec3b>(i, j)[0];
				result.at<Vec3b>(i, j)[1] = reference.at<Vec3b>(i, j)[1];
				result.at<Vec3b>(i, j)[2] = reference.at<Vec3b>(i, j)[2];
			}
		}
	}
}
void move(Mat &result)
{
	for (int i = 0; i < result.rows - 5; i++)
	{
		for (int j = 0; j < result.cols; j++)
		{
			result.at<Vec3b>(i, j)[0] = result.at<Vec3b>(i + 5, j)[0];
			result.at<Vec3b>(i, j)[1] = result.at<Vec3b>(i + 5, j)[1];
			result.at<Vec3b>(i, j)[2] = result.at<Vec3b>(i + 5, j)[2];
		}
	}
}
void gaussianfiltertmp(Mat & result)
{
	GaussianBlur(result, result, Size(3, 3), 3, 3);
}
void warp(Mat original, Mat &forwarp, Mat tp)
{
	imshow("rr", forwarp);
	for (int i = 0; i < forwarp.rows; i++)
	{
		for (int j = 0; j < forwarp.cols; j++)
		{
			if (tp.at<uchar>(i, j) != 0)
			{
				forwarp.at<Vec3b>(i, j)[0] = 1;
				forwarp.at<Vec3b>(i, j)[1] = 1;
				forwarp.at<Vec3b>(i, j)[2] = 1;
			}
			else
			{
				forwarp.at<Vec3b>(i, j)[0] = 0;
				forwarp.at<Vec3b>(i, j)[1] = 0;
				forwarp.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}
	imshow("after", forwarp);
	forwarp.mul(original);
	imshow("aftee", original);
}


void regionforeyes(Mat up, Mat division, Mat &result, Mat original)
{
	for (int i = 0; i < division.rows; i++)
	{
		for (int j = 0; j < division.cols; j++)
		{
			if (division.at<uchar>(i, j) != 0)
			{
				result.at<Vec3b>(i - 6, j)[0] = up.at<Vec3b>(i - 6, j)[0];
				result.at<Vec3b>(i - 6, j)[1] = up.at<Vec3b>(i - 6, j)[1];
				result.at<Vec3b>(i - 6, j)[2] = up.at<Vec3b>(i - 6, j)[2];
				result.at<Vec3b>(i, j)[0] = original.at<Vec3b>(i, j)[0];
				result.at<Vec3b>(i, j)[1] = original.at<Vec3b>(i, j)[1];
				result.at<Vec3b>(i, j)[2] = original.at<Vec3b>(i, j)[2];

			}
			else
			{
				result.at<Vec3b>(i, j)[0] = original.at<Vec3b>(i, j)[0];
				result.at<Vec3b>(i, j)[1] = original.at<Vec3b>(i, j)[1];
				result.at<Vec3b>(i, j)[2] = original.at<Vec3b>(i, j)[2];

			}

		}
	}
}
void shape(Mat * reference, Mat & result)
{
	for (int i = 0; i < n; i++)
	{
		result += reference[i];
	}

}
void regioncut(Mat up, Mat division, Mat &result, Mat original)   //special for input1 upward 10
{
	for (int i = 0; i < division.rows; i++)
	{
		for (int j = 0; j < division.cols; j++)
		{
			if (division.at<uchar>(i, j) == 0)
			{
				result.at<Vec3b>(i, j)[0] = up.at<Vec3b>(i, j)[0];
				result.at<Vec3b>(i, j)[1] = up.at<Vec3b>(i, j)[1];
				result.at<Vec3b>(i, j)[2] = up.at<Vec3b>(i, j)[2];

			}
			/*else
			{
			result.at<Vec3b>(i, j)[0] = original.at<Vec3b>(i, j)[0];
			result.at<Vec3b>(i, j)[1] = original.at<Vec3b>(i, j)[1];
			result.at<Vec3b>(i, j)[2] = original.at<Vec3b>(i, j)[2];

			} */

		}
	}
}
uchar * getaverage(Mat original)
{
	uchar pp[3];
	int tp1 = 0;
	int tp2 = 0;
	int tp3 = 0;
	double tl1 = 0.0f;
	double tl2 = 0.0f;
	double tl3 = 0.0f;
	for (int i = 0; i < original.rows; i++)
	{
		for (int j = 0; j < original.cols; j++)
		{
			if (original.at<Vec3b>(i, j)[0] != 0 && original.at<Vec3b>(i, j)[1] != 0 && original.at<Vec3b>(i, j)[2] != 0)
			{
				tp1++;
				tp2++;
				tp3++;
				tl1 += original.at<Vec3b>(i, j)[0];
				tl2 += original.at<Vec3b>(i, j)[1];
				tl3 += original.at<Vec3b>(i, j)[2];
				printf("%d %d %d\n", original.at<Vec3b>(i, j)[0], original.at<Vec3b>(i, j)[1], original.at<Vec3b>(i, j)[2]);
			}
		}
	}
	tp1 = (int)(tl1 / tp1);
	tp2 = (int)(tl2 / tp2);
	tp3 = (int)(tl3 / tp3);
	pp[0] = tp1;
	pp[1] = tp2;
	pp[2] = tp3;
	return pp;
}
void mergemap(Mat &result, uchar *pp)
{
	for (int i = 0; i < result.rows; i++)
	{
		for (int j = 0; j < result.cols; j++)
		{
			if (result.at<Vec3b>(i, j)[0] == 255 && result.at<Vec3b>(i, j)[1] == 255 && result.at<Vec3b>(i, j)[2] == 255)
			{
				result.at<Vec3b>(i, j)[0] = pp[0];
				result.at<Vec3b>(i, j)[0] = pp[1];
				result.at<Vec3b>(i, j)[0] = pp[2];
			}
		}
	}
}
void addimg(Mat &result, Mat reference)
{
	for (int i = 0; i < reference.rows; i++)
	{
		for (int j = 0; j < reference.cols; j++)
		{
			if (reference.at<Vec3b>(i, j)[0] != 0 && reference.at<Vec3b>(i, j)[1] != 0 && reference.at<Vec3b>(i, j)[2] != 0)
			{
				result.at<Vec3b>(i, j)[0] = reference.at<Vec3b>(i, j)[0];
				result.at<Vec3b>(i, j)[1] = reference.at<Vec3b>(i, j)[1];
				result.at<Vec3b>(i, j)[2] = reference.at<Vec3b>(i, j)[2];

			}
		}
	}
}
int main(int argc, char **argv)
{
	cout << "Please input how many parts you will make up for." << endl;
	cin >> n;
	Mat finalresult;
	Mat image_original;
	Mat image_makeup;
	Mat image_part;
	Mat image_warp;
	char path[200] = { 0 };
	cout << "Please input the original image." << endl;
	cin >> path;
	cout << path << endl;
	image_original = imread(path, 1);
	finalresult = image_original.clone();
	cout << "Please input the make-up image." << endl;
	cin >> path;
	cout << path << endl;
	image_makeup = imread(path, 1);
	Mat results = image_makeup.clone();
	image_warp = image_makeup.clone();
	IplImage *ColorImage;
	Mat original_lab;
	vector<Mat> o_lab(3);
	Mat makeup_lab;
	vector<Mat>m_lab(3);
	Mat mapping[10];
	Mat ptrtmp[3];
	Mat Is;
	Mat Id;
	Mat Ia;
	Mat Ib;
	Mat Es;
	Mat Ed;
	Mat Ea;
	Mat Eb;
	Mat Rd;
	Mat Rs;
	Mat Ra;
	Mat Rb;
	for (int i = 0; i < results.rows; i++)
	{
		for (int j = 0; j < results.cols; j++)
			results.at<Vec3b>(i, j) = (0, 0, 0);
	}
	for (int i = 0; i < n; i++)
	{
		cout << "Please input part of images for making up" << endl;
		cin >> path;
		cout << path << endl;
		mapping[i] = imread(path, 1);
		//printf("%d\n%d\n", image_part.rows, image_part.cols);
		//printf("%d\n", image_part.channels());
		//imshow("ss", image_part);
		cvtColor(mapping[i], image_part, CV_BGR2GRAY);
	}
	for (int i = 0; i < results.rows; i++)
	{
		for (int j = 0; j < results.cols; j++)
			results.at<Vec3b>(i, j) = (0, 0, 0);
	}
	shape(mapping, results);

	//warp(image_makeup, results);
	//cout << "Please input the numbers of pitures: ";
	//cin >> n;
	/*
	path =E:/�����ϸ�������/ͼ������-imageprocess/p2/input1/E.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input1/I.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input1/eyebrows.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input1/eyes.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input1/face.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input1/lips.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input1/noseE.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/E.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/I.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/eyebrows.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/eyes.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/lips.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/nose.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/tmplips.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input2/tmpeyebrows.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/E5.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/E9.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/eye5.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/eye9.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/eyebrow9.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/eyebrow5.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/lip5.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/lip9.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/I.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/I2.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/input3/1/eye99.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/detect.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/E6.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/keep.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/oo.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/tmpeye.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/tmpe.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/tmp.bmp
	=E:/�����ϸ�������/ͼ������-imageprocess/p2/demakeup/tmpr.bmp
	*/
	uchar *tpu;
	bilateralFilter(mapping[0], mapping[0], 9, 50.0f, 50.0f);
	imshow("after", mapping[0]);
	//addimg(mapping[0], mapping[1]);
	//imshow("after", mapping[0]);
	//tpu=getaverage(mapping[0]);
	//tpu[0] = 150;
	//tpu[1] = 170;
	//tpu[2] = 208;
	//mergemap(mapping[1], tpu);
	//imshow("after", mapping[1]);
	//printf("%d\n", tpu[0]);
	//printf("%d\n", tpu[1]);
	//printf("%d\n", tpu[2]);
	//printf("%d\n%d\n", image_part.rows, image_part.cols);
	//printf("%d\n", image_part.channels());
	//imshow("ss", image_part);
	//imshow("ss1", image_part);
	//printf("%d\n", image_part.channels());
	//cvtColor(mapping[0], image_part, CV_BGR2GRAY);
	//regioncut(image_makeup, image_part, mapping[0], image_original);
	//regioninverse(image_makeup, image_part, results, image_original);
	//revise(image_original, results);
	//regionforeyes(image_makeup, image_part, results, image_original);
	imshow("division", mapping[0]);
	imwrite("tmp.bmp", mapping[0]);
	//move(results);
	//gaussianfiltertmp(results);
	//printf("%d\n%d\n%d\n%d\n", image_original.rows, image_original.cols, image_makeup.rows, image_makeup.cols);
	//move(image_makeup);
	/*grb2labandsplit(image_original, original_lab, o_lab);
	copyforab(o_lab, Ia, Ib);
	bilateral_filter(o_lab[0], Is);
	imshow("Is", Is);
	getdetail(o_lab[0], Is, Id);
	addWeighted(Id, 1.0f, Id, 1.0f, 0.0f, Id);
	imshow("Id", Id);
	grb2labandsplit(results, makeup_lab, m_lab);
	copyforab(m_lab, Ea, Eb);
	bilateral_filter(m_lab[0], Es);
	imshow("Es", Es);
	getdetail(m_lab[0], Es, Ed);
	addWeighted(Ed, 1.0f, Ed, 1.0f, 0.0f, Ed);
	imshow("Ed", Ed);
	calcurd(Id, Ed, Rd);
	calcurs(Is, Rs);
	calcura(Ia, Ea, Ra);
	calcurb(Ib, Eb, Rb);
	// GaussianBlur(Ra,Ra,Size(29,29),31,31);    // filter for image1
	//GaussianBlur(Rb,Rb,Size(29,29),31,31);    //
	GaussianBlur(Ra, Ra, Size(19, 19), 21, 21);    //  and justin bieber
	GaussianBlur(Rb, Rb, Size(19, 19), 21, 21);    // justin bieber
	//merge(m_lab, results);
	//imshow("makeup", results);
	compose(Rd, Rs, Ra, Rb, results, ptrtmp);
	lab2grb(results);
	image_original = results;
	//GaussianBlur(image_original, image_original, Size(5, 5), 4, 4);
	imshow("final", image_original);
	imwrite("tmpeyes.bmp", image_original);
	//imshow("structure_filter", Is);
	//imshow("filter", Is);
	//imshow("L", o_lab[0]);
	//imagerela[i] = image[i].clone();
	//if (!image[i].empty())
	//imshow("original", image_original);
	//imshow("makeup", image_makeup);

	// n--;
	//lab2grb(finalresults);
	//imshow("final", finalresults);
	*/
	waitKey(0); // Wait for a keystroke in the window
	return 0;

}