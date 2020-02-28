/** @file
 *****************************************************************************

 Implementation of functionality that runs the R1CS GG-ppzkSNARK for
 a given R1CS example.

 See run_snark_for_completment.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef RUN_SNARK_FOR_COMPLETMENT_TCC_
#define RUN_SNARK_FOR_COMPLETMENT_TCC_

#include <sstream>
#include <type_traits>

#include <libff/common/profiling.hpp>

#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment.hpp>

namespace libsnark {

template<typename ppT>
typename std::enable_if<ppT::has_affine_pairing, void>::type
test_affine_verifier(const snark_for_completment_verification_key<ppT> &vk,
                     const snark_for_completment_primary_input<ppT> &primary_input,
                     const snark_for_completment_proof<ppT> &proof,
                     const bool expected_answer)
{
    libff::print_header("R1CS GG-ppzkSNARK Affine Verifier");
    const bool answer = snark_for_completment_affine_verifier_weak_IC<ppT>(vk, primary_input, proof);
    assert(answer == expected_answer);
}

template<typename ppT>
typename std::enable_if<!ppT::has_affine_pairing, void>::type
test_affine_verifier(const snark_for_completment_verification_key<ppT> &vk,
                     const snark_for_completment_primary_input<ppT> &primary_input,
                     const snark_for_completment_proof<ppT> &proof,
                     const bool expected_answer)
{
    libff::print_header("R1CS GG-ppzkSNARK Affine Verifier");
    libff::UNUSED(vk, primary_input, proof, expected_answer);
    printf("Affine verifier is not supported; not testing anything.\n");
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
bool run_snark_for_completment(const r1cs_example<libff::Fr<ppT> > &example,
                        const bool test_serialization)
{
    libff::enter_block("Call to run_snark_for_completment");

    libff::print_header("R1CS GG-ppzkSNARK Generator");
    snark_for_completment_keypair<ppT> keypair = snark_for_completment_generator<ppT>(example.constraint_system);
    printf("\n"); libff::print_indent(); libff::print_mem("after generator");

    libff::print_header("Preprocess verification key");
    snark_for_completment_processed_verification_key<ppT> pvk = snark_for_completment_verifier_process_vk<ppT>(keypair.vk);

    if (test_serialization)
    {
        libff::enter_block("Test serialization of keys");
        keypair.pk = libff::reserialize<snark_for_completment_proving_key<ppT> >(keypair.pk);
        keypair.vk = libff::reserialize<snark_for_completment_verification_key<ppT> >(keypair.vk);
        pvk = libff::reserialize<snark_for_completment_processed_verification_key<ppT> >(pvk);
        libff::leave_block("Test serialization of keys");
    }

    libff::print_header("R1CS GG-ppzkSNARK Prover");
    snark_for_completment_proof<ppT> proof = snark_for_completment_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input);
    printf("\n"); libff::print_indent(); libff::print_mem("after prover");

    if (test_serialization)
    {
        libff::enter_block("Test serialization of proof");
        proof = libff::reserialize<snark_for_completment_proof<ppT> >(proof);
        libff::leave_block("Test serialization of proof");
    }

    libff::print_header("R1CS GG-ppzkSNARK Verifier");
    const bool ans = snark_for_completment_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    libff::print_header("R1CS GG-ppzkSNARK Online Verifier");
    const bool ans2 = snark_for_completment_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
    assert(ans == ans2);

    test_affine_verifier<ppT>(keypair.vk, example.primary_input, proof, ans);

    libff::leave_block("Call to run_snark_for_completment");

    return ans;
}


// template<typename ppT>
// bool run_r1cs_conv_ppzksnark(const r1cs_example<libff::Fr<ppT> > &example, const r1cs_example<libff::Fr<ppT>> &example2,
//                         const bool test_serialization)
// {
//     printf("test!!\n");

//     libff::enter_block("Call to run_r1cs_gg_ppzksnark");
//     libff::print_header("R1CS GG-ppzkSNARK Generator");
//     r1cs_gg_ppzksnark_keypair<ppT> keypair = r1cs_gg_ppzksnark_generator<ppT>(example.constraint_system);
//     r1cs_conv_ppzksnark_keypair<ppT> keypair2 = r1cs_conv_ppzksnark_generator<ppT>(example2.constraint_system);

//     printf("\n"); libff::print_indent(); libff::print_mem("after generator");

//     libff::print_header("Preprocess verification key");
//     r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(keypair.vk);
//     r1cs_conv_ppzksnark_processed_verification_key<ppT> pvk2 = r1cs_conv_ppzksnark_verifier_process_vk<ppT>(keypair2.vk);

//     if (test_serialization)
//     {
//         libff::enter_block("Test serialization of keys");
//         keypair.pk = libff::reserialize<r1cs_gg_ppzksnark_proving_key<ppT> >(keypair.pk);
//         keypair.vk = libff::reserialize<r1cs_gg_ppzksnark_verification_key<ppT> >(keypair.vk);
//         pvk = libff::reserialize<r1cs_gg_ppzksnark_processed_verification_key<ppT> >(pvk);
//         libff::leave_block("Test serialization of keys");
//     }

//     // std::cout<<"ex 1"<<std::endl;
//     // for(size_t i=0;i<example.primary_input.size();i++){
//     //     std::cout<<i<<", "<<example.primary_input[i].as_ulong()<<std::endl;
//     // }
//     // std::cout<<"ex 2"<<std::endl;
//     // for(size_t i=0;i<example2.primary_input.size();i++){
//     //     std::cout<<i<<", "<<example2.primary_input[i].as_ulong()<<std::endl;
//     // }
//     size_t len = example2.constraint_system.num_convol_outputs(0);
//     size_t len2 = example2.constraint_system.num_convol_input_height(0) + example2.constraint_system.num_convol_kernel_height(0);
//     const accumulation_vector<libff::G1<ppT> > cx = pvk.gamma_ABC_g1.template accumulate_chunk<libff::Fr<ppT> >(example.primary_input.begin()+1, example.primary_input.begin() + 1 + len , 0);
//     const accumulation_vector<libff::G1<ppT> > cx2 = pvk2.gamma_ABC_g1.template accumulate_chunk<libff::Fr<ppT> >(example2.primary_input.begin() + 1 +len2, example2.primary_input.begin() + 1 + len2 + len, 0);

//     libff::G1<ppT> ccx = pvk.gamma_ABC_g1.first;

//     // std::cout<<"ex 1"<<std::endl;
//     for(size_t i=1;i<len+1;i++){
//         ccx = ccx + (example.primary_input[i] * pvk.gamma_ABC_g1.rest.values[i]);
//         // std::cout<<i<<", "<<example.primary_input[i].as_ulong()<<std::endl;
        
//     }
//     // std::cout<<"ex 2"<<std::endl;
//     libff::G1<ppT> ccx2 = pvk2.gamma_ABC_g1.first;
//     for(size_t i=1+len2;i<1+len2+len;i++){
//         ccx2 = ccx2 + (example2.primary_input[i] * pvk2.gamma_ABC_g1.rest.values[i]);
//         // std::cout<<i<<", "<<example2.primary_input[i].as_ulong()<<std::endl;
//     }

//     // accumulation_vector<libff::G1<ppT>> lego1(pvk.gamma_ABC_g1.first, std::vector<libff::G1<ppT>>(pvk.gamma_ABC_g1.rest.begin(), pvk.gamma_ABC_g1.rest.begin()+5));
//     // accumulation_vector<libff::G1<ppT>> lego2(pvk2.gamma_ABC_g1.first, std::vector<libff::G1<ppT>>(pvk2.gamma_ABC_g1.rest.begin(), pvk2.gamma_ABC_g1.rest.begin()+5));

//     libff::print_header("LEGO Keygen");
//     libff::enter_block("Call to LEGO_Keygen");
//     r1cs_legosnark_keypair<ppT> legokey(r1cs_legosnark_generator<ppT>(pvk.gamma_ABC_g1, pvk2.gamma_ABC_g1, len, len2));//lego1, lego2));
//     libff::leave_block("Call to LEGO_Keygen");
// 	printf("\n");

//     libff::print_header("LEGO Prover");
//     libff::enter_block("Call to LEGO_prover");
//     r1cs_legosnark_proof<ppT> lego_proof = r1cs_legosnark_prover<ppT>(legokey.pk, example.primary_input, len);//example.primary_input);
//     libff::leave_block("Call to LEGO_prover");

//     libff::print_header("LEGO Verifier");
//     libff::enter_block("Call to LEGO_verifier");
//     const bool legoans = r1cs_legosnark_verifier<ppT>(legokey.vk,ccx, ccx2, lego_proof); //cx.first, cx2.first, lego_proof);
//     printf("* The verification result is: %s\n", (legoans ? "PASS" : "FAIL"));
//     libff::leave_block("Call to LEGO_verifier");



// 	// libff::print_header("HFAL XP1 KEY Generation");
//     // libff::enter_block("Call to HFAL_xp1_Generator");
//     // r1cs_HFAL_XP1_ppzksnark_keypair<ppT> xp1_keypair(r1cs_HFAL_XP1_ppzksnark_generator<ppT>(xp1_pp_g1, pvk.gamma_ABC_g1));
//     // libff::leave_block("Call to HFAL_xp1_Generator");

	
//     // printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
//     // printf("* The XP1 verification result is: %s\n", (xp1_ans ? "PASS" : "FAIL"));
// 	printf("\n");
//     libff::print_header("R1CS GG-ppzkSNARK Prover");
//     r1cs_gg_ppzksnark_proof<ppT> proof = r1cs_gg_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input);
//     printf("\n"); libff::print_indent(); libff::print_mem("after prover");

//     r1cs_conv_ppzksnark_proof<ppT> proof2 = r1cs_conv_ppzksnark_prover<ppT>(keypair2.pk, example2.primary_input, example2.auxiliary_input);
//     printf("\n"); libff::print_indent(); libff::print_mem("after prover");

//     if (test_serialization)
//     {
//         libff::enter_block("Test serialization of proof");
//         proof = libff::reserialize<r1cs_gg_ppzksnark_proof<ppT> >(proof);
//         libff::leave_block("Test serialization of proof");
//     }

//     libff::print_header("R1CS GG-ppzkSNARK Verifier");
//     const bool ans = r1cs_gg_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
//     const bool ans3 = r1cs_conv_ppzksnark_verifier_strong_IC<ppT>(keypair2.vk, example2.primary_input, proof2);

//     printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
//     printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));
//     printf("* The verification result is: %s\n", (ans3 ? "PASS" : "FAIL"));

//     libff::print_header("R1CS GG-ppzkSNARK Online Verifier");
//     const bool ans2 = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
//     printf("* The verification result is: %s\n", (ans2 ? "PASS" : "FAIL"));
//     assert(ans == ans2);

//     //test_affine_verifier<ppT>(keypair.vk, example.primary_input, proof, ans);

//     libff::leave_block("Call to run_r1cs_gg_ppzksnark");

//     return ans;
// }

} // libsnark

#endif // RUN_SNARK_FOR_COMPLETMENT_TCC_
