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

using namespace libsnark;

template<typename ppT>
void test_r1cs_gg_ppzksnark()
{
    // libff::print_header("(enter) Test R1CS GG-ppzkSNARK");

    // const bool test_serialization = true;
    // r1cs_example<libff::Fr<ppT> > example = generate_r1cs_example_with_field_input<libff::Fr<ppT> >(num_constraints, input_size);
    int original_array[]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,
    0,0,255,0,0,0,0,0,255,0,0,0,255,0,0,0,
    0,0,255,0,0,0,0,255,0,0,0,0,0,255,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,255,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,255,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,255,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,255,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,255,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,255,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,255,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,255,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,255,255,255,255,255,255,255,255,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    int u1_array[]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    int u2_array[]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,
    0,0,0,0,0,0,0,0,255,0,0,0,255,0,0,0,
    0,0,0,0,0,0,0,255,0,0,0,0,0,255,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    std::vector<libff::Fr<ppT>> u1;
    std::vector<libff::Fr<ppT>> u2;
    std::vector<libff::Fr<ppT>> original;

    // u1.reserve(912600);
    // u2.reserve(912600);
    // original.reserve(912600);
    // for(size_t j=0;j<3600;j++){
    for(size_t i=0;i<256;i++){
        u1.push_back(libff::Fr<ppT>(u1_array[i]));
    }
    for(size_t i=0;i<256;i++){
        u2.push_back(libff::Fr<ppT>(u2_array[i]));
    }
    for(size_t i=0;i<256;i++){
        original.push_back(libff::Fr<ppT>(original_array[i]));
    }
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
