#pragma once
#pragma execution_character_set("utf-8")
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QTableWidget>
#include <qaxobject.h>

class QExcelEngine
{
public:
    QExcelEngine();
	QExcelEngine(QString xlsFile);
    ~QExcelEngine();
 
public:
    bool Open(int nSheet, bool visible);
    bool Open(QString xlsFile, int nSheet, bool visible);
    void Save();
    void Close();
    bool SaveDataFrTable(QTableWidget *tableWidget);
    bool ReadDataToTable(QTableWidget *tableWidget);
    QVariant GetCellData(int row, int column);
    bool SetCellData(int row, int column, QVariant data);
    void ClearAllData(QString strData);
	bool WriteCurrentSheet(const QList<QList<QVariant>>& cells);
	void CastListListVariant2Variant(const QList<QList<QVariant>>& cells, QVariant & res);
	void ConvertToColName(int data, QString & res);
	QString To26AlphabetString(int data);
    void Clear();
    bool IsOpen();
    bool IsValid();
    int GetRowCount() const;
    int GetColumnCount() const;
	void SetIsSaveAlready(bool is) { bIsSaveAlready = is; }
 
private:
    QAxObject *pExcel;
    QAxObject *pWorkbooks;
    QAxObject *pWorkbook;
    QAxObject *pWorksheet;
 
    QString sXlsFile;
 
    int nRowCount;
    int nColumnCount;
    int nStartRow;
    int nStartColumn;
    int nCurrSheet;
 
    bool bIsVisible;
    bool bIsOpen;
    bool bIsValid;
    bool bIsANewFile;
    bool bIsSaveAlready;
};
