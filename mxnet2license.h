#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <future>
#include "mxtypes.h"

typedef void (*ExitAppCallback)();

namespace mxnet2 {

    class License final {
        std::thread     m_waitThread;
        std::thread     m_loginThread;

        volatile bool   m_stop;
        bool            m_runUpdate;
        bool            m_errorExit;

        _mxINT32        m_userCode;
        int             m_interval;
        _mxINT16        m_appSlot;
        ExitAppCallback m_exitAppCallback;

        std::mutex                   mtx_wait;
        std::mutex                   mtx_loginUpdate;
        std::condition_variable      cv;
        std::condition_variable      cv2;

        void stopWaitThread();
        void stopUpdateThread();

        static bool getSync(_mxINT32 usercode, _mxINT16 appSlot);
        static void update(License* obj);
        static void WaitProc(License* obj);

    public:
        License() = delete;
        License(const License&) = delete;
        License& operator=(const License&) = delete;

        License(int userCode, short appSlot, int interval, ExitAppCallback callback);
        ~License();   

        bool get(_mxINT32 usercode, _mxINT16 appSlot);

        void startUpdate();
        void stopUpdate();
    };
}
