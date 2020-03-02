#ifndef SNARK_FOR_FILTERING_TCC_
#define SNARK_FOR_FILTERING_TCC_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <sstream>

#include <libff/algebra/scalar_multiplication/multiexp.hpp>
#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>

#ifdef MULTICORE
#include <omp.h>
#endif

#include <libsnark/knowledge_commitment/kc_multiexp.hpp>
#include <libsnark/reductions/r1cs_to_qap/r1cs_to_qap.hpp>


#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment.hpp>

namespace libsnark
{

template <typename ppT>
bool snark_for_filtering_proving_key<ppT>::operator==(const snark_for_filtering_proving_key<ppT> &other) const
{
    return (this->P_vector == other.P_vector &&
            this->alpha_g1 == other.alpha_g1 &&
            this->beta_g1 == other.beta_g1 &&
            this->beta_g2 == other.beta_g2 &&
            this->delta_g1 == other.delta_g1 &&
            this->delta_g2 == other.delta_g2 &&
            this->A_query == other.A_query &&
            this->B_query == other.B_query &&
            this->H_query == other.H_query &&
            this->constraint_system == other.constraint_system);

}
template <typename ppT>
std::ostream &operator<<(std::ostream &out, const snark_for_filtering_proving_key<ppT> &pk)
{
    out << pk.P_vector;
    out << pk.alpha_g1 << OUTPUT_NEWLINE;
    out << pk.beta_g1 << OUTPUT_NEWLINE;
    out << pk.beta_g2 << OUTPUT_NEWLINE;
    out << pk.delta_g1 << OUTPUT_NEWLINE;
    out << pk.delta_g2 << OUTPUT_NEWLINE;
    out << pk.A_query;
    out << pk.B_query;
    out << pk.H_query;
    out << pk.constraint_system;

    return out;
}
template <typename ppT>
std::istream &operator>>(std::istream &in, const snark_for_filtering_proving_key<ppT> &pk)
{
    in >> pk.P_vector;
    in >> pk.alpha_g1;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.beta_g1;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.beta_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.delta_g1;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.delta_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.A_query;
    in >> pk.B_query;
    in >> pk.H_query;
    in >> pk.constraint_system;

    return in;
}

template <typename ppT>
bool snark_for_filtering_verification_key<ppT>::operator==(const snark_for_filtering_verification_key<ppT> &other) const
{
    return (this->c0_g2 == other.c0_g2 &&
            this->c1_g2 == other.c1_g2 &&
            this->c2_g2==other.c2_g2 &&
            this->a_g2==other.a_g2 &&
            this->alpha_g1_beta_g2==other.alpha_g1_beta_g2 &&
            this->delta_g2==other.delta_g2);
}
template <typename ppT>
std::ostream &operator<<(std::ostream &out, const snark_for_filtering_verification_key<ppT> &vk)
{
    out << vk.c0_g2;
    out << vk.c1_g2;
    out << vk.c2_g2;
    out << vk.a_g2;
    out << vk.alpha_g1_beta_g2 << OUTPUT_NEWLINE;
    out << vk.delta_g2 << OUTPUT_NEWLINE;

    return out;
}
template <typename ppT>
std::istream &operator>>(std::istream &in, const snark_for_filtering_verification_key<ppT> &vk)
{
    in >> vk.c0_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.c1_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.c2_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.a_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.alpha_g1_beta_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.delta_g2;
    libff::consume_OUTPUT_NEWLINE(in);

    return in;
}

template <typename ppT>
bool snark_for_filtering_proof<ppT>::operator==(const snark_for_filtering_proof<ppT> &other) const
{
    return (this->g_A == other.g_A &&
            this->g_B == other.g_B &&
            this->g_C == other.g_C &&
            this->ss_proof_g1 == other.ss_proof_g1 &&
            this->_C_x == other._C_x
            );
}

template <typename ppT>
std::ostream &operator<<(std::ostream &out, const snark_for_filtering_proof<ppT> &proof)
{
    out << proof.g_A << OUTPUT_NEWLINE;
    out << proof.g_B << OUTPUT_NEWLINE;
    out << proof.g_C << OUTPUT_NEWLINE;
    out << proof.ss_proof_g1;
    out << proof._C_x;

    return out;
}

template <typename ppT>
std::istream &operator>>(std::istream &in, snark_for_filtering_proof<ppT> &proof)
{
    in >> proof.g_A;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> proof.g_B;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> proof.g_C;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> proof.ss_proof_g1;
    in >> proof._C_x;

    return in;
}

// template <typename ppT>
// snark_for_filtering_hash<ppT> snark_for_filtering_hashing(const snark_for_filtering_pp<ppT> &pp, const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input){

//     libff::G1<ppT> sigma_g1 = libff::G1<ppT>::zero();
//     sigma_g1 = pp.hfal_H[0];

//     for(size_t i = 0; i < primary_input.size(); i++){
//         sigma_g1 = primary_input[i] * pp.hfal_H[i+1] + sigma_g1;
//     }
//     snark_for_filtering_hash<ppT> hash = snark_for_filtering_hash<ppT>(std::move(sigma_g1));

// 	hash.print_size();

//     return hash;
// }

/**
 * pp 문제 있음 
*/

template <typename ppT>
snark_for_filtering_keypair<ppT> snark_for_filtering_generator(const snark_for_completment_constraint_system<ppT> &r1cs){
    /**
     * Setup
     */
    libff::enter_block("Generating G1 MSM window table");
    const libff::G1<ppT> g1_generator = libff::G1<ppT>::random_element();
    const size_t g1_scalar_count = non_zero_At + non_zero_Bt + qap.num_variables();
    const size_t g1_scalar_size = libff::Fr<ppT>::size_in_bits();
    const size_t g1_window_size = libff::get_exp_window_size<libff::G1<ppT> >(g1_scalar_count);

    libff::print_indent(); printf("* G1 window: %zu\n", g1_window_size);
    libff::window_table<libff::G1<ppT> > g1_table = libff::get_window_table(g1_scalar_size, g1_window_size, g1_generator);
    libff::leave_block("Generating G1 MSM window table");

    libff::enter_block("Generating G2 MSM window table");
    const libff::G2<ppT> G2_gen = libff::G2<ppT>::random_element();
    const size_t g2_scalar_count = non_zero_Bt;
    const size_t g2_scalar_size = libff::Fr<ppT>::size_in_bits();
    size_t g2_window_size = libff::get_exp_window_size<libff::G2<ppT> >(g2_scalar_count);

    libff::print_indent(); printf("* G2 window: %zu\n", g2_window_size);
    libff::window_table<libff::G2<ppT> > g2_table = libff::get_window_table(g2_scalar_size, g2_window_size, G2_gen);
    libff::leave_block("Generating G2 MSM window table");

    const int num_variables = r1cs.num_variables();
    
    libff::Fr<ppT> k0 = libff::Fr<ppT>::random_element();
    libff::Fr<ppT> k1 = libff::Fr<ppT>::random_element();
    libff::Fr<ppT> k2 = libff::Fr<ppT>::random_element();
    libff::Fr<ppT> a = libff::Fr<ppT>::random_element();
    libff::G2<ppT> a_g2 = a*libff::G2<ppT>::one();
    libff::G2<ppT> c0_g2 = k0*a_g2;
    libff::G2<ppT> c1_g2 = k1*a_g2;
    libff::G2<ppT> c2_g2 = k2*a_g2;
    libff::G1_vector<ppT> h_vector;
    libff::G1_vector<ppT> P_vector;

    P_vector.reserve(num_variables);
    
    for (size_t i = 0; i < num_variables/2; ++i)
    {
        h_vector.emplace_back(libff::G1<ppT>::random_element());
    }

    snark_for_completment_keypair<ppT> keypair = snark_for_completment_generator(r1cs);
    libff::G1_vector<ppT> f_vector = keypair.pk.L_query

    //Use h_vector, f_vector to build a matrix M
    // P <- M^T * k
    P_vector.emplace_back(k2 * f_vector.rest.values[0])
    P_vector.emplace_back(k1 * f_vector.rest.values[0])
    P_vector.emplace_back(k0 * h_vector.rest.values[0])

    for(size_t i = 1; i < num_variables/2; i++){
		P_vector.emplace_back((k0 * h_vector.rest.values[i]) + (k2 * f_vector.rest.values[i]));
    }
    for(size_t i = 1; i < num_variables/2; i++){
		P_vector.emplace_back((k0 * h_vector.rest.values[i]) + (k1 * f_vector.rest.values[num_variables/2+i-1]));
    }

    snark_for_filtering_proving_key<ppT> ek = snark_for_filtering_proving_key<ppT>(
        std::move(P_vector),
        std::move(keypair.pk.alpha_g1),
        std::move(keypair.pk.beta_g1),
        std::move(keypair.pk.beta_g2),
        std::move(keypair.pk.delta_g1),
        std::move(keypair.pk.delta_g2),
        std::move(keypair.pk.A_query),
        std::move(keypair.pk.B_query),
        std::move(keypair.pk.H_query),
        std::move(keypair.pk.r1cs_copy)
        );
    
    snark_for_filtering_verification_key<ppT> vk = snark_for_filtering_verification_key<ppT>(
        std::move(c0_g2),
        std::move(c1_g2),
        std::move(c2_g2),
        std::move(a_g2),
        std::move(keypair.vk.alpha_g1_beta_g2),
        std::move(keypair.vk.delta_g2)
        );
    
    ek.print_size();
	vk.print_size();

    return snark_for_filtering_keypair<ppT>(std::move(ek), std::move(vk));


}

template <typename ppT>
snark_for_filtering_proof<ppT> snark_for_filtering_prover(const snark_for_filtering_proving_key<ppT> &pk, const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input, size_t len){
    libff::G1<ppT> Proof_sum = libff::G1<ppT>::zero();
    Proof_sum = pk.P_g1[0];
    for(size_t i = 1; i <  len+1; i++){ 
        // std::cout<<i<<", "<<(primary_input[i]).as_ulong()<<std::endl;
        Proof_sum = (primary_input[i] * pk.P_g1[i]) + Proof_sum;
    }

    snark_for_filtering_proof<ppT> proof
        = snark_for_filtering_proof<ppT>(std::move(Proof_sum));

	proof.print_size();

    return proof;
}

template <typename ppT>
bool snark_for_filtering_verifier(const snark_for_filtering_verification_key<ppT> &vk, 
                                    const libff::G1<ppT> &cm1, 
                                    const libff::G1<ppT> &cm2,
                                    const snark_for_filtering_proof<ppT> &proof){


    libff::GT<ppT> left = ppT::reduced_pairing(proof.proof_G1, vk.a_g2);
    libff::GT<ppT> right = ppT::reduced_pairing(cm1, vk.C1_g2);
    libff::GT<ppT> right2 = ppT::reduced_pairing(cm2, vk.C2_g2);

    return (left == (right *  right2));
    //     printf("pass!!\n");
    
    
    // const libff::G1_precomp<ppT> proof_g_Proof_precomp = ppT::precompute_G1(proof.proof_G1);
    // const libff::G1_precomp<ppT> acc_precomp = ppT::precompute_G1(cm1);
    // const libff::G1_precomp<ppT> acc_precomp2 = ppT::precompute_G1(cm2);

    // const libff::G2_precomp<ppT> vk_C1_g2_precomp = ppT::precompute_G2(vk.C1_g2);
    // const libff::G2_precomp<ppT> vk_C2_g2_precomp = ppT::precompute_G2(vk.C2_g2);
    // const libff::G2_precomp<ppT> vk_a_g2_precomp = ppT::precompute_G2(vk.a_g2);

    // libff::Fqk<ppT> vf_right_one = ppT::miller_loop(proof_g_Proof_precomp, vk_a_g2_precomp);

    // libff::Fqk<ppT> vf_left_one = ppT::double_miller_loop(
    //     acc_precomp, vk_C1_g2_precomp,
    //     acc_precomp2, vk_C2_g2_precomp
    // );

    // libff::Fqk<ppT> vf_left_one2 = ppT::double_miller_loop(
    //     acc_precomp2, vk_C1_g2_precomp,
    //     acc_precomp, vk_C2_g2_precomp
    // ); 

    // vk.C1_g2.print();
    // vk.C2_g2.print();
    // vk.a_g2.print();

    // printf("vf_right_one<<\n");
    // vf_right_one.print();
    // printf("vf_left_one<<\n");
    // vf_left_one.print();
    // printf("vf_left_one2<<\n");
    // vf_left_one2.print();
    // return (vf_right_one == vf_left_one);
}
}// libsnark

#endif
