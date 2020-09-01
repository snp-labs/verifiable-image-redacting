#ifndef VERIFIABLE_IMAGE_REDACTING_TCC_
#define VERIFIABLE_IMAGE_REDACTING_TCC_

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

#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/r1cs_gg_ppzksnark_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/cc_SNARK.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/SE_cp_SNARK.hpp>

#include <openssl/sha.h>
#include "libbls_signatures.h"

namespace libsnark
{
    template <typename ppT>
    bool VIR_keypair<ppT>::operator==(const VIR_keypair<ppT> &other) const
    {
        return (this->pk == other.pk &&
                this->sk == other.sk
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_keypair<ppT> &keypair)
    {
        out << keypair.pk;
        out << keypair.sk;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_keypair<ppT> &keypair)
    {
        in >> keypair.pk;
        in >> keypair.sk;

        return in;
    }

    template <typename ppT>
    bool VIR_keypair_pk<ppT>::operator==(const VIR_keypair_pk<ppT> &other) const
    {
        return (this->ck == other.ck &&
                this->BLS_pk == other.BLS_pk
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_keypair_pk<ppT> &pk)
    {
        out << pk.ck;
        out << pk.BLS_pk;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_keypair_pk<ppT> &pk)
    {
        in >> pk.ck;
        in >> pk.BLS_pk;

        return in;
    }

    template <typename ppT>
    bool VIR_keypair_sk<ppT>::operator==(const VIR_keypair_sk<ppT> &other) const
    {
        return (this->ck == other.ck &&
                this->BLS_sk == other.BLS_sk
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_keypair_sk<ppT> &sk)
    {
        out << sk.ck;
        out << sk.BLS_sk;
        
        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_keypair_sk<ppT> &sk)
    {
        in >> sk.ck;
        in >> vk.BLS_sk;
        
        return in;
    }

    template <typename ppT>
    bool commitment<ppT>::operator==(const commitment<ppT> &other) const
    {
        return (this->m0 == other.m0 &&
                this->psi_m == other.psi_m);
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const commitment<ppT> &commit)
    {
        out << commit.m0 << OUTPUT_NEWLINE;
        out << commit.psi_m << OUTPUT_NEWLINE;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const commitment<ppT> &commit)
    {
        in >> commit.m0;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> commit.psi_m;
        libff::consume_OUTPUT_NEWLINE(in);

        return in;
    }

    template <typename ppT>
    bool VIR_authentication<ppT>::operator==(const VIR_authentication<ppT> &other) const
    {
        return (this->sigma == other.sigma &&
                this->commitment == other.commitment
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_authentication<ppT> &auth)
    {
        out << auth.sigma;
        out << auth.commitment;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_authentication<ppT> &auth)
    {
        in >> auth.sigma;
        in >> auth.commitment;

        return in;
    }

    template <typename ppT>
    bool VIR_CRS_without_Trapdoor<ppT>::operator==(const VIR_CRS_without_Trapdoor<ppT> &other) const
    {
        return (this->cc_crs == other.cc_crs &&
                this->cp_crs == other.cp_crs
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_CRS_without_Trapdoor<ppT> &crs)
    {
        out << crs.cc_crs;
        out << crs.cp_crs;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_CRS_without_Trapdoor<ppT> &crs)
    {
        in >> crs.cc_crs;
        in >> crs.cp_crs;

        return in;
    }

    template <typename ppT>
    bool VIR_Trapdoor<ppT>::operator==(const VIR_Trapdoor<ppT> &other) const
    {
        return (this->cc_crs_trapdoor == other.cc_crs_trapdoor &&
                this->cp_crs_trapdoor == other.cp_crs_trapdoor
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_Trapdoor<ppT> &td)
    {
        out << td.cc_crs_trapdoor;
        out << td.cp_crs_trapdoor;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_Trapdoor<ppT> &td)
    {
        in >> td.cc_crs_trapdoor;
        in >> td.cp_crs_trapdoor;

        return in;
    }

    template <typename ppT>
    bool VIR_CRS<ppT>::operator==(const VIR_CRS<ppT> &other) const
    {
        return (this->VIR_crs == other.VIR_crs &&
                this->trapdoor == other.trapdoor
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_CRS<ppT> &crs)
    {
        out << crs.VIR_crs;
        out << crs.trapdoor;

        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_CRS<ppT> &crs)
    {
        in >> crs.VIR_crs;
        in >> crs.trapdoor;

        return in;
    }

    template <typename ppT>
    bool VIR_proof<ppT>::operator==(const VIR_proof<ppT> &other) const
    {
        return (this->g_A == other.g_A &&
                this->g_B == other.g_B &&
                this->g_C == other.g_C &&
                this->SE_cp_proof_first == other.SE_cp_proof_first &&
                this->SE_cp_proof_second == other.SE_cp_proof_second &&
                this->_C_x == other._C_x
                );
    }
    template <typename ppT>
    std::ostream &operator<<(std::ostream &out, const VIR_proof<ppT> &proof)
    {
        out << proof.g_A << OUTPUT_NEWLINE;
        out << proof.g_B << OUTPUT_NEWLINE;
        out << proof.g_C << OUTPUT_NEWLINE;
        out << proof.SE_cp_proof_first << OUTPUT_NEWLINE;
        out << proof.SE_cp_proof_second << OUTPUT_NEWLINE;
        out << proof._C_x << OUTPUT_NEWLINE;
        
        return out;
    }
    template <typename ppT>
    std::istream &operator>>(std::istream &in, const VIR_proof<ppT> &proof)
    {
        in >> proof.g_A;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> proof.g_B;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> proof.g_C;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> proof.SE_cp_proof_first;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> proof.SE_cp_proof_second;
        libff::consume_OUTPUT_NEWLINE(in);
        in >> proof._C_x;
        libff::consume_OUTPUT_NEWLINE(in);
        
        return in;
    }

    template <typename ppT>
    VIR_keypair<ppT> VIR_Keygen(const size_t &u_size){

        // const libff::G1<ppT> G1_gen = libff::G1<ppT>::random_element();
        // const libff::G2<ppT> G2_gen = libff::G2<ppT>::random_element();

        // libff::G1_vector<ppT> ck = CommitmentKeyGen<ppT>(G1_gen, u_size);
        libff::G1_vector<ppT> ck = CommitmentKeyGen<ppT>(u_size);
        // BLS_keypair<ppT> BLS_keypair = BLS_Keygen<ppT>(G1_gen);
        PrivateKeyGenerateResponse BLS_sk = private_key_generate();
        PrivateKeyPublicKeyResponse BLS_pk = private_key_public_key(&BLS_sk);

        VIR_keypair_pk<ppT> pk = VIR_keypair_pk<ppT>(
            std::move(ck),
            std::move(BLS_pk)
            );
        VIR_keypair_sk<ppT> sk = VIR_keypair_sk<ppT>(
            std::move(ck),
            std::move(BLS_sk)
            );
        pk.print_size();
        sk.print_size();

        return VIR_keypair<ppT>(
            std::move(pk),
            std::move(sk)
            );
    }

    template <typename ppT>
    libff::G1_vector<ppT> CommitmentKeyGen(/*const libff::G1<ppT> &G1_gen,*/ 
                                           const size_t &num_variables){
        const libff::G1<ppT> G1_gen = libff::G1<ppT>::random_element();
        libff::G1_vector<ppT> h_g1_vector;
        libff::Fr_vector<ppT> h_vector;

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
        libff::window_table<libff::G1<ppT>> g1_table = libff::get_window_table(g1_scalar_size, g1_window_size, G1_gen);
        libff::leave_block("Generating G1 MSM window table");

        h_g1_vector = batch_exp(g1_scalar_size, g1_window_size, g1_table, h_vector);
        printf("offline key size: %zu\n", libff::size_in_bits(h_g1_vector));
        libff::leave_block("Compute the h_g1_vector", false);

        return h_g1_vector;
    }

    // template <typename ppT>
    // BLS_keypair<ppT> BLS_Keygen(const libff::G2<ppT> &G2_gen){

    //     libff::Fr<ppT> x = libff::Fr<ppT>::random_element();
    //     libff::G2<ppT> y = x * G2_gen;

    //     return BLS_keypair<ppT>(
    //         std::move(y),   //pk
    //         std::move(x)    //sk
    //         );
    // }

    // template <typename ppT>
    // libff::G1<ppT> Hash_to_G1<ppT>(const commitment<ppT> commitment){

    // }

    // template <typename ppT>
    // libff::G1<ppT> BLS_Sign(const commitment<ppT> commitment, const libff::Fr<ppT> BLS_sk){

    //     libff::G1<ppT> h = Hash_to_G1<ppT>(commitment);
        
    //     return BLS_sk * h;
    // }

    // template <typename ppT>
    // bool BLS_Verify(const libff::G2<ppT> &G2_gen, libff::G1<ppT> sigma, const commitment<ppT> commitment, const libff::G2<ppT> BLS_pk){
    
    //     libff::G1<ppT> h = Hash_to_G1<ppT>(commitment);
    //     libff::GT<ppT> left = ppT::reduced_pairing(h, BLS_pk);
    //     libff::GT<ppT> right = ppT::reduced_pairing(sigma, G2_gen);

    //     const bool ans = (left == right);

    //     return ans;
    // }
    template <typename ppT>
    commitment<ppT> Commit(const libff::G1_vector<ppT> &ck,
                           const std::vector<libff::Fr<ppT>>  &m){
        
        libff::G1<ppT> m0 = libff::G1<ppT>::random_element();
        libff::G1<ppT> psi_m;
        size_t len = m.size();
        
#ifdef MULTICORE
#pragma omp declare reduction(+                             \
                              : libff::G1 <ppT>             \
                              : omp_out = omp_in + omp_out) \
    initializer(omp_priv = libff::G1 <ppT>::zero())
#pragma omp parallel for reduction(+ : psi_m)
#else
        psi_m = libff::G1<ppT>::zero();
#endif
        for (size_t i = 0; i < len; i++)
        {
            psi_m = psi_m + m[i] * ck[i + 1];
        }

        psi_m = psi_m + m0 * pp.h_vector[0];

        printf("psi_m size: %zu\n", libff::G1<ppT>::size_in_bits());

        return commitment<ppT>(
            std::move(m0),
            std::move(psi_m)
            );
    }

    template <typename ppT>
    VIR_authentication<ppT> VIR_Authenticate(const std::vector<libff::Fr<ppT>>  &m,
                                             const uint8_t &raw_private_key_ptr){
        
        commitment<ppT> commitment = Commit<ppT>(sk.ck, m);
        uint8_t *message = commitment.psi_m.coord;
        PrivateKeySignResponse sigma = private_key_sign(&raw_private_key_ptr, &message, message.size());

        return VIR_authentication<ppT>(
            std::move(sigma),
            std::move(commitment)
            );
    }

    template <typename ppT>
    VIR_CRS<ppT> VIR_Setup(const VIR_keypair_pk<ppT> pk,
                           const r1cs_constraint_system<libff::Fr<ppT>> &r1cs)
    {
        libff::enter_block("Call to VIR_Setup");

        const libff::G1<ppT> G1_gen = libff::G1<ppT>::random_element();
        const libff::G2<ppT> G2_gen = libff::G2<ppT>::random_element();

        libff::enter_block("Commit and Carry zkSNARKs Setup");
        cc_CRS<ppT> cc_crs = cc_SNARK_Setup<ppT>(r1cs, G1_gen, G2_gen);
        libff::leave_block("Commit and Carry zkSNARKs Setup");
        libff::G1_vector<ppT> f_vector = cc_CRS.CRS.L_query;

        libff::enter_block("Simulation-Extractable Commit and Prove zkSNARKs Setup");
        SE_cp_CRS<ppT> cp_crs = SE_cp_SNARK_Setup<ppT>(pk.ck, f_vector);
        libff::leave_block("Simulation-Extractable Commit and Prove zkSNARKs Setup");

        VIR_CRS_without_Trapdoor<ppT> VIR_crs = VIR_CRS_without_Trapdoor<ppT>(
            std::move(cc_crs),
            std::move(cp_crs)
            );
        VIR_Trapdoor<ppT> trapdoor = VIR_Trapdoor<ppT>(
            std::move(cc_crs.trapdoor),
            std::move(cp_crs.trapdoor)
            );

        libff::leave_block("Call to VIR_Setup");

        VIR_crs.print_size();
        trapdoor.print_size();

        return VIR_CRS<ppT>(std::move(VIR_crs), std::move(trapdoor));
        // return;
    }

    template <typename ppT>
    VIR_proof<ppT> VIR_prove(const VIR_CRS<ppT> &crs,
                             const VIR_authentication<ppT> &authentication,
                             const PrivateKeyPublicKeyResponse &BLS_pk,
                             const r1cs_primary_input<libff::Fr<ppT>> &u,
                             const r1cs_primary_input<libff::Fr<ppT>> &u_)
    {
    
        libff::enter_block("Call to VIR_prove");

        libff::enter_block("BLS signature Verify");
        uint8_t *message = authentication.commitment.psi_m.coord;
        const int ans = verify(&authentication.sigma, &message, message.size(), &BLS_pk, BLS_pk.size());
        libff::leave_block("BLS signature Verify");
        if(! ans) exit(0);  // verify faile

        libff::enter_block("Commit and Carry zkSNARKs Prove");
        cc_proof<ppT> cc_proof = cc_SNARK_Prove<ppT>(crs.cc_crs, u, u_);
        libff::leave_block("Commit and Carry zkSNARKs Prove");

        libff::enter_block("Compute C_m");
        libff::G1<ppT> C_m = libff::G1<ppT>::zero();
        const size_t len = u.size();
#ifdef MULTICORE
#pragma omp declare reduction (+ : libff::G1<ppT> : omp_out = omp_in + omp_out)
    #pragma omp parallel for reduction(+ : C_m)
#endif    
        for(size_t i = 1; i < len; i++){//1 ~ 256
		    C_m = C_m + u[i] * crs.cp_crs.f_vector[i];
        }
        libff::leave_block("Compute C_m");

        libff::enter_block("Simulation-Extractable Commit and Prove zkSNARKs Prove");
        SE_cp_proof<ppT> SE_cp_proof = SE_cp_SNARK_Prove<ppT>(crs.cp_crs, C_m, cc_proof._C_m, authentication.commitment.psi_m, cc_proof.r, commitment.m0, u, u_);
        libff::leave_block("Simulation-Extractable Commit and Prove zkSNARKs Prove");

        libff::leave_block("Call to VIR_prove");

        VIR_proof<ppT> proof = VIR_proof<ppT>(std::move(cc_proof.g_A), std::move(cc_proof.g_B), 
                                              std::move(cc_proof.g_C), std::move(SE_cp_proof.first), 
                                              std::move(SE_cp_proof.second), std::move(cc_proof._C_m));

        proof.print_size();

        return proof;
    }

    template <typename ppT>
    bool VIR_verify(const VIR_CRS_without_Trapdoor<ppT> &crs,
                    const PrivateKeyPublicKeyResponse &BLS_pk,
                    const r1cs_primary_input<libff::Fr<ppT>> &u,
                    const r1cs_primary_input<libff::Fr<ppT>> &u_,
                    const VIR_authentication<ppT> &authentication,
                    const VIR_proof<ppT> &proof)
    {

        libff::enter_block("Call to VIR_verify");

        libff::enter_block("BLS signature Verify");
        uint8_t *message = commitment.psi_m.coord;
        const int ans = verify(&authentication.sigma, &message, message.size(), &BLS_pk, BLS_pk.size());
        libff::leave_block("BLS signature Verify");
        if(! ans) exit(0);  // verify faile

        libff::enter_block("Commit and Carry zkSNARKs Verify");
        const bool cc_ans = cc_SNARK_Verifye<ppT>(crs.cc_crs, u, u_);
        libff::leave_block("Commit and Carry zkSNARKs Verify");

        printf("Commit and Carry zkSNARKs protocol: %s\n", (cc_ans) ? "PASS" : "FAIL");
        
        libff::enter_block("Compute C_m");
        libff::G1<ppT> C_m = libff::G1<ppT>::zero();
        const size_t len = u.size();
#ifdef MULTICORE
#pragma omp declare reduction (+ : libff::G1<ppT> : omp_out = omp_in + omp_out)
    #pragma omp parallel for reduction(+ : C_m)
#endif    
        for(size_t i = 1; i < len; i++){//1 ~ 256
		    C_m = C_m + u[i] * crs.cp_crs.f_vector[i];
        }
        libff::leave_block("Compute C_m");

        libff::enter_block("Simulation-Extractable Commit and Prove zkSNARKs Verify");
        const bool SE_cp_ans = SE_cp_SNARK_Verifye<ppT>(crs.cp_crs, authentication.sigma C_m, proof.cc_proof._C_m, proof.SE_cp_proof);
        libff::leave_block("Simulation-Extractable Commit and Prove zkSNARKs Verify");

        printf("Simulation-Extractable Commit and Prove zkSNARKs protocol: %s\n", (cc_ans) ? "PASS" : "FAIL");

        // return left == (right0 *  right1 * right2);
        return ans && cc_ans && SE_cp_ans;
        // (left == (right0 *  right1 * right2) &&
        // // return snark_for_completment_verifier_weak_IC(completment_vk, proof._C_x, C_x, proof.completment_proof);
        //         snark_for_completment_verifier_weak_IC<ppT>(completment_vk, proof._C_x, C_x, proof.completment_proof));
    }
} // namespace libsnark

#endif
