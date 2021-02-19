#include <iostream>
#include <thread>
#include <io.h> //access
#include <direct.h> //mkdir
#include "windows.h"
#include <tchar.h>
#include <opencv2/opencv.hpp>
#include "alg.h"
#include "qsettings.h"
#include "qcoreapplication.h"
#include "qfileinfo.h"
#include "qdir.h"
#include "ciwamsgdialog.h"


typedef void *HANDLE;
typedef struct
{
    std::vector<cv::Mat> result_imgs;
    std::vector<bool> isok;
}SResult;

typedef struct
{
    std::string funname;
    void *private_data;
}SRunInfo;

typedef struct convertinfo {
    unsigned char runflag;
    int mode;
};

typedef HANDLE(_cdecl *LPINIT)(std::string algName, std::string configFileName, int gpuid);
typedef void*(_cdecl *LPRUN)(HANDLE handle, SRunInfo *runinfo);
typedef void(_cdecl *LPUNINIT)(HANDLE handle);
typedef std::string(_cdecl *LPGETLASTERR)(HANDLE handle);
typedef void(_cdecl *LPCLEARLASTERR)(HANDLE handle);

namespace alg {
    class AlgService {
    public:
        static AlgService& instance() {
            static AlgService the_instance;
            return the_instance;
        }

    private:
        AlgService() = default;

    public:
        void deinitialize() {
            running_ = false;
            trainflag_ = false;
            testflag_ = false;
            condvar_.notify_one();
            thread_.join();
            FreeLibrary(m_hmod);
        }
        bool initialize() {
#ifdef USE_ALG
            m_hmod = LoadLibrary(_TEXT("manuvision.dll"));
            if (NULL == m_hmod)
            {
                DWORD dwErr = ::GetLastError();
                CiWaMsgDialog::MessageSignal("ERROR:", "加载错误：manuvision.dll");
                exit(1);
            }

            m_lpInit = (LPINIT)GetProcAddress(m_hmod, "Init");
            m_lpRun = (LPRUN)GetProcAddress(m_hmod, "Run");
            m_lpUnInit = (LPUNINIT)GetProcAddress(m_hmod, "UnInit");
            m_lpGetLastErr = (LPGETLASTERR)GetProcAddress(m_hmod, "GetLastErr");
            m_lpClearLastErr = (LPCLEARLASTERR)GetProcAddress(m_hmod, "ClearLastErr");

            if (NULL == m_lpInit || NULL == m_lpUnInit || NULL == m_lpRun
                || NULL == m_lpGetLastErr || NULL == m_lpGetLastErr) {
                exit(1);
            }
#endif
            runflag = 0;
            running_ = true;
            thread_ = std::thread(&AlgService::run, this);

            return true;
        };

        void start() {
            runflag = 1;
            condvar_.notify_one();
        };

        void stop() {
            runflag = 0;
        };

        void train() {
            trainflag_ = true;
            runflag = 1;
            condvar_.notify_one();
        };

        void test() {
            testflag_ = true;
            runflag = 1;
            condvar_.notify_one();
        };

        void convert(int mode) {
            convertflag_ = true;
            runflag = 1;
            convertmode_ = mode;
            condvar_.notify_one();
        };

        unsigned char getstatus() {
            return trainflag_ || testflag_ || convertflag_;
        }
    private:
        void run() {
            std::unique_lock<std::mutex> lock(mutex_);
#ifdef USE_ALG
            QString path = QCoreApplication::applicationDirPath();
            HANDLE handle = m_lpInit("Ainno", std::string(path.toLocal8Bit()), 0);
            if (handle) {
                errcode = m_lpGetLastErr(handle);
                m_lpClearLastErr(handle);
                if (errcode != "") {
                    CiWaMsgDialog::MessageSignal("ERROR:ALG", QString::fromLocal8Bit(errcode.c_str()));
                }
            }
            else {
                CiWaMsgDialog::MessageSignal("ERROR:ALG", QString::fromLocal8Bit("不存在算法:Ainno"));
            }

#endif
            while (running_) {
                if (runflag == 0) {
                    condvar_.wait_for(lock, std::chrono::milliseconds(10));
                    continue;
                }
#ifdef USE_ALG
                SRunInfo runinfo;
                if (trainflag_) {
                    runinfo.funname = "train";
                    runinfo.private_data = (void *)&runflag;
                    m_lpRun(handle, &runinfo);
                    trainflag_ = false;
                    errcode = m_lpGetLastErr(handle);
                    m_lpClearLastErr(handle);
                    if (errcode != "") {
                        CiWaMsgDialog::MessageSignal("WARNNING:ALG", QString::fromLocal8Bit(errcode.c_str()));
                    }
                }

                if (testflag_) {
                    runinfo.funname = "inference";
                    runinfo.private_data = (void *)&runflag;
                    m_lpRun(handle, &runinfo);
                    testflag_ = false;
                    errcode = m_lpGetLastErr(handle);
                    m_lpClearLastErr(handle);
                    if (errcode != "") {
                        CiWaMsgDialog::MessageSignal("WARNNING:ALG", QString::fromLocal8Bit(errcode.c_str()));
                    }
                }

                if (convertflag_) {
                    convertinfo tmpinfo;
                    tmpinfo.mode = convertmode_;
                    tmpinfo.runflag = runflag;
                    //pytorch to onnx
                    runinfo.funname = "convert";
                    runinfo.private_data = (void *)&tmpinfo;
                    m_lpRun(handle, &runinfo);
                    convertflag_ = false;
                    errcode = m_lpGetLastErr(handle);
                    m_lpClearLastErr(handle);
                    if (errcode != "") {
                        CiWaMsgDialog::MessageSignal("ERROR:ALG", QString::fromLocal8Bit(errcode.c_str()));
                        continue;
                    }
                }
#endif
                runflag = 0;
            }
#ifdef USE_ALG
            m_lpUnInit(handle);
#endif
        }

    private:
        bool running_;
        bool trainflag_;
        bool testflag_;
        bool convertflag_;
        std::mutex mutex_;
        std::condition_variable condvar_;
        std::thread thread_;
        unsigned char runflag;
        int convertmode_;

        HINSTANCE m_hmod;
        LPINIT             m_lpInit;
        LPRUN            m_lpRun;
        LPUNINIT        m_lpUnInit;
        LPGETLASTERR m_lpGetLastErr;
        LPCLEARLASTERR m_lpClearLastErr;
        std::string errcode;
    };

    bool deinitialize() {
        AlgService::instance().deinitialize();
        return true;
    };

    bool initialize() {
        return AlgService::instance().initialize();
    };

    void start() {
        return AlgService::instance().start();
    };

    void stop() {
        return AlgService::instance().stop();
    };

    void train() {
        return AlgService::instance().train();
    };

    void test() {
        return AlgService::instance().test();
    };

    void convert(int mode) {
        return AlgService::instance().convert(mode);
    };

    unsigned char getstatus() {
        return AlgService::instance().getstatus();
    }
}