#ifndef NETWORK_H
#define NETWORK_H
#include "diffieHellmanHelper.h"
#include <vector>
#include <string>

const mpz_class SAFE_PRIME(
    "8540ad0156c1640c4371f0c6b5ad8b42cdfa1ec1dacd981eb6241fa149173a5c"
    "17ce905be00e5bd0d0f2165ee3a5ee5dfb2ba586e114efb1c23072e23601559a"
    "65055da6f4fedcc1b40c9028efdaba4a1e0e8119f6588c305cda79adb5e61cb2"
    "91fe7e0740c5a6873f01c1cddb17f5205d3b2cdfa9ba136da615fc46ccd06d11"
    "fa2e363d74b6aad70f2b2ef50cd110e80b6cd5740a2c15ff4891260aee5618ad"
    "d1afbe65516bc251cf91c43cd6fdf15501418051eb2a35f0e926a9bcbf1e95a8"
    "a96395313576ada9f56e77f8c0303aabccf9a868c0ed8dc02631fbcddcee58c7"
    "17ebe564e2791c903159d57716f90175eb54d0701d372873ec4397dc7ec8c153",
    16
    // "0", 16
);

class Network
{
public:
    mpz_class GetP() const { return p; };
    mpz_class GetG() const { return g; };
    mpz_class GetQ() const { return (p-1)/2; }; // p = 2q + 1, q = (p-1)/2
    Network();
    void InsertMsgLog(std::string strMsg);
    std::vector<std::string> GetMsgLog() const {return str_msg_log; };
private:
    mpz_class p; // prime for diffie-hellman
    mpz_class g; // generator for diffie-hellman
    void GenerateP(int numBits = 2048);
    std::vector<std::string> str_msg_log; // all msg between 2 users
private:
    int n_bit = 2048;
};


#endif
