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

#include <sstream>
#include <type_traits>

#include <libff/common/profiling.hpp>

 //#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/r1cs_gg_ppzksnark_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/cc-SNARK.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/SE-cp-SNARK.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/Verifiable_image_redacting.hpp>


#ifdef MULTICORE
#include <omp.h>    //병렬 프로그램
#endif

namespace libsnark {

template<typename ppT>
bool run_Verifiable_image_redacting(const r1cs_example<libff::Fr<ppT> > &example, 
                                    const std::vector<libff::Fr<ppT>>  &m,
                                    const bool test_serialization)
{

    r1cs_primary_input<libff::Fr<ppT>> u(m.begin(), m.begin() + m.size()/2);
    r1cs_primary_input<libff::Fr<ppT>> u_(m.begin() + m.size()/2, m.end());
   
    libff::enter_block("Certificate part");

    libff::print_header("Verifiable image redacting Keygen");
    VIR_keypair<ppT> keypair = VIR_Keygen<ppT>(u.length());
    printf("\n"); libff::print_indent(); libff::print_mem("after Keygen");

    libff::print_header("Verifiable image redacting Authenticate");
    VIR_authentication<ppT> authentication = VIR_Authenticate<ppT>(m, keypair.sk.BLS_sk);
    printf("\n"); libff::print_indent(); libff::print_mem("after Authenticate");

    libff::print_header("Verifiable image redacting Setup");
    VIR_CRS<ppT> CRS = VIR_Setup<ppT>(keypair.pk, example.constraint_system);
    printf("\n"); libff::print_indent(); libff::print_mem("after Setup");

    libff::leave_block("Certificate part");

    libff::enter_block("Prover part");

    libff::print_header("Verifiable image redacting Prove");
    VIR_proof<ppT> proof = VIR_prove<ppT>(CRS, authentication, keypair.pk.BLS_pk, u, u_);
    printf("\n"); libff::print_indent(); libff::print_mem("after Prove");

    libff::leave_block("Prover part");

    libff::enter_block("Verifier part");

    libff::print_header("Verifiable image redacting Verify");
    const bool ans = VIR_verify(CRS.VIR_crs, keypair.pk.BLS_pk, u, u_, authentication, proof);
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

     libff::leave_block("Verifier part");

    libff::leave_block("Call to run_Verifiable_image_redacting");

    return ans;
}

} // libsnark

#endif // RUN_SNARK_FOR_FILTERING_TCC_
