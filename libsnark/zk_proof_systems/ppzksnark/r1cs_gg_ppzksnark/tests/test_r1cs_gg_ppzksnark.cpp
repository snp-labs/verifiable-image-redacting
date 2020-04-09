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
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/examples/run_r1cs_gg_ppzksnark.hpp>

#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace libsnark;
using namespace cv;

bool mouse_is_pressing = false;
int start_x = -1, start_y = -1;
int end_x = -1, end_y = -1;
Mat original_array = cv::imread("/home/itsp/snark_for_filtering/libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/tests/original.jpg", IMREAD_GRAYSCALE);
Mat u1_array;
Mat u2_array = original_array.clone();


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
        if(start_x>x) std::swap(start_x, end_x);
        if(start_y>y) std::swap(start_y, end_y);
		Mat img_crop(original_array, Rect(start_x, start_y, x-start_x, y-start_y));

        for (int i=start_y; i<end_y; i++){
            uchar* ptr = u2_array.ptr<uchar>(i);
            for (int j=start_x; j<end_x; j++){
                ptr[j] = 0;
            }
        }
        bitwise_xor(img_result, u2_array, u1_array);
		imshow("original", img_result);
		imshow("u2", u2_array);
        imshow("u1", u1_array);
	}
}


template<typename ppT>
void test_r1cs_gg_ppzksnark()
{
    // libff::print_header("(enter) Test R1CS GG-ppzkSNARK");

    // const bool test_serialization = true;
    // r1cs_example<libff::Fr<ppT> > example = generate_r1cs_example_with_field_input<libff::Fr<ppT> >(num_constraints, input_size);

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
	setMouseCallback("original", mouse_callback);

    cv::waitKey(0);
	destroyAllWindows();

    std::vector<libff::Fr<ppT>> u1;
    std::vector<libff::Fr<ppT>> u2;
    std::vector<libff::Fr<ppT>> original;

    size_t img_size = original_array.rows * original_array.cols;
    
    // size_t img_size = 256;

    for(size_t i=0;i<img_size;i++){
        u1.push_back(libff::Fr<ppT>(u1_array.data[i]));
    }
    for(size_t i=0;i<img_size;i++){
        u2.push_back(libff::Fr<ppT>(u2_array.data[i]));
    }
    for(size_t i=0;i<img_size;i++){
        original.push_back(libff::Fr<ppT>(original_array.data[i]));
    }

    // for(size_t i=0;i<img_size;i++){
    //     u1.push_back(libff::Fr<ppT>(u1_array[i]));
    // }
    // for(size_t i=0;i<img_size;i++){
    //     u2.push_back(libff::Fr<ppT>(u2_array[i]));
    // }
    // for(size_t i=0;i<img_size;i++){
    //     original.push_back(libff::Fr<ppT>(original_array[i]));
    // }
    
    libff::print_header("(enter) Test Snark for Filtering");
    
    const bool test_serialization = true;
    r1cs_example<libff::Fr<ppT> > example = generate_r1cs_filtering_example<libff::Fr<ppT> >(u1, u2);
    
    const bool bit = run_r1cs_gg_ppzksnark<ppT>(example, original, test_serialization);
    assert(bit);

    libff::print_header("(leave) Test R1CS GG-ppzkSNARK");
}

int main()
{
    default_r1cs_gg_ppzksnark_pp::init_public_params();
    libff::start_profiling();

    test_r1cs_gg_ppzksnark<default_r1cs_gg_ppzksnark_pp>();
}
