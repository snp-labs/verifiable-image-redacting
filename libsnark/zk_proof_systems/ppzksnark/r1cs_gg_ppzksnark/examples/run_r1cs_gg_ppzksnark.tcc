/** @file
 *****************************************************************************

 Implementation of functionality that runs the R1CS GG-ppzkSNARK for
 a given R1CS example.

 See run_r1cs_gg_ppzksnark.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef RUN_R1CS_GG_PPZKSNARK_TCC_
#define RUN_R1CS_GG_PPZKSNARK_TCC_

#include <sstream>
#include <type_traits>

#include <libff/common/profiling.hpp>

#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/snark_for_filtering.hpp>

namespace libsnark {

// template<typename ppT>
// typename std::enable_if<ppT::has_affine_pairing, void>::type
// test_affine_verifier(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
//                      const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
//                      const r1cs_gg_ppzksnark_proof<ppT> &proof,
//                      const bool expected_answer)
// {
//     libff::print_header("R1CS GG-ppzkSNARK Affine Verifier");
//     const bool answer = r1cs_gg_ppzksnark_affine_verifier_weak_IC<ppT>(vk, primary_input, proof);
//     assert(answer == expected_answer);
// }

// template<typename ppT>
// typename std::enable_if<!ppT::has_affine_pairing, void>::type
// test_affine_verifier(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
//                      const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
//                      const r1cs_gg_ppzksnark_proof<ppT> &proof,
//                      const bool expected_answer)
// {
//     libff::print_header("R1CS GG-ppzkSNARK Affine Verifier");
//     libff::UNUSED(vk, primary_input, proof, expected_answer);
//     printf("Affine verifier is not supported; not testing anything.\n");
// }

/**
 * The code below provides an example of all stages of running a R1CS GG-ppzkSNARK.
 *
 * Of course, in a real-life scenario, we would have three distinct entities,
 * mangled into one in the demonstration below. The three entities are as follows.
 * (1) The "generator", which runs the ppzkSNARK generator on input a given
 *     constraint system CS to create a proving and a verification key for CS.
 * (2) The "prover", which runs the ppzkSNARK prover on input the proving key,
 *     a primary input for CS, and an auxiliary input for CS.
 * (3) The "verifier", which runs the ppzkSNARK verifier on input the verification key,
 *     a primary input for CS, and a proof.
 */
template<typename ppT>
bool run_r1cs_gg_ppzksnark(const r1cs_example<libff::Fr<ppT> > &example,
                        const std::vector<libff::Fr<ppT>> &xi_vector,
                        const bool test_serialization)
{
    libff::enter_block("Call to run_r1cs_gg_ppzksnark");

    libff::print_header("R1CS GG-ppzkSNARK Generator");
    // r1cs_gg_ppzksnark_keypair<ppT> keypair = r1cs_gg_ppzksnark_generator<ppT>(example.constraint_system);
    snark_for_filtering_keypair<ppT> keypair = snark_for_filtering_generator<ppT>(example.constraint_system);
    printf("\n"); libff::print_indent(); libff::print_mem("after generator");

    // libff::print_header("Preprocess verification key");
    // r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(keypair.vk);

    // if (test_serialization)
    // {
    //     libff::enter_block("Test serialization of keys");
    //     keypair.pk = libff::reserialize<r1cs_gg_ppzksnark_proving_key<ppT> >(keypair.pk);
    //     keypair.vk = libff::reserialize<r1cs_gg_ppzksnark_verification_key<ppT> >(keypair.vk);
    //     pvk = libff::reserialize<r1cs_gg_ppzksnark_processed_verification_key<ppT> >(pvk);
    //     libff::leave_block("Test serialization of keys");
    // }
    libff::enter_block("Generating Commitment");
    snark_for_filtering_Commit<ppT> commitment = Commit<ppT>(keypair.pp, xi_vector);
    size_t len = example.auxiliary_input.size();//514
    libff::Fr<ppT> o1(example.auxiliary_input[0]);
    libff::G1<ppT> C_x = o1 * keypair.pk.f_vector[0];
    for(size_t i = 1; i < len/2; i++){//1 ~ 256
		C_x = C_x + example.auxiliary_input[i] * keypair.pk.f_vector[i];
    }
    libff::leave_block("Generating Commitment");

    libff::print_header("R1CS GG-ppzkSNARK Prover");
    // r1cs_gg_ppzksnark_proof<ppT> proof = r1cs_gg_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input);
    snark_for_filtering_proof<ppT> proof = snark_for_filtering_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input, commitment.x0);
    printf("\n"); libff::print_indent(); libff::print_mem("after prover");

    // if (test_serialization)
    // {
    //     libff::enter_block("Test serialization of proof");
    //     proof = libff::reserialize<r1cs_gg_ppzksnark_proof<ppT> >(proof);
    //     libff::leave_block("Test serialization of proof");
    // }

    // libff::print_header("R1CS GG-ppzkSNARK Verifier");
    // const bool ans = r1cs_gg_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
    // printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    // printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    // size_t len = example.auxiliary_input.size();//514
    // libff::Fr<ppT> o1(example.auxiliary_input[0]);
    // libff::Fr<ppT> o2(example.auxiliary_input[len/2]);
    // libff::G1<ppT> _C_x = o2 * keypair.pk.L_query[len/2];
    // libff::G1<ppT> C_x = o1 * keypair.pk.L_query[0];

    // for(size_t i = 0; i < len/2-1; i++){//0 ~ n-1까지
	// 	_C_x = _C_x + example.auxiliary_input[i+len/2+1] * keypair.pk.L_query[i+len/2+1];
    // }


    // for(size_t i = 1; i < len/2; i++){//1 ~ 256
	// 	C_x = C_x + example.auxiliary_input[i] * keypair.pk.L_query[i];
    // }
    
    // libff::G1<ppT> tmp = C_x + _C_x;
    // printf("tmp: ");
    // tmp.print();

    // libff::print_header("R1CS GG-ppzkSNARK Online Verifier");
    // const bool ans2 = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
    // assert(ans == ans2);

    // test_affine_verifier<ppT>(keypair.vk, example.primary_input, proof, ans);

    libff::print_header("snark_for_filtering Verifier");
    const bool ans = snark_for_filtering_verifier(keypair.vk, commitment.sigma_x, C_x, proof);
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    libff::leave_block("Call to run_r1cs_gg_ppzksnark");

    return ans;
}

} // libsnark

#endif // RUN_R1CS_GG_PPZKSNARK_TCC_
