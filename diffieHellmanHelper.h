#ifndef BIG_INT_HELPER_H
#define BIG_INT_HELPER_H

#include <gmpxx.h>
#include <openssl/rand.h>
#include <iostream>
#include <vector>
#include <openssl/sha.h>
#include <openssl/evp.h>


/*
Square and Multiply
Convert the exponent to Binary.
For the first 1, simply list the number
For each ensuing 0, do Square operation
For each ensuing 1, do Square and Multiply operations
a^b mod n
*/
inline mpz_class ModExpo(const mpz_class& base, const mpz_class& exponent, const mpz_class& mod)
{
    mpz_class result = 1;
    mpz_class a = base % mod;
    
    size_t num_bits = mpz_sizeinbase(exponent.get_mpz_t(), 2);
    // mpz_class is in big-endian
    for (ssize_t i = num_bits - 1; i >= 0; i--)
    {
        result = (result * result) % mod;
        // true when bit = 1
        if (mpz_tstbit(exponent.get_mpz_t(), i)) 
        {
            result = (result * a) % mod;
        }
    }

    return result;
}

inline mpz_class GenerateRandomNumber(const int& numBytes)
{

    unsigned char buffer[numBytes]; 
    if (RAND_bytes(buffer, sizeof(buffer)) != 1) {
        throw std::runtime_error("RAND_bytes failed");
    }
    mpz_class retVal;

    mpz_import(retVal.get_mpz_t(), numBytes, 1, 1, 0, 0, buffer);
    
    return retVal;

}

inline mpz_class GenerateRandomNumberInBits(const int& numBits)
{
    int numBytes = (numBits + 7) / 8;
    unsigned char buffer[numBytes];

    if (RAND_bytes(buffer, sizeof(buffer)) != 1) {
        throw std::runtime_error("RAND_bytes failed");
    }

    mpz_class retVal;
    mpz_import(retVal.get_mpz_t(), numBytes, 1, 1, 0, 0, buffer);

    // Mask extra bits
    int extraBits = numBytes * 8 - numBits;
    if (extraBits > 0) {
        retVal &= ((mpz_class(1) << numBits) - 1);
    }

    // Ensure number is exactly numBits
    mpz_setbit(retVal.get_mpz_t(), numBits - 1);

    return retVal;
}

// based on : https://crypto.stanford.edu/pbc/notes/numbertheory/millerrabin.html
inline bool MillerRabinPrimeNumberTest(const mpz_class& num, int num_passes = 10)
{
    if (!mpz_tstbit(num.get_mpz_t(), 0) || num < 2) // if least significant bit is 0, even  number
    {
        return false;
    }

    // step 1 : find n - 1 = 2^s * q
    mpz_class nMinusOne = num - 1;
    mpz_class q = nMinusOne;
    unsigned long s = 0;

    // a^(n-1) roots := a^(2^s * q), a^(2^(s-1) * q), a^(2^(s-2) * q), ... , a^q

    while (mpz_even_p(q.get_mpz_t())) {  // while q % 2 == 0
        q /= 2;
        s++;
    }

    gmp_randclass rng(gmp_randinit_default);
    rng.seed(time(nullptr));
    mpz_class a;
    mpz_class b;
    while (num_passes > 0)
    {
        bool passes_this_iteration = false;
        // step 2 : choose a: 1 <= a <= n - 1
        a = rng.get_z_range(nMinusOne) + 1;
        // step 3 : 
        // 1. compute b0 = a^q (mod n), bi = (bi-1)^2
        // 2. if b is -1/+1 then prime
        // 3. otherse do bi = (bi-1)^2
        // 4. if bi is +1 then not prime, if -1 then prime otherwise repeat 3&4
        b = ModExpo(a, q, num); // a^q mod n 
        if (b == 1 || b == nMinusOne)
        {
            passes_this_iteration = true;
        }
        else
        {
            for (unsigned long i = 0; i < s-1; i++)
            {
                b = ModExpo(b, 2, num);
                if (b == nMinusOne)
                {
                    passes_this_iteration = true;
                    break;
                }
                else if (b == 1)
                {
                    return false;
                }
            }
        }
        
        if (!passes_this_iteration)
        {
            return false;
        }
        num_passes--;
    }

    return true;
    
}

// test gen prime
inline void TestGen2048BitPrime()
{
    while (true)
    {
        mpz_class maybePrime = GenerateRandomNumber(256);
        maybePrime |= 1;
        if (MillerRabinPrimeNumberTest(maybePrime))
        {
            std::cout << maybePrime << std::endl;
            std::cout << "prime" << std::endl;
            return;
        }
    }
   
    return;
}



inline bool DivisibleBySmallPrime(const mpz_class& n)
{
    static const int small_primes[] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
    };
    
    for (int p : small_primes)
    {
        if (mpz_divisible_ui_p(n.get_mpz_t(), p))
            return true;
    }
    return false;
}


inline mpz_class GenSafePrime(const int numBit = 2048)
{
    while (true)
    {
        mpz_class q = GenerateRandomNumberInBits(numBit - 1) | 1;
        
        // if q mod 6 != 5, then divisible by 2,3 or 6
        // q mod 6 = 1 might be prime but p = q*2 + 1 will not be prime as 1*2 + 1 = 3 divisible by 3
        mpz_class remainder = q % 6;
        // Add offset to make it ≡ 5 (mod 6)
        q += (5 - remainder + 6) % 6;  

        mpz_class p = 2*q + 1;

        if (!DivisibleBySmallPrime(q) && !DivisibleBySmallPrime(p))
        {
            if (MillerRabinPrimeNumberTest(q, 40) && 
                MillerRabinPrimeNumberTest(p, 40))
            {
                return p;
            }
        }
    }
}


inline mpz_class FindGenerator(const mpz_class& p, const mpz_class&q)
{
    while (true)
    {
        mpz_class g = 2;
        for (; g < p - 2; g++)
        {
            if (ModExpo(g, q, p) == 1) return g; // prime-order subgroup | order q
        }
    }
}


inline mpz_class GeneratePrivateKey(const mpz_class& q)
{
    int numBytes = (mpz_sizeinbase(q.get_mpz_t(), 2) + 7) / 8;
    mpz_class r;

    // [1, q-1]
    while (true)
    {
        r = GenerateRandomNumber(numBytes);
        if (r < q - 1) break;
    }
    return r + 1;
}

inline mpz_class GeneratePublicKey(const mpz_class& g, const mpz_class& private_key, const mpz_class& p)
{
    // publicKey = g^private_key mod p
    return ModExpo(g, private_key, p);
}


inline mpz_class ComputeSharedSecretKey(const mpz_class& other_public_key, const mpz_class& my_private_key, const mpz_class& p)
{
    // shared_secret_key = other_public_key^my_private_key mod p
    return ModExpo(other_public_key, my_private_key, p);
}

inline std::vector<unsigned char> BigIntToBufferVec(const mpz_class& bigIntVal)
{
    size_t num_bytes = (mpz_sizeinbase(bigIntVal.get_mpz_t(), 2) + 7) / 8;
    std::vector<unsigned char> buffer(num_bytes);
    mpz_export(buffer.data(), nullptr, 1, 1, 0, 0, bigIntVal.get_mpz_t());
    return buffer;
}

inline std::vector<unsigned char> DeriveAESKey(const mpz_class& shared_secret)
{
    size_t num_bytes = (mpz_sizeinbase(shared_secret.get_mpz_t(), 2) + 7) / 8;
    std::vector<unsigned char> buffer(num_bytes);
    mpz_export(buffer.data(), nullptr, 1, 1, 0, 0, shared_secret.get_mpz_t());

    unsigned char hash[32]; // SHA-256 output
    SHA256(buffer.data(), buffer.size(), hash);

    return std::vector<unsigned char>(hash, hash + 32);
}

// inline unsigned int GetIntFromBuffer(std::vector<unsigned char> buffer)
// {
//     unsigned int retVal = 0;
//     // big endian
//     unsigned int bufferSize = (unsigned int)buffer.size();
//     for (unsigned int i = 0; i < bufferSize; i++)
//     {
//         retVal += (static_cast<unsigned int>(buffer[i]) << ((bufferSize - i)*8));
//     }
//     return retVal;
// }


inline std::vector<unsigned char> EncryptAES256(const std::vector<unsigned char>& plaintext, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
    std::vector<unsigned char> ciphertext(plaintext.size() + 16); 
    int len, ciphertext_len;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    ciphertext.resize(ciphertext_len);
    return ciphertext;
}


inline std::vector<unsigned char> DecryptAES256(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
    std::vector<unsigned char> plaintext(ciphertext.size());
    int len = 0, plaintext_len = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()))
        throw std::runtime_error("EVP_DecryptInit_ex failed");

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()))
        throw std::runtime_error("EVP_DecryptUpdate failed");
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len))
        throw std::runtime_error("EVP_DecryptFinal_ex failed");
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    return plaintext;
}

inline std::string ToBase64(const unsigned char* data, size_t len) {
    int outlen = 4 * ((len + 2) / 3);
    std::string out(outlen, '\0');
    EVP_EncodeBlock(reinterpret_cast<unsigned char*>(&out[0]), data, len);
    return out;
}

#endif