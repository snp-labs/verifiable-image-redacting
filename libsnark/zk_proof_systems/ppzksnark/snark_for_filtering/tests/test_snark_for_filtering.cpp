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
#ifdef MULTICORE
#include <omp.h>    //병렬 프로그램
#endif

using namespace libsnark;
using namespace cv;

bool mouse_is_pressing = false;
Mat original_array;
Mat u1_array;
Mat u2_array;
int stride=1;
int end_x_ = -1, end_y_ = -1;
int ratio = 1;

struct Image_ROI{
    vector<int> start_x;
    vector<int> start_y;
    vector<int> end_x;
    vector<int> end_y;
};

template<typename ppT>
void mouse_callback(int event, int x, int y, int flags, void *pt)
{
	Mat img_result = original_array.clone();
    Image_ROI* ROI = (Image_ROI *) pt;

	if (event == EVENT_LBUTTONDOWN){

		mouse_is_pressing = true;
		ROI->start_x.push_back(x);
		ROI->start_y.push_back(y);

		circle(img_result, Point(x, y), 10, Scalar(0, 255, 0), -1);
		imshow("original", img_result);

	} else if (event == EVENT_MOUSEMOVE){

		if (mouse_is_pressing){
			
			rectangle(img_result, Point(ROI->start_x.back(), ROI->start_y.back()), Point(x, y),
				Scalar(0, 255, 0), 1);

			imshow("original", img_result);
            end_x_ = x;
            end_y_ = y;
		}

	} else if (event == EVENT_LBUTTONUP){
		mouse_is_pressing = false;
        if(ROI->start_x.back() > end_x_){
            ROI->end_x.push_back(ROI->start_x.back());
            ROI->start_x.back() = end_x_;
        }
        else ROI->end_x.push_back(end_x_);

        if(ROI->start_y.back() > end_y_){
            ROI->end_y.push_back(ROI->start_y.back());
            ROI->start_y.back() = end_y_;
        }
        else ROI->end_y.push_back(end_y_);

        // /*TEST*/
        // ROI->start_x.back() = 100*ratio;
        // ROI->start_y.back() = 100*ratio;
        // ROI->end_x.back() = 300*ratio;
        // ROI->end_y.back() = 300*ratio;

        ROI->start_x.back() = ROI->start_x.back() - ROI->start_x.back()%stride;
        ROI->start_y.back() = ROI->start_y.back() - ROI->start_y.back()%stride;
        ROI->end_x.back() = ((ROI->end_x.back()+stride-1)/stride)*stride;
        ROI->end_y.back() = ((ROI->end_y.back()+stride-1)/stride)*stride;

        for (int i=ROI->start_y.back(); i<ROI->end_y.back(); i++){
            uchar* ptr = u2_array.ptr<uchar>(i);
            for (int j=ROI->start_x.back(); j<ROI->end_x.back(); j++){
                ptr[j*3] = 0;
                ptr[j*3+1] = 0;
                ptr[j*3+2] = 0;
            }
        }
        bitwise_xor(original_array, u2_array, u1_array);
	    imshow("original", original_array);
	    imshow("u2", u2_array);
        imshow("u1", u1_array);
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
    // std::cout<<"stride? ";
    // std::cin>>stride;
    
    // if (stride >1){
    //     for (int i=1; i<=original_array.rows/stride; i++){
    //         line(img_show, Point(0, i*stride), Point(original_array.cols, i*stride), Scalar(0, 0, 0), 1);
    //         for (int j=1; j<=original_array.cols/stride; j++){
    //             line(img_show, Point(j*stride, 0), Point(j*stride, original_array.rows), Scalar(0, 0, 0), 1);

    //         }
    //     }
    // }
    // imshow("original", original_array);
    // imshow("stride", img_show);
    // cv::waitKey(0);

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
    original_array = resize_original_array.clone();
    u2_array = original_array.clone();
    // imshow("original", original_array);
	// setMouseCallback("original", mouse_callback<ppT>, (void *)&pt);
    // cv::waitKey(0);
	// destroyAllWindows();
    /*TEST*/
        pt.start_x.push_back(100*ratio);
        pt.start_y.push_back(100*ratio);
        pt.end_x.push_back(300*ratio);
        pt.end_y.push_back(300*ratio);
        pt.start_x[0] = pt.start_x[0] - pt.start_x[0]%stride;
        pt.start_y[0] = pt.start_y[0] - pt.start_y[0]%stride;
        pt.end_x[0] = ((pt.end_x[0]+stride-1)/stride)*stride;
        pt.end_y[0] = ((pt.end_y[0]+stride-1)/stride)*stride;
        for (int i=pt.start_y[0]; i<pt.end_y[0]; i++){
            uchar* ptr = u2_array.ptr<uchar>(i);
            for (int j=pt.start_x[0]; j<pt.end_x[0]; j++){
                ptr[j*3] = 0;
                ptr[j*3+1] = 0;
                ptr[j*3+2] = 0;
            }
        }
        bitwise_xor(original_array, u2_array, u1_array);

    libff::leave_block("Set the Images");

    libff::enter_block("Compute SHA256");
    u1.resize(stride_rows*stride_cols);
    u2.resize(stride_rows*stride_cols);
    original.resize(stride_rows*stride_cols);

#ifdef MULTICORE
    #pragma omp parallel for
#endif    
    for (int i=0; i<stride_rows; i++){
        for (int j=0; j<stride_cols; j++){
            Mat temp = resize_original_array(Rect(j*stride, i*stride, stride, stride));
            bool opening = false;
            unsigned char digest[SHA256_DIGEST_LENGTH] = {};
            SHA256_CTX context;
            SHA256_Init (&context);
            SHA256_Update (&context, (unsigned char*)&temp.data, stride*stride);
            SHA256_Final (digest, &context);
            // free(digest);
            temp.release();

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
            for (int k=0; k<pt.start_x.size(); k++){
                if(pt.start_y[k]/stride<= i && i < pt.end_y[k]/stride && pt.start_x[k]/stride <= j && j < pt.end_x[k]/stride){
                    opening = true;
                    break;
                }
            }

            if(opening){
                u1[stride_cols*i+j] = sha_value;
                u2[stride_cols*i+j] = libff::Fr<ppT>::zero();
            }
            else{
                u1[stride_cols*i+j] = libff::Fr<ppT>::zero();
                u2[stride_cols*i+j] = sha_value;
            }
            original[stride_cols*i+j] = sha_value;
        }
    }
    libff::leave_block("Compute SHA256");

    // printf("u1:\n");
    // for(int i=0; i<u1.size();i++){
    //     u1[i].print();
    // }
    // printf("u2:\n");
    // for(int i=0; i<u2.size();i++){
    //     u2[i].print();
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
    stride = 64;
    original_array =  cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/360p.jpg",IMREAD_COLOR);
    // for(int i=0;i<1;i++){
        printf("360p stride: %d",stride);
        test_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>();
    //     stride *= 2;
    // }
    // stride = 1;
    // ratio = 2;
    original_array =  cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/hd.jpg",IMREAD_COLOR);
    // for(int i=0;i<7;i++){
        printf("hd stride: %d",stride);
        test_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>();
        // stride *= 2;
    // }
    // stride = 4;
    // ratio = 3;
    original_array =  cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/fhd.jpg",IMREAD_COLOR);
    // for(int i=0;i<3;i++){
        printf("fhd stride: %d",stride);
        test_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>();
    //     stride *= 4;
    // }
    // stride = 1;
    // ratio = 4;
    original_array =  cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/qhd.jpg",IMREAD_COLOR);
    // for(int i=0;i<7;i++){
        printf("qhd stride: %d",stride);
        test_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>();
    //     stride *= 2;
    // }
    // stride = 1;
    // ratio = 6;
    original_array =  cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/uhd.jpg",IMREAD_COLOR);
    // for(int i=0;i<7;i++){
        printf("uhd stride: %d",stride);
        test_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>();
    //     stride *= 2;
    // }

}


// template <typename ppT>
// void test_snark_for_filtering_simple_compare(int n)
// {
//     std::vector<libff::Fr<ppT>> u1;
//     std::vector<libff::Fr<ppT>> u2;
//     std::vector<libff::Fr<ppT>> original;
//     original.resize(n);
//     u1.resize(n);
//     u2.resize(n);

// #ifdef MULTICORE
// #pragma omp parallel for
// #endif
//     for (int i = 0; i < n; i++)
//     {
//         original[i] = libff::Fr<ppT>(i % 255);
//         if (i < n / 2)
//         { //절반을 삭제 하는 예제
//             u1[i] = libff::Fr<ppT>(i % 255);
//             u2[i] = libff::Fr<ppT>::zero();
//         }
//         else
//         {
//             u1[i] = libff::Fr<ppT>::zero();
//             u2[i] = libff::Fr<ppT>(i % 255);
//         }
//     }

//     const bool test_serialization = true;
//     r1cs_example<libff::Fr<ppT>> example = generate_r1cs_filtering_example<libff::Fr<ppT>>(u1, u2);
//     const bool bit = run_snark_for_filtering<ppT>(example, original, test_serialization);
//     assert(bit);
//     libff::print_header("(leave) Test Snark for Filtering");
// }

// int main(int argc, char **argv)
// {
//     default_r1cs_gg_ppzksnark_pp::init_public_params();
//     libff::start_profiling();
//     // stride = 16;
//     // ratio = 3;
//     // original_array = cv::imread("/home/glycogen/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/fhd.jpg", IMREAD_COLOR);
//     // printf("fhd stride: %d", stride);
//     // test_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>();
//     int n = atoi(argv[1]);

//     test_snark_for_filtering_simple_compare<default_r1cs_gg_ppzksnark_pp>(n);
// }
