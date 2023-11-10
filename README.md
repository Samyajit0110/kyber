> **Warning** 
This Kyber implementation is conformant with Kyber specification https://pq-crystals.org/kyber/data/kyber-specification-round3-20210804.pdf and I also *try* to make it constant-time but be informed that it is not yet audited. *If you consider using it in production, be careful !*

# kyber
CRYSTALS-Kyber: Post-Quantum Public-key Encryption &amp; Key-establishment Algorithm

## Motivation

Kyber is being standardized by NIST as post-quantum secure key encapsulation mechanism (KEM), which can be used for key establishment.

Kyber offers an *IND-CCA2-secure* Key Encapsulation Mechanism - its security is based on the hardness of solving the learning-with-errors (LWE) problem in module (i.e. structured) lattices.

Kyber Key Encapsulation Mechanism is built on top of *IND-CPA-secure Kyber Public Key Encryption*, where two communicating parties, both generating their key pairs, while publishing their public keys to each other, can encrypt fixed length ( = 32 -bytes ) message using peer's public key. Cipher text can be decrypted by corresponding secret key ( which is private to the keypair owner ) and 32 -bytes message can be recovered back. Then a slightly tweaked Fujisaki–Okamoto (FO) transform is applied on *IND-CPA-secure Kyber PKE* - giving us the *IND-CCA2-secure KEM* construction. In KEM scheme, two parties interested in establishing a secure communication channel over public & insecure channel, can generate a shared secret key ( of arbitrary byte length ) from a key derivation function ( i.e. KDF which is SHAKE256 Xof in this context ) which is obtained by both of these parties as result of seeding SHAKE256 Xof with same secret. This secret is 32 -bytes and that's what is communicated by sender to receiver using underlying Kyber PKE scheme.

Algorithm | Input | Output
--- | :-: | --:
KEM KeyGen | - | Public Key and Secret Key
Encapsulation | Public Key | Cipher Text and SHAKE256 KDF
Decapsulation | Secret Key and Cipher Text | SHAKE256 KDF

> **Note**
*IND-CCA2-secure Kyber KEM* can be used for synchronous secure communication such as TLS.

Here I'm maintaining `kyber` - a header-only and easy-to-use ( see more in [usage](#usage) ) C++ library implementing Kyber KEM, supporting Kyber-{512, 768, 1024} parameter sets, as defined in table 1 of Kyber specification. `sha3` and `subtle` are two dependencies of this library, which are pinned to specific commit, using git submodule.

> **Note**
Find Kyber specification https://pq-crystals.org/kyber/data/kyber-specification-round3-20210804.pdf - this is the document that I followed when implementing Kyber. I suggest you go through the specification to get an in-depth understanding of Kyber PQC suite.

> **Note**
Find progress of NIST PQC standardization effort @ https://csrc.nist.gov/projects/post-quantum-cryptography.

## Prerequisites

- A C++ compiler with C++20 standard library such as `clang++`/ `g++`.

```bash
$ clang++ --version
Ubuntu clang version 17.0.2 (1~exp1ubuntu2.1)
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

$  g++ --version
g++ (Ubuntu 13.2.0-4ubuntu3) 13.2.0
```

- Build tools such as `make`, `cmake`.

```bash
$ make --version
GNU Make 4.3

$ cmake --version
cmake version 3.25.1
```

- For testing Kyber KEM implementation, you need to globally install `google-test` library and headers. Follow [this](https://github.com/google/googletest/tree/main/googletest#standalone-cmake-project) guide, if you don't have it installed.
- For benchmarking Kyber KEM implementation, targeting CPU systems, you'll need to have `google-benchmark` header and library globally installed. I found guide @ https://github.com/google/benchmark#installation helpful.

> **Note**
If you are on a machine running GNU/Linux kernel and you want to obtain *CPU cycle* count for KEM routines, you should consider building `google-benchmark` library with `libPFM` support, following https://gist.github.com/itzmeanjan/05dc3e946f635d00c5e0b21aae6203a7, a step-by-step guide. Find more about libPFM @ https://perfmon2.sourceforge.net.

- For importing dependencies `sha3`, `subtle` - initialize & update git submodule after cloning this repository.

```bash
git clone https://github.com/itzmeanjan/kyber.git

pushd kyber
git submodule update --init
# Now you can {test, benchmark, use} `kyber` library
popd
```

## Testing

For testing functional correctness and conformance with Kyber specification, you have to issue

> **Note**
Known Answer Test (KAT) files living in [this](./kats/) directory are generated by following (reproducible) steps, described in https://gist.github.com/itzmeanjan/c8f5bc9640d0f0bdd2437dfe364d7710.

```bash
make -j
```

```bash
[==========] Running 10 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 10 tests from KyberKEM
[ RUN      ] KyberKEM.CompressDecompressZq
[       OK ] KyberKEM.CompressDecompressZq (97 ms)
[ RUN      ] KyberKEM.ArithmeticOverZq
[       OK ] KyberKEM.ArithmeticOverZq (143 ms)
[ RUN      ] KyberKEM.Kyber512KeygenEncapsDecaps
[       OK ] KyberKEM.Kyber512KeygenEncapsDecaps (0 ms)
[ RUN      ] KyberKEM.Kyber768KeygenEncapsDecaps
[       OK ] KyberKEM.Kyber768KeygenEncapsDecaps (0 ms)
[ RUN      ] KyberKEM.Kyber1024KeygenEncapsDecaps
[       OK ] KyberKEM.Kyber1024KeygenEncapsDecaps (0 ms)
[ RUN      ] KyberKEM.Kyber512KnownAnswerTests
[       OK ] KyberKEM.Kyber512KnownAnswerTests (6 ms)
[ RUN      ] KyberKEM.Kyber768KnownAnswerTests
[       OK ] KyberKEM.Kyber768KnownAnswerTests (10 ms)
[ RUN      ] KyberKEM.Kyber1024KnownAnswerTests
[       OK ] KyberKEM.Kyber1024KnownAnswerTests (16 ms)
[ RUN      ] KyberKEM.NumberTheoreticTransform
[       OK ] KyberKEM.NumberTheoreticTransform (0 ms)
[ RUN      ] KyberKEM.PolynomialSerialization
[       OK ] KyberKEM.PolynomialSerialization (0 ms)
[----------] 10 tests from KyberKEM (275 ms total)

[----------] Global test environment tear-down
[==========] 10 tests from 1 test suite ran. (275 ms total)
[  PASSED  ] 10 tests.
```

## Benchmarking

For benchmarking Kyber KEM routines ( i.e. keygen, encaps and decaps ) for various suggested parameter sets, you have to issue.

```bash
make benchmark  # If you haven't built google-benchmark library with libPFM support.
make perf       # If you have built google-benchmark library with libPFM support.
```

> **Note**
Benchmarking expects presence of `google-benchmark` header and library in global namespace ( so that it can be found by the compiler ).

> **Warning**
When benchmarking, ensure that you've disabled CPU frequency scaling, by following guide @ https://github.com/google/benchmark/blob/main/docs/reducing_variance.md.

> **Note**
`make perf` - was issued when collecting following benchmarks. Notice, *cycles* column, denoting cost of executing Kyber KEM routines in terms of CPU cycles. Follow [this](https://github.com/google/benchmark/blob/main/docs/perf_counters.md) for more details.

### On 12th Gen Intel(R) Core(TM) i7-1260P ( compiled with GCC-13.2.0 )

```bash
2023-11-10T23:09:58+05:30
Running ./build/perf.out
Run on (16 X 3475.61 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.58, 0.37, 0.37
----------------------------------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations     CYCLES items_per_second
----------------------------------------------------------------------------------------------
kyber768/keygen_mean          32.8 us         32.8 us            8    153.61k       30.4661k/s
kyber768/keygen_median        32.8 us         32.8 us            8   153.499k       30.5186k/s
kyber768/keygen_stddev       0.112 us        0.112 us            8    370.822        103.635/s
kyber768/keygen_cv            0.34 %          0.34 %             8      0.24%            0.34%
kyber768/encap_mean           39.7 us         39.7 us            8   185.442k       25.1981k/s
kyber768/encap_median         39.7 us         39.7 us            8   185.491k       25.2125k/s
kyber768/encap_stddev        0.252 us        0.250 us            8     571.21        158.491/s
kyber768/encap_cv             0.63 %          0.63 %             8      0.31%            0.63%
kyber512/encap_mean           25.0 us         25.0 us            8   116.878k       40.0448k/s
kyber512/encap_median         25.0 us         25.0 us            8   116.902k       40.0477k/s
kyber512/encap_stddev        0.043 us        0.042 us            8    189.697        68.0994/s
kyber512/encap_cv             0.17 %          0.17 %             8      0.16%            0.17%
kyber768/decap_mean           47.8 us         47.8 us            8    223.57k       20.9403k/s
kyber768/decap_median         47.8 us         47.8 us            8   223.666k       20.9417k/s
kyber768/decap_stddev        0.061 us        0.059 us            8    319.863        26.0114/s
kyber768/decap_cv             0.13 %          0.12 %             8      0.14%            0.12%
kyber1024/encap_mean          56.7 us         56.7 us            8   264.067k       17.6263k/s
kyber1024/encap_median        56.8 us         56.8 us            8   263.945k        17.617k/s
kyber1024/encap_stddev       0.333 us        0.334 us            8    605.632        103.857/s
kyber1024/encap_cv            0.59 %          0.59 %             8      0.23%            0.59%
kyber1024/decap_mean          66.4 us         66.4 us            8   311.206k       15.0527k/s
kyber1024/decap_median        66.4 us         66.4 us            8   311.099k        15.062k/s
kyber1024/decap_stddev       0.162 us        0.160 us            8     569.95         36.147/s
kyber1024/decap_cv            0.24 %          0.24 %             8      0.18%            0.24%
kyber1024/keygen_mean         49.3 us         49.3 us            8   229.629k       20.2711k/s
kyber1024/keygen_median       49.1 us         49.1 us            8   229.512k        20.367k/s
kyber1024/keygen_stddev      0.494 us        0.494 us            8    417.077        200.907/s
kyber1024/keygen_cv           1.00 %          1.00 %             8      0.18%            0.99%
kyber512/keygen_mean          20.1 us         20.1 us            8   93.7752k       49.8383k/s
kyber512/keygen_median        20.1 us         20.1 us            8    93.788k       49.8088k/s
kyber512/keygen_stddev       0.070 us        0.070 us            8    424.419        174.601/s
kyber512/keygen_cv            0.35 %          0.35 %             8      0.45%            0.35%
kyber512/decap_mean           30.9 us         30.9 us            8   144.779k       32.3551k/s
kyber512/decap_median         30.9 us         30.9 us            8   144.761k       32.3463k/s
kyber512/decap_stddev        0.051 us        0.051 us            8    243.229        53.6646/s
kyber512/decap_cv             0.17 %          0.17 %             8      0.17%            0.17%
```

### On 12th Gen Intel(R) Core(TM) i7-1260P ( compiled with Clang-17.0.2 )

```bash
2023-11-10T23:08:58+05:30
Running ./build/perf.out
Run on (16 X 452.225 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.20, 0.25, 0.33
----------------------------------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations     CYCLES items_per_second
----------------------------------------------------------------------------------------------
kyber768/decap_mean           32.9 us         32.9 us            8   154.408k       30.3581k/s
kyber768/decap_median         33.0 us         33.0 us            8   154.456k       30.3446k/s
kyber768/decap_stddev        0.142 us        0.141 us            8    670.058        130.207/s
kyber768/decap_cv             0.43 %          0.43 %             8      0.43%            0.43%
kyber768/keygen_mean          23.0 us         23.0 us            8   107.701k       43.5071k/s
kyber768/keygen_median        23.0 us         23.0 us            8   107.814k       43.4851k/s
kyber768/keygen_stddev       0.055 us        0.061 us            8    343.297        116.603/s
kyber768/keygen_cv            0.24 %          0.27 %             8      0.32%            0.27%
kyber1024/keygen_mean         35.8 us         35.8 us            8   167.758k       27.9348k/s
kyber1024/keygen_median       35.7 us         35.7 us            8   167.493k       27.9923k/s
kyber1024/keygen_stddev      0.516 us        0.516 us            8   2.34183k          402.6/s
kyber1024/keygen_cv           1.44 %          1.44 %             8      1.40%            1.44%
kyber768/encap_mean           28.3 us         28.2 us            8   132.394k       35.3991k/s
kyber768/encap_median         28.2 us         28.2 us            8   132.309k        35.425k/s
kyber768/encap_stddev        0.099 us        0.098 us            8    448.832         122.72/s
kyber768/encap_cv             0.35 %          0.35 %             8      0.34%            0.35%
kyber1024/decap_mean          49.4 us         49.4 us            8   230.411k       20.2582k/s
kyber1024/decap_median        49.2 us         49.2 us            8   230.322k       20.3074k/s
kyber1024/decap_stddev       0.376 us        0.379 us            8   2.14278k        154.659/s
kyber1024/decap_cv            0.76 %          0.77 %             8      0.93%            0.76%
kyber1024/encap_mean          42.5 us         42.5 us            8   198.951k        23.548k/s
kyber1024/encap_median        42.4 us         42.4 us            8   198.595k       23.5846k/s
kyber1024/encap_stddev       0.253 us        0.241 us            8   1.14419k        132.681/s
kyber1024/encap_cv            0.60 %          0.57 %             8      0.58%            0.56%
kyber512/keygen_mean          13.9 us         13.9 us            8   65.0898k       72.0204k/s
kyber512/keygen_median        13.9 us         13.9 us            8   65.0964k       71.9964k/s
kyber512/keygen_stddev       0.133 us        0.130 us            8    605.256        673.997/s
kyber512/keygen_cv            0.96 %          0.93 %             8      0.93%            0.94%
kyber512/encap_mean           17.6 us         17.6 us            8   82.6474k       56.6978k/s
kyber512/encap_median         17.6 us         17.6 us            8   82.6171k        56.724k/s
kyber512/encap_stddev        0.105 us        0.107 us            8    515.213        342.806/s
kyber512/encap_cv             0.59 %          0.60 %             8      0.62%            0.60%
kyber512/decap_mean           21.0 us         21.0 us            8    98.379k       47.6137k/s
kyber512/decap_median         21.0 us         21.0 us            8    98.279k       47.6444k/s
kyber512/decap_stddev        0.079 us        0.087 us            8    464.746        197.312/s
kyber512/decap_cv             0.38 %          0.41 %             8      0.47%            0.41%
```

## Usage

`kyber` is written as a header-only C++ library, majorly targeting 64 -bit platforms and it's pretty easy to get started with. All you need to do is following.

- Clone `kyber` repository.

```bash
cd
git clone https://github.com/itzmeanjan/kyber.git
```

- Initialize and update git submodule, so that dependencies i.e. `sha3` and `subtle` are available inside `kyber` source tree.

```bash
cd kyber
git submodule update --init
```

- Write your program while including proper header files ( based on which variant of Kyber KEM you want to use, see [include](./include) directory ), which includes declarations ( and definitions ) of all required KEM routines and constants ( such as byte length of public/ private keys and cipher text ).

```cpp
// main.cpp

#include "kyber512_kem.hpp"
#include <algorithm>
#include <array>
#include <cassert>

int
main()
{
  std::array<uint8_t, 32> d{}; // seed
  std::array<uint8_t, 32> z{}; // seed
  std::array<uint8_t, kyber512_kem::PKEY_LEN> pkey{};
  std::array<uint8_t, kyber512_kem::SKEY_LEN> skey{};
  std::array<uint8_t, 32> m{}; // seed
  std::array<uint8_t, kyber512_kem::CIPHER_LEN> cipher{};

  // Be careful !
  //
  // Read API documentation in include/prng.hpp
  prng::prng_t prng;

  prng.read(d);
  prng.read(z);
  prng.read(m);

  kyber512_kem::keygen(d, z, pkey, skey);
  auto skdf = kyber512_kem::encapsulate(m, pkey, cipher);
  auto rkdf = kyber512_kem::decapsulate(skey, cipher);

  std::array<uint8_t, 32> sender_key{};
  skdf.squeeze(sender_key);

  std::array<uint8_t, 32> receiver_key{};
  rkdf.squeeze(receiver_key);

  assert(std::ranges::equal(sender_key, receiver_key));
  return 0;
}
```

- When compiling your program, let your compiler know where it can find `kyber`, `sha3` and `subtle` headers, which includes their definitions ( kyber being a header-only library ) too.

```bash
# Assuming `kyber` was cloned just under $HOME

KYBER_HEADERS=~/kyber/include
SHA3_HEADERS=~/kyber/sha3/include
SUBTLE_HEADERS=~/kyber/subtle/include

g++ -std=c++20 -Wall -O3 -march=native -I $KYBER_HEADERS -I $SHA3_HEADERS -I $SUBTLE_HEADERS main.cpp
```

Kyber KEM Variant | Namespace | Header
:-- | :-: | --:
Kyber512 KEM Routines | `kyber512_kem::` | [include/kyber512_kem.hpp](include/kyber512_kem.hpp)
Kyber768 KEM Routines | `kyber768_kem::` | [include/kyber768_kem.hpp](include/kyber768_kem.hpp)
Kyber1024 KEM Routines | `kyber1024_kem::` | [include/kyber1024_kem.hpp](include/kyber1024_kem.hpp)

> **Note**
Kyber parameter sets are selected from table 1 of Kyber specification https://pq-crystals.org/kyber/data/kyber-specification-round3-20210804.pdf.

See example [program](./example/kyber512_kem.cpp), where I show how to use Kyber512 KEM API. You can almost similarly use Kyber768 or Kyber1024 KEM API, by just importing correct header file and using KEM functions/ constants from respective namespace.

```bash
g++ -std=c++20 -Wall -O3 -march=native -I ./include -I ./sha3/include -I ./subtle/include/ examples/kyber512_kem.cpp && ./a.out
```

```bash
Kyber512 KEM

pubkey        : 175782d35b2666833aee098617626d88dbcc47091a011882d52105acc218c9287a95276a3259a6a94aa386d8148886abdcc1841f39260ce4754ebacc1fd36102905d4c623d0b27930b4c249ee7380758c0ac5982b0e932eda95184a40f55c451d835861ca2b314dbce97829f1b92752dda592d8960b2540f464988ea1c974c63467c439b1de540490b0af0491a6507951ebc971887bd2b4a11327381d99586f10668c83abe92fb649b113da7ec666729bc1cc38a1de137dd3cc4e3a6abb9881a2ee63e7df3ad6cb680664ba1559ca17448c968b7c867ac5f324911ffd43993b8a7b8f57094c786877c1208fa7f53e51d6f1a46ae71bc81f78ebe5808d48200b7e1bc81ec3d31070a6993aa5db237eb3a4c592aa559a73bd769583a0ad095ec1669b952be4a71fe8603f5d597f007a048cc9d7fea6735383b6b8bbf896b74dc48a21840a92c497a9bc7434b0241a9e42e6428515d477c4e0b3678fab1d619b794f01b828648e7577bb2e5297915b9fdf33cb291a37de51b51c7aca6f07994193bd981134da2340c23a93cda8b68e429ac801d3748b8d112b57e388511e3305e50a51184b623607447468be94351cd0b9111a119b4b3c6f270c1cfea749a2ac89455590280c369163946481dbaeb4693dbb376202db2d8464c61aea6411cd887080f5c59e1587da01510cd1b0e8b030a5c200639ba26376134e88279891b90373cc92e7a76c0aaa33d084ab3f61e175010996652e441300ad5aefda9cc88f17fef2102b643179e0a49a60c47ce06c5b1a0b150b09ca4593e5dd48a9b1979d103ba862c43ed354d2ec99575b70e741808288aa0e1cb792c0a458d4584ddfa1870d7b797e2aac7d4cc08916015401338d8841d226d9656661cda93f53343e0f906b82bce8f25428b02a639a47f7dda5b946a3785656fb6d083df5a5ec7493cc017a2469b1f43c96f2e3bbc9d6cb07bec82d721a4cfba6ca2c59b0e01bda98585692b9da753923f830b52c843b6d963f959ad60189f42d61df7808f4d131c4d233e246c4735193e516452061701e6114cf1587a54c79105f48fdce9c2134bb60550b242945ea011ec54c570054b93d96f072426b7c9b524db8d2f136b7db2d1f38897
seckey        : a598a250c2008688af8f71a285abae5b528a19479acf915cd2f92a7365bc757c670accc4b2190aa77b7d0c76355962a0ea9b6a1f4400be77797a6851776815032307913aa475b733a1ba698b2134ea25a57bd9b979e2bcb7d99f24f06ee760227486ae1cdaba79065bc3180d79a0906c514e5b973435c00f34b87e882643ef6b42bcbca4a3b65207abb5dca76e49a9be7a6013d256bc09b1211b70bb28e2151200c6c1e00082e88634600a29e3cf5ff541051c703ac373a91228a6d30491221df6749e22b21429612ed4ba07a7d7789717809e498f2e3a1b8a6a40afe0a7d2460350074a2a5127cc20c0b03446977a612a096324337cd5bc455f77cdbe4600e147b02fe58bc9c383b1e84ea3bc5755d3a87ce515b07c96741a72d9eb702d445acc3374531c70ef221216db2c9198110d83084f7b508da18fd34b8ee9f45d1204a627609d09a89c73e8bfc1f987c6bc906fac0d01720b169061b3d8015a5121a0d3beab454a03cc24a9bc5725e6c4aaf44d8f8b7242443f289c0751226448ec794a02a1ba411caeabb99f7a90510b8812a91a0ad69f1476408940381724a1dbfb7f69642788267b068c585bf41bf3f857fd14bcd9506b95b6a257d7481a07628004944e136a4c97842c13451e960cf4e08a8b6666e17a6aac016d701c1a00c82072939a092397c7104d7fd6332b860034f2ace5191e2792cf10e21f5166304bf329696128d63640b7882809b750f1f89e5d513fa08a8439e1ad5fe0affd887b0f06ab91798c35d48f39261af3dab7ddbc899be21d1f751b8d317b8e280f400b637ac6a471b4065973a6253235c94117e22083562b715ce680fafb78da9113f0f52692c52625ea8e1c24a1d8837beb963a5ab078455c8a43cbab68dc4eaa4c7646c9bb45803442250e935738944c7228aa3f7137567eb1231c63bff7552a7858525b92bdca832a41cb20fb24647a62af1da27e50c41f3cd070ed8c1d1213c22b1540a5c1412d67ab4ff334c2e5217c06a5f8a93c0637bd0fb4736c19591f67c378aa80f7c9587b346bbfe81eff8574c7e0acc3164c3df048019639a80377b97457175782d35b2666833aee098617626d88dbcc47091a011882d52105acc218c9287a95276a3259a6a94aa386d8148886abdcc1841f39260ce4754ebacc1fd36102905d4c623d0b27930b4c249ee7380758c0ac5982b0e932eda95184a40f55c451d835861ca2b314dbce97829f1b92752dda592d8960b2540f464988ea1c974c63467c439b1de540490b0af0491a6507951ebc971887bd2b4a11327381d99586f10668c83abe92fb649b113da7ec666729bc1cc38a1de137dd3cc4e3a6abb9881a2ee63e7df3ad6cb680664ba1559ca17448c968b7c867ac5f324911ffd43993b8a7b8f57094c786877c1208fa7f53e51d6f1a46ae71bc81f78ebe5808d48200b7e1bc81ec3d31070a6993aa5db237eb3a4c592aa559a73bd769583a0ad095ec1669b952be4a71fe8603f5d597f007a048cc9d7fea6735383b6b8bbf896b74dc48a21840a92c497a9bc7434b0241a9e42e6428515d477c4e0b3678fab1d619b794f01b828648e7577bb2e5297915b9fdf33cb291a37de51b51c7aca6f07994193bd981134da2340c23a93cda8b68e429ac801d3748b8d112b57e388511e3305e50a51184b623607447468be94351cd0b9111a119b4b3c6f270c1cfea749a2ac89455590280c369163946481dbaeb4693dbb376202db2d8464c61aea6411cd887080f5c59e1587da01510cd1b0e8b030a5c200639ba26376134e88279891b90373cc92e7a76c0aaa33d084ab3f61e175010996652e441300ad5aefda9cc88f17fef2102b643179e0a49a60c47ce06c5b1a0b150b09ca4593e5dd48a9b1979d103ba862c43ed354d2ec99575b70e741808288aa0e1cb792c0a458d4584ddfa1870d7b797e2aac7d4cc08916015401338d8841d226d9656661cda93f53343e0f906b82bce8f25428b02a639a47f7dda5b946a3785656fb6d083df5a5ec7493cc017a2469b1f43c96f2e3bbc9d6cb07bec82d721a4cfba6ca2c59b0e01bda98585692b9da753923f830b52c843b6d963f959ad60189f42d61df7808f4d131c4d233e246c4735193e516452061701e6114cf1587a54c79105f48fdce9c2134bb60550b242945ea011ec54c570054b93d96f072426b7c9b524db8d2f136b7db2d1f3889778f791d583227a702cdfa4a9f95014df019495f14e02318b3704dc3794af523705be75f29753f47b2888ceef235d82caca9f983b40bf10b29672da272113a973
cipher        : bcee459c896ea378dcc458a532c35c029eff6b8cf8adc83f484fb6f9bfe32612f7c936cbf4dbd7c5262288dc3966a0d769f94a0bd57913a60a71efae09321c22c53839d836cef5fb8bf5c630bd3b3d657492eabfc7e67a42a631c95391656f0fce607a181e418144dff3d97f1192a2825a94da5113bcffc2e5f3e043f7583e6159902ddd009f8bcb18046a05695917bdef48accc2e3708f8536aabb420a7fd7989c60bca6c1941af45eac2f03cf71c8506721f8cd69bd3c573f036e3e8ae72b85632d06e0cab6fa1fea078d84aa1a116ac58ee632a0542b2d0e6a7026ae814ceeb46478d1cefd082c9b19efa7bb6ddd7abda8e43eab7b5a5204449273ea056b36d3797371f855d0c7ff0436279b21b831ad0970c26cc39f8627deb932689b8df48e73b1b5893987fa4dbc65571a78287f1573beeb85db52a3edbad6f50725bcbfa40423e3ce1ab00c16ea3922bc42e6782ce224ccfb3c978d8704584b9768a8edb6a950c0208b1c1c9a6a4e0d6300a9cfe788389697460efc41308448e9752d2022dfdecd118440346e2fabb07559b76301943f3b186adaaba09828efb28db1cd4a5e82e01f360451cb3c487f371af05725ea0e7d61932a8dc38108e99182e9b50d2aa828a773a2e18f5271ac75e5a5c50b9221f893e5f7076732beb0ffb9e4b82e1c0648192c9547870372b78c6a3e3a1b00d904a4a1492d5944e0510acee62e40c78cecef97922b04807cdd47d4d403a7bb16316598e6eee760b257382d9648c9920c3395717d8ac829bd37465c0f3e7f0c7e6fc351aac802edb722200776906eb36f622c0b8702958e44317961f583265a83b8cfcd9eed80f15b9ef848ebb7355df9718a60c532e20074854797685b3e4a25f929fce9ad02a5af114f92210abd3b73fddf28f116c2d4c27ceda6428a3892eb0c18fc12b07596e4153f2a3df9aa440957704bc56bbbee06cd99def3218c046344b4c5a811840a088bcbbad76fca4a20b9bf608873b2830afd6097b05022e8b1d42af3e5e4f00303adc9f130a84cdde3fef9335ccd1120b3f2050f17ef0c10fd226268965cbfc13738ada0632
shared secret : 508ac79bf97e90d75267159ba5189b73c48ab41a91aec0f32edd6cd1e66465b5
```

---

## Important Implementation Note

You'll notice Kyber KEM API accepts 32 -bytes seeds ( `d`, `z` for key generation and `m` for encapsulation ) for all three concrete instantiations ( i.e. Kyber{512, 768, 1024} ) - this is what you, as an user of Kyber KEM API, need to ensure that you call those routines with uniformly random sampled seeds.

I provide you with a PRNG implementation, which lives in [include/prng.hpp](./include/prng.hpp). Before you start using that, I want you to take a moment and understand what can be the implication of using the default constructor of `prng::prng_t`.

- In case default constructor is used, `std::random_device` is requested for 32 random bytes ( in form of eight `uint32_t`s ), which is hashed using SHAKE256 Xof. When you request ( using `read()` function ) arbitrary many random bytes from that initialized PRNG, it's actually squeezed out from SHAKE256 Xof state. Now one thing to note here is `std::random_device` itself is not guaranteed to provide you with system randomness in all possible usecases/ targets. It's an implementation defined behaviour. So it's better to be careful. Read https://en.cppreference.com/w/cpp/numeric/random/random_device/random_device 's notes section. 
- But there's another way of using `prng::prng_t` - you can use its explicit constructor for creating a PRNG by hashing N -many random bytes, supplied as input. These N bytes input can be presampled from any secure randomness source that you may have access to. After that same underlying SHAKE256 Xof is used for squeezing arbitrary many bytes arbitrary many times from PRNG.

```cpp
#include "prng.hpp"

// Prefer N to be >= 32
constexpr size_t slen = 32; // = N bytes
std::array<uint8_t, slen> seed{};

// fill `seed` with N many random bytes

// default initialization ( recommended only if you're sure that target system provides you with reliable randomness source when using `std::random_device` )
prng::prng_t prng0;
// explicit initialization ( safer alternative )
prng::prng_t prng1{seed};
```

> **Note** Looking at API documentation, in header files, can give you good idea of how to use Kyber KEM API. Note, this library doesn't expose any raw pointer based interface, rather everything is wrapped under statically defined `std::span` - which one can easily create from `std::{array, vector}`. I opt for using statically defined `std::span` based function interfaces because we always know, at compile-time, how many bytes the seeds/ keys/ cipher-texts/ shared-secrets are, for various different Kyber KEM parameters. This gives much better type safety and compile-time error reporting.
