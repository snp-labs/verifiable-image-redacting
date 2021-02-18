Verifiable image redacting
================================================================================
--------------------------------------------------------------------------------
Overview
--------------------------------------------------------------------------------

Image is a visual representation of a certain fact and canbe used as proof of events. As the utilization of the image increases, itis required to prove its authenticity with the protection of its sensitivepersonal information. It is a challenging problem to efficiently prove au-thenticity of partially redacted image without giving any informationabout the redacted part. In this paper, we propose a new efficient verifi-able image redacting scheme based on zk-SNARKs, a commitment, and adigital signature scheme. We adopt a commit-and-prove SNARK schemewhich takes commitments as inputs, in which the authenticity can bequickly verified outside the circuit. We also specify relations between theoriginal and redacted images to guarantee redacting correctness. Ourexperimental results show that the proposed scheme is superior to theexisting works in terms of the key size and proving time without sacrific-ing the other parameters. The security of the proposed scheme is provenformally.

You can read the details of VIR in https://eprint.iacr.org/2020/1579


--------------------------------------------------------------------------------
Build instructions
--------------------------------------------------------------------------------

First, setup up libsnark prerequires in reference to https://github.com/scipr-lab/libsnark

### Prerequisites

The repository additionally requires the following:

- OpenCV 3.4.0
- OpenSSL
- OpenMP

### Building

Create the Makefile:

    $ mkdir build && cd build && cmake ..

Then, to compile the library, tests, and profiling harness, run this within the `build` directory:

    $ make

To create the HTML documentation, run

    $ make doc

and then view the resulting `README.html` (which contains the very text you are reading now).

To compile and run the tests for this library, run:

    $ make check

--------------------------------------------------------------------------------
Build options
--------------------------------------------------------------------------------

The following flags change the behavior of the compiled code. Use

     $ cmake -Dname1=ON -Dname2=OFF ...

to control these (you can see the default at the top of CMakeLists.txt).

*   `cmake -DCURVE=choice` (where `choice` is one of: ALT_BN128, BN128, EDWARDS, MNT4, MNT6)

*   `cmake -DLOWMEM=ON`

     Limit the size of multi-exponentiation tables, for low-memory platforms.

*   `cmake -DWITH_PROCPS=OFF`

     Do not link against libprocps. This disables memory profiling.

*   `cmake -DWITH_SUPERCOP=OFF`

     Do not link against SUPERCOP for optimized crypto. The ADSNARK executables will not be built.

*   `cmake -DMULTICORE=ON`

     Enable parallelized execution of the ppzkSNARK generator and prover, using OpenMP.
     This will utilize all cores on the CPU for heavyweight parallelizable operations such as
     FFT and multiexponentiation. The default is single-core.

     To override the maximum number of cores used, set the environment variable `OMP_NUM_THREADS`
     at runtime (not compile time), e.g., `OMP_NUM_THREADS=8 test_r1cs_sp_ppzkpc`. It defaults
     to the autodetected number of cores, but on some devices, dynamic core management confused
     OpenMP's autodetection, so setting `OMP_NUM_THREADS` is necessary for full utilization.

*   `cmake -DUSE_PT_COMPRESSION=OFF`

    Do not use point compression.
    This gives much faster serialization times, at the expense of ~2x larger
    sizes for serialized keys and proofs.

*   `cmake -DMONTGOMERY_OUTPUT=ON` (enabled by default)

    Serialize Fp elements as their Montgomery representations. If this
    option is disabled then Fp elements are serialized as their
    equivalence classes, which is slower but produces human-readable
    output.

*    `cmake -DBINARY_OUTPUT=ON` (enabled by default)

     In serialization, output raw binary data (instead of decimal), which is smaller and faster.

*   `cmake -DPROFILE_OP_COUNTS=ON`

    Collect counts for field and curve operations inside static variables
    of the corresponding algebraic objects. This option works for all
    curves except bn128.

*    `cmake -DUSE_ASM=ON` (enabled by default)

    Use architecture-specific assembly routines for F[p] arithmetic and heap in
    multi-exponentiation. (If disabled, use GMP's `mpn_*` routines instead.)

*   `cmake -DUSE_MIXED_ADDITION=ON`

    Convert each element of the proving key and verification key to
    affine coordinates. This allows using mixed addition formulas in
    multiexponentiation and results in slightly faster prover and
    verifier runtime at expense of increased generator runtime.

*   `cmake -DPERFORMANCE=ON`

    Enables compiler optimizations such as link-time optimization, and disables debugging aids.
    (On some distributions this causes a `plugin needed to handle lto object` link error and `undefined reference`s, which can be remedied by `AR=gcc-ar make ...`.)

*   `cmake -DOPT_FLAGS=...`

    Set the C++ compiler optimization flags, overriding the default (e.g., `-DOPT_FLAGS="-Os -march=i386"`).

*   `cmake -DDEPENDS_DIR=...`

    Sets the dependency installation directory to the provided absolute path (default: installs dependencies in the respective submodule directories)

*   `cmake -DUSE_LINKED_LIBRARIES=ON`

    Setting this flag enables CMake to include installed `libfqfft` and `libff` libraries. This will tell the compiler to ignore the `libfqfft` and `libff` dependencies provided in the `depends` folder.

Not all combinations are tested together or supported by every part of the codebase.


--------------------------------------------------------------------------------
Portability
--------------------------------------------------------------------------------

Tested configurations include:

* Ubuntu 16.04 LTS with g++ 4.8 on x86-64: BN128
* Ubuntu 16.04 LTS with g++ 4.8 on ARM AArch32/AArch64: ALT_BN128

--------------------------------------------------------------------------------
Directory structure
--------------------------------------------------------------------------------

The following is the structure changed in libsnark

* [__libsnark__](libsnark)
    * [__common__](libsnark/common)
    * [__gadgetlib1__](libsnark/gadgetlib1)
    * [__gadgetlib2__](libsnark/gadgetlib2)
    * [__relations__](libsnark/relations)
        * [__constraint_satisfaction_problems__](libsnark/relations/constraint_satisfaction_problems): constructs r1cs filtering_example
    * [__zk_proof_systems__](libsnark/zk_proof_systems)
        * [__ppzksnark__](libsnark/relations/constraint_satisfaction_problems): constructs a VIR constraint system
    * [__reductions__](libsnark/reductions)
* [__depends__](depends)

Some of these module directories have the following subdirectories:

* ...
    * __examples__: example code and tutorials for this module
    * __tests__: unit tests for this module
    
