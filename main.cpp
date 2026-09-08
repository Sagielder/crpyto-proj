#include "network.h"
#include "user.h"
#include <stdio.h>
int main()
{
    Network my_network;
    User user_alice("Alice", &my_network);
    User user_bob("Bob", &my_network);


    user_alice.SendMsgToOtherUserUnEncrypted(&user_bob, "hello bob from alice");
    user_bob.SendMsgToOtherUserUnEncrypted(&user_alice, "hello alice from bob");

    user_alice.KeyExchange(&user_bob);
    user_bob.KeyExchange(&user_alice);

    user_alice.SendMsgToOtherUserEncrypted(&user_bob, "hello bob this is encrypted from alice");
    user_bob.SendMsgToOtherUserEncrypted(&user_alice, "hello alice this is encrypted from bob");


    std::vector<std::string> str_network_log_list = my_network.GetMsgLog();
    std::cout << "==================== NETWORK ====================" << std::endl;
    for (std::string s : str_network_log_list)
    {
        std::cout << s << std::endl;
    }
    std::vector<std::string> str_alice_log = user_alice.GetStrListMsg();
    std::cout << "==================== Alice ====================" << std::endl;
    for (std::string s : str_alice_log)
    {
        std::cout << s << std::endl;
    }
    std::vector<std::string> str_bob_log = user_bob.GetStrListMsg();
    std::cout << "==================== Bob ====================" << std::endl;
    for (std::string s : str_bob_log)
    {
        std::cout << s << std::endl;
    }
    return 0;
}