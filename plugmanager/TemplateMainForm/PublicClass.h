#pragma once

#include <QObject>

class PublicClass
{

public:
    PublicClass();
    ~PublicClass();
    static PublicClass* instance()
    {
        if (P_publicClass_ == NULL)
        {
            P_publicClass_ = new PublicClass();
        }
        return P_publicClass_;
    }
    static void Release()
    {
        if (P_publicClass_ != NULL)
        {
            delete P_publicClass_;
            P_publicClass_ = NULL;
        }

    }
    void SetKeyStatus(bool checked) {
        isctrlpress_ = checked;
    }
    bool GetKeyStatus() {
        return isctrlpress_;
    }
private:
    static PublicClass* P_publicClass_;
    bool isctrlpress_=false;
};
