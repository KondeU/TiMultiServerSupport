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

class LifecycleTimer {
public:
    LifecycleTimer(double& usedTime)
        : usedTime(usedTime)
    {
        tick = std::chrono::high_resolution_clock::now();
    }

    ~LifecycleTimer()
    {
        usedTime = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - tick).count() / 1000.0;
    }

    double& usedTime; // ms
    std::chrono::high_resolution_clock::time_point tick;
};

void CallFunc1(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestFunc1");
    }
    PrintInfo("CallFunc: void TestFunc1();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc2(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestFunc2@a");
    }
    PrintInfo("CallFunc: void TestFunc2();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc3(ti::RpcProcessRequest& requester)
{
    int arg1 = rand();
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<int> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<int>("TestFunc2@b", arg1);
    }
    PrintInfo("CallFunc: int TestFunc2(int);");
    PrintInfo("  args: " + std::to_string(arg1));
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  retv: " + std::to_string(ret.value));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc4(ti::RpcProcessRequest& requester)
{
    std::vector<double> arg1(1);
    std::tuple<std::tuple<int, double>, double> arg2;
    arg1[0] = rand();
    std::get<int>(std::get<0>(arg2)) = rand();
    std::get<double>(std::get<0>(arg2)) = rand();
    std::get<1>(arg2) = rand();
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<std::vector<double>> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<std::vector<double>>("TestFunc3", arg1, arg2);
    }
    PrintInfo("CallFunc: std::vector<double> TestFunc3(std::vector<double> v1,"
              "std::tuple<std::tuple<int, double>, double> v2);");
    std::string retv;
    for (auto& each : ret.value) {
        retv += std::to_string(each) + ", ";
    }
    if (ret.value.size() > 0) {
        retv.pop_back(); // Last of ','
        retv.pop_back(); // Last of ' '
    }
    PrintInfo("  args: " + std::to_string(arg1[0]) + ", "
                         + std::to_string(std::get<int>(std::get<0>(arg2))) + ", "
                         + std::to_string(std::get<double>(std::get<0>(arg2))) + ", "
                         + std::to_string(std::get<1>(arg2)));
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  retv: " + retv);
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

struct CustomStruct {
    int a, b;
    double c;
    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(a, b, c);
    }
};

void CallFunc5(ti::RpcProcessRequest& requester)
{
    CustomStruct arg1;
    arg1.a = rand();
    arg1.b = rand();
    arg1.c = rand();
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<CustomStruct> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<CustomStruct>("TestFunc4", arg1);
    }
    PrintInfo("CallFunc: CustomStruct TestFunc4(CustomStruct&);");
    PrintInfo("  args: " + std::to_string(arg1.a) + ", "
                         + std::to_string(arg1.b) + ", "
                         + std::to_string(arg1.c));
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  retv: " + std::to_string(ret.value.a) + ", "
                         + std::to_string(ret.value.b) + ", "
                         + std::to_string(ret.value.c));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc6(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestClassBase::TestFunc5");
    }
    PrintInfo("CallFunc: static void TestClassBase::TestFunc5();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc7(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestClass::TestFunc6@a");
    }
    PrintInfo("CallFunc: static void TestClass::TestFunc6();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc8(ti::RpcProcessRequest& requester)
{
    int arg1 = rand();
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<int> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<int>("TestClass::TestFunc6@b", arg1);
    }
    PrintInfo("CallFunc: static int TestClass::TestFunc6(int);");
    PrintInfo("  args: " + std::to_string(arg1));
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  retv: " + std::to_string(ret.value));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc9(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestClassBase::TestFunc7");
    }
    PrintInfo("CallFunc: void testClassBase.TestFunc7();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc10(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestClass::TestFunc8@a");
    }
    PrintInfo("CallFunc: void testClass.TestFunc8();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc11(ti::RpcProcessRequest& requester)
{
    int arg1 = rand();
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<int> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<int>("TestClass::TestFunc8@b", arg1);
    }
    PrintInfo("CallFunc: int testClass.TestFunc8(int);");
    PrintInfo("  args: " + std::to_string(arg1));
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  retv: " + std::to_string(ret.value));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc12(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestClassBase::TestFunc9");
    }
    PrintInfo("CallFunc: void testClassBase.TestFunc9();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc13(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestClass::TestFunc9");
    }
    PrintInfo("CallFunc: void testClass.TestFunc9();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc14(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("TestClass::TestFunc10@a");
    }
    PrintInfo("CallFunc: void testClassBase.TestFunc10();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc15(ti::RpcProcessRequest& requester)
{
    int arg1 = rand();
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<int> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<int>("TestClass::TestFunc10@b", arg1);
    }
    PrintInfo("CallFunc: int testClass.TestFunc10(int);");
    PrintInfo("  args: " + std::to_string(arg1));
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  retv: " + std::to_string(ret.value));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallFunc16(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<void> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<void>("Lambda");
    }
    PrintInfo("CallFunc: Lambda[]();");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

void CallInvalidFunc(ti::RpcProcessRequest& requester)
{
    double usedTime = 0;
    ti::RpcProcessRequest::CallReturn<float> ret;
    {
        LifecycleTimer timer(usedTime);
        ret = requester.CallFunc<float>("InvalidFunc");
    }
    PrintInfo("CallFunc: InvalidFunc");
    PrintInfo("  retc: " + std::to_string(static_cast<int>(ret.error)));
    PrintInfo("  used: " + std::to_string(usedTime) + " ms");
}

int main(int argc, char* argv[])
{
    signal(SIGINT, SignalHandler);

    srand(unsigned(time(NULL))); // For rand

    std::vector<std::function<void(ti::RpcProcessRequest&)>>
    callFuncs = {
        CallFunc1,  CallFunc2,  CallFunc3,  CallFunc4,
        CallFunc5,  CallFunc6,  CallFunc7,  CallFunc8,
        CallFunc9,  CallFunc10, CallFunc11, CallFunc12,
        CallFunc13, CallFunc14, CallFunc15, CallFunc16,
        CallInvalidFunc
    };

    std::string ip = "127.0.0.1";
    int port = 6018;
    if (argc == 3) {
        ip = argv[1];
        port = std::stoi(argv[2]);
    }
    PrintInfo("IP: " + ip + ", PORT: " + std::to_string(port));

    ti::RpcProcessRequest requester;

    bool ret1 = requester.ConnectNetwork(ip, port);
    PrintInfo("ConnectNetwork:" + std::to_string(ret1));

    size_t index = 0;
    while (g_loop) {
        callFuncs[index % callFuncs.size()](requester);
        index++;
    }

    bool ret2 = requester.DisconnectNetwork();
    PrintInfo("DisconnectNetwork:" + std::to_string(ret2));

    #ifdef WIN32
    system("pause");
    #endif
    return 0;
}
