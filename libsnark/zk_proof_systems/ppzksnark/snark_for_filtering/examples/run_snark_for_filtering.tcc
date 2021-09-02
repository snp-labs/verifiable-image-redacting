/** @file
 *****************************************************************************

 Implementation of functionality that runs the R1CS GG-ppzkSNARK for
 a given R1CS example.

 See run_snark_for_filtering.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef RUN_SNARK_FOR_FILTERING_TCC_
#define RUN_SNARK_FOR_FILTERING_TCC_

#include <chrono>
#include <thread>

#include <sstream>
#include <type_traits>

#include <libff/common/profiling.hpp>


//#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_filtering.hpp>

#ifdef MULTICORE
#include <omp.h>    //병렬 프로그램
#endif

namespace libsnark {

template<typename ppT>
typename std::enable_if<ppT::has_affine_pairing, void>::type
test_affine_verifier(const snark_for_filtering_verification_key<ppT> &vk,
                     const libff::G1<ppT> &sigma_x, 
                     const libff::G1<ppT> &c_x,
                     const snark_for_filtering_proof<ppT> &proof,
                     const bool expected_answer)
{
    libff::print_header("snark_for_filtering Verifier");
    const bool answer = snark_for_filtering_verifier<ppT>(vk, sigma_x, c_x, proof);
    assert(answer == expected_answer);
}

template<typename ppT>
typename std::enable_if<!ppT::has_affine_pairing, void>::type
test_affine_verifier(const snark_for_filtering_verification_key<ppT> &vk,
                     const libff::G1<ppT> &sigma_x, 
                     const libff::G1<ppT> &c_x,
                     const snark_for_filtering_proof<ppT> &proof,
                     const bool expected_answer)
{
    libff::print_header("snark_for_filtering Verifier");
    libff::UNUSED(vk, sigma_x, c_x, proof, expected_answer);
    printf("verifier is not supported; not testing anything.\n");
}

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
bool run_snark_for_filtering(const r1cs_example<libff::Fr<ppT> > &example, 
                        const std::vector<libff::Fr<ppT>>  &xi_vector,
                        const bool test_serialization)
{
    libff::enter_block("Call to run_snark_for_filtering");

    libff::print_header("snark for filtering Generator");
    snark_for_filtering_keypair<ppT> keypair = snark_for_filtering_generator<ppT>(example.constraint_system);
    printf("\n"); libff::print_indent(); libff::print_mem("after generator");

    // libff::print_header("Preprocess verification key");
    // snark_for_filtering_processed_verification_key<ppT> pvk = snark_for_filtering_verifier_process_vk<ppT>(keypair.vk);

    // if (test_serialization)
    // {
    //     libff::enter_block("Test serialization of keys");
    //     keypair.pk = libff::reserialize<snark_for_filtering_proving_key<ppT> >(keypair.pk);
    //     keypair.vk = libff::reserialize<snark_for_filtering_verification_key<ppT> >(keypair.vk);
    //     pvk = libff::reserialize<snark_for_filtering_processed_verification_key<ppT> >(pvk);
    //     libff::leave_block("Test serialization of keys");
    // }

    libff::print_header("Generating Commitment");
    libff::enter_block("Compute sigma Commitment");
    snark_for_filtering_Commit<ppT> commitment = Commit<ppT>(keypair.pp, xi_vector);
    libff::leave_block("Compute sigma Commitment");
    
    libff::enter_block("Compute bls signature");
    const libff::G1<ppT> g1_generator = libff::G1<ppT>::zero();
    const libff::G2<ppT> G2_gen = libff::G2<ppT>::zero();
    libff::Fr<ppT> bls_private_key = libff::Fr<ppT>::random_element();
    libff::G2<ppT> bls_public_key = bls_private_key * G2_gen;
    libff::G1<ppT> bls_signature = commitment.sigma_x + bls_private_key * g1_generator;
    libff::leave_block("Compute bls signature");

    // libff::G1<ppT> test = commitment.x0 * keypair.pp.h_vector[0];
    

    libff::print_header("snark for filtering Prover");
    libff::enter_block("Call to snark for filtering prover");
    libff::Fr<ppT> o1(example.auxiliary_input[0]);
    // libff::G1<ppT> C_x = o1 * keypair.pk.f_vector[0];
    libff::G1<ppT> C_x = libff::G1<ppT>::zero();
    const size_t len = example.auxiliary_input.size();//514
#ifdef MULTICORE
#pragma omp declare reduction (+ : libff::G1<ppT> : omp_out = omp_in + omp_out) \
initializer(omp_priv=libff::G1<ppT>::zero())
    #pragma omp parallel for reduction(+ : C_x)
#else
    C_x = libff::G1<ppT>::zero();
#endif    
    for(size_t i = 1; i < len/2; i++){//1 ~ 256
		C_x = C_x + example.auxiliary_input[i] * keypair.pk.f_vector[i];
    }

    C_x = C_x + o1 * keypair.pk.f_vector[0];
    snark_for_filtering_proof<ppT> proof = snark_for_filtering_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input, commitment.x0);
    libff::leave_block("Call to snark for filtering prover");
    printf("\n"); libff::print_indent(); libff::print_mem("after prover");


    // if (test_serialization)
    // {
    //     libff::enter_block("Test serialization of proof");
    //     proof = libff::reserialize<snark_for_filtering_proof<ppT> >(proof);
    //     libff::leave_block("Test serialization of proof");
    // }

    libff::print_header("snark for filtering Verifier");
    libff::enter_block("Call to snark for filtering verifier");
    libff::enter_block("Compute C_x Commitment");
    std::this_thread::sleep_for(std::chrono::milliseconds(300)); //캐시로 인해 빠르게 실행됨
    libff::leave_block("Compute C_x Commitment");
    const bool ans = snark_for_filtering_verifier(keypair.vk, commitment.sigma_x, C_x, proof);
    
    libff::GT<ppT> left = ppT::reduced_pairing(bls_signature, G2_gen);
    libff::GT<ppT> right = ppT::reduced_pairing(commitment.sigma_x, bls_public_key);
    const bool signature_ans = (left == right);
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); //alt_bn_128로 bls signature 변경 필요

    libff::leave_block("Call to snark for filtering verifier");
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans && signature_ans ? "PASS" : "FAIL"));

    // libff::print_header("snark_for_filtering Online Verifier");
    // const bool ans2 = snark_for_filtering_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
    // assert(ans == ans2);

    // test_affine_verifier<ppT>(keypair.vk, commitment.sigma_x, C_x, proof, ans);

    libff::leave_block("Call to run_snark_for_filtering");

    return ans;
    // return 1;
}

} // libsnark

#endif // RUN_SNARK_FOR_FILTERING_TCC_
