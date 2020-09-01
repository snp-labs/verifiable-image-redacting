/** @file
 *****************************************************************************
 Profiling program that exercises the ppzkSNARK (first generator, then prover,
 then verifier) on a synthetic R1CS instance.

 The command

     $ libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/profiling/profile_r1cs_gg_ppzksnark 1000 10 Fr

 exercises the ppzkSNARK (first generator, then prover, then verifier) on an R1CS instance with 1000 equations and an input consisting of 10 field elements.

 (If you get the error `zmInit ERR:can't protect`, see the discussion [above](#elliptic-curve-choices).)

 The command

     $ libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/profiling/profile_r1cs_gg_ppzksnark 1000 10 bytes

 does the same but now the input consists of 10 bytes.

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
// #include <libsnark/common/default_types/snark_for_completment.hpp> //수정 필요?
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/examples/run_snark_for_filtering.hpp>

using namespace libsnark;

int main(int argc, const char * argv[])
{
    default_r1cs_gg_ppzksnark_pp::init_public_params();
    libff::start_profiling();

    if (argc == 2 && strcmp(argv[1], "-v") == 0)
    {
        libff::print_compilation_info();
        return 0;
    }

    if (argc != 3 && argc != 4)
    {
        printf("usage: %s num_constraints input_size [Fr|bytes]\n", argv[0]);
        return 1;
    }
    const int num_constraints = atoi(argv[1]);
    int input_size = atoi(argv[2]);
    if (argc == 4)
    {
        assert(strcmp(argv[3], "Fr") == 0 || strcmp(argv[3], "bytes") == 0);
        if (strcmp(argv[3], "bytes") == 0)
        {
            input_size = libff::div_ceil(8 * input_size, libff::Fr<libff::default_ec_pp>::capacity());
        }
    }

    libff::enter_block("Generate R1CS example");
    // r1cs_example<libff::Fr<default_r1cs_gg_ppzksnark_pp> > example = generate_r1cs_example_with_field_input<libff::Fr<default_r1cs_gg_ppzksnark_pp> >(num_constraints, input_size);
    libff::Fr_vector<libff::default_ec_pp> original{
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
    libff::Fr_vector<libff::default_ec_pp> u1{
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
    libff::Fr_vector<libff::default_ec_pp> u2{
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
    r1cs_example<libff::Fr<libff::default_ec_pp> > example = generate_r1cs_filtering_example<libff::Fr<libff::default_ec_pp> >(u1, u2);
    libff::leave_block("Generate R1CS example");

    libff::print_header("(enter) Profile R1CS GG-ppzkSNARK");
    run_snark_for_filtering<default_r1cs_gg_ppzksnark_pp>(example, original, test_serialization);
    libff::print_header("(leave) Profile R1CS GG-ppzkSNARK");
}
