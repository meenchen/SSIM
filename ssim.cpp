#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
//#include <opencv2/legacy/legacy.hpp>
#include <cmath>

using std::cout;
using std::endl;
using namespace cv;

Scalar getMSSIM( const Mat&, const Mat&);
int main(int argc, char** argv){
    int prefix=0,postfix=0;

    int ends = atoi(argv[2]);//e.g. 13
    int endms = atoi(argv[3]);//e.g. 123
    int sec=0,msec=0;
    
    Mat ref,comp;
    char path[100];
    bool readfirst = true;
	int framecount = 0;

    while(ends > sec || (ends == sec && endms >= msec)){
		sprintf(path, "%s_%06d.%03d.png", argv[1], sec, msec);
		msec++;
		if(msec == 1000) {sec++; msec=0;}
		comp = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
		if(comp.data == NULL)
		    continue;
		//data exist
		framecount++;
        if(readfirst){
			ref = comp;
			readfirst = false;
			continue;
		}
		Scalar ssimVal;
	    ssimVal=getMSSIM(ref,comp);
		cout << "frame:" << framecount << "," << ssimVal.val[0] << "\n";
		ref = comp.clone();
		//printf("%s\n",path);
    }

	return 0;
}



Scalar getMSSIM( const Mat& i1, const Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    Mat mu1, mu2;   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map

    return mssim;
}

