#ifndef USER_H
#define USER_H
#include <vector>
#include <string>
#include "diffieHellmanHelper.h"
#include "network.h"
#include <unordered_map>

class User
{
public:
    User(std::string name, Network* network);
    std::vector<std::string> GetStrListMsg() const { return str_list_msg; };
    mpz_class GetPublicKey() const { return public_key; };
    void SendMsgToOtherUserUnEncrypted(User *other_user, std::string msg);
    void InsertUnencryptedMessage(const std::string &strMsg);
    void KeyExchange(User* other_user);
    void SendMsgToOtherUserEncrypted(User* other_user, std::string msg);
    void ReceiveEncryptedMsg(const std::vector<unsigned char>& ciphertext_bytes, const std::vector<unsigned char>& randomIV);
    std::string GetName() const { return my_name; };
private:
    std::vector<std::string> str_list_msg;
    mpz_class public_key;
    mpz_class private_key;
    std::vector<unsigned char> shared_secret_key;
    Network* my_network;
    std::string my_name;
};



#endif