#include "stdafx.h"
#include <future>

#include "mxnet2license.h"
#include "mxnet2api.h"

using namespace std::chrono_literals;

namespace mxnet2 {

    License::License(int userCode, short appSlot, int interval,  ExitAppCallback callback) :
        m_interval(interval), m_stop(false), m_userCode(userCode), m_appSlot(appSlot), m_runUpdate(false),
        m_errorExit(false), m_exitAppCallback(callback) {}


    License::~License()
    {
        std::cout << "mxnet2::License Destructor " << std::endl;
        stopUpdate();
    }

    bool License::getSync(_mxINT32 usercode, _mxINT16 appSlot)
    {
        bool        bInitOk = false;
        short       ret;
        _mxINT32    serNr;

        ret = mxnet2::rInit_MatrixAPI();

        //Init_MatrixAPI失敗
        if (ret < 0)
            goto EXIT1;

        ret = mxnet2::rDongle_Count(85);
        std::cout << "カウント=" << ret << std::endl;
        if (ret <= 0)
            goto EXIT;

        serNr = mxnet2::rDongle_ReadSerNr(usercode, 1, 85);
        std::cout << "シリアル番号=" << serNr << std::endl;
        if (serNr <= 0)
            goto EXIT;

        ret = mxnet2::rLogIn_MatrixNet(usercode, appSlot, 1);
        std::cout << "Login=" << ret << std::endl;
        if (ret < 0)
            goto EXIT;

        bInitOk = true;
        goto EXIT1;

    EXIT:
        mxnet2::rRelease_MatrixAPI();

    EXIT1:
        return bInitOk;
    }

    bool License::get(_mxINT32 usercode, _mxINT16 appSlot)
    {
        // 別スレッドで呼出
        std::future<bool> detectAsync = std::async(getSync, usercode, appSlot);

        //しばらく待つ　サーバに問題なく接続できれば、待っている間に処理は完了するはずなので
        //メッセージ表示されずに sampleApp::app_main()行へ
        std::future_status status = detectAsync.wait_for(500ms);

        while (status != std::future_status::ready)
        {
            std::cout << "ライセンス取得中..." << std::endl;
            status = detectAsync.wait_for(2s);
        }

        return detectAsync.get();
    }

    void License::startUpdate()
    {
        m_loginThread = std::thread(update, this);
        m_waitThread = std::thread(WaitProc, this);
    }

    void License::stopWaitThread()
    {
        //エラーでプログラムを終了させるとき２度呼び出される
       //デストラクタでも呼び出される
       //一度 呼び出されたら以降は何もしない（ループ防止）
        if (m_stop == false)
        {
            std::lock_guard<std::mutex> lock(mtx_wait);
            m_stop = true;
            cv.notify_one();
        }
    }

    void License::stopUpdateThread()
    {
         std::lock_guard<std::mutex> lock(mtx_loginUpdate);
         m_runUpdate = true;
         cv2.notify_one();
    }

    // rLogIn_MatrixNet呼び出し用スレッド
    // 定期的に WaitProcスレッドによって起こされる
    void License::update(License* obj)
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(obj->mtx_loginUpdate);

            obj->cv2.wait(lock, [obj] { return obj->m_runUpdate; });
            {
                //m_stopが立っている状態で呼び出されたら即座に終了
                if (obj->m_stop == true) break;

                short ret = mxnet2::rLogIn_MatrixNet(obj->m_userCode, obj->m_appSlot, 1);
                std::cout << ">>login=" << ret << std::endl;

                //m_stopを確認することで、できるだけ早くスレッドを終了できる
                if (obj->m_stop == true) break;

                bool bOk = true;

                if (ret < 0)
                {
                    bOk = false;

                    //再試行
                    //-100　接続可能であってもクライアント接続が多すぎたり/回線不安定だと接続不能になることがあるため
                    //-105  セッションタイムアウトでセッション解放 / サーバが落ちてセッション開放
                    if (ret == -100 || ret == -105)
                    {
                        for (int retry = 0; retry < 3; retry++)
                        {
                            std::this_thread::sleep_for(250ms);

                            if (ret == -105)
                            {
                                ret = mxnet2::rInit_MatrixAPI();
                                if (ret > 0)
                                {
                                    bOk = true;
                                    break;
                                }
                            }
                            else
                            {
                                ret = mxnet2::rLogIn_MatrixNet(obj->m_userCode, obj->m_appSlot, 1);
                                if (ret >= 0)
                                {
                                    bOk = true;
                                    break;
                                }
                            }
                            if (obj->m_stop == true) break;
                        }
                    }
                }

                if (obj->m_stop == true) break;

                //エラ-でプログラムはエラー終了
                if (bOk==false) {

                    //エラー発生フラッグ
                    obj->m_errorExit = true;

                    //waitスレッドを止める
                    obj->stopWaitThread();

                    //このスレッド終了
                    break;
                }

                //処理が長引いたら 1。。。
                //std::this_thread::sleep_for(6s);

                //処理が長引いたら 2。。。
                //for (int i = 0; i < 6 && !obj->m_stop; i++)
                //    std::this_thread::sleep_for(1s);

                //処理終了
                obj->m_runUpdate = false;

            }


        }
    }

    void License::WaitProc(License* obj)
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(obj->mtx_wait);

            //m_intervalでタイムアウト、または m_stopがtrue(キー押し下げ）を待つ

            //キーが押された
            if (obj->cv.wait_for(lock, std::chrono::seconds(obj->m_interval), [obj] { return obj->m_stop; }))
            {
                // m_stopが立っている
                // login スレッドを終了させる
                obj->stopUpdateThread();

                //このスレッド終了
                break;
            }
            //m_intervalタイムアウト
            else
            {

                //別スレッドで LogIn_MatrixNetを呼び出す
                //(このスレッドでは LogIn_MatrixNet呼出でブロックしないように )          

                std::unique_lock<std::mutex> lock(obj->mtx_loginUpdate, std::try_to_lock);

                // login スレッド処理中. 何もしない
                if (!lock)
                {
                    std::cout << "mxnet2::updateスレッド処理中..." << std::endl;
                    continue;
                }
                // login スレッドを起こす
                else
                {
                    obj->m_runUpdate = true;
                    obj->cv2.notify_one();
                }
            }
        }


        //loginスレッド終了待ち
        //loginスレッドが rLoginIn_MatrixNetでブロックしていたら
        //直ぐに終了しない
        if (obj->m_loginThread.joinable())
            obj->m_loginThread.join();

        //エラー発生フラッグ

        //メインスレッドはキー押下げ待ちをしている
        //キー押下をしないで終了させる
        if (obj->m_errorExit == true)
            obj->m_exitAppCallback();
    }

    void License::stopUpdate()
    {
        // waitスレッドを終了させる
        stopWaitThread();

        // m_waitThread内で m_loginThreadスレッド終了待
        // ここで、m_waitThread終了待ち
        if (m_waitThread.joinable())
            m_waitThread.join();

        //セッション終了
        mxnet2::rRelease_MatrixAPI();
    }


}
