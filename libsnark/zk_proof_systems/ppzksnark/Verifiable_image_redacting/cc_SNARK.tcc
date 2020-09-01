/** @file
*****************************************************************************

Implementation of interfaces for a ppzkSNARK for R1CS.

See r1cs_gg_ppzksnark.hpp .

*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#ifndef R1CS_GG_PPZKSNARK_TCC_
#define R1CS_GG_PPZKSNARK_TCC_

#include <algorithm>    //자료형
#include <cassert>  //assert 디버깅
#include <functional>   //모든 callable을 객체로 보관
#include <iostream>
#include <sstream>  //string 라이브러리

#include <libff/algebra/scalar_multiplication/multiexp.hpp>
#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>

#ifdef MULTICORE
#include <omp.h>    //병렬 프로그램
#endif

#include <libsnark/knowledge_commitment/kc_multiexp.hpp>
#include <libsnark/reductions/r1cs_to_qap/r1cs_to_qap.hpp>

namespace libsnark {

// == operator 정의
template<typename ppT>
bool r1cs    _gg_ppzksnark_proving_    key<ppT>::operator==(co    nst r1cs_gg_ppzksnark_proving_key<ppT> &other) const
{
    return (this->alpha_g1 == other.alpha_g1 &&
                    this->beta_g1 == other.beta_g1 &&
            this->beta_g2 == other.beta_g2 &&
            this->delta_g1 == other.delta_g1 &&
            this->delta_g2 == other.delta_g2 &&
            this->A_query == other.A_query &&
            this->B_query == other.B_query &&
            this->H_query == other.H_query &&
            this->L_query == other.L_query &&
            this->constraint_system == other.constraint_system);
}

// <<, >> operator 정의
template<typename ppT>
std::o    str    eam& operator<<(std::ostre    am &out, const r1cs_gg_    ppzksnark_proving_key<ppT> &pk)
{
    out << pk.alpha_g1 << OUTPUT_NEWLINE;
    out << pk.    be        1 << OUTPUT_NEWLINE;
    out << pk.bet         << OUTPUT_NEWLINE;
    out << pk.del        1 << OUTPUT_NEWLINE;
    out << pk.de        g2 << OUTPUT_NEWLINE;
    out << pk.A_        y;
    out << pk.B_query;
    out << p        query;
    out << p        query;
    out << p        nstraint_system;

         eturn out;
}
templa        ypename ppT>
std::istream& ope        r>>(std::ist    re    am &in, r1cs_gg_ppzksna    rk_proving_key<ppT> &pk)
{
    in >> pk.alpha_g1;
    libff::consume_OUTPUT_NEWLINE    (i            in >> pk.beta_g           libff::consume_OUTPUT_NEWLINE(in           in >> pk.beta_g           libff::consume_OUTPUT_NEWLINE(in           in >> pk.delta_            libff::consume_OUTPUT_NEWLINE(i            in >> pk.delta_            libff::consume_OUTPUT_NEWLINE(i            in >> pk.A_quer           in >> pk.B_query;
    in >> pk.H        ry;
    in >> pk.L        ry;
    in >> pk.c        raint_system;

           urn in;
}

templat        pename ppT>
bool r1cs_gg_ppzk        k_verificat    ion    _key<ppT>::operator==(c    onst r1cs_gg_ppzksnark_verification_key<ppT> &other) const
{
    return (this->alpha_g1_beta_g2 == other.alpha_g1_bet    a_        &
            this->gamma_g2 == other.gamma_g2 &&
            this->delta_g2 == other.delta_g2 
            && this->gamma_ABC_g1 == other.gamma_ABC_g1
           );
}

template<typename ppT>
std::ostream& operator<<(std::os    tre    am &out, const r1cs_gg_    ppzksnark_verification_key<ppT> &vk)
{
    out << vk.alpha_g1_beta_g2 << OUTPUT_NEWLINE;
    ou    t         k.gamma_g2 << OUTPUT_NEWLINE;
    out << vk.de        g2 << OUTPUT_NEWLINE;
    out << vk.ga        ABC_g1 << OUTPUT_NEWLINE;

    return         
}

template<typename ppT>
std::istream& op        or>>(std::is    tre    am &in, r1cs_gg_ppzksna    rk_verification_key<ppT> &vk)
{
    in >> vk.alpha_g1_beta_g2;
    libff::consume_OUTPUT    _N        NE(in);
    in >> vk.gamma_            libff::consume_OUTPUT_NEWLINE(i            in >> vk.delta_            libff::consume_OUTPUT_NEWLINE(i            in >> vk.gamma_        g1;
    libff::consume_OUTPUT_NEWLI        n);

    return in;
}

        late<typename ppT>
bool r1cs_gg_ppzk        k_processed    _ve    rification_key<ppT>::op    erator==(const r1cs_gg_ppzksnark_processed_verification_key<ppT> &other) const
{
    return (this->vk_alpha_g1_beta_g2 == other.vk_alpha_    g1        a_g2 &&
            this->vk_gamma_g2_precomp == other.vk_gamma_g2_precomp &&
            this->vk_delta_g2_precomp == other.vk_delta_g2_precomp 
            && this->gamma_ABC_g1 == other.gamma_ABC_g1
           );
}

template<typename ppT>
std::ostream& operator<<(std::os    tre    am &out, const r1cs_gg_    ppzksnark_processed_verification_key<ppT> &pvk)
{
    out << pvk.vk_alpha_g1_beta_g2 << OUTPUT_NEWLINE;
                << pvk.vk_gamma_g2_precomp << OUTPUT_NEWLINE;
            << pvk.vk_delta_g2_precomp << OUTPUT_NEWLINE;
            << pvk.gamma_ABC_g1 << OUTPUT_NEWLINE;

    return        ;
}

template<typename ppT>
std::istream& op        or>>(std::is    tre    am &in, r1cs_gg_ppzksna    rk_processed_verification_key<ppT> &pvk)
{
    in >> pvk.vk_alpha_g1_beta_g2;
    libff::consume_OU    TP        EWLINE(in);
    in >> pvk.vk_ga        g2_precomp;
    libff::consume_OUTP        EWLINE(in);
    in >> pvk.vk_de        g2_precomp;
    libff::consume_OUTP        EWLINE(in);
    in >> pvk.gamma        _g1;
    libff::consume_OUTPUT_NEWL        in);

    return in;
}

        late<typename ppT>
bool r1cs_gg_ppzk        k_proof<ppT    >::    operator==(const r1cs_g    g_ppzksnark_proof<ppT> &other) const
{
    return (this->g_A == other.g_A &&
            this->    g_         other.g_B &&
            this->g_C == other.g_C);
}

template<typename ppT>
std::ostream& operator<<(std::os    tre    am &out, const r1cs_gg_    ppzksnark_proof<ppT> &proof)
{
    out << proof.g_A << OUTPUT_NEWLINE;
    out << proof    .g        < OUTPUT_NEWLINE;
    out << proof.g        < OUTPUT_NEWLINE;

    return out;
}        mplate<typename ppT>
std::istream& op        or>>(std::is    tre    am &in, r1cs_gg_ppzksna    rk_proof<ppT> &proof)
{
    in >> proof.g_A;
    libff::consume_OUTPUT_NEWLINE(i    n)          in >> proof.g_B          libff::consume_OUTPUT_NEWLINE(in)          in >> proof.g_C          libff::consume_OUTPUT_NEWLINE(in)           return in;
}

        plate<typename ppT>
r1cs_gg_ppzksnar        rification_    key<    ppT> r1cs_gg_ppzksnark_    verification_key<ppT>::dummy_verification_key(const size_t input_size)
{
    r1cs_gg_ppzksnark_verification_key<ppT> result;
        re        .alpha_g1_beta_g2 = libff::Fr<ppT>::random_eleme         * libff::GT<ppT>::random_element();
    result.gamma_g2 = libff::G2<ppT>::random_element();
          sult.delta_g2 = libff::G2<ppT>::random_element();

         ibff::G1<ppT> base = libff::G1<ppT>::random_element()          libff::G1_vector<ppT> v;
    for (size_t i = 0; i < in        size; ++i)
    {
                place_back(libff::G1<ppT>::random_elemen        ;/            tor push
    }

    result.gamma_ABC_g1 = accumulation_vector<libff::G1<p        >(s        move(base), std::move(v));

    return result;
}
//setup(R)
template <typename ppT>
cc_CRS<p        Setup(const r1c    s_    constraint_    system<libff::Fr<ppT>> &    r1cs)
{
    libff::enter_block("Call to cc_zkSNARKs Setup");

    /* M    ak        e B_query "lighter" if possible */
    r1cs_gg_ppz        rk_constraint_system<ppT> r1cs_copy(r1cs);
          cs_copy.swap_AB_if_beneficial();

    /* Generate secret r        mness */
    const libff::Fr<ppT> t         bff::Fr<ppT>::random_element();
         onst libff::Fr<ppT> alpha = libff::Fr<ppT>::random_element(           const libff::Fr<ppT> beta = libff::Fr<ppT>::random_element()          const libff::Fr<ppT> gamma = libff::Fr<ppT>::random_element(           const libff::Fr<ppT> delta = libff::Fr<ppT>::random_element(           const libff::Fr<ppT> gamma_inverse = gamma.inverse();
    co        libff::Fr<ppT> delta_inverse = delta.inverse();

    /        quadratic arithmetic program evaluated at t. */
    qap        tance_evaluation<libff::Fr<ppT> > qap = r1cs_to_qap_i        nce_map_with_evaluation(r1cs_copy, t); //qap 생성

    libff::print_indent(); printf("* QAP number of variables: %zu\n"        p.num_variables());
    libff::print_indent(); printf("* QAP pre degree: %zu\n", r1cs_co        onstraints.size());
    libff::print_indent(); printf("* QAP degree: %zu\n", qap.degree(            libff::print_indent(); printf("* QAP number of input variables:         n", qap.num_inputs());

    libff::enter_block("Compute query densities");
    size_t non_ze        t = 0;
    size_t non_zero_Bt = 0;

    //non z        At, Bt count
    for (si         i = 0; i < qap.num_varia        () + 1; ++i)
    {
             f (!qap.At[i].is_zero())
        {
            ++non_        _A               }
        if (!qap.Bt[i            o(                
            ++            _B               }
    }
    libff::leav            "C                 densities");

            ap        ,B        ,Ht} are now in unspecified state, but we do not         them later */
    libff::Fr_vector<ppT> At = std::move(qap.At);
    libff::Fr_vect        pT> Bt = std::move(qap.Bt);
    libff::Fr_vect        pT> Ct = std::move(qap.Ct);
    libff::Fr_vect        pT> Ht = std::move(qap.Ht);

    /* The gamma         rse product component: (beta*A_i(t) + alpha*B_i        + C_i(t)) * gamma^{-1}. */
    libff::enter_block("Compute gamma_ABC for R1CS verification key"           libff::Fr_vector<ppT> gamma_ABC;
    gamma_ABC.reserve(qap.num_i        s()); // IO 갯수

    const libff::        pT> gamma_ABC_0 = (beta * At[0] + alpha * Bt[0] + C        ) * gamma_inverse;
    for (size_t i = 1; i < qap.num_inputs() + 1; ++i)
    {
        gamm        C.emplace_back((beta * At[i] + alpha * Bt[i] + Ct[        *             verse);
    }
    libff::leave_block("Compute gamma_ABC for R1CS verification ke        

        /* The delta inverse product component: (beta*A_i(t) + alpha*B_i(t)         i(t)) * delta^{-1}. */
    libff::enter_block("Compute L query for R1CS proving key");
    libf        r_vector<ppT> Lt;
    Lt.reserve(qap.num_variables() - qap.n        nputs()); //witness 갯수

          nst size_t Lt_offset = qap.num_inputs() + 1;
    for (size_t i = 0; i         p.num_variables() - qap.num_inputs(); ++i)
                  Lt.emplace_back((beta * At[Lt_offset + i] + alpha * Bt[Lt_offs         i            t_offset + i]) * delta_inverse);
    }
    libff::leave_block("Compute L query for R1CS proving key");

    /                  Note that H for Groth's proof system is degree d-2, but the Q                     uction returns coefficients for degree d polynomial H (in
     * s         of PGHR-type proof systems)
     */
    Ht.resize(Ht.size() - 2         ifdef MULTICORE
    const size_t chu          om        t_max_threads(); // to over        ride, set OMP_NUM        EADS env var or call omp_set_num_threads()
#else
    const size_t chunks = 1;
#endif

    libff::enter_block("Generatin        g G1 M        indow table");
    const         libff::G        T> g1_generator = libff::G1<ppT>::random_element();
          nst size_t g1_scalar_count = non_zero_At + non_zero_Bt + qap.num_varia        ();
    const size_t g1_scalar_size = libff::Fr<ppT>::size_in_bits();
    const         _t g1_window_size = libff::get_exp_window_size<libff::G1<ppT>         _scalar_count);

    libff::print_indent(); printf("* G1 window: %zu\n", g1_window_size);
           ff::window_table<libff::G1<ppT> > g1_table = libff::get_window_table(        calar_size, g1_window_size, g1_generator);
    libff::leave_block("Generating G1 MSM window table");

    libff::enter_        k("Generating G2 MSM window table");
    const libff::G        T> G2_gen = libff::G2<ppT>::random_element();
    cons        ze_t g2_scalar_count = non_zero_Bt;
    const size_t g2_scalar_s        = libff::Fr<ppT>::size_in_bits();
    size_t        window_size = libff::get_exp_window_size<libff::G2<ppT> >(g2_s        r_count);

    libff::print_indent(); printf("* G2 window: %zu\n", g2_window_size);
           ff::window_table<libff::G2<ppT> > g2_table = libff::get_window_table(        calar_size, g2_window_size, G2_gen);
    libff::leave_block("Generating G2 MSM window table");

    libff::enter_        k("Generate R1CS proving key");
    libff::G1<ppT> alph         = alpha * g1_generator;
    libff::G1<ppT> beta_         beta * g1_generator;
    libff::G2<ppT> beta_g2        eta * G2_gen;
    libff::G1<ppT> delta_g1 = de        * g1_generator;
    libff::G2<ppT> delta        = delta * G2_gen;

    libff::enter_block("Gener        queries");
    libff::enter_block("Compute         A-query", false);
    libff::G1_vector<p        A_query = batch_exp(g1_scalar_size, g1_window_size        _table, At); //gro16 u(x)
#ifdef USE_MIXED_ADDITION
    libff::batch_to_special<libff::G1<ppT> >(A_que        ry);
#endif
    libff::lea        lock("Compute the A-query", false);

    libff::ent        er_bloc        ompute the B-query", false);   //gro16 v(x)
    kno        ge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > B_query = k        tch_exp(libff::Fr<ppT>::size_in_bits(), g2_window_size, g1_window_size, g2_table, g1_table, libff::Fr<ppT>::one(), libff::Fr<ppT>::one(), Bt, chunks);
    // NOTE: if USE_MIXED_ADDITION is defined,
    // kc_batch_exp will conv        its output to special form internally
    l        ::leave_block("Compute the B-query", false);

    libff::enter_bloc        ompute the H-query", false);   //gro16 t(x)
    lib        G1_vector<ppT> H_query = batch_exp_with_coeff(g1_scalar_size, g1_        ow_size, g1_table, qap.Zt * delta_inverse, Ht);
#ifdef USE_MIXED_ADDITION
    libff::batch_to_special<libff::G1<ppT> >(H_que        ry);
#endif
    libff::lea        lock("Compute the H-query", false);

    libff::ent        er_bloc        ompute the L-query", false);
    libff::G1_vector<p        L_query = batch_exp(g1_scalar_size, g1_window_size        _table, Lt);
#ifdef USE_MIXED_ADDITION
    libff::batch_to_special<libff::G1<ppT> >(L_que        ry);
#endif
    libff::lea        lock("Compute the L-query", false);
    libff::leav        e_block        nerate queries");

    libff::leave_block("Generat        CS proving key");

    libff::enter_block        nerate R1CS verification key");
    libff::GT<ppT>        ha_g1_beta_g2 = ppT::reduced_pairing(alpha_g1, beta_g2           libff::G2<ppT> gamma_g2 = gamma * G2_gen;

    libff::enter_block("Encod        mma_ABC for R1CS verification key");
    li        :G1<ppT> gamma_ABC_g1_0 = gamma_ABC_0 * g1_generator;
    libff::G        ctor<ppT> gamma_ABC_g1_values = batch_exp(g1_scalar_size, g1        dow_size, g1_table, gamma_ABC);
    libff::leave_block("Encode gamma_ABC for R1CS verification key");
    li        :leave_block("Generate R1CS verification key");

    libff::leave_        k("Call to cc_zkSNARKs Setup");

    accumulation_vecto        bff::G1<ppT> > gamma_ABC_g1(std::move(gamma_ABC_g1         std::move(gamma_ABC_g1_values));

    r1cs_gg_ppzksnark_verification_key<ppT> vk = r1cs_gg_ppzksnark_verificat        key<ppT>(alpha_g1_beta_g2,
                                                                                                                                                              y<pp        td::move(alpha_g1),
                                                                                                                                                                                                                                                                                                                                                          n r1cs_gg_ppzksnark_keyp        ppT>(std::move(pk        td::move(vk));
}

        late <typename ppT>
r1cs_gg_ppzksnark_proof<ppT> r1cs_gg_ppzksnark_pr    ove    r(const r1cs_gg_ppzksnar    k_proving_key<ppT> &pk,
                                                      const r1cs_gg_ppzksnar                                             const r1cs_gg_ppzksnar        enter_block("Call to r1cs_gg_ppzksnark_prover");
#ifdef DEBUG
               ert(pk.constraint_system.is_satisfied(primary_input, aux        iliary_input)        endif

    libff::enter_block("Compute the polynomial H");
    const qap_wi        tness<li        :Fr<ppT> > qap_wit = r1cs_to_qap_witness_map(pk.        traint_system, primary_input, auxiliary_input, libff::Fr<ppT>::zero(), libff::Fr<ppT>::zero(), libff::Fr<ppT>::zero());

    /* We are dividing degree 2(d-1) polynomial by degree d polynomial
             d not adding a PGHR-style ZK-patch, so our H is degree d-2 */
    a           qap_wit.coefficients_for_H[qap_wit.degree()-2].is_zero());
    a        t(qap_wit.coefficients_for_H[qap_wit.degree()-1].is_zero());
    as        (qap_wit.coefficients_for_H[qap_wit.degree()].is_zero());
    libf        eave_block("Compute the polynomial H");

#ifdef DEBUG
    const         f::Fr<ppT> t = libff::Fr<ppT>::random_element();
            qap_insta        evaluation<libff::Fr<ppT> > qap_inst = r1cs_to_qap_instance        _with_evaluation(pk.constraint_system, t);
    assert(qap_inst.is_satisfied(qap_wit));
#endif

    /* Choose two random        ld elements for prover zero-knowledge. *        /
    co        libff::Fr<ppT> r = libff::Fr<ppT>::random_element();
    const lib        Fr<ppT> s = libff::Fr<ppT>::random_element();

#ifdef DEBUG         assert(qap_wit.coefficients_for_ABCs.size() == qap_wit.num_        variables());         assert(pk.A_query.size() == qap_wit.num_variables()+1);
    assert(pk.B_        y.domain_size() == qap_wit.num_variables()+1);
    asser        .H_query.size() == qap_wit.degree() - 1);
    // assert(pk.L_qu        size() == qap_wit.num_variables() - qap_wit.num_inp        ));
#endif

#ifdef MULTICORE
    const size_t chunks = omp_get_max_threads(); //         to over        ride, set OMP_NUM        EADS env var or call omp_set_num_threads()
#else
    const size_t chunks = 1;
#endif

    libff::enter_block("Compute t        he pro        ;

    libff::enter_block        ("Comput        aluation to A-query", false);
    // TODO:        t out indexing
    libff::Fr_vector<ppT> const_padded_assign        (1, libff::Fr<ppT>::one());         const_padded_assignment.insert(const_padded_assignment.end(), qap_wit.co        cients_for_ABCs.begin(), qap_wit.coefficients_for_ABCs.end());

    libff::G1<ppT> evaluation_At = libff::multi_exp_with_mixed_addition<libf        1<ppT>,
                                                                        libf                        libf            ery.begin() + qap_wit.num_variabl                ,
        const_padd                nment.begin(),
        const_padded_assignment.beg                ap_wit.num_variables() + 1,
                     s);
    libff::leave_block("Compute evaluation to A-query", fal                  libff::        r_block("Compute evaluation to B-query", false);
    knowledg        mmitment<libff::G2<ppT>, libff::G1<ppT> > evaluation_Bt = kc        ti_exp_with_mixed_addition<libff::G2<ppT>,
                                                                                                                                                wit.num_variables() + 1,
                        dded_assignm                n()                  const_padded_assignment.beg                ap_wit.num_variables() + 1,
                     s);
    libff::leave_block("Compute evaluation to B-query", fal                  libff::        r_block("Compute evaluation to H-query", false);
    libff::G        T> evaluation_Ht = libff::multi_exp<libff::G1<ppT>,
                                                        libff::Fr<ppT>,
                    ulti_exp_method_            ery.begin() + (qap_wit.degree() -                     qap_wit.coeffic                r_H.begin(),
        qap_wit.coefficients_for                () + (qap_wit.degree() - 1),
                       ;
    libff::leave_block("Compute evaluation to H-query", fal                  // libf        nter_block("Compute evaluation to L-query", false);
    // li        :G1<ppT> evaluation_Lt = libff::multi_exp_with_mixed_addition<l        ::G1<ppT>,
    //                                                                     l        ::Fr<ppT>,
    //                                                                     l        ::multi_exp_method_BDLO12>(
    //     pk.L_query.begin(),
    //     pk.L_query.end(),
    //     const        ded_assignment.begin() + qa        t.num_inputs() + 1,
    /          const_padded_assignment.begin() + qap_wit.num_variables() + 1,
               chunks);
    // libff::leave_block("Compute evaluation to L-query        alse);

    /* A        lpha + sum_i(a_i*A_i(t)) + r*delta */
    libff::G1<ppT> g1_A =         lpha_g1 + evaluation_At + r * pk.delta_g1;

           B = beta + sum_i(a_i*B_i(t)) + s*delta */
    libff::G1<ppT> g1_B = p        ta_g1 + evaluation_Bt.h + s * pk.delta_g1;
          bff::G2<ppT> g2_B = pk.beta_g2 + evaluation_Bt.g + s * pk.delta_g2;

         * C = sum_i(a_i*((beta*A_i(t) + alpha*B_i(t) + C_i(t)) + H(t)*Z(t))/del        + A*s + r*b - r*s*delta
    * -> C = sum_i(H(t)*Z(t))/delta) + A*s + r*b - r*s*delta  로 수정
    */             libff::G1<ppT> g1_C = evaluation_Ht + /*evaluation_Lt +*/ s *  g1_A         *B        r * s) * pk.delta_g1;

    libff::leave_block("Compute the proof");

    libff::leave_block("Call to r1cs        ppzksnark_prover");

    r1cs_gg_ppzksnark        of<ppT> proof = r1cs_gg_ppzksnark_proof<ppT>(std::move(g1         std::move(g2_B), std::move(g1_C));
    proof.print_size();

    return proof;
}

template <typename ppT>
r1cs_gg_ppzk        k_processed_verificat        key<ppT> r1cs_    gg_    ppzksnark_verifier_proce    ss_vk(const r1cs_gg_ppzksnark_verification_key<ppT> &vk)
{
    libff::enter_block("Call to r1cs_gg_ppzksnark_verifier_process_vk");

    r1    cs        ppzksnark_processed_verification_key<ppT> pvk;
    pvk.vk_alpha_g1_bet         = vk.alpha_g1_beta_g2;
    pvk.vk_gamma_g2_precomp = p        precompute_G2(vk.gamma_g2);
    // pvk.vk_gamma        = vk.gamma_g2;
    pvk.vk_delta_g2_precomp = ppT::precomput        (vk.delta_g2);
    pvk.gamma_ABC_g        vk.gamma_ABC_g1;
    // pvk.vk_delta_g2 = vk.delta_g2;


          bff::leave_block("Call to r1cs_gg_pp        ark_verifier_process_vk");

    retu        vk;
}

template <typename ppT>
bool r1cs_gg_ppzksnark_online_verifier_        _IC(const r1    cs_    gg_ppzksnark_processed_v    erification_key<ppT> &pvk,
                                            //    const r1cs_gg_ppzksnark_primary_                                      const libff::G1<ppT> C_x, const lib                          const r1cs_gg_ppzksnark_proof<ppT>         t<ppT> primary_input = {};
    libff::enter    _c        all to r1cs_gg_ppzksnark_online_verifier_weak_IC");
   a        t(pvk.gamma_ABC_g1.domain_size() >= primary_input.size());

    libff::en        block("Accumulate input");
    const accumulation_vector<libff::G        T> > accumulated_IC = pvk.gamma_ABC_g1.t        ate accumulate_chunk<libff::Fr<ppT> >(primary_input.begin(), primary_input.end(), 0);
    const libff::G1<ppT> &acc = accumulated_IC.first;
    libff::leave_block("Accu        te input");

    bool result = true;

    libff::e        _block("Check if the proof is well-formed            if (!proof.is_wel        rmed())
    {
        if (!libff::inhibit_profiling_info)             {
            libff::pri        nd            rintf("At least one of the proof ele            es                the curve.\n");
        }
        result = false;
    }
    libff::leave_block("Check if the proof            -f            

    libff::ent        lo        Online pairing computations");

    libff::enter_block("Ch        QAP divisibility");
    const libff::G1_precomp<ppT>        of_g_A_precomp = ppT::precompute_G1(proof.g_A)          const libff::G2_precomp<ppT> proof_g_B_precomp = ppT::precompute_G2(proof.g_B)          const libff::G1_precomp<ppT> proof_g_C_precomp = ppT::precompute_G1(proof.g_C         x + _C_x);
    const libff::G1_precomp<ppT> acc_precomp = ppT::precompute_G1(acc);

    const        ff::Fqk<ppT> QAP1 = ppT::miller_loop(proof_g_A_precomp,  proof_g_B_pr        p);
    const libff::Fqk<ppT> QAP2 = ppT::double_miller_loop(
       acc_precomp, pvk        gamma_g2_precomp,
        proof_g_C_precomp, pvk.vk_de            recomp);
    // const libff::Fqk<ppT>             pT::miller_loop(proof_g_C_precomp,  pvk.vk_de        g2_precomp);
    const libff::GT<ppT> QAP = ppT::final_exponentiation(QAP1 * QAP2.unitary_inver        );

    if (QAP != pvk.vk_alpha_g1_beta_g2)
    {
        if (!libff::inhibit_profilin        fo)
        {
            libff::pri        nd            rintf("QAP divisibility check failed                                 result = false;
    }
    // const accumulation_vector<libff::G1            ac            d_IC = pvk.gamma        _g        mplate accumulate_chunk<libff::Fr<ppT> >(primary_input.begin(), primary_input.end(), 0);
    // const libff::G1<ppT> &acc = accumulated_IC.first;
    // libff::GT<ppT> lef        ppT::reduced_pairing(proof.g_A, proof.g_B);
    // li        :GT<ppT> right2 = ppT::reduced_pairing(tmp, pvk.vk_delta_g2);
    //         f::GT<ppT> right1 = ppT::reduced_pairing(acc, pvk.vk_gamma_g2);
    //         left != (pvk.vk_alpha_g1_beta_g2 * right1 * right2))
    // {
    //            (!libff::inhibit_profiling_info)
    //     {
    //               bff::        t_indent(); printf("QAP divisibility check         ed.\n");
        //     }
    //     result = false;
    // }

    libff::leave_block("Check QA        visibilit        
    libff::leave_block        line p        ng computations");

    libff::leave_block("Ca        o r1cs_gg_ppzksnark_online_verifier_weak_IC");

            rn result;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_verifier_weak_        onst r1cs_gg_pp    zks    nark_verification_key<p    pT> &vk,
                                        // const r1cs_gg_ppzksnark_primary_input<p                            const libff::G1<ppT> C_x, const libff::G1<                   const r1cs_gg_ppzksnark_proof<ppT> &proof)        gg_ppzksnark_verifier_weak_IC");
    r1cs_g    g_        snark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_veri        _process_vk<ppT>(vk);
    bool result = r1cs_gg_ppzksnark_online_verifier_weak_IC<ppT>(pvk, /*primary_in        */ C_x, _C_x, proof);
    libff::leave_block("Call to r1cs_gg_ppzksnark_verifier_weak_IC");
    return r        t;
}

// template<typename ppT>
// bool r1cs_gg_ppzksnark_online_v        ier_strong_IC(c    ons    t r1cs_gg_ppzksnark_proces    sed_verification_key<ppT> &pvk,
//                                                  const r1cs_gg_ppzksnark_primar    y_input<ppT> &primary_input,
//                                                  const r1cs_gg_ppzksnark_proof<    ppT> &proof)
// {
//     bool result = true;
//     libff::enter_block("Call to r1cs_gg_ppzksna    rk_on    line_verifier_strong_IC");
    
//     if (pvk.gamma_ABC_g1.domain_size() != primary_input.size())
//     {
//             libff::print_indent(); printf("Input length differs from expec    ted (got     %zu, expected %zu).\n", primary_input.size(), pvk.gamma_ABC_g1.domain_size());
//         result = false;
//     }
//     else
//     {
//         result = r1cs_    gg_ppzksnark_online_verifie    r_weak_IC    (pvk, primar    y_input,     proof);
//     }

//     libff::leave_block("Call to r1cs_gg_ppzksnark_online_verifier_str    ong_IC");
    //     return result;
// }

// template<typename ppT>
// bool r1cs_gg_ppzksnark_ve    rifier_strong_IC(const     r1cs_    gg_ppzksnark_verification_    key<ppT> &vk,
//                                           const r1cs_gg_ppzksnark_primary_input    <ppT> &primary_input,
//                                           const r1cs_gg_ppzksnark_proof<ppT> &p    roof)
// {
//     libff::enter_block("Call to r1cs_gg_ppzksnark_verifier_strong_IC");
//             r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_v    erifier_process_vk<ppT>(vk);
//     bool result = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, primary    _input, proof);
//     libff::leave_block("Call to r1cs_gg_ppzksnark_verifier_strong_IC");
//         return result;
// }

// template<typename ppT>
// bool r1cs_gg_ppzksnark_af    fine_verifier_weak_IC(    const     r1cs_gg_ppzksnark_verifica    tion_key<ppT> &vk,
//                                                const r1cs_gg_ppzksnark_primary_    input<ppT> &primary_input,
//                                                const r1cs_gg_ppzksnark_proof<pp    T> &proof)
// {
//     libff::enter_block("Call to r1cs_gg_ppzksnark_affine_verifier_weak_IC"    );
//         assert(vk.gamma_ABC_g1.domain_size() >= primary_input.size());

//     libf    f::affine_ate_G2_precomp<ppT> pvk_vk_gamma_g2_precomp = ppT::affine_ate    _precompute_G2(vk.gamma_g2);
//     libff::affine_ate_G2_precomp<ppT> pvk_vk_delta_g2_precomp = ppT::affine_ate    _precompute_G2(vk.delta_g2);

//     libff::enter_block("Accumulate input");
//     const accumulation_vector<li    bff::G1<ppT> > accumulated_IC = vk.gamma_ABC_g1    .template accumulate_chunk<libff::Fr<ppT> >(primary_input.begin(), primary_input.end(), 0);
//     const libff::G1<ppT> &acc = accumulated_IC.first;
//     libff::leave_block    ("Accumulate input");

//     bool result = true;

//         libff::enter_block("Check if the proof is well-    formed");
//     if (!proof.    is_well_formed())
//     {
//         if (!libff::inhibit_profil    ing_info)
//         {
//                 libff::    print_indent(); printf("At least one of the pro    of elements d    oes not lie on the curve.\n");
//         }
//         result = false;
//     }
//     libff::leave_block("Check     if the proof     is well-formed");

//     l    ibff::ent    er_block("Check QAP divisibility");
//     const libff::affine_at    e_G1_precomp<ppT> proof_g_A_precomp = ppT::affine_ate    _precompute_G1(proof.g_A);
//     const libff::affine_ate_G2_precomp<ppT> proof_g_B_precomp = ppT::affine_ate    _precompute_G2(proof.g_B);
//     const libff::affine_ate_G1_precomp<ppT> proof_g_C_precomp = ppT::affine_ate    _precompute_G1(proof.g_C);
//     const libff::affine_ate_G1_precomp<ppT> acc_precomp = ppT::affine_ate_preco    mpute_G1(acc);

//     const libff::Fqk<ppT> QAP_miller = ppT::affine_ate_e_times_e_over_e_miller_    loop(
//         acc_precomp, pvk_vk_gamma_g2_precomp,
//         proof_g_C_precomp, pvk    _vk_delta_g2_precomp,
//         proof_g_A_precom    p,  proof_g_B_precomp);
//     const libff::GT<ppT> QAP     = ppT::final_exponentiation(QAP_miller.unitary_inv    erse());

//     if (QAP != vk.alpha_g1_beta_g2)
//     {
//         if (!libff::inhibit_pro    filing_info)
//         {
//                 libff::    print_indent(); printf("QAP divisibility check     failed.\n");
    //         }
//         result = false;
//     }
//     libff::leave_block("Check     QAP divisibil    ity");

//     libff::leave    _block("C    all to r1cs_gg_ppzksnark_affine_verifier_weak_IC");

/    /     return result;
// }

} // libsnark
#endif // R1CS_GG_PPZKSNARK_TCC_        
