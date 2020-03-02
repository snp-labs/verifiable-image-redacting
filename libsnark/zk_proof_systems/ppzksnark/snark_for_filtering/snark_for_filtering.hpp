#ifndef SNARK_FOR_FILTERING_HPP_
#define SNARK_FOR_FILTERING_HPP_

#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/common/data_structures/accumulation_vector.hpp>
#include <libsnark/knowledge_commitment/knowledge_commitment.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/zk_proof_systems/ppzkadsnark/snark_for_filtering/snark_for_completment_params.hpp>
/*
namespace libsnark{

	template <typename ppT>
	class snark_for_filtering_proving_key;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const snark_for_filtering_proving_key<ppT> &pk);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, snark_for_filtering_proving_key<ppT> &pk);

	template <typename ppT>
	class snark_for_filtering_proving_key{
	public:
		libff::G1_vector<ppT> P_g1;

		snark_for_filtering_proving_key(){};
		snark_for_filtering_proving_key<ppT> &operator=(const snark_for_filtering_proving_key<ppT> &other) = default;
		snark_for_filtering_proving_key(const snark_for_filtering_proving_key<ppT> &other) = default;
		snark_for_filtering_proving_key(snark_for_filtering_proving_key<ppT> &&other) = default;
		snark_for_filtering_proving_key(const libff::G1_vector<ppT> &P_g1) : 
										P_g1(P_g1)
										{};

		size_t P_g1_size_in_bits() const
		{
			return P_g1.size()*libff::G1<ppT>::size_in_bits();
		}
		size_t size_in_bits() const
		{
			return P_g1_size_in_bits();
		}
		void print_size() const
		{
			libff::print_indent(); printf("Snark for Filtering PK size in bits: %zu\n",this->size_in_bits());
		}

    	bool operator==(const snark_for_filtering_proving_key<ppT> &other) const;
    	friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_filtering_proving_key<ppT> &pk);
    	friend std::istream& operator>> <ppT>(std::istream &in, snark_for_filtering_proving_key<ppT> &pk);
	};

	template <typename ppT>
	class snark_for_filtering_verification_key;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const snark_for_filtering_verification_key<ppT> &vk);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, snark_for_filtering_verification_key<ppT> &vk);

	template <typename ppT>
	class snark_for_filtering_verification_key{
	public:
		libff::G2<ppT> C1_g2; // g2^(k1 * a)
		libff::G2<ppT> C2_g2; // g2^(k2 * a)
		libff::G2<ppT> a_g2; // g2^a

		snark_for_filtering_verification_key() = default;
		snark_for_filtering_verification_key(const libff::G2<ppT> &&C1_g2,
												 const libff::G2<ppT> &&C2_g2,
												 const libff::G2<ppT> &&a_g2) :
			C1_g2(std::move(C1_g2)),
			C2_g2(std::move(C2_g2)),
			a_g2(std::move(a_g2))
		{};

		size_t C1_g2_size() const
		{
			return 2;
		}
		size_t C2_g2_size() const
		{
			return 2;
		}
		size_t a_g2_size() const
		{
			return 2;
		}
		size_t size_in_bits() const
		{
			return C1_g2_size() * libff::G2<ppT>::size_in_bits() 
				+ C2_g2_size() * libff::G2<ppT>::size_in_bits() 
				+ a_g2_size() * libff::G2<ppT>::size_in_bits();
		}
		void print_size() const
		{
			libff::print_indent(); printf("* LEGO VK size in bits: %zu\n",this->size_in_bits());
		}
    	bool operator==(const snark_for_filtering_verification_key<ppT> &other) const;
    	friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_filtering_verification_key<ppT> &pk);
    	friend std::istream& operator>> <ppT>(std::istream &in, snark_for_filtering_verification_key<ppT> &pk);
	};

	template <typename ppT>
	class snark_for_filtering_keypair;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const snark_for_filtering_keypair<ppT> &KEYPAIR);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, snark_for_filtering_keypair<ppT> &KEYPAIR);
	
	template <typename ppT>
	class snark_for_filtering_keypair
	{
	  public:
		snark_for_filtering_proving_key<ppT> pk;
		snark_for_filtering_verification_key<ppT> vk;

		snark_for_filtering_keypair(const snark_for_filtering_keypair<ppT> &other):
			pk(std::move(other.pk)),
			vk(std::move(other.vk))
		{};
		snark_for_filtering_keypair(snark_for_filtering_proving_key<ppT> &&pk,
									snark_for_filtering_verification_key<ppT> &&vk) : 
				pk(std::move(pk)),
				vk(std::move(vk))
		{};

		bool operator==(const snark_for_filtering_keypair<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const snark_for_filtering_keypair<ppT> &KEYPAIR);
		friend std::istream &operator>><ppT>(std::istream &in, snark_for_filtering_keypair<ppT> &KEYPAIR);
	};

	template <typename ppT>
	class snark_for_filtering_proof;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const snark_for_filtering_proof<ppT> &proof);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, snark_for_filtering_proof<ppT> &proof);

	template <typename ppT>
	class snark_for_filtering_proof
	{
	  public:
	  	libff::G1<ppT> proof_G1;

		snark_for_filtering_proof();
		snark_for_filtering_proof(libff::G1<ppT> &&proof_G1) :
			proof_G1(std::move(proof_G1))
		{};

		size_t proof_G1_size() const
		{
			return 1;
		}
		size_t size_in_bits() const
		{
			return proof_G1_size() * libff::G1<ppT>::size_in_bits();
		}
		void print_size() const
		{
			libff::print_indent(); printf("* LEGO Proof size in bits: %zu\n", this->size_in_bits());
		}


		bool operator==(const snark_for_filtering_proof<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const snark_for_filtering_proof<ppT> &proof);
		friend std::istream &operator>><ppT>(std::istream &in, snark_for_filtering_proof<ppT> &proof);
	};

	template<typename ppT>
	snark_for_filtering_keypair<ppT> snark_for_filtering_generator(accumulation_vector<libff::G1<ppT> > &gamma_H, accumulation_vector<libff::G1<ppT> > &gamma_F, size_t len);
	
	template<typename ppT>
	snark_for_filtering_proof<ppT> snark_for_filtering_prover(const snark_for_filtering_proving_key<ppT> &pk,
																  const r1cs_gg_ppzksnark_primary_input<ppT> &r1cs_gg_ppzksnark_primary_input, size_t len);

	template<typename ppT>
	bool snark_for_filtering_verifier(const snark_for_filtering_verification_key<ppT> &vk, 
                                    const libff::G1<ppT> &cm1, 
                                    const libff::G1<ppT> &cm2,
                                    const snark_for_filtering_proof<ppT> &proof);    
}
*/
#endif