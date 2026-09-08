#include "user.h"


User::User(std::string name, Network* network)
{
    my_name = name;
    my_network = network;
    private_key = GeneratePrivateKey(network->GetQ());
    public_key = GeneratePublicKey(network->GetG(), private_key, network->GetP());
}

void User::SendMsgToOtherUserUnEncrypted(User* other_user, std::string msg)
{
    other_user->InsertUnencryptedMessage(msg);
    my_network->InsertMsgLog("[" + my_name + "]:\n" + msg);
}

void User::InsertUnencryptedMessage(const std::string& strMsg)
{
    str_list_msg.push_back(strMsg);
}


void User::KeyExchange(User* other_user)
{
    auto temp = ComputeSharedSecretKey(other_user->GetPublicKey(), private_key, my_network->GetP());
    shared_secret_key = DeriveAESKey(ComputeSharedSecretKey(other_user->GetPublicKey(), private_key, my_network->GetP()));
    my_network->InsertMsgLog("[" + other_user->GetName() + "]:\n" + other_user->GetPublicKey().get_str(16));
}

void User::SendMsgToOtherUserEncrypted(User* other_user, std::string msg)
{
    mpz_class randomIV = GenerateRandomNumber(32);
    std::vector<unsigned char> randomIVBuff = BigIntToBufferVec(randomIV);
    std::vector<unsigned char> ciphertext_bytes = EncryptAES256(std::vector<unsigned char>(msg.begin(), msg.end()), shared_secret_key, randomIVBuff);
    std::string ciphertextBase64 = ToBase64(ciphertext_bytes.data(), ciphertext_bytes.size());
    other_user->ReceiveEncryptedMsg(ciphertext_bytes, randomIVBuff);
    std::string ciphertext(reinterpret_cast<const char*>(ciphertext_bytes.data()), ciphertext_bytes.size());
    // my_network->InsertMsgLog("[" + my_name + "]:\n" + "[ENCRYPTED (base64)] " + ciphertextBase64 + "\nwith IV: " + randomIV.get_str(16));
    my_network->InsertMsgLog("[" + my_name + "]:\n" + ciphertext + "\nwith IV: " + randomIV.get_str(16));

}

void User::ReceiveEncryptedMsg(const std::vector<unsigned char>& ciphertext_bytes, const std::vector<unsigned char>& randomIV)
{
    std::vector<unsigned char> plaintext_bytes = DecryptAES256(ciphertext_bytes, shared_secret_key, randomIV);
    std::string plaintext(plaintext_bytes.begin(), plaintext_bytes.end());
    str_list_msg.push_back(plaintext);
}
