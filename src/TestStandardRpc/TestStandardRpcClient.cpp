#include <csignal>

#define TI_RPC_IMPL
#include <TiRPC.hpp>

void PrintInfo(const std::string& info)
{
    std::stringstream ss;
    ss << "[tid:" << std::this_thread::get_id() << "] "
       << info << std::endl;
    std::cout << ss.str();
}

bool g_loop = true;

void SignalHandler(int signum)
{
    switch (signum) {
    case SIGINT:
        g_loop = false;
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, SignalHandler);

    std::string ip = "127.0.0.1";
    int port = 6018;
    if (argc == 3) {
        ip = argv[1];
        port = std::stoi(argv[2]);
    }
    PrintInfo("IP: " + ip + ", PORT: " + std::to_string(port));

    system("pause");
    return 0;
}
