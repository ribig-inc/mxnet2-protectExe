#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <future>
#include "mxtypes.h"

namespace mxnet2license {

    bool getLicense(_mxINT32 usercode, _mxINT16 appSlot);

    class LoginUpdate final {
        std::thread     m_waitThread;
        std::thread     m_loginThread;

        volatile bool   m_stop;
        bool            m_runUpdate;
        bool            m_errorExit;

        _mxINT32        m_userCode;
        int             m_interval;
        _mxINT16        m_appSlot;

        std::mutex                   mtx_wait;
        std::mutex                   mtx_loginUpdate;
        std::condition_variable      cv;
        std::condition_variable      cv2;

        void stopWaitThread();
        void stopLoginThread();

    public:
        LoginUpdate() = delete;
        LoginUpdate(const LoginUpdate&) = delete;
        LoginUpdate& operator=(const LoginUpdate&) = delete;

        LoginUpdate(int userCode, short appSlot, int interval);
        ~LoginUpdate();
        void start();
        void stop();

        static void login(LoginUpdate* obj);
        static void WaitProc(LoginUpdate* obj);
    };
}
