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
            this->f_vector == other.f_vector &&
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
    out << pk.f_vector;
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
    in >> pk.f_vector;
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
        std::move(f_vector),
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
snark_for_filtering_proof<ppT> snark_for_filtering_prover(const snark_for_filtering_proving_key<ppT> &pk, 
                                                    const snark_for_completment_primary_input<ppT> &primary_input,
                                                    const snark_for_completment_auxiliary_input<ppT> &auxiliary_input,
                                                    libff::Fr_vector<ppT> u1_vector, libff::Fr_vector<ppT> u2_vector, libff::Fr<ppT> x0){
    libff::Fr<ppT> o2 = libff::Fr<ppT>::random_element();
    libff::Fr<ppT> o1 = libff::Fr<ppT>::zero();
    libff::G1<ppT> _C_x = o2 * pk.f_vector.rest.values[0];
    libff::G1<ppT> ss_proof_g1 = o1 * pk.P_vector.rest.values[0];
    const int len = u1_vector.size;//len = n-1

    for(size_t i = 0; i <= len; i++){//0 ~ n-1까지
		_C_x += u2_vector.rest.values[i] * pk.f_vector.rest.values[i+1];
    }

    ss_proof_g1 += o2 * pk.P_vector.rest.values[1];
    ss_proof_g1 += x0 * pk.P_vector.rest.values[2];
    for(size_t i = 0; i <= len; i++){//0 ~ n-1까지
		ss_proof_g1 += u1_vector.rest.values[i] * pk.P_vector.rest.values[i+3];
    }
    for(size_t i = 0; i <= len; i++){//0 ~ n-1까지
		ss_proof_g1 += u2_vector.rest.values[i] * pk.P_vector.rest.values[i+len+3];
    }

    snark_for_filtering_proving_key<ppT> pk = snark_for_filtering_proving_key<ppT>(
        std::move(pk.alpha_g1),
        std::move(pk.beta_g1),
        std::move(pk.beta_g2),
        std::move(pk.delta_g1),
        std::move(pk.delta_g2),
        std::move(pk.A_query),
        std::move(pk.B_query),
        std::move(pk.H_query),
        std::move(pk.r1cs_copy)
        );
    
    snark_for_completment_proof<ppT> completment_proof = snark_for_completment_prover(&pk, &primary_input, auxiliary_input)

    snark_for_filtering_proof<ppT> proof
        = snark_for_filtering_proof<ppT>(std::move(completment_proof), std::move(ss_proof_g1), std::move(_C_x));

	proof.print_size();

    return proof;
}

template <typename ppT>
bool snark_for_filtering_verifier(const snark_for_filtering_verification_key<ppT> &vk, 
                                    const libff::G1<ppT> &sigma_x, 
                                    const libff::G1<ppT> &c_x,
                                    const snark_for_filtering_proof<ppT> &proof){


    libff::GT<ppT> left = ppT::reduced_pairing(proof.ss_proof_g1, vk.a_g2);
    libff::GT<ppT> right0 = ppT::reduced_pairing(c_x, vk.c0_g2);
    libff::GT<ppT> right1 = ppT::reduced_pairing(proof._C_x, vk.c1_g2);
    libff::GT<ppT> right2 = ppT::reduced_pairing(sigma_x, vk.c2_g2);

    snark_for_completment_verification_key<ppT> completment_vk = snark_for_completment_verification_key<ppT>(
        std::move(vk.alpha_g1_beta_g2),
        std::move(vk.delta_g2)
        );

    return (left == (right0 *  right1 * right2) &&
            snark_for_completment_affine_verifier_weak_IC(completment_vk, proof._C_x, c_x, proof.completment_proof));
}
}// libsnark

#endif
