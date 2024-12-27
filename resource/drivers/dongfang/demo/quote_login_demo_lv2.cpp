#include <iostream>
#include <thread>
#include "quote_api_lv2.h"

namespace demo {
    class LoginDemoLv2 : public EMQ::API::QuoteSpiLv2 {
    public:
        LoginDemoLv2() {
            quote_api_ = nullptr;
        }
        ~LoginDemoLv2() {
            quote_api_->Release();
        }
        int32_t Login(const std::string& ip, int port, const std::string& user_name, const std::string& password, const std::string& log_path="./emq_api_log/emq.log")
        {
            quote_api_ = EMQ::API::QuoteApiLv2::CreateQuoteApiLv2(log_path.c_str());
            quote_api_->RegisterSpi(this);

            return quote_api_->Login(ip.c_str(), port, user_name.c_str(), password.c_str());
        }
    private:
        EMQ::API::QuoteApiLv2 *quote_api_;
    };
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <ip> <port> <user_name> <password> [log_path]" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = std::stoi(argv[2]);
    std::string user_name = argv[3];
    std::string password = argv[4];
    std::string log_path = (argc >= 6) ? argv[5] : "./emq_api_log/emq.log";

    demo::LoginDemoLv2 login_demo_lv2;
    auto ret = login_demo_lv2.Login(ip, port, user_name, password, log_path);
    if (ret != 0) {
        printf("Login Failed.\n");
        return 0;
    }

    printf("Login Success.\n");

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
