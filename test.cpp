

#include "stdafx.h"
#include "mxtypes.h"

#include <iostream>
#include <string>
#include <future>
#include <locale>
#include <exception>

#include <stdexcept>

#include "LoginUpdate.h"

#ifdef  _MSC_VER
    #if defined _WIN64
        #pragma comment( lib, "lib\\x64\\mxnet2st_mt2022.lib")
    #else
        #pragma comment( lib, "lib\\x86\\mxnet2st_mt2022.lib")
    #endif
#endif


using namespace std::chrono_literals;


namespace sampleApp{

    constexpr _mxINT32   USERCODE = 1234;
    constexpr _mxINT16   APPSLOT = 7;
    constexpr int        INTERVAL = 5;  // second

    mxnet2license::LoginUpdate updater(USERCODE, APPSLOT, INTERVAL);

    struct UserCred {
        std::wstring user{ L"protectedApp" };
        std::wstring passwd{ L"secret" };
        std::wstring domain{ L"." }; //local account domain
    };

    PROCESS_INFORMATION pi{};


    void exitApp()
    {
        std::cout << "terminating process\n";

        //他の方法で終了させる必要がある可能性あり
        TerminateProcess(pi.hProcess, -1);
    }

    static BOOL impersonateUser(const UserCred& cred, HANDLE& hUser)
    {
        if (!LogonUser(cred.user.c_str(), cred.domain.c_str(), cred.passwd.c_str(), LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hUser))
            return FALSE;

        return ImpersonateLoggedOnUser(hUser);
    }

    static const std::pair<BOOL, long>runApp( std::wstring& commandLine)
    {
        std::pair<BOOL,long> retPair;

        UserCred cred;

        HANDLE hUser;
        if (!impersonateUser( cred, hUser))
        {
            std::cout << "impersonate error: " << GetLastError() << std::endl;
            return std::make_pair(FALSE, 0);
        }

        STARTUPINFO si{};
        si.cb = sizeof(si);

        if (!CreateProcess(nullptr, const_cast<wchar_t*>(commandLine.c_str()), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
        {
            RevertToSelf();

            std::cout << "CreateProcess Error: " << GetLastError() << std::endl;
            retPair = std::make_pair(FALSE, 1);
        }
        else
        {
            RevertToSelf();

            std::cout << "終了待ち" << std::endl;
            WaitForSingleObject(pi.hProcess, INFINITE);

            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            long ec = static_cast<long>(exitCode);

            std::cout << "Exit code: " << ec << std::endl;

            // ハンドルを閉じる
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
         
            retPair = std::make_pair(TRUE, ec);
        }

        CloseHandle(hUser);

        return retPair;
    }

    static int app_main()
    {
        int retVal{ 0 };

        try {

            //LoginUpdate バックグラウンド処理開始（５秒毎に LogIn_MatrixNet 呼出してライセンス更新)
            updater.start();

            //フォアグラウンドでプログラム処理
            std::wstring commandLine = L"absolute path to EXE file";
            std::pair<BOOL, long>retPair = runApp(commandLine);

            std::cout << "終了します" << std::endl;

            if (retPair.first == FALSE)
                retVal = -1;
            else
                retVal = retPair.second;
        }
        catch (std::exception& e)
        {
            std::cout << "Exception raised: " << e.what() << std::endl;
            retVal = -1;
        }

        return retVal;
    }

}

#if defined(CONSOLE_APP)
//　プロジェクトプロパティ: リンカー　-　システム - サブシステム - コンソール (/SUBSYSTEM:CONSOLE)にセット
//　コンソールウィンドウ表示
int main()
{

#else
//　プロジェクトプロパティ: リンカー　-　システム - サブシステム - コンソール Windows (/SUBSYSTEM:WINDOWS)にセット
//　コンソールウィンドウは表示されない
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
#endif

     std::setlocale(LC_ALL, "");

    // MxNet2 API はサーバに接続できないとブロックするため
    // 別スレッドで呼出
    std::future<bool> detectAsync = std::async(&mxnet2license::getLicense, sampleApp::USERCODE, sampleApp::APPSLOT);

    //しばらく待つ　サーバに問題なく接続できれば、待っている間に処理は完了するはずなので
    //メッセージ表示されずに sampleApp::app_main()行へ
    std::future_status status = detectAsync.wait_for(500ms);

    while (status != std::future_status::ready)
    {
        std::cout << "ライセンス取得中..." << std::endl;
        status = detectAsync.wait_for(2s);
    }

    if (detectAsync.get() == false) {
        std::cout << "ライセンスを取得できませんでした" << std::endl;
        return -1;
    }


    //プログラム本体の処理開始
    return sampleApp::app_main();
}
