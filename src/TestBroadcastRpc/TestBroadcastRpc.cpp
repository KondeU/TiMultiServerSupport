#include <csignal>
#include <TiRPC.hpp>
namespace ti = tirpc;

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
    srand(unsigned(time(NULL)));

    std::string ip = "127.0.0.1";
    int callfunc = 6019;
    int broadcast = 6020;
    if (argc == 4) {
        ip = argv[1];
        callfunc = std::stoi(argv[2]);
        broadcast = std::stoi(argv[3]);
    }
    PrintInfo("IP: " + ip + ", PORT: "
        + std::to_string(callfunc) + ","
        + std::to_string(broadcast));

    #ifdef SERVER
    PrintInfo("ROLE: Server");
    #else
    PrintInfo("ROLE: Client");
    #endif
    PrintInfo("------------");

    ti::RpcAsyncBroadcast rpc;

    rpc.RegistReceiveTimeoutCallback(
    [](int count) {
        PrintInfo("(WAR) Receive timeout: " +
            std::to_string(count));
    });

    rpc.BindFunc("CallAsyncBroadcastRpc",
    std::function<void(unsigned int)>(
    [](unsigned int id) {
        PrintInfo("(R::) CallAsyncBroadcastRpc "
            + std::to_string(id));
    }));

    bool ret1 = rpc.Start(
    #ifdef SERVER
    ti::RpcAsyncBroadcast::Role::Server,
    #else
    ti::RpcAsyncBroadcast::Role::Client,
    #endif
    "127.0.0.1", callfunc, broadcast);
    PrintInfo("START: " + std::to_string(ret1));

    for (unsigned int index = 0; g_loop; index++) {
        auto ret = rpc.CallFunc("CallAsyncBroadcastRpc", index);
        PrintInfo("(S::) CallAsyncBroadcastRpc "
            + std::to_string(index) + ", return: "
            + std::to_string(static_cast<int>(ret)));

        for (int delay = rand() % 1500; g_loop && delay; delay--) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    bool ret2 = rpc.Stop();
    PrintInfo("STOP: " + std::to_string(ret2));

    #ifdef WIN32
    system("pause");
    #endif
    return 0;
}
