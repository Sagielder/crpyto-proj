#include "network.h"

Network::Network()
{
    if (SAFE_PRIME == 0)
    {
        GenerateP(n_bit);
    }
    else
    {
        p = SAFE_PRIME;
    }
    g = FindGenerator(p , GetQ());
    // std::cout << g.get_str(10) << std::endl;
}

void Network::InsertMsgLog(std::string strMsg)
{
    str_msg_log.push_back(strMsg);
}

void Network::GenerateP(int numBits)
{
    p = GenSafePrime(numBits);
    std::cout << p.get_str(16) << std::endl;
}