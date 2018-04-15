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
	warp(image_makeup, results);
	//cout << "Please input the numbers of pitures: ";
	//cin >> n;
	/*
	path =E:/大三上各种数据/图像处理技术-imageprocess/p2/input1/E.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input1/I.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input1/eyebrows.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input1/eyes.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input1/face.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input1/lips.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input1/noseE.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/E.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/I.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/eyebrows.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/eyes.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/lips.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/nose.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/tmplips.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input2/tmpeyebrows.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/E5.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/E9.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/eye5.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/eye9.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/eyebrow9.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/eyebrow5.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/lip5.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/lip9.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/I.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/I2.bmp
	=E:/大三上各种数据/图像处理技术-imageprocess/p2/input3/1/eye99.bmp

	*/
	image_part = imread(path, 1);
	//printf("%d\n%d\n", image_part.rows, image_part.cols);
	//printf("%d\n", image_part.channels());
	//imshow("ss", image_part);
	//imshow("ss1", image_part);
	//printf("%d\n", image_part.channels());
	regioninverse(image_makeup, image_part, results, image_original);
	//revise(image_original, results);
	//regionforeyes(image_makeup, image_part, results, image_original);
	imshow("division", results);
	//move(results);
	//gaussianfiltertmp(results);
	//printf("%d\n%d\n%d\n%d\n", image_original.rows, image_original.cols, image_makeup.rows, image_makeup.cols);
	//move(image_makeup);
	grb2labandsplit(image_original, original_lab, o_lab);
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
	waitKey(0); // Wait for a keystroke in the window
	return 0;

}