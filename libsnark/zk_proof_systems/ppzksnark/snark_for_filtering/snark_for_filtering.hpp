#ifndef SNARK_FOR_FILTERING_HPP_
#define SNARK_FOR_FILTERING_HPP_

#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/common/data_structures/accumulation_vector.hpp>
#include <libsnark/knowledge_commitment/knowledge_commitment.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_completment.hpp>

namespace libsnark{

	template<typename ppT>
	class snark_for_filtering_Commit;

	template<typename ppT>
	std::ostream& operator<<(std::ostream &out, const snark_for_filtering_Commit<ppT> &commit);

	template<typename ppT>
	std::istream& operator>>(std::istream &in, snark_for_filtering_Commit<ppT> &commit);

	template<typename ppT>
	class snark_for_filtering_Commit {
	public:
		libff::G1<ppT> sigma_x;
		libff::Fr<ppT> x0;


		bool operator==(const snark_for_filtering_Commit &other) const;
		friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_filtering_Commit<ppT> &commit);
		friend std::istream& operator>> <ppT>(std::istream &in, snark_for_filtering_Commit<ppT> &commit);
		
		snark_for_filtering_Commit() = default;
		snark_for_filtering_Commit(const libff::G1<ppT> &&sigma_x,
								   const libff::Fr<ppT> &&x0) :
			sigma_x(std::move(sigma_x)),
			x0(std::move(x0))
		{};
	};
	template <typename ppT>
	class snark_for_filtering_proving_key;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const snark_for_filtering_proving_key<ppT> &pk);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, snark_for_filtering_proving_key<ppT> &pk);

	template <typename ppT>
	class snark_for_filtering_proving_key{
	public:
		libff::G1_vector<ppT> P_vector;
		libff::G1_vector<ppT> f_vector;
		libff::G1<ppT> alpha_g1;
    	libff::G1<ppT> beta_g1;
    	libff::G2<ppT> beta_g2;
    	libff::G1<ppT> delta_g1;
    	libff::G2<ppT> delta_g2;

		libff::G1_vector<ppT> A_query; // this could be a sparse vector if we had multiexp for those
    	knowledge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > B_query;
    	libff::G1_vector<ppT> H_query;
	    snark_for_completment_constraint_system<ppT> constraint_system;

    snark_for_filtering_proving_key() {};
    snark_for_filtering_proving_key<ppT>& operator=(const snark_for_filtering_proving_key<ppT> &other) = default;
    snark_for_filtering_proving_key(const snark_for_filtering_proving_key<ppT> &other) = default;
    snark_for_filtering_proving_key(snark_for_filtering_proving_key<ppT> &&other) = default;
    snark_for_filtering_proving_key(libff::G1_vector<ppT> &&P_vector,
								  libff::G1_vector<ppT> &&f_vector,
								  libff::G1<ppT> &&alpha_g1,
                                  libff::G1<ppT> &&beta_g1,
                                  libff::G2<ppT> &&beta_g2,
                                  libff::G1<ppT> &&delta_g1,
                                  libff::G2<ppT> &&delta_g2,
                                  libff::G1_vector<ppT> &&A_query,
                                  knowledge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > &&B_query,
                                  libff::G1_vector<ppT> &&H_query,
                                //   libff::G1_vector<ppT> &&L_query,
                                  snark_for_completment_constraint_system<ppT> &&constraint_system) :
        P_vector(std::move(P_vector)),
		f_vector(std::move(f_vector)),
		alpha_g1(std::move(alpha_g1)),
        beta_g1(std::move(beta_g1)),
        beta_g2(std::move(beta_g2)),
        delta_g1(std::move(delta_g1)),
        delta_g2(std::move(delta_g2)),
        A_query(std::move(A_query)),
        B_query(std::move(B_query)),
        H_query(std::move(H_query)),
        // L_query(std::move(L_query)),
        constraint_system(std::move(constraint_system))
    {};

    size_t G1_size() const
    {
        return 1 + A_query.size() + B_query.domain_size() + H_query.size() + P_vector.size() + f_vector.size();
    }

    size_t G2_size() const
    {
        return 1 + B_query.domain_size();
    }

    size_t G1_sparse_size() const
    {
        return 1 + A_query.size() + B_query.size() + H_query.size() + P_vector.size() + f_vector.size();
    }

    size_t G2_sparse_size() const
    {
        return 1 + B_query.size();
    }

    size_t size_in_bits() const
    {
        return (libff::size_in_bits(A_query) + B_query.size_in_bits() +
                libff::size_in_bits(H_query) + libff::size_in_bits(P_vector) + libff::size_in_bits(f_vector) +
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
		libff::G2<ppT> c0_g2;
		libff::G2<ppT> c1_g2;
		libff::G2<ppT> c2_g2;
		libff::G2<ppT> a_g2;
		libff::GT<ppT> alpha_g1_beta_g2;
    	libff::G2<ppT> delta_g2;


		snark_for_filtering_verification_key() = default;
		snark_for_filtering_verification_key(const libff::G2<ppT> &c0_g2,
											const libff::G2<ppT> &c1_g2,
											const libff::G2<ppT> &c2_g2,
											const libff::G2<ppT> &a_g2,
											const libff::GT<ppT> &alpha_g1_beta_g2,
                                       		const libff::G2<ppT> &delta_g2) :
			c0_g2(c0_g2),
			c1_g2(c1_g2),
			c2_g2(c2_g2),
			a_g2(a_g2),
			alpha_g1_beta_g2(alpha_g1_beta_g2),
        	delta_g2(delta_g2)
		{};

		size_t G1_size() const
		{
			return 0;
		}

		size_t G2_size() const
		{
			return 6;
		}

		size_t GT_size() const
		{
			return 1;
		}

		size_t size_in_bits() const
		{
			// TODO: include GT size
			return (2 * libff::G2<ppT>::size_in_bits());
		}

		void print_size() const
		{
			libff::print_indent(); printf("* G1 elements in VK: %zu\n", this->G1_size());
			libff::print_indent(); printf("* G2 elements in VK: %zu\n", this->G2_size());
			libff::print_indent(); printf("* GT elements in VK: %zu\n", this->GT_size());
			libff::print_indent(); printf("* VK size in bits: %zu\n", this->size_in_bits());
		}

		bool operator==(const snark_for_filtering_verification_key<ppT> &other) const;
		friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_filtering_verification_key<ppT> &vk);
		friend std::istream& operator>> <ppT>(std::istream &in, snark_for_filtering_verification_key<ppT> &vk);

		static snark_for_filtering_verification_key<ppT> dummy_verification_key(const size_t input_size);
};

template <typename ppT>
	class snark_for_filtering_public_parameter;

	template <typename ppT>
	std::ostream &operator<<(std::ostream &out, const snark_for_filtering_public_parameter<ppT> &pp);

	template <typename ppT>
	std::istream &operator>>(std::istream &in, snark_for_filtering_public_parameter<ppT> &pp);

	template <typename ppT>
	class snark_for_filtering_public_parameter{
	public:
		libff::G1_vector<ppT> h_vector;

		snark_for_filtering_public_parameter() = default;
		snark_for_filtering_public_parameter(
											const libff::G1_vector<ppT> &&h_vector) :
			h_vector(std::move(h_vector))
		{};

		size_t G1_size() const
		{
			return h_vector.size();
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
			return (2 * libff::G2<ppT>::size_in_bits());
		}

		void print_size() const
		{
			libff::print_indent(); printf("* G1 elements in pp: %zu\n", this->G1_size());
			libff::print_indent(); printf("* G2 elements in pp: %zu\n", this->G2_size());
			libff::print_indent(); printf("* GT elements in pp: %zu\n", this->GT_size());
			libff::print_indent(); printf("* PP size in bits: %zu\n", this->size_in_bits());
		}

		bool operator==(const snark_for_filtering_public_parameter<ppT> &other) const;
		friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_filtering_public_parameter<ppT> &vk);
		friend std::istream& operator>> <ppT>(std::istream &in, snark_for_filtering_public_parameter<ppT> &vk);
};

/************************ Processed verification key *************************/

template<typename ppT>
class snark_for_filtering_processed_verification_key;

template<typename ppT>
std::ostream& operator<<(std::ostream &out, const snark_for_filtering_processed_verification_key<ppT> &pvk);

template<typename ppT>
std::istream& operator>>(std::istream &in, snark_for_filtering_processed_verification_key<ppT> &pvk);

/**
 * A processed verification key for the R1CS GG-ppzkSNARK.
 *
 * Compared to a (non-processed) verification key, a processed verification key
 * contains a small constant amount of additional pre-computed information that
 * enables a faster verification time.
 */
template<typename ppT>
class snark_for_filtering_processed_verification_key {
public:
    libff::G2<ppT> a_g2;
    libff::G2<ppT> c0_g2;
    libff::G2<ppT> c1_g2;
    libff::G2<ppT> c2_g2;
    libff::GT<ppT> vk_alpha_g1_beta_g2;
    libff::G2_precomp<ppT> vk_delta_g2_precomp;


    bool operator==(const snark_for_filtering_processed_verification_key &other) const;
    friend std::ostream& operator<< <ppT>(std::ostream &out, const snark_for_filtering_processed_verification_key<ppT> &pvk);
    friend std::istream& operator>> <ppT>(std::istream &in, snark_for_filtering_processed_verification_key<ppT> &pvk);
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
		snark_for_filtering_public_parameter<ppT> pp;

		snark_for_filtering_keypair(const snark_for_filtering_keypair<ppT> &other):
			pk(std::move(other.pk)),
			vk(std::move(other.vk)),
			pp(std::move(other.pp))
		{};
		snark_for_filtering_keypair(snark_for_filtering_proving_key<ppT> &&pk,
									snark_for_filtering_verification_key<ppT> &&vk,
									snark_for_filtering_public_parameter<ppT> &&pp) : 
				pk(std::move(pk)),
				vk(std::move(vk)),
				pp(std::move(pp))
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
	  	snark_for_completment_proof<ppT> completment_proof;
		libff::G1<ppT> ss_proof_g1;
		libff::G1<ppT> _C_x;

		snark_for_filtering_proof()
		{
        // invalid proof with valid curve points
        this->completment_proof->g_A = libff::G1<ppT>::one();
        this->completment_proof->g_B = libff::G2<ppT>::one();
        this->completment_proof->g_C = libff::G1<ppT>::one();
		this->ss_proof_g1 = libff::G1<ppT>::one();
		this->_C_x = libff::G1<ppT>::one();
    	}
		snark_for_filtering_proof(snark_for_completment_proof<ppT> &&completment_proof, 
								  libff::G1<ppT> &&ss_proof_g1, libff::G1<ppT> &&_C_x) :
			completment_proof(std::move(completment_proof)),
			ss_proof_g1(std::move(ss_proof_g1)),
			_C_x(std::move(_C_x))
		{};

		size_t proof_size() const
		{
			return 5;
		}
		size_t size_in_bits() const
		{
			return proof_size() * libff::G1<ppT>::size_in_bits();
		}
		void print_size() const
		{
			libff::print_indent(); printf("* snark_for_filtering_proof size in bits: %zu\n", this->size_in_bits());
		}


		bool operator==(const snark_for_filtering_proof<ppT> &other) const;
		friend std::ostream &operator<<<ppT>(std::ostream &out, const snark_for_filtering_proof<ppT> &proof);
		friend std::istream &operator>><ppT>(std::istream &in, snark_for_filtering_proof<ppT> &proof);
	};

	template<typename ppT>
	snark_for_filtering_Commit<ppT> Commit(const snark_for_filtering_public_parameter<ppT> &pp,
                                       const libff::Fr_vector<ppT> &xi_vector);

	template <typename ppT>
	snark_for_filtering_keypair<ppT> snark_for_filtering_generator(const r1cs_constraint_system<libff::Fr<ppT> > &r1cs);
	
	template<typename ppT>
	snark_for_filtering_proof<ppT> snark_for_filtering_prover(const snark_for_filtering_proving_key<ppT> &pk, 
                                                    const snark_for_completment_primary_input<ppT> &primary_input,
                                                    const snark_for_completment_auxiliary_input<ppT> &auxiliary_input,
                                                    const libff::Fr<ppT> &x0);

	template<typename ppT>
	snark_for_filtering_processed_verification_key<ppT> 
	snark_for_filtering_verifier_process_vk	(const snark_for_filtering_verification_key<ppT> &vk);

	template <typename ppT>
	bool snark_for_filtering_verifier(const snark_for_filtering_verification_key<ppT> &vk, 
                                    const libff::G1<ppT> &sigma_x, 
                                    const libff::G1<ppT> &c_x,
                                    const snark_for_filtering_proof<ppT> &proof);
}

#include <libsnark/zk_proof_systems/ppzksnark/snark_for_filtering/snark_for_filtering.tcc>

#endif