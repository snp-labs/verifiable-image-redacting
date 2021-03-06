/** @file
*****************************************************************************

Declaration of interfaces for a ppzkSNARK for R1CS with a security proof
in the generic group (GG) model.

This includes:
- class for proving key
- class for verification key
- class for processed verification key
- class for key pair (proving key & verification key)
- class for proof
- generator algorithm
- prover algorithm
- verifier algorithm (with strong or weak input consistency)
- online verifier algorithm (with strong or weak input consistency)

The implementation instantiates the protocol of \[Gro16].


Acronyms:

- R1CS = "Rank-1 Constraint Systems"
- ppzkSNARK = "PreProcessing Zero-Knowledge Succinct Non-interactive ARgument of Knowledge"

References:

\[Gro16]:
 "On the Size of Pairing-based Non-interactive Arguments",
 Jens Groth,
 EUROCRYPT 2016,
 <https://eprint.iacr.org/2016/260>


*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#ifndef SNARK_FOR_COMPLETMENT_HPP_
#define SNARK_FOR_COMPLETMENT_HPP_

#include <memory>

#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/common/data_structures/accumulation_vector.hpp>
#include <libsnark/knowledge_commitment/knowledge_commitment.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment_params.hpp>
// #include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_filtering.hpp>

namespace libsnark {

/******************************** Proving key ********************************/

template<typename ppT>
class snark_for_completment_proving_key;

//입출력 연산 수정
template<typename ppT>
std::ostream& operator<<(std::ostream &out, const snark_for_completment_proving_key<ppT> &pk);

template<typename ppT>
std::istream& operator>>(std::istream &in, snark_for_completment_proving_key<ppT> &pk);

/**
 * A proving key for the R1CS GG-ppzkSNARK.
 * L_query 필요 없음
 */
template<typename ppT>
class snark_for_completment_proving_key {
public:
    libff::G1<ppT> alpha_g1;
    libff::G1<ppT> beta_g1;
    libff::G2<ppT> beta_g2;
    libff::G1<ppT> delta_g1;
    libff::G2<ppT> delta_g2;

    libff::G1_vector<ppT> A_query; // this could be a sparse vector if we had multiexp for those
    knowledge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > B_query;
    libff::G1_vector<ppT> H_query;
    libff::G1_vector<ppT> L_query;

    r1cs_constraint_system<libff::Fr<ppT> > constraint_system;

    snark_for_completment_proving_key() {};
    snark_for_completment_proving_key<ppT>& operator=(const snark_for_completment_proving_key<ppT> &other) = default;
    snark_for_completment_proving_key(const snark_for_completment_proving_key<ppT> &other) = default;
    snark_for_completment_proving_key(snark_for_completment_proving_key<ppT> &&other) = default;
    snark_for_completment_proving_key(libff::G1<ppT> &&alpha_g1,
                                  libff::G1<ppT> &&beta_g1,
                                  libff::G2<ppT> &&beta_g2,
                                  libff::G1<ppT> &&delta_g1,
                                  libff::G2<ppT> &&delta_g2,
                                  libff::G1_vector<ppT> &&A_query,
                                  knowledge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > &&B_query,
                                  libff::G1_vector<ppT> &&H_query,
                                  libff::G1_vector<ppT> &&L_query,
                                  r1cs_constraint_system<libff::Fr<ppT> > &&constraint_system) :
        alpha_g1(std::move(alpha_g1)),
        beta_g1(std::move(beta_g1)),
        beta_g2(std::move(beta_g2)),
        delta_g1(std::move(delta_g1)),
        delta_g2(std::move(delta_g2)),
        A_query(std::move(A_query)),
        B_query(std::move(B_query)),
        H_query(std::move(H_query)),
        L_query(std::move(L_query)),
        constraint_system(std::move(constraint_system))
    {};

    size_t G1_size() const
    {
        return 1 + A_query.size() + B_query.domain_size() + H_query.size() + L_query.size();
    }

    size_t G2_size() const
    {
        return 1 + B_query.domain_size();
    }

    size_t G1_sparse_size() const
    {
        return 1 + A_query.size() + B_query.size() + H_query.size()+ L_query.size();
    }

    size_t G2_sparse_size() const
    {
        return 1 + B_query.size();
    }

    size_t size_in_bits() const
    {
        return (libff::size_in_bits(A_query) + B_query.size_in_bits() +
                libff::size_in_bits(H_query) + libff::size_in_bits(L_query) +
                1 * libff::G1<ppT>::size_in_bits() + 1 * libff::G2<ppT>::size_in_bits());
    }

    void print_size() const
    {
        libff::print_indent(); printf("* G1 elements in PK: %zu\n", this->G1_size());
        libff::print_indent(); printf("* Non-zero G1 elements in PK: %zu\n", this->G1_sparse_size());
        libff::print_indent(); printf("* G2 elements in PK: %zu\n", this->G2_size());
        libff::print_indent(); printf("* Non-zero G2 elements in PK: %zu\n", this->G2_sparse_size());
        libff::print_indent(); printf("* PK size in bits: %zu\n", this->size_in_bits());
    }

    bool operator==(const snark_for_completment_proving_key<ppT> &other) const;
    friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_completment_proving_key<ppT> &pk);
    friend std::istream& operator>> <ppT>(std::istream &in, snark_for_completment_proving_key<ppT> &pk);
};

// template<typename ppT>
// class snark_for_completment_proving_key_with_out_L_query;

// //입출력 연산 수정
// template<typename ppT>
// std::ostream& operator<<(std::ostream &out, const snark_for_completment_proving_key_with_out_L_query<ppT> &pk);

// template<typename ppT>
// std::istream& operator>>(std::istream &in, snark_for_completment_proving_key_with_out_L_query<ppT> &pk);

// template<typename ppT>
// class snark_for_completment_proving_key_with_out_L_query {
// public:
//     libff::G1<ppT> alpha_g1;
//     libff::G1<ppT> beta_g1;
//     libff::G2<ppT> beta_g2;
//     libff::G1<ppT> delta_g1;
//     libff::G2<ppT> delta_g2;

//     libff::G1_vector<ppT> A_query; // this could be a sparse vector if we had multiexp for those
//     knowledge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > B_query;
//     libff::G1_vector<ppT> H_query;
//     // libff::G1_vector<ppT> L_query;

//     snark_for_completment_constraint_system<ppT> constraint_system;

//     snark_for_completment_proving_key_with_out_L_query() {};
//     snark_for_completment_proving_key_with_out_L_query<ppT>& operator=(const snark_for_completment_proving_key_with_out_L_query<ppT> &other) = default;
//     snark_for_completment_proving_key_with_out_L_query(const snark_for_completment_proving_key_with_out_L_query<ppT> &other) = default;
//     snark_for_completment_proving_key_with_out_L_query(snark_for_completment_proving_key_with_out_L_query<ppT> &&other) = default;
//     snark_for_completment_proving_key_with_out_L_query(libff::G1<ppT> &&alpha_g1,
//                                   libff::G1<ppT> &&beta_g1,
//                                   libff::G2<ppT> &&beta_g2,
//                                   libff::G1<ppT> &&delta_g1,
//                                   libff::G2<ppT> &&delta_g2,
//                                   libff::G1_vector<ppT> &&A_query,
//                                   knowledge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > &&B_query,
//                                   libff::G1_vector<ppT> &&H_query,
//                                 //   libff::G1_vector<ppT> &&L_query,
//                                   snark_for_completment_constraint_system<ppT> &&constraint_system) :
//         alpha_g1(std::move(alpha_g1)),
//         beta_g1(std::move(beta_g1)),
//         beta_g2(std::move(beta_g2)),
//         delta_g1(std::move(delta_g1)),
//         delta_g2(std::move(delta_g2)),
//         A_query(std::move(A_query)),
//         B_query(std::move(B_query)),
//         H_query(std::move(H_query)),
//         // L_query(std::move(L_query)),
//         constraint_system(std::move(constraint_system))
//     {};

//     size_t G1_size() const
//     {
//         return 1 + A_query.size() + B_query.domain_size() + H_query.size(); //+ L_query.size();
//     }

//     size_t G2_size() const
//     {
//         return 1 + B_query.domain_size();
//     }

//     size_t G1_sparse_size() const
//     {
//         return 1 + A_query.size() + B_query.size() + H_query.size(); //+ L_query.size();
//     }

//     size_t G2_sparse_size() const
//     {
//         return 1 + B_query.size();
//     }

//     size_t size_in_bits() const
//     {
//         return (libff::size_in_bits(A_query) + B_query.size_in_bits() +
//                 libff::size_in_bits(H_query) /*+ libff::size_in_bits(L_query)*/ +
//                 1 * libff::G1<ppT>::size_in_bits() + 1 * libff::G2<ppT>::size_in_bits());
//     }

//     void print_size() const
//     {
//         libff::print_indent(); printf("* G1 elements in PK: %zu\n", this->G1_size());
//         libff::print_indent(); printf("* Non-zero G1 elements in PK: %zu\n", this->G1_sparse_size());
//         libff::print_indent(); printf("* G2 elements in PK: %zu\n", this->G2_size());
//         libff::print_indent(); printf("* Non-zero G2 elements in PK: %zu\n", this->G2_sparse_size());
//         libff::print_indent(); printf("* PK size in bits: %zu\n", this->size_in_bits());
//     }

//     bool operator==(const snark_for_completment_proving_key_with_out_L_query<ppT> &other) const;
//     friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_completment_proving_key_with_out_L_query<ppT> &pk);
//     friend std::istream& operator>> <ppT>(std::istream &in, snark_for_completment_proving_key_with_out_L_query<ppT> &pk);
// };


/******************************* Verification key ****************************/

template<typename ppT>
class snark_for_completment_verification_key;

template<typename ppT>
std::ostream& operator<<(std::ostream &out, const snark_for_completment_verification_key<ppT> &vk);

template<typename ppT>
std::istream& operator>>(std::istream &in, snark_for_completment_verification_key<ppT> &vk);

/**
 * A verification key for the R1CS GG-ppzkSNARK.
 * L_query 필요 없음
 * gamma 관련 필요 없음
 */
template<typename ppT>
class snark_for_completment_verification_key {
public:
    libff::GT<ppT> alpha_g1_beta_g2;
    // libff::G2<ppT> gamma_g2;
    libff::G2<ppT> delta_g2;

    // accumulation_vector<libff::G1<ppT> > gamma_ABC_g1;

    snark_for_completment_verification_key() = default;
    snark_for_completment_verification_key(const libff::GT<ppT> &alpha_g1_beta_g2,
                                       //const libff::G2<ppT> &gamma_g2,
                                       const libff::G2<ppT> &delta_g2//,
                                       //const accumulation_vector<libff::G1<ppT> > &gamma_ABC_g1
                                       ) :
        alpha_g1_beta_g2(alpha_g1_beta_g2),
        //gamma_g2(gamma_g2),
        delta_g2(delta_g2)//,
       // gamma_ABC_g1(gamma_ABC_g1){}
       {};

    size_t G1_size() const
    {
        return 0; //gamma_ABC_g1.size();
    }

    size_t G2_size() const
    {
        return 2;
    }

    size_t GT_size() const
    {
        return 1;
    }

    size_t size_in_bits() const
    {
        // TODO: include GT size
        return (/*gamma_ABC_g1.size_in_bits() + */2 * libff::G2<ppT>::size_in_bits());
    }

    void print_size() const
    {
        libff::print_indent(); printf("* G1 elements in VK: %zu\n", this->G1_size());
        libff::print_indent(); printf("* G2 elements in VK: %zu\n", this->G2_size());
        libff::print_indent(); printf("* GT elements in VK: %zu\n", this->GT_size());
        libff::print_indent(); printf("* VK size in bits: %zu\n", this->size_in_bits());
    }

    bool operator==(const snark_for_completment_verification_key<ppT> &other) const;
    friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_completment_verification_key<ppT> &vk);
    friend std::istream& operator>> <ppT>(std::istream &in, snark_for_completment_verification_key<ppT> &vk);

    static snark_for_completment_verification_key<ppT> dummy_verification_key(const size_t input_size);
};


/************************ Processed verification key *************************/

template<typename ppT>
class snark_for_completment_processed_verification_key;

template<typename ppT>
std::ostream& operator<<(std::ostream &out, const snark_for_completment_processed_verification_key<ppT> &pvk);

template<typename ppT>
std::istream& operator>>(std::istream &in, snark_for_completment_processed_verification_key<ppT> &pvk);

/**
 * A processed verification key for the R1CS GG-ppzkSNARK.
 *
 * Compared to a (non-processed) verification key, a processed verification key
 * contains a small constant amount of additional pre-computed information that
 * enables a faster verification time.
 */
template<typename ppT>
class snark_for_completment_processed_verification_key {
public:
    libff::GT<ppT> vk_alpha_g1_beta_g2;
    //libff::G2_precomp<ppT> vk_gamma_g2_precomp;
    libff::G2_precomp<ppT> vk_delta_g2_precomp;
    libff::G2<ppT> delta_g2;


    //accumulation_vector<libff::G1<ppT> > gamma_ABC_g1;

    bool operator==(const snark_for_completment_processed_verification_key &other) const;
    friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_completment_processed_verification_key<ppT> &pvk);
    friend std::istream& operator>> <ppT>(std::istream &in, snark_for_completment_processed_verification_key<ppT> &pvk);
};


/********************************** Key pair *********************************/

/**
 * A key pair for the R1CS GG-ppzkSNARK, which consists of a proving key and a verification key.
 */
template<typename ppT>
class snark_for_completment_keypair {
public:
    snark_for_completment_proving_key<ppT> pk;
    snark_for_completment_verification_key<ppT> vk;

    snark_for_completment_keypair() = default;
    snark_for_completment_keypair(const snark_for_completment_keypair<ppT> &other) = default;
    snark_for_completment_keypair(snark_for_completment_proving_key<ppT> &&pk,
                              snark_for_completment_verification_key<ppT> &&vk) :
        pk(std::move(pk)),
        vk(std::move(vk))
    {}
    
    snark_for_completment_keypair(snark_for_completment_keypair<ppT> &&other) = default;
};


/*********************************** Proof ***********************************/

template<typename ppT>
class snark_for_completment_proof;

template<typename ppT>
std::ostream& operator<<(std::ostream &out, const snark_for_completment_proof<ppT> &proof);

template<typename ppT>
std::istream& operator>>(std::istream &in, snark_for_completment_proof<ppT> &proof);

/**
 * A proof for the R1CS GG-ppzkSNARK.
 *
 * While the proof has a structure, externally one merely opaquely produces,
 * serializes/deserializes, and verifies proofs. We only expose some information
 * about the structure for statistics purposes.
 */
template<typename ppT>
class snark_for_completment_proof {
public:
    libff::G1<ppT> g_A;
    libff::G2<ppT> g_B;
    libff::G1<ppT> g_C;

    snark_for_completment_proof()
    {
        // invalid proof with valid curve points
        this->g_A = libff::G1<ppT>::one();
        this->g_B = libff::G2<ppT>::one();
        this->g_C = libff::G1<ppT>::one();
    }
    snark_for_completment_proof(libff::G1<ppT> &&g_A,
                            libff::G2<ppT> &&g_B,
                            libff::G1<ppT> &&g_C) :
        g_A(std::move(g_A)),
        g_B(std::move(g_B)),
        g_C(std::move(g_C))
    {};

    size_t G1_size() const
    {
        return 2;
    }

    size_t G2_size() const
    {
        return 1;
    }

    size_t size_in_bits() const
    {
        return G1_size() * libff::G1<ppT>::size_in_bits() + G2_size() * libff::G2<ppT>::size_in_bits();
    }

   void print_size() const
    {
        libff::print_indent(); printf("* G1 elements in proof: %zu\n", this->G1_size());
        libff::print_indent(); printf("* G2 elements in proof: %zu\n", this->G2_size());
        libff::print_indent(); printf("* Proof size in bits: %zu\n", this->size_in_bits());
    }

    bool is_well_formed() const
    {
        return (g_A.is_well_formed() &&
                g_B.is_well_formed() &&
                g_C.is_well_formed());
    }

    bool operator==(const snark_for_completment_proof<ppT> &other) const;
    friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_completment_proof<ppT> &proof);
    friend std::istream& operator>> <ppT>(std::istream &in, snark_for_completment_proof<ppT> &proof);
};


/***************************** Main algorithms *******************************/

/**
 * A generator algorithm for the R1CS GG-ppzkSNARK.
 *
 * Given a R1CS constraint system CS, this algorithm produces proving and verification keys for CS.
 * 
 * primary: statement 없음
 */
// template<typename ppT>
// snark_for_completment_keypair<ppT> snark_for_completment_generator(const r1cs_constraint_system<libff::Fr<ppT> > &r1cs);
template <typename ppT>
snark_for_completment_keypair<ppT> snark_for_completment_generator(const r1cs_constraint_system<libff::Fr<ppT> > &r1cs,
                                                                    const libff::G1<ppT> &g1_generator,
                                                                    const libff::G2<ppT> &G2_gen);
/**
 * A prover algorithm for the R1CS GG-ppzkSNARK.
 *
 * Given a R1CS primary input X and a R1CS auxiliary input Y, this algorithm
 * produces a proof (of knowledge) that attests to the following statement:
 *               ``there exists Y such that CS(X,Y)=0''.
 * Above, CS is the R1CS constraint system that was given as input to the generator algorithm.
 */

//primary: statement
//auxiliary: witness
template<typename ppT>
snark_for_completment_proof<ppT> snark_for_completment_prover(snark_for_completment_proving_key<ppT> &pk,
                                                      const snark_for_completment_primary_input<ppT> &primary_input,
                                                      const snark_for_completment_auxiliary_input<ppT> &auxiliary_input);

/*
  Below are four variants of verifier algorithm for the R1CS GG-ppzkSNARK.

  These are the four cases that arise from the following two choices:

  (1) The verifier accepts a (non-processed) verification key or, instead, a processed verification key.
  In the latter case, we call the algorithm an "online verifier".

  (2) The verifier checks for "weak" input consistency or, instead, "strong" input consistency.
  Strong input consistency requires that |primary_input| = CS.num_inputs, whereas
  weak input consistency requires that |primary_input| <= CS.num_inputs (and
  the primary input is implicitly padded with zeros up to length CS.num_inputs).
*/

/**
 * A verifier algorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts a non-processed verification key, and
 * (2) has weak input consistency.
 * 
 * primary: statement 없음 
 * 대신 C_x, _C_x가 들어감
 */
template<typename ppT>
bool snark_for_completment_verifier_weak_IC(const snark_for_completment_verification_key<ppT> &vk,
                                        const snark_for_completment_primary_input<ppT> &primary_input,
                                        const libff::G1<ppT> C_x, const libff::G1<ppT> _C_x,
                                        const snark_for_completment_proof<ppT> &proof);

/**
 * A verifier algorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts a non-processed verification key, and
 * (2) has strong input consistency.
 */
template<typename ppT>
bool snark_for_completment_verifier_strong_IC(const snark_for_completment_verification_key<ppT> &vk,
                                          const snark_for_completment_primary_input<ppT> &primary_input,
                                          const libff::G1<ppT> C_x, const libff::G1<ppT> _C_x,
                                          const snark_for_completment_proof<ppT> &proof);

/**
 * Convert a (non-processed) verification key into a processed verification key.
 */
template<typename ppT>
snark_for_completment_processed_verification_key<ppT> snark_for_completment_verifier_process_vk(const snark_for_completment_verification_key<ppT> &vk);

/**
 * A verifier algorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts a processed verification key, and
 * (2) has weak input consistency.
 * primary: statement 없음 
 * 대신 C_x, _C_x가 들어감
 */
template<typename ppT>
bool snark_for_completment_online_verifier_weak_IC(const snark_for_completment_processed_verification_key<ppT> &pvk,
                                               const snark_for_completment_primary_input<ppT> &input,
                                               const libff::G1<ppT> C_x, const libff::G1<ppT> _C_x,
                                               const snark_for_completment_proof<ppT> &proof);

/**
 * A verifier algorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts a processed verification key, and
 * (2) has strong input consistency.
 */
template<typename ppT>
bool snark_for_completment_online_verifier_strong_IC(const snark_for_completment_processed_verification_key<ppT> &pvk,
                                                 const snark_for_completment_primary_input<ppT> &primary_input,
                                                 const libff::G1<ppT> C_x, const libff::G1<ppT> _C_x,
                                                 const snark_for_completment_proof<ppT> &proof);

/****************************** Miscellaneous ********************************/

/**
 * For debugging purposes (of snark_for_completment_snark_for_completment_verifier_gadget):
 *
 * A verifier algorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts a non-processed verification key,
 * (2) has weak input consistency, and
 * (3) uses affine coordinates for elliptic-curve computations.
 * 
 * primary: statement 없음 
 * 대신 C_x, _C_x가 들어감
 */
template<typename ppT>
bool snark_for_completment_affine_verifier_weak_IC(const snark_for_completment_verification_key<ppT> &vk,
                                            //    const snark_for_completment_primary_input<ppT> &primary_input,
                                               const libff::G1<ppT> C_x, const libff::G1<ppT> _C_x,
                                               const snark_for_completment_proof<ppT> &proof);


} // libsnark

#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment.tcc>

#endif // SNARK_FOR_COMPLETMENT_HPP_
