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

using namespace libsnark;

template<typename ppT>
void test_snark_for_filtering()
/*void test_r1cs_gg_ppzksnark(size_t num_constraints,
                         size_t input_size)*/
{
    libff::Fr_vector<ppT> original{
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
    libff::Fr_vector<ppT> u1{
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
    libff::Fr_vector<ppT> u2{
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
