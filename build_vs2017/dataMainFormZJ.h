#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <qpushbutton.h>
#include <QDateTime>
#include <QModelIndex>
#include "tableViewEditItemForm.h"
#include "proCategoryItem.h"
#include <qdebug.h>
#include <qaxobject.h>
#include <qfiledialog.h>
#include <QMessageBox>
#include <QDesktopServices>
#include <qprinter.h>
#include <qpainter.h>
#include <QTextDocument>
#include <winsock2.h>
#include <QTextBlock>
#include <qtextedit.h>
#include <qclipboard.h>
#include <QFileInfoList>
#include <QKeyEvent>
#include "dataSql.h"
#include "threadTaskSql.h"
#include "addTestReord.h"
#include "databaseSqlInset.h"
#include "DatabaseSqlite.h"
#include "sqlOperation.h"
#include "ui_dataMainFormZJ.h"
#include <QAxWidget>
#include "operationPDF.h"

#define INDEX_NUM 1;
#define RECORD_DAYS 3
#pragma execution_character_set("utf-8")
class dataMainFormZJ : public QWidget
{
    Q_OBJECT

public:
    dataMainFormZJ(QWidget *parent = Q_NULLPTR, QRect rect = QRect());
    ~dataMainFormZJ();
    void initTableView();
    void initStripQualityExcelItem(QAxObject *Object);
    void initVolumeNum();//初始化卷号
    void addComboxVolumeNumItem(const QString &text);//添加卷号
    void setTableView_stripQuality_Data(const QString &recordID);//加载订单信息数据
    void setTableView_stripQuality_raw_material_Data(const QString &recordID);//加载原材料信息
    void setTableView_stripQuality_test_record_Data(const QString &recordID);//加载在线检测记录数据

    std::map<QString, QString> getTableView_stripQuality_Data();//获取订单信息数据
    std::map<QString, QString> getTableView_stripQuality_Data(const QString &recordId);//获取订单信息数据
    std::map<QString, QString> getTableView_stripQuality_raw_material_Data();//原材料信息
    std::map<QString, QString> getTableView_stripQuality_test_record_Data();//在线检测记录数据
    //std::map<QString, QString> getTableView_stripQuality_volume_id_Data();//卷号与id映射表
    
    //QList<QVariantList> getRecordData(const QString &volume);
    void setTableViewEditFlags();
    void setTempData(const QMap<QString, QVariantList> &map);
    void initTempArea();
    void setUpdataNewVolumeNum();
    void setLastRolllength(QStringList lengthData,bool isEnd);
    void updateChangeItem(const QString & text);
    void showForm(QRect rect);
    void setModelName();
public slots:
    void slotChangeItem(const QString &);
    void slotDoWork(int);
protected:
   // void keyPressEvent(QKeyEvent *event);   //键盘按下事件
   // void keyReleaseEvent(QKeyEvent *event); //键盘松开事件
signals:
    void signalGetModelName(bool);
    void signalThresholdJsonPath(const QString &);
private:
    void setRowData(int row, const QMap<QString,QStringList> & maplist);
    void initListWidgetFilelist(const QFileInfoList &list);
private slots:
    void slotDoubleClicked(const QModelIndex &);
    void slotEditItemData(int, int, QString);
    void on_toolButton_export_clicked();//导出带材生产工艺记录表
    void on_toolButton_test_clicked();
    void on_toolButton_test_clicked_1();
    void on_toolButton_pdf_clicked();
    void on_toolButtonAdd_clicked();//在线检测记录添加item
    void on_toolButton_export_stripQuality_clicked();//导出带材质量记录表
    void on_pushButton_save_setting_clicked();//保存配置文件
    void on_toolButton_add_clicked();//添加阈值
    void initThresholdJsonFile();
    void ItemDoubleClicked(QListWidgetItem* item);
    void ItemClicked(QListWidgetItem* item);
    void OnListWidgetCurrentListChanged(int);//选中行
    void slotRefreshVolumeNum(bool,int);//刷新卷号列表
    void on_toolButton_addRecord_test_clicked();//添加新的ID，用来测试
    void on_toolButton_saveData_test_clicked();//新数据保存到数据库
    void on_toolButton_editDataSave_clicked();//保存编辑数据
    void on_toolButton_startUpdate_clicked();//启动检测记录刷新
	void on_toolButton_load_clicked();//jiazaiwendu
    void slotAddDataSql(const std::map<QString, QString> data);
    void slotAddRowData(QMap<QString, QString>,bool);
    void slotModeName(QString&);//获取方案名称
    void on_toolButton_export_endReport_clicked();//导出最终表格
    void on_toolButton_inspection_clicked();//存档（首检，尾检）
    std::vector<std::map<QString, QString>> getRecordData();//获取导出时需要传入的数据
    QMap<QString, QString> getLastRollLength();//获取卷长
    void slotTimerAddRecord();
    void on_btnPdf_clicked();
	void on_toolButton_previous_clicked();//检测记录上一页
	void on_toolButton_next_clicked();//检测记录下一页
    void on_toolButton_savesql_clicked();//温度区域数据存档
	void slotComboxPageIndex(const QString&,int);//检测记录当前页
	void slotComboxUpDatePage(const QString &);//检测记录翻页
	void slotSpinBoxUpDatePage(int);//检测记录翻页
	void slotOldDataClear(bool);//在线检测记录历史数据清空
	//获取当前页数
	void slotGetSpinBoxValue();
private:
    Ui::dataMainFormZJ ui;
    proCategoryItem *P_proCategoryItem = Q_NULLPTR;//产品种类定制item
    int _index = 0;
    QString _settingFilePath;//配置文件路径
    QFileInfoList _fileInfoList;//json文件列表
    int _RenameIndex;//重命名选中行
    QString _PrevName;//旧名称
    bool _bIsRename = false;//是否重命名
    QString _dateTimeKey;
    QString _volumeNum;
    QMap<int, QMap<QPoint, QString>> _mapTempData;//加热区温度 key 0：白班1，1：白班2，2：夜班1,3：夜班2
    QList<QPoint> _listTempDay1;//白班1
    QList<QPoint> _listTempDay2;//白班2
    QList<QPoint> _listTempNight1;//夜班1
    QList<QPoint> _listTempNight2;//夜班2
    threadTaskSql * _threadSql = Q_NULLPTR;
    std::map<QString, QString> _dataRecord;
    QMap<QString, QString> _mapVolume_recordID;

    sqlOperation *p_sqlOperation = Q_NULLPTR;//新的数据库操作类，非单例
    sqlOperation *p_sqlOperationJson = Q_NULLPTR;//配置文件json操作
    sqlOperation *p_sqlOperationTestRecord = Q_NULLPTR;//检测记录
    sqlOperation *p_sqlOperationReportImg = Q_NULLPTR;//获取检测记录数据导出图片
    sqlOperation *p_sqlOperationOrder = Q_NULLPTR;//订单操作
    sqlOperation *p_sqlOperationLastRoll = Q_NULLPTR;//卷长
    sqlOperation *p_sqlOperationEditOrder = Q_NULLPTR;//编辑订单
    sqlOperation *p_sqlOperationTem = Q_NULLPTR;//温度区域数据
    bool _isRefresh = false;
    QString _modelName;//方案名称
    std::vector<std::map<QString, QString>> _testRecordData;//检测记录
    long long _indexNum = 0;
    bool _isEnd = false;
    QTimer *_timerAddRecord = Q_NULLPTR;//定时添加检测记录
    QAxObject *_excel = Q_NULLPTR;
    QString _addRecordIndexjuanhao;//手动添加检测记录时，由于线程还在刷新，需要把编辑的卷号先保存在内存中
	int _pageSize;//总页数
	bool _isAddFlag = false;
	bool _isUpDateCombox = false;
	bool _isUpDateSpinBox = false;
};