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

using namespace libsnark;
using namespace cv;

bool mouse_is_pressing = false;
int start_x = -1, start_y = -1;
int end_x = -1, end_y = -1;
Mat original_array = cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/tests/test.png",IMREAD_COLOR);
Mat u1_array;
Mat u2_array = original_array.clone();
Mat img_show = original_array.clone();
vector<unsigned char> u1_digest_vector[SHA256_DIGEST_LENGTH];
vector<unsigned char> u2_digest_vector[SHA256_DIGEST_LENGTH];
int stride=1;


void mouse_callback(int event, int x, int y, int flags, void *userdata)
{
	Mat img_result = original_array.clone();

	if (event == EVENT_LBUTTONDOWN){

		mouse_is_pressing = true;
		start_x = x;
		start_y = y;

		circle(img_result, Point(x, y), 10, Scalar(0, 255, 0), -1);
		imshow("original", img_result);

	} else if (event == EVENT_MOUSEMOVE){

		if (mouse_is_pressing){
			
			rectangle(img_result, Point(start_x, start_y), Point(x, y),
				Scalar(0, 255, 0), 1);

			imshow("original", img_result);
            end_x = x;
            end_y = y;
		}

	} else if (event == EVENT_LBUTTONUP){

		mouse_is_pressing = false;
        if(start_x>end_x) std::swap(start_x, end_x);
        if(start_y>end_y) std::swap(start_y, end_y);
        // std::cout << "start_x: " << start_x << std::endl;
        // std::cout << "end_x: " << end_x << std::endl;
        // std::cout << "start_y: " << start_y << std::endl;
        // std::cout << "end_y: " << end_y << std::endl;

        start_x = start_x - start_x%stride;
        start_y = start_y - start_y%stride;
        end_x = ((end_x+stride-1)/stride)*stride;
        end_y = ((end_y+stride-1)/stride)*stride;

        // std::cout << "start_x: " << start_x << std::endl;
        // std::cout << "end_x: " << end_x << std::endl;
        // std::cout << "start_y: " << start_y << std::endl;
        // std::cout << "end_y: " << end_y << std::endl;
        
        for (int i=start_y; i<end_y; i++){
            uchar* ptr = u2_array.ptr<uchar>(i);
            for (int j=start_x; j<end_x; j++){
                ptr[j*3] = 0;
                ptr[j*3+1] = 0;
                ptr[j*3+2] = 0;
            }
        }
        bitwise_xor(img_result, u2_array, u1_array);
		imshow("original", img_result);
		imshow("u2", u2_array);
        imshow("u1", u1_array);
    // {
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,
    // 0,0,255,0,0,0,0,0,255,0,0,0,255,0,0,0,
    // 0,0,255,0,0,0,0,255,0,0,0,0,0,255,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,255,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,255,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,255,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,255,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,255,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,255,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,255,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,255,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,255,255,255,255,255,255,255,255,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    // };
        // for (int i=0; i<original_array.rows/stride; i++){
        //     uchar* ptr_u1 = u1_array.ptr<uchar>(i*stride);
        //     uchar* ptr_u2 = u2_array.ptr<uchar>(i*stride);
        //     for (int j=0; j<original_array.cols/stride; j++){
        //         unsigned char u1_digest[SHA256_DIGEST_LENGTH];
        //         unsigned char u2_digest[SHA256_DIGEST_LENGTH];
        //        SHA256((unsigned char*)&u1_array.at<uchar>(), strlen(u1_array), (unsigned char*)&u1_digest);
        //        SHA256((unsigned char*)&u2_array.at<uchar>(), strlen(u2_array), (unsigned char*)&u2_digest);
        //     }
        // }
	}
}

template<typename ppT>
void test_snark_for_filtering()
{   
    std::cout<<"stride? ";
    std::cin>>stride;
    
    for (int i=1; i<=original_array.rows/stride; i++){
        line(img_show, Point(0, i*stride), Point(original_array.cols, i*stride), Scalar(0, 0, 0), 1);
        for (int j=1; j<=original_array.cols/stride; j++){
            line(img_show, Point(j*stride, 0), Point(j*stride, original_array.rows), Scalar(0, 0, 0), 1);

        }
    }

    // char string[] = "hello world";

    

    // SHA256((unsigned char*)&string, strlen(string), (unsigned char*)&digest);    

    // int original_array[]= {
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,
    // 0,0,255,0,0,0,0,0,255,0,0,0,255,0,0,0,
    // 0,0,255,0,0,0,0,255,0,0,0,0,0,255,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,255,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,255,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,255,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,255,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,255,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,255,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,255,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,255,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,255,255,255,255,255,255,255,255,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    // };
    // int u1_array[]= {
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    // };
    // int u2_array[]= {
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,
    // 0,0,0,0,0,0,0,0,255,0,0,0,255,0,0,0,
    // 0,0,0,0,0,0,0,255,0,0,0,0,0,255,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    // };
    imshow("original", original_array);
    imshow("stride", img_show);
	setMouseCallback("original", mouse_callback);

    cv::waitKey(0);
	destroyAllWindows();

    std::vector<libff::Fr<ppT>> u1;
    std::vector<libff::Fr<ppT>> u2;
    std::vector<libff::Fr<ppT>> original;

    size_t img_size = original_array.rows * original_array.cols * 3;

    // for(size_t i=0;i<256;i++){
    //     u1.push_back(libff::Fr<ppT>(u1_array[i]));
    // }
    // for(size_t i=0;i<256;i++){
    //     u2.push_back(libff::Fr<ppT>(u2_array[i]));
    // }
    // for(size_t i=0;i<256;i++){
    //     original.push_back(libff::Fr<ppT>(original_array[i]));

    // }
    // }
    // for(size_t i=0;i<256;i++){
    //     u2.push_back(libff::Fr<ppT>(u2_array[i]));
    // }
    // for(size_t i=0;i<256;i++){
    //     original.push_back(libff::Fr<ppT>(original_array[i]));
    // }
    // }

    for(size_t i=0;i<img_size;i++){
        u1.push_back(libff::Fr<ppT>(u1_array.data[i]));
    
        u2.push_back(libff::Fr<ppT>(u2_array.data[i]));
    
        original.push_back(libff::Fr<ppT>(original_array.data[i]));
    }

    libff::print_header("(enter) Test Snark for Filtering");
    
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

