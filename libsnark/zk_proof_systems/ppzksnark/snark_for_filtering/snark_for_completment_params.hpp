/** @file
 *****************************************************************************

 Declaration of public-parameter selector for the R1CS GG-ppzkSNARK.

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef SNARK_FOR_COMPLETMENT_PARAMS_HPP_
#define SNARK_FOR_COMPLETMENT_PARAMS_HPP_

#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>

namespace libsnark {

/**
 * Below are various template aliases (used for convenience).
 * 
 * primary = statement 필요 없음
 */

template<typename ppT>
using snark_for_completment_constraint_system = r1cs_constraint_system<libff::Fr<ppT> >;//linear_combination<FieldT> a, b, c;

template<typename ppT>
using snark_for_completment_primary_input = r1cs_primary_input<libff::Fr<ppT> >;//std::vector<FieldT>

template<typename ppT>
using snark_for_completment_auxiliary_input = r1cs_auxiliary_input<libff::Fr<ppT> >;//std::vector<FieldT>

} // libsnark

#endif // #define SNARK_FOR_COMPLETMENT_PARAMS_HPP_

