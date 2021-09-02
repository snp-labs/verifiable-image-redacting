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

#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_filtering.hpp>

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
                // this->L_query == other.L_query &&
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
        // out << pk.L_query;
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
        // in >> pk.L_query;
        in >> pk.constraint_system;

        return in;
    }

    // template <typename ppT>
    // bool snark_for_completment_proving_key_with_out_L_query<ppT>::operator==(const snark_for_completment_proving_key_with_out_L_query<ppT> &other) const
    // {
    //     return (this->P_vector == other.P_vector &&
    //             this->f_vector == other.f_vector &&
    //             this->alpha_g1 == other.alpha_g1 &&
    //             this->beta_g1 == other.beta_g1 &&
    //             this->beta_g2 == other.beta_g2 &&
    //             this->delta_g1 == other.delta_g1 &&
    //             this->delta_g2 == other.delta_g2 &&
    //             this->A_query == other.A_query &&
    //             this->B_query == other.B_query &&
    //             this->H_query == other.H_query &&
    //             this->constraint_system == other.constraint_system);

    // }

    // template <typename ppT>
    // std::ostream &operator<<(std::ostream &out, const snark_for_completment_proving_key_with_out_L_query<ppT> &pk)
    // {
    //     out << pk.P_vector;
    //     out << pk.f_vector;
    //     out << pk.alpha_g1 << OUTPUT_NEWLINE;
    //     out << pk.beta_g1 << OUTPUT_NEWLINE;
    //     out << pk.beta_g2 << OUTPUT_NEWLINE;
    //     out << pk.delta_g1 << OUTPUT_NEWLINE;
    //     out << pk.delta_g2 << OUTPUT_NEWLINE;
    //     out << pk.A_query;
    //     out << pk.B_query;
    //     out << pk.H_query;
    //     out << pk.constraint_system;

    //     return out;
    // }

    // template <typename ppT>
    // std::istream &operator>>(std::istream &in, const snark_for_completment_proving_key_with_out_L_query<ppT> &pk)
    // {
    //     in >> pk.P_vector;
    //     in >> pk.f_vector;
    //     in >> pk.alpha_g1;
    //     libff::consume_OUTPUT_NEWLINE(in);
    //     in >> pk.beta_g1;
    //     libff::consume_OUTPUT_NEWLINE(in);
    //     in >> pk.beta_g2;
    //     libff::consume_OUTPUT_NEWLINE(in);
    //     in >> pk.delta_g1;
    //     libff::consume_OUTPUT_NEWLINE(in);
    //     in >> pk.delta_g2;
    //     libff::consume_OUTPUT_NEWLINE(in);
    //     in >> pk.A_query;
    //     in >> pk.B_query;
    //     in >> pk.H_query;
    //     in >> pk.constraint_system;

    //     return in;
    // }

    template <typename ppT>
    bool snark_for_filtering_verification_key<ppT>::operator==(const snark_for_filtering_verification_key<ppT> &other) const
    {
        return (this->c0_g2 == other.c0_g2 &&
                this->c1_g2 == other.c1_g2 &&
                this->c2_g2 == other.c2_g2 &&
                this->a_g2 == other.a_g2 &&
                this->alpha_g1_beta_g2 == other.alpha_g1_beta_g2 &&
                this->delta_g2 == other.delta_g2);
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
    bool snark_for_filtering_Commit<ppT>::operator==(const snark_for_filtering_Commit<ppT> &other) const
    {
        return (this->sigma_x == other.sigma_x &&
                this->x0 == other.x0);
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const snark_for_filtering_Commit<ppT> &commit)
    {
        out << commit.sigma_x << OUTPUT_NEWLINE;
        out << commit.x0 << OUTPUT_NEWLINE;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const snark_for_filtering_Commit<ppT> &commit)
    {
        in >> commit.sigma_x;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> commit.x0;
        libff::consume_OUTPUT_NEWLINE(in);

        return in;
    }

    template <typename ppT>
    bool snark_for_filtering_public_parameter<ppT>::operator==(const snark_for_filtering_public_parameter<ppT> &other) const
    {
        return (this->g1_generator == other.g1_generator &&
                this->G2_gen == other.G2_gen &&
                this->h_vector == other.h_vector);
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const snark_for_filtering_public_parameter<ppT> &pp)
    {
        out << pp.g1_generator << OUTPUT_NEWLINE;
        out << pp.G2_gen << OUTPUT_NEWLINE;
        out << pp.h_vector;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const snark_for_filtering_public_parameter<ppT> &pp)
    {
        in >> pp.g1_generator;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> pp.G2_gen;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> pp.h_vector;

        return in;
    }

    template <typename ppT>
    bool snark_for_filtering_proof<ppT>::operator==(const snark_for_filtering_proof<ppT> &other) const
    {
        return (this->g_A == other.g_A &&
                this->g_B == other.g_B &&
                this->g_C == other.g_C &&
                this->ss_proof_g1 == other.ss_proof_g1 &&
                this->_C_x == other._C_x);
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

    template <typename ppT>
    snark_for_filtering_Commit<ppT> Commit(const snark_for_filtering_public_parameter<ppT> &pp,
                                           const std::vector<libff::Fr<ppT>> &xi_vector)
    {
        libff::Fr<ppT> x0 = libff::Fr<ppT>::random_element();
        // libff::G1<ppT> sigma_x = x0 * pp.h_vector[0];
        libff::G1<ppT> sigma_x;
        size_t len = xi_vector.size();

#ifdef MULTICORE
#pragma omp declare reduction(+                             \
                              : libff::G1 <ppT>             \
                              : omp_out = omp_in + omp_out) \
    initializer(omp_priv = libff::G1 <ppT>::zero())
#pragma omp parallel for reduction(+ \
                                   : sigma_x)
#else
        sigma_x = libff::G1<ppT>::zero();
#endif
        for (size_t i = 0; i < len; i++)
        {
            sigma_x = sigma_x + xi_vector[i] * pp.h_vector[i + 1];
        }
        sigma_x = sigma_x + x0 * pp.h_vector[0];
        printf("offline sigma size: %zu\n", libff::G1<ppT>::size_in_bits());

        return snark_for_filtering_Commit<ppT>(std::move(sigma_x), std::move(x0));
    }
    /**
 * pp 문제 있음 
*/

    template <typename ppT>
    snark_for_filtering_keypair<ppT> snark_for_filtering_generator(const r1cs_constraint_system<libff::Fr<ppT>> &r1cs)
    {
        // void snark_for_filtering_generator(const r1cs_constraint_system<libff::Fr<ppT> > &r1cs){
        /**
     * Setup
     */
        libff::enter_block("Call to snark for filtering generator");

        const libff::G1<ppT> g1_generator = libff::G1<ppT>::random_element();
        const libff::G2<ppT> G2_gen = libff::G2<ppT>::random_element();

        size_t num_variables = r1cs.num_variables();

        libff::G1_vector<ppT> h_g1_vector;
        libff::G1_vector<ppT> P_vector;
        libff::Fr_vector<ppT> h_vector;
        P_vector.resize(num_variables + 1);

        libff::enter_block("Compute the h_g1_vector", false);
        libff::enter_block("Compute h_vector densities");

        // for (size_t i = 0; i < num_variables/2; i++)
        // {
        //     h_vector.emplace_back(libff::G1<ppT>::random_element());
        // }

        for (size_t i = 0; i < num_variables / 2; i++)
        {
            h_vector.emplace_back(libff::Fr<ppT>::random_element());
        }
        libff::leave_block("Compute h_vector densities");

#ifdef MULTICORE
        const size_t chunks = omp_get_max_threads(); // to override, set OMP_NUM_THREADS env var or call omp_set_num_threads()
#else
        const size_t chunks = 1;
#endif

        libff::enter_block("Generating G1 MSM window table");
        const size_t g1_scalar_count = num_variables / 2;
        const size_t g1_scalar_size = libff::Fr<ppT>::size_in_bits();
        const size_t g1_window_size = libff::get_exp_window_size<libff::G1<ppT>>(g1_scalar_count);

        libff::print_indent();
        printf("* G1 window: %zu\n", g1_window_size);
        libff::window_table<libff::G1<ppT>> g1_table = libff::get_window_table(g1_scalar_size, g1_window_size, g1_generator);
        libff::leave_block("Generating G1 MSM window table");

        h_g1_vector = batch_exp(g1_scalar_size, g1_window_size, g1_table, h_vector);
        printf("offline key size: %zu\n", libff::size_in_bits(h_g1_vector));
        libff::leave_block("Compute the h_g1_vector", false);

        libff::enter_block("Generating snark key pair Generator");
        snark_for_completment_keypair<ppT> keypair = snark_for_completment_generator<ppT>(r1cs, g1_generator, G2_gen);
        libff::leave_block("Generating snark key pair Generator");
        libff::G1_vector<ppT> f_vector = keypair.pk.L_query;

        //Use h_vector, f_vector to build a matrix M
        // P <- M^T * k
        libff::enter_block("Compute p_vector & a_vector for snark for filterring proving key");
        // P_vector.emplace_back(k2 * f_vector[0]);
        // P_vector.emplace_back(k1 * f_vector[num_variables/2]);
        // P_vector.emplace_back(k0 * h_g1_vector[0]);
        libff::Fr<ppT> k0 = libff::Fr<ppT>::random_element();
        libff::Fr<ppT> k1 = libff::Fr<ppT>::random_element();
        libff::Fr<ppT> k2 = libff::Fr<ppT>::random_element();
        libff::Fr<ppT> a = libff::Fr<ppT>::random_element();
        libff::G2<ppT> a_g2 = a * G2_gen;
        libff::G2<ppT> c0_g2 = k0 * a_g2;
        libff::G2<ppT> c1_g2 = k1 * a_g2;
        libff::G2<ppT> c2_g2 = k2 * a_g2;
        P_vector[0] = k2 * f_vector[0];
        P_vector[1] = k1 * f_vector[num_variables / 2];
        P_vector[2] = k0 * h_g1_vector[0];

#ifdef MULTICORE
#pragma omp parallel for
#endif
        for (size_t i = 1; i < num_variables / 2; i++)
        {
            P_vector[i + 2] = k0 * h_g1_vector[i] + k2 * f_vector[i];
        }
#ifdef MULTICORE
#pragma omp parallel for
#endif
        for (size_t i = 1; i < num_variables / 2; i++)
        {
            P_vector[num_variables / 2 + i + 1] = k0 * h_g1_vector[i] + k1 * f_vector[num_variables / 2 + i];
        }
        libff::leave_block("Compute p_vector & a_vector for snark for filterring proving key");

        libff::leave_block("Call to snark for filtering generator");

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
            std::move(keypair.pk.constraint_system));
        snark_for_filtering_verification_key<ppT> vk = snark_for_filtering_verification_key<ppT>(
            c0_g2,
            c1_g2,
            c2_g2,
            a_g2,
            keypair.vk.alpha_g1_beta_g2,
            keypair.vk.delta_g2);

        snark_for_filtering_public_parameter<ppT> pp = snark_for_filtering_public_parameter<ppT>(
            std::move(h_g1_vector));

        ek.print_size();
        vk.print_size();
        pp.print_size();

        return snark_for_filtering_keypair<ppT>(std::move(ek), std::move(vk), std::move(pp));
        // return;
    }

    template <typename ppT>
    snark_for_filtering_proof<ppT> snark_for_filtering_prover(snark_for_filtering_proving_key<ppT> &pk,
                                                              const snark_for_completment_primary_input<ppT> &primary_input,
                                                              const snark_for_completment_auxiliary_input<ppT> &auxiliary_input,
                                                              const libff::Fr<ppT> &x0)
    {

        const size_t len = auxiliary_input.size(); //len = 514
        libff::Fr<ppT> o2(auxiliary_input[len / 2]);
        libff::Fr<ppT> o1(auxiliary_input[0]);

        libff::enter_block("Compute _C_x");
        libff::G1<ppT> _C_x;

        // snark_for_completment_auxiliary_input<ppT> completment_auxiliary_input;
        libff::G1_vector<ppT> L_query = {};

#ifdef MULTICORE
#pragma omp declare reduction(+                             \
                              : libff::G1 <ppT>             \
                              : omp_out = omp_in + omp_out) \
    initializer(omp_priv = libff::G1 <ppT>::zero())
#pragma omp parallel for reduction(+ \
                                   : _C_x)
#else
        _C_x = libff::G1<ppT>::zero();
#endif
        for (size_t i = 1; i < len / 2; i++)
        { //0 ~ n-1까지
            _C_x = _C_x + auxiliary_input[i + len / 2] * pk.f_vector[i + len / 2];
        }
        _C_x = _C_x + o2 * pk.f_vector[len / 2];
        libff::leave_block("Compute _C_x");

        libff::enter_block("Compute ss_proof");
        libff::G1<ppT> ss_proof_g1;
        libff::G1<ppT> ss_proof_g1_tmp;

#ifdef MULTICORE
#pragma omp parallel for reduction(+ \
                                   : ss_proof_g1)
#else
        ss_proof_g1 = libff::G1<ppT>::zero();
#endif
        for (size_t i = 0; i < len / 2 - 1; i++)
        { //0 ~ n-1까지
            ss_proof_g1 = ss_proof_g1 + auxiliary_input[i + 1] * pk.P_vector[i + 3];
        }

#ifdef MULTICORE
#pragma omp parallel for reduction(+ \
                                   : ss_proof_g1_tmp)
#else
        ss_proof_g1_tmp = libff::G1<ppT>::zero();
#endif
        for (size_t i = 0; i < len / 2 - 1; i++)
        { //0 ~ n-1까지
            ss_proof_g1_tmp = ss_proof_g1_tmp + auxiliary_input[i + len / 2 + 1] * pk.P_vector[i + len / 2 + 2];
        }

        ss_proof_g1 = ss_proof_g1 + ss_proof_g1_tmp;
        ss_proof_g1 = ss_proof_g1 + o1 * pk.P_vector[0];

        ss_proof_g1 = ss_proof_g1 + o2 * pk.P_vector[1];
        ss_proof_g1 = ss_proof_g1 + x0 * pk.P_vector[2];
        libff::leave_block("Compute ss_proof");

        snark_for_completment_proving_key<ppT> completment_pk = snark_for_completment_proving_key<ppT>(
            std::move(pk.alpha_g1),
            std::move(pk.beta_g1),
            std::move(pk.beta_g2),
            std::move(pk.delta_g1),
            std::move(pk.delta_g2),
            std::move(pk.A_query),
            std::move(pk.B_query),
            std::move(pk.H_query),
            std::move(L_query),
            std::move(pk.constraint_system));

        // completment_auxiliary_input.push_back(o1);
        // for(size_t i = 0; i < len/2; i++){//0 ~ n-1까지
        // 	completment_auxiliary_input.push_back(auxiliary_input[i]);
        // }
        // completment_auxiliary_input.push_back(o2);
        // for(size_t i = 0; i < len/2; i++){//0 ~ n-1까지
        // 	completment_auxiliary_input.push_back(auxiliary_input[len/2+i]);
        // }

        snark_for_completment_proof<ppT> completment_proof = snark_for_completment_prover<ppT>(
            std::move(completment_pk),
            std::move(primary_input),
            std::move(auxiliary_input));


        snark_for_filtering_proof<ppT> proof = snark_for_filtering_proof<ppT>(std::move(completment_proof), std::move(ss_proof_g1), std::move(_C_x));

        // snark_for_filtering_proof<ppT> proof
        //     = snark_for_filtering_proof<ppT>(std::move(completment_proof), std::move(ss_proof_g1), std::move(_C_x));

        proof.print_size();

        return proof;
    }

    template <typename ppT>
    snark_for_filtering_processed_verification_key<ppT> snark_for_filtering_verifier_process_vk(const snark_for_filtering_verification_key<ppT> &vk)
    {
        libff::enter_block("Call to snark_for_filtering_verifier_process_vk");

        snark_for_filtering_processed_verification_key<ppT> pvk;

        libff::G2<ppT> a_g2;
        libff::G2<ppT> c0_g2;
        libff::G2<ppT> c1_g2;
        libff::G2<ppT> c2_g2;
        libff::GT<ppT> vk_alpha_g1_beta_g2;
        libff::G2_precomp<ppT> vk_delta_g2_precomp;

        pvk.a_g2 = vk.a_g2;
        pvk.c0_g2 = vk.c0_g2;
        pvk.c1_g2 = vk.c1_g2;
        pvk.c2_g2 = vk.c2_g2;
        pvk.vk_alpha_g1_beta_g2 = vk.alpha_g1_beta_g2;
        //pvk.vk_gamma_g2_precomp = ppT::precompute_G2(vk.gamma_g2);
        pvk.vk_delta_g2_precomp = ppT::precompute_G2(vk.delta_g2);
        //pvk.gamma_ABC_g1 = vk.gamma_ABC_g1;

        libff::leave_block("Call to snark_for_completment_verifier_process_vk");

        return pvk;
    }

    template <typename ppT>
    bool snark_for_filtering_verifier(const snark_for_filtering_verification_key<ppT> &vk,
                                      const libff::G1<ppT> &sigma_x,
                                      const libff::G1<ppT> &C_x,
                                      const snark_for_filtering_proof<ppT> &proof)
    {

        // libff::enter_block("Call to snark for filtering verifier");

        libff::enter_block("pairing computations");
        libff::GT<ppT> left = ppT::reduced_pairing(proof.ss_proof_g1, vk.a_g2);
        libff::GT<ppT> right0 = ppT::reduced_pairing(C_x, vk.c2_g2);
        libff::GT<ppT> right1 = ppT::reduced_pairing(proof._C_x, vk.c1_g2);
        libff::GT<ppT> right2 = ppT::reduced_pairing(sigma_x, vk.c0_g2);

        snark_for_completment_verification_key<ppT> completment_vk = snark_for_completment_verification_key<ppT>(
            std::move(vk.alpha_g1_beta_g2),
            std::move(vk.delta_g2));

        // const bool ans =(left == (right0 *  right1 * right2);

        libff::enter_block("Check QAP divisibility");
        const bool ans = (left == (right0 * right1 * right2));
        printf("snark for filtering protocol: %s\n", (ans) ? "PASS" : "FAIL");
        libff::leave_block("Check QAP divisibility");
        libff::leave_block("pairing computations");

        const bool ans2 = snark_for_completment_verifier_weak_IC<ppT>(completment_vk, proof._C_x, C_x, proof.completment_proof);

        // libff::leave_block("Call to snark for filtering verifier");

        // return left == (right0 *  right1 * right2);
        return ans && ans2;
        // (left == (right0 *  right1 * right2) &&
        // // return snark_for_completment_verifier_weak_IC(completment_vk, proof._C_x, C_x, proof.completment_proof);
        //         snark_for_completment_verifier_weak_IC<ppT>(completment_vk, proof._C_x, C_x, proof.completment_proof));
    }
} // namespace libsnark

#endif
