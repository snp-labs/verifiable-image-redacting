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

#ifndef R1CS_GG_PPZKSNARK_HPP_
#define R1CS_GG_PPZKSNARK_HPP_

#include <memory>

#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/common/data_structures/accumulation_vector.hpp>
#include <libsnark/knowledge_commitment/knowledge_commitment.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/r1cs_gg_ppzksnark_params.hpp>

namespace libsnark {

    /******************************** Proving key ********************************/

    template<typename ppT>
    class r1cs_gg_ppzksnark_proving_key;

    //입출력 연산 수정
template<typen    ame ppT>
std::ostream&     operator<<(std::ostream &out, const r1cs_gg_ppzksnark_proving_key<ppT> &pk);

template<typen    ame ppT>
std::istream&     operator>>(std::istream &in, r1cs_gg_ppzksnark_proving_key<ppT> &pk);

/**
 * A provi    ng k     y for the R1CS GG-ppzkSNARK.
 */
template<t     pen    ame ppT>
class r1cs_gg_    ppzksnark_proving_key {
public:
    li    bff::G1<         alpha_g1;
    libff::G1<         beta_g1;
    libff::G2<         beta_g2;
    libff::G1<         delta_g1;
    libff::G2<         delta_g2;

    libff::G1_        or<ppT> A_query; // this could be a sparse vector if we had multiexp for those
    knowledge_        itment_vector<libff::G2<ppT>, libff::G1<ppT> > B_query;
    libff::G1_        or<ppT> H_query;
    libff::G1_        or<ppT> L_query;

    r1cs_gg_pp        ark_constraint_system<ppT> constraint_system;

    r1cs_gg_pp        ark_proving_key() {};
    r1cs_gg_pp        ark_proving_key<ppT>& operator=(const r1cs_gg_ppzksnark_proving_key<ppT> &other) = default;
    r1cs_gg_pp        ark_proving_key(const r1cs_gg_ppzksnark_proving_key<ppT> &other) = default;
    r1cs_gg_pp        ark_proving_key(r1cs_gg_ppzksnark_proving_key<ppT> &&other) = default;
    r1cs_gg_pp        ark_proving_key(libff::G1<ppT> &&alpha_g1,
                           &&beta_g1,
                           &&beta_g2,
                           &&delta_g1,
                           &&delta_g2,
                          or<ppT> &&A_query,
                          itment_vector<libff::G2<ppT>, libff::G1<ppT> > &&B_query,
                          or<ppT> &&H_query,
                          or<ppT> &&L_query,
                          ark_constraint_system<ppT> &&constraint_system) :
        alpha_            move(alpha_g1)),
        beta_g            ove(beta_g1)),
        beta_g            ove(beta_g2)),
        delta_            move(delta_g1)),
        delta_            move(delta_g2)),
        A_quer            ove(A_query)),
        B_quer            ove(B_query)),
        H_quer            ove(H_query)),
        L_quer            ove(L_query)),
        constr            tem(std::move(constraint_system))
    {};

    s        t G1_        () const
    {
                rn            uery.size() + B_query.domain_size() + H_query.size() + L_query.size();
    }

    siz        G2_        () const
    {
                rn            uery.domain_size();
    }

    siz        G1_        se_size() const
    {
                rn            uery.size() + B_query.size() + H_query.size() + L_query.size();
    }

    siz        G2_        se_size() const
    {
                rn            uery.size();
    }

    siz        siz        _bits() const
    {
                rn            :size_in_bits(A_query) + B_query.size_in_bits() +
                libff::size_in_bits(H_query) + libff::size_in_bits(L_query) +
                1 * libff::G1<ppT>::size_in_bits() + 1 * libff::G2<ppT>::size_in_bits());
    }

    voi        int        e() const
    {
                f:            ndent(); printf("* G1 elements in PK: %zu\n", this->G1_size());
        libff:            ndent(); printf("* Non-zero G1 elements in PK: %zu\n", this->G1_sparse_size());
        libff:            ndent(); printf("* G2 elements in PK: %zu\n", this->G2_size());
        libff:            ndent(); printf("* Non-zero G2 elements in PK: %zu\n", this->G2_sparse_size());
        libff:            ndent(); printf("* PK size in bits: %zu\n", this->size_in_bits());
    }

    boo        era        =(const r1cs_gg_ppzksnark_proving_key<ppT> &other) const;
    friend std        tream& operator<< <ppT>(std::ostream &out, const r1cs_gg_ppzksnark_proving_key<ppT> &pk);
    friend std        tream& operator>> <ppT>(std::istream &in, r1cs_gg_ppzksnark_proving_key<ppT> &pk);
};


/********    *****    ****************** Verification key ****************************/

template<typen    ame ppT>
class r1cs_gg_    ppzksnark_verification_key;

template<typen    ame ppT>
std::ostream&     operator<<(std::ostream &out, const r1cs_gg_ppzksnark_verification_key<ppT> &vk);

template<typen    ame ppT>
std::istream&     operator>>(std::istream &in, r1cs_gg_ppzksnark_verification_key<ppT> &vk);

/**
 * A verif    icat     on key for the R1CS GG-ppzkSNARK.
 */
template<t     pen    ame ppT>
class r1cs_gg_    ppzksnark_verification_key {
public:
    li    bff::GT<         alpha_g1_beta_g2;
    libff::G2<         gamma_g2;
    libff::G2<         delta_g2;

    accumulati        ector<libff::G1<ppT> > gamma_ABC_g1;

    r1cs_gg_pp        ark_verification_key() = default;
    r1cs_gg_pp        ark_verification_key(const libff::GT<ppT> &alpha_g1_beta_g2,
                          2<ppT> &gamma_g2,
                          2<ppT> &delta_g2
                          lation_vector<libff::G1<ppT> > &gamma_ABC_g1
                      pha_            g2(alpha_g1_beta_g2),
        gamma_            _g2),
        delta_            _g2)
        , gamm            (gamma_ABC_g1)
    {};

    s        t G1_        () const
    {
                rn            BC_g1.size();
    }

    siz        G2_        () const
    {
                rn            }

    siz        GT_        () const
    {
                rn            }

    siz        siz        _bits() const
    {
                OD            de GT size
        return            ABC_g1.size_in_bits() + 2 * libff::G2<ppT>::size_in_bits());
    }

    voi        int        e() const
    {
                f:            ndent(); printf("* G1 elements in VK: %zu\n", this->G1_size());
        libff:            ndent(); printf("* G2 elements in VK: %zu\n", this->G2_size());
        libff:            ndent(); printf("* GT elements in VK: %zu\n", this->GT_size());
        libff:            ndent(); printf("* VK size in bits: %zu\n", this->size_in_bits());
    }

    boo        era        =(const r1cs_gg_ppzksnark_verification_key<ppT> &other) const;
    friend std        tream& operator<< <ppT>(std::ostream &out, const r1cs_gg_ppzksnark_verification_key<ppT> &vk);
    friend std        tream& operator>> <ppT>(std::istream &in, r1cs_gg_ppzksnark_verification_key<ppT> &vk);

    static r1c        _ppzksnark_verification_key<ppT> dummy_verification_key(const size_t input_size);
};


/********    *****    *********** Processed verification key *************************/

template<typen    ame ppT>
class r1cs_gg_    ppzksnark_processed_verification_key;

template<typen    ame ppT>
std::ostream&     operator<<(std::ostream &out, const r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk);

template<typen    ame ppT>
std::istream&     operator>>(std::istream &in, r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk);

/**
 * A proce    ssed     verification key for the R1CS GG-ppzkSNARK.
 *
 * Compared     to     a (non-processed) verification key, a processed verification key
 * contains a      mall constant amount of additional pre-computed information that
 * enables a f     ster verification time.
 */
template<t     pen    ame ppT>
class r1cs_gg_    ppzksnark_processed_verification_key {
public:
    li    bff::GT<         vk_alpha_g1_beta_g2;
    libff::G2_        omp<ppT> vk_gamma_g2_precomp;
    // libff::        pT> vk_gamma_g2;
    libff::G2_        omp<ppT> vk_delta_g2_precomp;
    // libff::        pT> vk_delta_g2;

    accumulati        ector<libff::G1<ppT> > gamma_ABC_g1;

    bool opera        =(const r1cs_gg_ppzksnark_processed_verification_key &other) const;
    friend std        tream& operator<< <ppT>(std::ostream &out, const r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk);
    friend std        tream& operator>> <ppT>(std::istream &in, r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk);
};


/********    *****    ********************* Key pair *********************************/

/**
 * A key p    air      or the R1CS GG-ppzkSNARK, which consists of a proving key and a verification key.
 */
template<t     pen    ame ppT>
class r1cs_gg_    ppzksnark_keypair {
public:
    r1    cs_gg_pp        ark_proving_key<ppT> pk;
    r1cs_gg_pp        ark_verification_key<ppT> vk;

    r1cs_gg_pp        ark_keypair() = default;
    r1cs_gg_pp        ark_keypair(const r1cs_gg_ppzksnark_keypair<ppT> &other) = default;
    r1cs_gg_pp        ark_keypair(r1cs_gg_ppzksnark_proving_key<ppT> &&pk,
                          ark_verification_key<ppT> &&vk) :
        pk(std            k)),
        vk(std            k))
    {}

    r1        g_pp        ark_keypair(r1cs_gg_ppzksnark_keypair<ppT> &&other) = default;
};


/********    *****    ********************** Proof ***********************************/

template<typen    ame ppT>
class r1cs_gg_    ppzksnark_proof;

template<typen    ame ppT>
std::ostream&     operator<<(std::ostream &out, const r1cs_gg_ppzksnark_proof<ppT> &proof);

template<typen    ame ppT>
std::istream&     operator>>(std::istream &in, r1cs_gg_ppzksnark_proof<ppT> &proof);

/**
 * A proof     for     the R1CS GG-ppzkSNARK.
 *
 * While th      p     oof has a structure, externally one merely opaquely produces,
 * serializes/     eserializes, and verifies proofs. We only expose some information
 * about the s     ructure for statistics purposes.
 */
template<t     pen    ame ppT>
class r1cs_gg_    ppzksnark_proof {
public:
    li    bff::G1<         g_A;
    libff::G2<         g_B;
    libff::G1<         g_C;

    r1cs_gg_pp        ark_proof()
    {
                nv            of with valid curve points
        this->            bff::G1<ppT>::one();
        this->            bff::G2<ppT>::one();
        this->            bff::G1<ppT>::one();
    }
    r1cs        pp        ark_proof(libff::G1<ppT> &&g_A,
                           &&g_B,
                           &&g_C) :
        g_A(st            g_A)),
        g_B(st            g_B)),
        g_C(st            g_C))
    {};

    s        t G1_        () const
    {
                rn            }

    siz        G2_        () const
    {
                rn            }

    siz        siz        _bits() const
    {
                rn            () * libff::G1<ppT>::size_in_bits() + G2_size() * libff::G2<ppT>::size_in_bits();
    }

    voi        int        e() const
    {
                f:            ndent(); printf("* G1 elements in proof: %zu\n", this->G1_size());
        libff:            ndent(); printf("* G2 elements in proof: %zu\n", this->G2_size());
        libff:            ndent(); printf("* Proof size in bits: %zu\n", this->size_in_bits());
    }

    boo        _we        ormed() const
    {
                rn            _well_formed() &&
                g_B.is_well_formed() &&
                g_C.is_well_formed());
    }

    boo        era        =(const r1cs_gg_ppzksnark_proof<ppT> &other) const;
    friend std        tream& operator<< <ppT>(std::ostream &out, const r1cs_gg_ppzksnark_proof<ppT> &proof);
    friend std        tream& operator>> <ppT>(std::istream &in, r1cs_gg_ppzksnark_proof<ppT> &proof);
};


/********    *****    **************** Main algorithms *******************************/

/**
 * A gener    ator     algorithm for the R1CS GG-ppzkSNARK.
 *
 * Given a      1C      constraint system CS, this algorithm produces proving and verification keys for CS.
 */
template<t     pen    ame ppT>
r1cs_gg_ppzksn    ark_keypair<ppT> r1cs_gg_ppzksnark_generator(const r1cs_gg_ppzksnark_constraint_system<ppT> &cs);

/**
 * A prove    r al     orithm for the R1CS GG-ppzkSNARK.
 *
 * Given a      1C      primary input X and a R1CS auxiliary input Y, this algorithm
 * produces a      roof (of knowledge) that attests to the following statement:
 *                   ``there exists Y such that CS(X,Y)=0''.
 * Above, CS i      the R1CS constraint system that was given as input to the generator algorithm.
 */

//primary      wit     ness
//auxiliary: s     tatement
template<typen    ame ppT>
r1cs_gg_ppzksn    ark_proof<ppT> r1cs_gg_ppzksnark_prover(const r1cs_gg_ppzksnark_proving_key<ppT> &pk,
                      ppzksnark_primary_input<ppT> &primary_input,
                      ppzksnark_auxiliary_input<ppT> &auxiliary_input);

/*
  Below are     fo       variants of verifier algorithm for the R1CS GG-ppzkSNARK.

  These are th      four cases that arise from the following two choices:

  (1) The veri      er accepts a (non-processed) verification key or, instead, a processed verification key.
  In the latte      case, we call the algorithm an "online verifier".

  (2) The veri      er checks for "weak" input consistency or, instead, "strong" input consistency.
  Strong input      onsistency requires that |primary_input| = CS.num_inputs, whereas
  weak input c      sistency requires that |primary_input| <= CS.num_inputs (and
  the primary       put is implicitly padded with zeros up to length CS.num_inputs).
*/

/**
 * A v    erif    ier      lgorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts     a non-processed verification key, and
 * (2) has wea      input consistency.
 */
template<t     pen    ame ppT>
bool r1cs_gg_p    pzksnark_verifier_weak_IC(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
                      gg_ppzksnark_primary_input<ppT> &primary_input,
                      1<ppT> C_x, const libff::G1<ppT> _C_x,
                      ppzksnark_proof<ppT> &proof);

/**
 * A verif    ier      lgorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts     a non-processed verification key, and
 * (2) has str     ng input consistency.
 */
// templat     <ty     pename ppT>
// bool r1cs_g     g_ppzksnark_verifier_strong_IC(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
//                                                const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
//                                                const r1cs_gg_ppzksnark_proof<ppT> &proof);

/**
 * Convert      a (      on-processed) verification key into a processed verification key.
 */
template<t      pen    ame ppT>
r1cs_gg_ppzksn    ark_processed_verification_key<ppT> r1cs_gg_ppzksnark_verifier_process_vk(const r1cs_gg_ppzksnark_verification_key<ppT> &vk);

/**
 * A verif    ier      lgorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts     a processed verification key, and
 * (2) has wea      input consistency.
 */
template <     ype    name ppT>
bool r1cs_gg_p    pzksnark_online_verifier_weak_IC(const r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk,
                      cs_gg_ppzksnark_primary_input<ppT> &primary_input,
                      1<ppT> C_x, const libff::G1<ppT> _C_x,
                      ppzksnark_proof<ppT> &proof);

/**
 * A verif    ier      lgorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts     a processed verification key, and
 * (2) has str     ng input consistency.
 */
// templat     <ty     pename ppT>
// bool r1cs_g     g_ppzksnark_online_verifier_strong_IC(const r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk,
//                                                       const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
//                                                       const r1cs_gg_ppzksnark_proof<ppT> &proof);

/*************     ***************** Miscellaneous ********************************/

/**
 * For deb     uggi      g purposes (of r1cs_gg_ppzksnark_r1cs_gg_ppzksnark_verifier_gadget):
 *
 * A verifi      r       lgorithm for the R1CS GG-ppzkSNARK that:
 * (1) accepts      a non-processed verification key,
 * (2) has wea       input consistency, and
 * (3) uses af      ine coordinates for elliptic-curve computations.
 */
// templat      <ty      pename ppT>
// bool r1cs_g      g_ppzksnark_affine_verifier_weak_IC(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
//                                                      const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
//                                                      const r1cs_gg_ppzksnark_proof<ppT> &proof);


} // libsnark

#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.tcc>

#endif // R1CS_GG_PPZKSNARK_HPP_
