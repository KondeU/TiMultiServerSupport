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

void TestFunc1()
{
    PrintInfo("CALL: void TestFunc1()");
}

void TestFunc2()
{
    PrintInfo("CALL: void TestFunc2()");
}

int TestFunc2(int i)
{
    PrintInfo("CALL: int TestFunc2(int) +params "
        + std::to_string(i));
    return i;
}

std::vector<double> TestFunc3(std::vector<double> v1,
    std::tuple<std::tuple<int, double>, double> v2)
{
    std::vector<double> res;
    PrintInfo("CALL: vector<double> TestFunc3(vector<double>, "
        "tuple<tuple<int, double>, double>) +params "
        + std::to_string(v1[0]) + ", "
        + std::to_string(std::get<int>(std::get<0>(v2))) + ", "
        + std::to_string(std::get<double>(std::get<0>(v2))) + ", "
        + std::to_string(std::get<1>(v2)));
    res.emplace_back(v1[0]);
    res.emplace_back(std::get<int>(std::get<0>(v2)));
    res.emplace_back(std::get<double>(std::get<0>(v2)));
    res.emplace_back(std::get<1>(v2));
    return res;
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

CustomStruct TestFunc4(const CustomStruct& v)
{
    PrintInfo("CALL: CustomStruct TestFunc4(CustomStruct) +params " +
        std::to_string(v.a) + ", " +
        std::to_string(v.b) + ", " +
        std::to_string(v.c));
    return v;
}

class TestClassBase {
public:
    static void TestFunc5()
    {
        PrintInfo("CALL: static void TestClassBase::TestFunc5()");
    }

    void TestFunc7()
    {
        PrintInfo("CALL: void TestClassBase::TestFunc7()");
    }

    void TestFunc8()
    {
        PrintInfo("CALL: void TestClassBase::TestFunc8()");
    }

    int TestFunc8(int i)
    {
        PrintInfo("CALL: int TestClassBase::TestFunc8(int) +params "
            + std::to_string(i));
        return i;
    }

    virtual void TestFunc9()
    {
        PrintInfo("CALL: virtual void TestClassBase::TestFunc9()");
    }

    virtual void TestFunc10() = 0;
    virtual int TestFunc10(int i) = 0;
};

class TestClass : public TestClassBase {
public:
    static void TestFunc6()
    {
        PrintInfo("CALL: static void TestClass::TestFunc6()");
    }

    static int TestFunc6(int i)
    {
        PrintInfo("CALL: static int TestClass::TestFunc6(int) +params "
            + std::to_string(i));
        return i;
    }

    void TestFunc9() override
    {
        PrintInfo("CALL: override void TestClass::TestFunc9()");
    }

    void TestFunc10() override
    {
        PrintInfo("CALL: override(virtual=0) void TestClass::TestFunc10()");
    }

    int TestFunc10(int i) override
    {
        PrintInfo("CALL: override(virtual=0) int TestClass::TestFunc10(int) +params "
            + std::to_string(i));
        return i;
    }
};

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
    TestClass testClass;
    TestClassBase& testClassBase = testClass;

    signal(SIGINT, SignalHandler);

    std::string ip = "*";
    int port = 6018;
    if (argc == 3) {
        ip = argv[1];
        port = std::stoi(argv[2]);
    }
    PrintInfo("IP: " + ip + ", PORT: " + std::to_string(port));

    ti::RpcProcessResponse responder;

    PrintInfo("BindFunc: General function.");
    responder.BindFunc("TestFunc1", &TestFunc1); //1//

    PrintInfo("BindFunc: General function with the same function name.");
    responder.BindFunc<void()>("TestFunc2@a", &TestFunc2); //2//
    responder.BindFunc<int(int)>("TestFunc2@b", &TestFunc2); //3//

    PrintInfo("BindFunc: General function with the STL.");
    responder.BindFunc("TestFunc3", &TestFunc3); //4//

    PrintInfo("BindFunc: General function with the custom struct.");
    responder.BindFunc("TestFunc4", &TestFunc4); //5//

    PrintInfo("BindFunc: Class static function.");
    responder.BindFunc("TestClassBase::TestFunc5", &TestClassBase::TestFunc5); //6//

    PrintInfo("BindFunc: Class static function with the same function name.");
    responder.BindFunc<void()>("TestClass::TestFunc6@a", &TestClass::TestFunc6); //7//
    responder.BindFunc<int(int)>("TestClass::TestFunc6@b", &TestClass::TestFunc6); //8//

    PrintInfo("BindFunc: Class function.");
    responder.BindFunc("TestClassBase::TestFunc7",
        &TestClassBase::TestFunc7, testClassBase); //9//

    PrintInfo("BindFunc: Class function with the same function name.");
    responder.BindFunc<void(TestClass::*)()>("TestClass::TestFunc8@a",
        &TestClass::TestFunc8, testClass); //10//
    responder.BindFunc<int(TestClass::*)(int)>("TestClass::TestFunc8@b",
        &TestClass::TestFunc8, testClass); //11//

    PrintInfo("BindFunc: Class virtual function, Base class or Sub class.");
    responder.BindFunc("TestClassBase::TestFunc9",
        &TestClassBase::TestFunc9, testClassBase); //12// void(TestClassBase::*)()
    responder.BindFunc("TestClass::TestFunc9",
        &TestClass::TestFunc9, testClass); //13// void(TestClass::*)()

    PrintInfo("BindFunc: Class virtual function with the same function name.");
    responder.BindFunc<void(TestClassBase::*)()>("TestClass::TestFunc10@a",
        &TestClassBase::TestFunc10, testClassBase); //14// from testClassBase
    responder.BindFunc<int(TestClass::*)(int)>("TestClass::TestFunc10@b",
        &TestClass::TestFunc10, testClass); //15// from testClass

    PrintInfo("BindFunc: Lambda.");
    responder.BindFunc("Lambda",
        std::function<void()>([]() { PrintInfo("CALL: Lambda"); })); //16//

    PrintInfo("Bind function finished.");

    bool retStart = responder.StartProcess(ip, port);
    PrintInfo("StartProcess: " + std::to_string(retStart));

    while (g_loop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    bool retStop = responder.StopProcess();
    PrintInfo("StopProcess: " + std::to_string(retStop));

    system("pause");
    return 0;
}
