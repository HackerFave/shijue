#pragma once
#include <map>
#include <vector>
#include "qstring.h"
#include "filedatastream.h"

enum FileOperationType
{
    NEW,
    DESTROY,
    OPEN,
    CLOSE,
    ADDIMAGE,
    DELETEIMAGE,
    SAVE,
    LOADLABEL,
    ADDOKIMAGE,
    FOCOUNT
};

class FileProjectManager
{
public:
    ~FileProjectManager();

private:
    ainno_stream m_stream;
    static FileProjectManager* g_FileProjectManager;
    QString projectabspath_; //��ǰ��Ŀ�ľ���·��
    QString ainnoabspath_;   //��ǰ��Ŀ�����ļ��ľ���·��
    QString imageabspath_;   //��ǰ��ĿͼƬĿ¼�ľ���·��
    QString trainabspath_;   //��ǰ��Ŀѵ��Ŀ¼�ľ���·��
    bool projectisopen_;

    bool bsavetestresult_;

    std::vector<std::string> trainimglist_;//ѵ��ͼƬ��
    std::map<std::string, std::map<int,cv::Mat>> predictresultimagelist_;//Ԥ����ͼƬ�б�

    int classnum_;          //�����
private:
    FileProjectManager();

public:
    static FileProjectManager* Instance();

    //֪ͨ����m_stream��ʹ�����ͷ���Դ��Ȼ�����m_stream
    bool StreamUninit();
    //��ʼ��m_stream
    bool StreamInit(ainno_stream & ainno_stream);
    //��m_stream������ͼƬ
    bool StreamAddImage(std::vector<QString> imgabspath, bool isOk);
    //��ȡͼƬ��Ϣ
    ainno_image StreamGetImageInfo(int idx);
    //���ñ�ע��Ϣ
    bool StreamSetImageLabelInfo(int idx, labelinfo retlabelinfo);
    //����ѵ������
    bool StreamSetImageTrainpolicy(int idx, ainno_train_policy trainpolicy);
    //��ȡ��ע��Ϣ
    labelinfo StreamGetImageLabelInfo(int idx);
    //��ȡѵ����־
    bool StreamGetTrainedFlag(int idx);
    //��ȡgood��־
    bool StreamGetGoodFlag(int idx);
    //��ȡbad��־
    bool StreamGetBadFlag(int idx);
    //��ȡlabeled��־
    bool StreamGetLabeledFlag(int idx);

    //��ȡ������ͼƬ����
    int StreamGetImageNum();
    //��ȡ��Ӧ������ͼƬ�ľ���·��
    QString StreamGetImageAbsPath(int idx);
    //ɾ��ͼƬ
    void StreamDeleteImage(int idx);

    //�½�����
    bool ProjectNew();
    //ɾ������
    bool ProjectDestroy();
    //�򿪹���
    bool ProjectOpen(bool newFile,const QString &filePath);
    //�������
    void ProjectSetClassNum(int classnum);
    //�رչ���
    bool ProjectClose();
    //���ͼ��
    bool ProjectAddImage(bool isOk);
    //ɾ��ͼ��
    bool ProjectDeleteImage(int idx);
    bool ProjectDeleteImage(const QString& fileName);
    //���湤���ļ�
    bool ProjectSave();
    //���ر�ע�ļ�
    bool ProjectLoadLabel();
    //������(��\��������Ŀ¼)
    bool FileOperation(FileOperationType type, int idx = -1,const bool newFile = true,const QString FilePath = "");
    //��ȡ����log�ļ�����·��
    QString ProjectGetTestLogAbsPath();
    //��ȡѵ��log�ļ�����·��
    QString ProjectGetTrainLogAbsPath();
    //��ȡ���������ļ�����·��
    QString ProjectGetTestCfgAbsPath();
    //��ȡѵ�������ļ�����·��
    QString ProjectGetTrainCfgAbsPath();
    //��ȡѵ��Ŀ¼����·��
    QString ProjectGetTrainAbsPath();
    //��ȡ��Ŀ����·��
    QString ProjectGetProjectAbsPath();
    //��ȡ��ĿͼƬĿ¼����·��
    QString ProjectGetImageAbsPath();
    //�ָ�Ĭ������
    bool ResetTrainConfig();
    
    //׼��ѵ������
	bool PrepareTrainData(float trainratio, bool bkeep);
    //����ѵ����־
    bool UpdateTrainFlag(bool btrained);
    //׼����������
    bool PrepareTestData(std::vector<int> indexes);
    //���ز��Խ��ͼƬ
    bool LoadPredictImage(std::vector<int> indexes);
    //�Ƿ񱣴���Խ��
    void SetTestResultFlag(bool bflag);
    //���ز��Խ��
    bool LoadPredictResult(int threshold);
    //׼��ģ��ת����������
    bool PrepareConvertData();
    //�����������
    std::vector<ainno_contour_info> AnalyzeLabeledData(void);
    //��ȡ�ļ����б�
    std::vector<std::string>  StreamGetImageName();
};

