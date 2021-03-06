/** @file
 *****************************************************************************
 Test program that exercises the ppzkSNARK (first generator, then
 prover, then verifier) on a synthetic R1CS instance.

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/
#include <cassert>
#include <cstdio>

#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>

#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/examples/run_snark_for_filtering.hpp>

#include <algorithm>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <cmath>
// #include <omp.h>

using namespace libsnark;
using namespace cv;

bool mouse_is_pressing = false;
Mat original_array =  cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/uhd.jpg",IMREAD_COLOR);
Mat u1_array;
Mat u2_array = original_array.clone();
int stride=1;

struct Image_ROI{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
};

template<typename ppT>
void mouse_callback(int event, int x, int y, int flags, void *pt)
{
	Mat img_result = original_array.clone();
    Image_ROI* ROI = (Image_ROI *) pt;

	if (event == EVENT_LBUTTONDOWN){

		mouse_is_pressing = true;
		ROI->start_x = x;
		ROI->start_y = y;

		circle(img_result, Point(x, y), 10, Scalar(0, 255, 0), -1);
		imshow("original", img_result);

	} else if (event == EVENT_MOUSEMOVE){

		if (mouse_is_pressing){
			
			rectangle(img_result, Point(ROI->start_x, ROI->start_y), Point(x, y),
				Scalar(0, 255, 0), 1);

			imshow("original", img_result);
            ROI->end_x = x;
            ROI->end_y = y;
		}

	} else if (event == EVENT_LBUTTONUP){

		mouse_is_pressing = false;
        if(ROI->start_x > ROI->end_x) std::swap(ROI->start_x, ROI->end_x);
        if(ROI->start_y > ROI->end_y) std::swap(ROI->start_y, ROI->end_y);
        // ROI->start_x = ROI->start_x - ROI->start_x%stride;
        // ROI->start_y = ROI->start_y - ROI->start_y%stride;
        // ROI->end_x = ((ROI->end_x+stride-1)/stride)*stride;
        // ROI->end_y = ((ROI->end_y+stride-1)/stride)*stride;
        ROI->start_x = 100;
        ROI->start_y = 100;
        ROI->end_x = 300;
        ROI->end_y = 300;

        for (int i=ROI->start_y; i<ROI->end_y; i++){
            uchar* ptr = u2_array.ptr<uchar>(i);
            for (int j=ROI->start_x; j<ROI->end_x; j++){
                ptr[j*3] = 0;
                ptr[j*3+1] = 0;
                ptr[j*3+2] = 0;
            }
        }
        bitwise_xor(original_array, u2_array, u1_array);
	    // imshow("original", original_array);
	    // imshow("u2", u2_array);
        // imshow("u1", u1_array);
	}
}

template<typename ppT>
void test_snark_for_filtering()
{   
    libff::print_header("(enter) Test Snark for Filtering");
    libff::enter_block("Set the Images");
    Image_ROI pt;
    Mat img_show = original_array.clone();

    std::vector<libff::Fr<ppT>> u1;
    std::vector<libff::Fr<ppT>> u2;
    std::vector<libff::Fr<ppT>> original;
    std::cout<<"stride? ";
    std::cin>>stride;
    
    if (stride >1){
        for (int i=1; i<=original_array.rows/stride; i++){
            line(img_show, Point(0, i*stride), Point(original_array.cols, i*stride), Scalar(0, 0, 0), 1);
            for (int j=1; j<=original_array.cols/stride; j++){
                line(img_show, Point(j*stride, 0), Point(j*stride, original_array.rows), Scalar(0, 0, 0), 1);

            }
        }
    }

    imshow("original", original_array);
    imshow("stride", img_show);
	setMouseCallback("original", mouse_callback<ppT>, (void *)&pt);
    cv::waitKey(0);

    int stride_rows = (int)ceil((double)original_array.rows/stride);
    int stride_cols = (int)ceil((double)original_array.cols/stride);
    Mat resize_original_array = Mat::zeros(stride_rows*stride, stride_cols*stride, CV_8UC3);
    for(int i=0; i<original_array.rows; i++){
        for(int j=0; j<original_array.cols; j++){
            resize_original_array.at<Vec3b>(i,j)[0] = original_array.at<Vec3b>(i,j)[0];
            resize_original_array.at<Vec3b>(i,j)[1] = original_array.at<Vec3b>(i,j)[1];
            resize_original_array.at<Vec3b>(i,j)[2] = original_array.at<Vec3b>(i,j)[2];
        }
    }
    imshow("resize_original_array", resize_original_array);
    cv::waitKey(0);
	destroyAllWindows();

    libff::leave_block("Set the Images");

    libff::enter_block("Compute SHA256");

    for (int i=0; i<stride_rows; i++){
        for (int j=0; j<stride_cols; j++){
            Mat temp = resize_original_array(Rect(j*stride, i*stride, stride, stride));
            unsigned char digest[SHA256_DIGEST_LENGTH];
            // libff::enter_block("SHA256");
            SHA256_CTX context;
            SHA256_Init (&context);
            SHA256_Update (&context, (unsigned char*)&temp.data, stride*stride);
            SHA256_Final (digest, &context);
            free(digest);
            temp.release();
            // SHA256((unsigned char*)&temp.data, stride*stride, (unsigned char*)&digest);
            // libff::leave_block("SHA256");
            // libff::enter_block("Copy SHA256");

            libff::Fr<ppT> sha_value = libff::Fr<ppT>(context.h[0] * 4294967296);
            sha_value += context.h[1];
            sha_value *= 4294967296;
            sha_value += context.h[2];
            sha_value *= 4294967296;
            sha_value += context.h[3];
            sha_value *= 4294967296;
            sha_value += context.h[4];
            sha_value *= 4294967296;
            sha_value += context.h[5];
            sha_value *= 4294967296;
            sha_value += context.h[6];
            sha_value *= 4294967296;
            sha_value += context.h[7];

            // libff::Fr<ppT> sha_value;      
            // for (int k=0; k<SHA256_DIGEST_LENGTH; k++){
            //     libff::Fr<ppT> tmp = sha_value * 256;
            //     sha_value = tmp + digest[k];
            // }
            // libff::leave_block("Copy SHA256");
            if(pt.start_y/stride<= i && i < pt.end_y/stride && pt.start_x/stride <= j && j < pt.end_x/stride){
                u1.push_back(sha_value);
                u2.push_back(libff::Fr<ppT>::zero());
            }
            else{
                u1.push_back(libff::Fr<ppT>::zero());
                u2.push_back(sha_value);
            }
            original.push_back(sha_value);
        }
    }

    libff::leave_block("Compute SHA256");


    // size_t img_size = original_array.rows * original_array.cols * 3;

    // for(size_t i=0;i<img_size;i++){
    //     u1.push_back(libff::Fr<ppT>(u1_array.data[i]));
    
    //     u2.push_back(libff::Fr<ppT>(u2_array.data[i]));
    
    //     original.push_back(libff::Fr<ppT>(original_array.data[i]));
    // }
    
    const bool test_serialization = true;
    r1cs_example<libff::Fr<ppT> > example = generate_r1cs_filtering_example<libff::Fr<ppT> >(u1, u2);
    const bool bit = run_snark_for_filtering<ppT>(example, original, test_serialization);
    assert(bit);
    libff::print_header("(leave) Test Snark for Filtering");
}

int main()
{
    default_r1cs_gg_ppzksnark_pp::init_public_params();
    libff::start_profiling();
    
    test_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>();

}

