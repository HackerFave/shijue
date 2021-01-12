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
    // 设置PDF文件名字
    void setPdfName(QString fileName);
    // 写入文字到pdf上
    void writeTextToPdf(QString text);
    // 插入图片到PDF上
    void insertPictureToPdf(QString picFile);
    // 结束绘制
    void endPainter();

    // html转化为PDF
    QString saveHtmlToPDF(std::vector<std::map<QString, QString>> dataTable, QMap<QString, QString> map, const QString & pngName, int width, int height);
    /**
     * @brief SavePdf 将数据保存成PDF表格
     * @param fileName 文件名
     * @param title 表格标题
     * @param subTitle 表格副标题
     * @param columnNames 列名
     * @param columnWidths 列宽
     * @param content 数据内容
     * @param landscape 纸张是否横向
     * @param check  是否给某列加特殊样式
     * @param pageSize 纸张类型
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
