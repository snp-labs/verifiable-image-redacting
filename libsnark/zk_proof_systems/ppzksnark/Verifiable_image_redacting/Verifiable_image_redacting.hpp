#ifndef VERIFIABLE_IMAGE_REDACTING_HPP_
#define VERIFIABLE_IMAGE_REDACTING_HPP_

#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/common/data_structures/accumulation_vector.hpp>
#include <libsnark/knowledge_commitment/knowledge_commitment.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/r1cs_gg_ppzksnark_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/cc_SNARK.hpp>

#include "libbls_signatures.h"

namespace libsnark
{

	template <typename ppT>
	class VIR_keypair;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_keypair<ppT> &keypair);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_keypair<ppT> &keypair);

	template <typename ppT>
	class VIR_keypair
	{
	public:
		VIR_keypair_pk<ppT> pk;
		VIR_keypair_sk<ppT> sk;

		bool operator==(const VIR_keypair &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_keypair<ppT> &keypair);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_keypair<ppT> &keypair);

		VIR_keypair() = default;
		VIR_keypair(const VIR_keypair_pk<ppT> &&pk,
					const VIR_keypair_sk<ppT> &&sk) : pk(std::move(pk)),
													  sk(std::move(sk)){};
	};

	template <typename ppT>
	class VIR_keypair_pk;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_keypair_pk<ppT> &pk);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_keypair_pk<ppT> &pk);

	template <typename ppT>
	class VIR_keypair_pk
	{
	public:
		libff::G1_vector<ppT> ck;
		PrivateKeyPublicKeyResponse BLS_pk;

		VIR_keypair_pk(){};
		VIR_keypair_pk<ppT> &operator=(const VIR_keypair_pk<ppT> &other) = default;
		VIR_keypair_pk(const VIR_keypair_pk<ppT> &other) = default;
		VIR_keypair_pk(VIR_keypair_pk<ppT> &&other) = default;
		VIR_keypair_pk(libff::G1_vector<ppT> &&ck,
					   PrivateKeyPublicKeyResponse &&BLS_pk) : ck(std::move(ck)),
															   BLS_pk(std::move(BLS_pk))
															   {};

		size_t G1_size() const
		{
			return ck.size();
		}
		
		size_t G2_size() const
		{
			return 0;
		}

		size_t size_in_bits() const
		{
			return (libff::size_in_bits(ck) + sizeof(BLS_pk)*CHAR_BIT);
		}

		void print_size() const
		{
			libff::print_indent();
			printf("* G1 elements in PK: %zu\n", this->G1_size());
			libff::print_indent();
			printf("* G2 elements in PK: %zu\n", this->G2_size());
			libff::print_indent();
			printf("* PK size in bits: %zu\n", this->size_in_bits());
		}

		bool operator==(const VIR_keypair_pk<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_keypair_pk<ppT> &pk);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_keypair_pk<ppT> &pk);
	};

	template <typename ppT>
	class VIR_keypair_sk;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_keypair_sk<ppT> &sk);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_keypair_sk<ppT> &sk);

	template <typename ppT>
	class VIR_keypair_sk
	{
	public:
		libff::G1_vector<ppT> ck;
		PrivateKeyGenerateResponse BLS_sk;

		snark_for_filtering_verification_key() = default;
		snark_for_filtering_verification_key(const libff::G1_vector<ppT> &&ck,
											 const PrivateKeyGenerateResponse &&BLS_sk) 
											 : ck(std::move(ck)),
											   BLS_sk(std::move(BLS_sk))
											   {};

		size_t G1_size() const
		{
			return ck.size();
		}

		size_t G2_size() const
		{
			return 0;
		}

		size_t size_in_bits() const
		{
			return (libff::size_in_bits(ck) + sizeof(BLS_sk)*CHAR_BIT);
		}

		void print_size() const
		{
			libff::print_indent();
			printf("* G1 elements in VK: %zu\n", this->G1_size());
			libff::print_indent();
			printf("* G2 elements in VK: %zu\n", this->G2_size());
			libff::print_indent();
			printf("* VK size in bits: %zu\n", this->size_in_bits());
		}

		bool operator==(const VIR_keypair_sk<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_keypair_sk<ppT> &sk);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_keypair_sk<ppT> &sk);
	};

	template <typename ppT>
	class commitment;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const commitment<ppT> &commitmentpp);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, commitment<ppT> &commitment);

	template <typename ppT>
	class commitment
	{
	public:
		libff::G1<ppT> m0;
        libff::G1<ppT> psi_m;

		commitment() = default;
		commitment(const libff::G1<ppT> &&m0,
				   const libff::G1<ppT> &&psi_m) : m0(std::move(m0)),
											       psi_m(std::move(psi_m)){};

		size_t G1_size() const
		{
			return 2;
		}

		size_t G2_size() const
		{
			return 0;
		}

		size_t GT_size() const
		{
			return 0;
		}

		size_t size_in_bits() const
		{
			// TODO: include GT size
			return (G1_size() * libff::G1<ppT>::size_in_bits());
		}

		void print_size() const
		{
			libff::print_indent();
			printf("* G1 elements in pp: %zu\n", this->G1_size());
			libff::print_indent();
			printf("* G2 elements in pp: %zu\n", this->G2_size());
			libff::print_indent();
			printf("* GT elements in pp: %zu\n", this->GT_size());
			libff::print_indent();
			printf("* PP size in bits: %zu\n", this->size_in_bits());
		}

		bool operator==(const commitment<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const commitment<ppT> &commitment);
		friend std::istream &operator>><ppT>(std::istream &in, commitment<ppT> &commitment);
	};

	template <typename ppT>
	class VIR_authentication;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_authentication<ppT> &authentication);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_authentication<ppT> &authentication);

	template <typename ppT>
	class VIR_authentication
	{
	public:
		PrivateKeySignResponse sigma;
		commitment<ppT> commitment;

		commitment() = default;
		commitment(const PrivateKeySignResponse &&sigma,
				   const commitment<ppT> &&commitment) : sigma(std::move(sigma)),
											       		 commitment(std::move(commitment))
														 {};

		bool operator==(const VIR_authentication<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_authentication<ppT> &authentication);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_authentication<ppT> &authentication);
	};

	template <typename ppT>
	class VIR_CRS_without_Trapdoor;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_CRS_without_Trapdoor<ppT> &VIR_crs);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_CRS_without_Trapdoor<ppT> &VIR_crs);

	template <typename ppT>
	class VIR_CRS_without_Trapdoor
	{
	public:
		cc_CRS<ppT> cc_crs;
		SE_cp_CRS<ppT> cp_crs;

		VIR_CRS_without_Trapdoor() = default;
		VIR_CRS_without_Trapdoor(const cc_CRS<ppT> &&cc_crs,
								 const SE_cp_CRS<ppT> &&cp_crs) : cc_crs(std::move(cc_crs)),
													   			  cp_crs(std::move(cp_crs))
													   			  {};

		bool operator==(const VIR_CRS_without_Trapdoor<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_CRS_without_Trapdoor<ppT> &VIR_crs);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_CRS_without_Trapdoor<ppT> &VIR_crs);
	};

	template <typename ppT>
	class VIR_Trapdoor;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_Trapdoor<ppT> &trapdoor);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_Trapdoor<ppT> &trapdoor);

	template <typename ppT>
	class VIR_Trapdoor
	{
	public:
		cc_Trapdoor<ppT> cc_crs_trapdoor;
		cp_Trapdoor<ppT> cp_crs_trapdoor;

		VIR_Trapdoor() = default;
		VIR_Trapdoor(const cc_Trapdoor<ppT> &&cc_crs_trapdoor,
					 const cp_Trapdoor<ppT> &&cp_crs_trapdoor) : cc_crs_trapdoor(std::move(cc_crs_trapdoor)),
													   			 cp_crs_trapdoor(std::move(cp_crs_trapdoor))
													   			 {};

		bool operator==(const VIR_Trapdoor<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_Trapdoor<ppT> &trapdoor);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_Trapdoor<ppT> &trapdoor);
	};

	template <typename ppT>
	class VIR_CRS;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_CRS<ppT> &CRS);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_CRS<ppT> &CRS);

	template <typename ppT>
	class VIR_CRS
	{
	public:
		VIR_CRS_without_Trapdoor<ppT> VIR_crs;
		VIR_Trapdoor<ppT> trapdoor;

		VIR_CRS() = default;
		VIR_CRS(const VIR_CRS_without_Trapdoor<ppT> &&VIR_crs,
				const VIR_Trapdoor<ppT> &&trapdoorp) : VIR_crs(std::move(VIR_crs)),
													   trapdoorp(std::move(trapdoorp))
													   {};

		bool operator==(const VIR_CRS<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_CRS<ppT> &CRS);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_CRS<ppT> &CRS);
	};

	template <typename ppT>
	class VIR_proof;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const VIR_proof<ppT> &proof);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, VIR_proof<ppT> &proof);

	template <typename ppT>
	class VIR_proof
	{
	public:
		libff::G1<ppT> g_A;
    	libff::G2<ppT> g_B;
    	libff::G1<ppT> g_C;
		libff::G1<ppT> SE_cp_proof_first;
		libff::G1<ppT> SE_cp_proof_second;
		libff::G1<ppT> _C_x;

		snark_for_filtering_proof()
		{
			// invalid proof with valid curve points
			this->g_A = libff::G1<ppT>::one();
			this->g_B = libff::G2<ppT>::one();
			this->g_C = libff::G1<ppT>::one();
			this->SE_cp_proof_first = libff::G1<ppT>::one();
			this->SE_cp_proof_second = libff::G1<ppT>::one();
			this->_C_x = libff::G1<ppT>::one();
		}
		snark_for_filtering_proof(libff::G1<ppT> &&g_A,
								  libff::G2<ppT> &&g_B,
								  libff::G1<ppT> &&g_C,
								  libff::G1<ppT> &&SE_cp_proof_first, 
								  libff::G1<ppT> &&SE_cp_proof_second, 
								  libff::G1<ppT> &&_C_x) : 
								  g_A(std::move(g_A)),
								  g_B(std::move(g_B)),
								  g_C(std::move(g_C)),
								  SE_cp_proof_first(std::move(SE_cp_proof_first)),
								  SE_cp_proof_second(std::move(SE_cp_proof_second)),
								  _C_x(std::move(_C_x)){};

		size_t g1_proof_size() const //g1 group
		{
			return 5;
		}
		size_t size_in_bits() const
		{
			return g1_proof_size() * libff::G1<ppT>::size_in_bits() + libff::G2<ppT>::size_in_bits();
		}
		void print_size() const
		{
			libff::print_indent();
			printf("* VIR_proof size in bits: %zu\n", this->size_in_bits());
		}

		bool operator==(const VIR_proof<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const VIR_proof<ppT> &proof);
		friend std::istream &operator>><ppT>(std::istream &in, VIR_proof<ppT> &proof);
	};

	template <typename ppT>
	VIR_keypair<ppT> VIR_Keygen(const size_t &u_size);

	template <typename ppT>
	libff::G1_vector<ppT> CommitmentKeyGen(/*const libff::G1<ppT> &G1_gen,*/ 
                                           const size_t &num_variables);

	template <typename ppT>
	commitment<ppT> Commit(const libff::G1_vector<ppT> &ck,
                           const std::vector<libff::Fr<ppT>>  &m);

	template <typename ppT>
	VIR_authentication<ppT> VIR_Authenticate(const std::vector<libff::Fr<ppT>>  &m,
                                             const uint8_t &raw_private_key_ptr);

	template <typename ppT>
	VIR_CRS<ppT> VIR_Setup(const VIR_keypair_pk<ppT> pk,
                           const r1cs_constraint_system<libff::Fr<ppT>> &r1cs);

	template <typename ppT>
	VIR_proof<ppT> VIR_prove(const VIR_CRS<ppT> &crs,
                             const VIR_authentication<ppT> &authentication,
                             const PrivateKeyPublicKeyResponse &BLS_pk,
                             const r1cs_primary_input<libff::Fr<ppT>> &u,
                             const r1cs_primary_input<libff::Fr<ppT>> &u_);

	template <typename ppT>
	bool VIR_verify(const VIR_CRS_without_Trapdoor<ppT> &crs,
                    const PrivateKeyPublicKeyResponse &BLS_pk,
                    const r1cs_primary_input<libff::Fr<ppT>> &u,
                    const r1cs_primary_input<libff::Fr<ppT>> &u_,
                    const VIR_authentication<ppT> &authentication,
                    const VIR_proof<ppT> &proof);

} // namespace libsnark

#include <libsnark/zk_proof_systems/ppzksnark/Verifiable_image_redacting/Verifiable_image_redacting.tcc>

#endif