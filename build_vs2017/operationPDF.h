#pragma once

#include <QObject>
#include <QPrinter>
#include <QPainter>
#include <QPixmap>
#include <QTextDocument>
#include <QTextBlock>
#include <qtextcodec.h>
#pragma execution_character_set("utf-8")

class operationPDF : public QObject
{
    Q_OBJECT

public:
    operationPDF(QObject *parent);
    ~operationPDF();
    // ����PDF�ļ�����
    void setPdfName(QString fileName);
    // д�����ֵ�pdf��
    void writeTextToPdf(QString text);
    // ����ͼƬ��PDF��
    void insertPictureToPdf(QString picFile);
    // ��������
    void endPainter();

    // htmlת��ΪPDF
    QString saveHtmlToPDF(std::vector<std::map<QString, QString>> dataTable, QMap<QString, QString> map, const QString & pngName, int width, int height);
    /**
     * @brief SavePdf �����ݱ����PDF���
     * @param fileName �ļ���
     * @param title ������
     * @param subTitle ��񸱱���
     * @param columnNames ����
     * @param columnWidths �п�
     * @param content ��������
     * @param landscape ֽ���Ƿ����
     * @param check  �Ƿ��ĳ�м�������ʽ
     * @param pageSize ֽ������
     */
    void SavePdf(QString fileName, QString title, QString subTitle,
        QList<QString> columnNames, QList<int> columnWidths,
        QStringList content, bool landscape, bool check,
        QPrinter::PageSize pageSize = QPrinter::A4);

    void SavePdf(QString fileName, QString title,
        QList<QString> columnNames, QList<int> columnWidths,
        QStringList subTitle1, QStringList subTitle2,
        QStringList content, bool landscape, bool check,
        QPrinter::PageSize pageSize = QPrinter::A4);

private:
    QPrinter *m_printer;
    QPainter *m_painter;
    int m_x;
    int m_y;
    QStringList html;
};
