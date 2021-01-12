#include "operationPDF.h"
#include "publicClass.h"
operationPDF::operationPDF(QObject *parent)
    : QObject(parent)
{
    m_printer = new QPrinter;
    m_painter = new QPainter;
    m_x = 10;
    m_y = 20;
}

operationPDF::~operationPDF()
{

}
void operationPDF::setPdfName(QString fileName)
{
    m_printer->setPageSize(QPrinter::A4);
    m_printer->setOutputFormat(QPrinter::PdfFormat);
    m_printer->setOutputFileName(fileName);
    m_painter->begin(m_printer);
}
void operationPDF::writeTextToPdf(QString text)
{
    if (m_printer == NULL)
        return;

    int height = 100;
    int width = 300;
    m_painter->drawText(m_x, m_y, width, height, 0, text);
    m_y += height;
}

void operationPDF::insertPictureToPdf(QString picFile)
{
    if (m_printer == NULL)
        return;

    QPixmap *pixmap = new QPixmap(picFile);

    int width = pixmap->width();
    int height = pixmap->height();
    m_painter->drawPixmap(m_x, m_y, width, height, *pixmap);
    m_y += height;
}

void operationPDF::endPainter()
{
    m_painter->end();
}

QString operationPDF::saveHtmlToPDF(std::vector<std::map<QString, QString>> dataTable,QMap<QString,QString> map,const QString & pngName , int width ,int height)
{

	/*<div style="width:500px;">
            <img src="images/bkjj.png" align="right" width="120" hspace="5" vspace="5" />
            <p>HTML文字环绕图片布局HTML文字环绕图片布局HTML文字环绕图片布局</p>
            <p>第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局第二段HTML文字环绕图片布局</p>
</div>*/
	QString strPath = QString("%1/icons/logo.jpg").arg(publicClass::instance()->getExeDirPath());
    QString timePro;
    QString oper;
    QString remark;
    QString html = "";
    html += "<html>";
    html += "<head>";
    html += "<title>qt实现生成PDF文档</title>";
    html += "<head>";
    html += "<body bgcolor=\"white\">";
	html += "<div style=\"position: relative;\">";
	html += "<div>";
	html += tr("<img src=\"%1\" style=\"float: left; margin-right: 200px;\" width=\"300\" height=\"300\" />")
		.arg(strPath);
	html += "</div>";
	html += "<div>";
    html += "<h1 style=\"background-color:white\">PP带材</h1>";
    html += tr("<h2 style=\"background-color:white\">工作令: %1</h2>").arg(map.value("work_order"));
    html += tr("<h2 style=\"background-color:white\">产品牌号: %1</h2>").arg(map.value("product_brand"));
    html += tr("<h2 style=\"background-color:white\">卷号: %1</h2>").arg(map.value("volume_number"));
    html += tr("<h2 style=\"background-color:white\">玻纤含量: %1    面密度: %2    厚度: %3</h2>")
        .arg(map.value("glass_fiber_content_technical"))
        .arg(map.value("areal_density_technical"))
        .arg(map.value("thickness_technical"));
    html += tr("<h2 style=\"background-color:white\">米数: %1    宽度: %2    重量: %3</h2>")
        .arg(map.value("last_roll_length"))
        .arg(map.value("wide_format_technical"))
        .arg(map.value(""));
    html += tr("<h2 style=\"background-color:white\">树脂: %1    玻纤: %2    其他: %3</h2>")
        .arg(map.value("glass_fiber_grade"))
        .arg(map.value("resin_grade"))
        .arg(map.value("finished_product_grade"));
	html += "</div>";
	html += "</div>";
    //html += "<hr>";
   // html += "<table width=\"100%\">";// border=\"1\" cellspacing=\"4\" cellpadding=\"4\" align=\"center\"
    // html += "<tr>";
    // html += "<td align=\"center\" height=\"60\">编程语言统计</td>";
    // html += "</tr>";
    // html += "<tr>";
    // html += "<td align=\"center\" height=\"25\">2017-01-18---2018-01-18</td>";
    // html += "</tr>";
   // html += "</table>";//expression((document.body.clientWidth-this.offsetWidth)/3)
    html += "<table width = \"800\" style = \"position:absolute;z-index:1;left:1;top:expression((document.body.clientHeight-this.offsetHeight)/2)\ width=\"80%\" border=\"1\" cellspacing=\"0\" cellpadding=\"4\" bgcolor=\"white\" align=\"left\">";
    html += "<tr>";
    html += "<th>米数</th>";
    html += "<th>位置</th>";
    html += "<th>长度</th>";
    html += "<th>缺陷</th>";
    html += "<th>描述</th>";
	html += "<th>处置</th>";
    html += "</tr>";
    std::map<QString, QString> mapData;
    int size = dataTable.size();
    for (std::vector<std::map<QString, QString>>::iterator it = dataTable.begin(); it != dataTable.end(); it++) {
		if ((*it)["outprint"].toInt() == 0) {
			continue;
		}
        html += "<tr>";
        html += tr("<td align=\"center\">%1</td>").arg((*it)["meters"]);
        html += tr("<td align=\"center\">%1</td>").arg((*it)["position"]);
        html += tr("<td align=\"center\">%1</td>").arg((*it)["length"]);
        html += tr("<td align=\"center\">%1</td>").arg((*it)["defect"]);
        html += tr("<td align=\"center\">%1</td>").arg((*it)["description"]);
		html += tr("<td align=\"center\">%1</td>").arg((*it)["concession"].toInt()==1?"让步":"剪切让步" );
        html += "</tr>";
        //std::map<QString, QString> mapData = (*it);
       // timePro = (*it)["production_time"];
       // oper = (*it)["operator"];
       // remark = (*it)["remarks"];
    }
	timePro = QString("%1---%2").arg(map["start_time"]).arg(map["end_time"]);
	QString startTime = map["start_time"];
	QString endTime = map["endtime"];
	oper = map["operator"];
	remark = map["remark"];
    html += "<tr>";
    html += "<td align=\"center\">备注</td>";
    html += tr("< td align=\"center\" colspan = \"5\">%1< / td>").arg(/*mapData["remarks"]*/remark);
    html += tr("<!--<td>%1.3< / td>-->").arg(size);
    html += tr("<!--<td>%1.4< / td>-->").arg(size);
    html += tr("<!--<td>%1.5< / td>-->").arg(size);
	html += tr("<!--<td>%1.6< / td>-->< / tr>").arg(size);
    html += "</table>";
    html += tr("<h2 style=\"background-color:white\">生产时间: %1         操作员: %2</h2>")
        .arg(/*mapData["production_time"]*/timePro)
        .arg(/*mapData["operator"]*/oper);

    html += tr("<p><img src=\"%1\" alt=\"picture\" width=\"%2\" height=\"%3\"></p>")
        .arg(pngName)
        .arg(width)
        .arg(height);
    html += "</body>";
    html += "</html>";
    return html;
}
void operationPDF::SavePdf(QString fileName, QString title, QString subTitle, QList<QString> columnNames,
    QList<int> columnWidths, QStringList content, bool landscape, bool check,
    QPrinter::PageSize pageSize)
{
    //计算行数列数
    int columnCount = columnNames.count();
    int rowCount = content.count();

    //清空原有数据,确保每次都是新的数据
    html.clear();

    //表格开始
    html.append("<table border='0.5' cellspacing='10' cellpadding='3'>");

    //标题占一行,居中显示
    if (title.length() > 0) {
        html.append(QString("<tr><td align='center' style='vertical-align:middle;font-weight:bold;' colspan='%1'>")
            .arg(columnCount));
        html.append(title);
        html.append("</td></tr>");
    }

    //副标题占一行,左对齐显示
    if (subTitle.length() > 0) {
        html.append(QString("<tr><td align='left' style='vertical-align:middle;' colspan='%1'>")
            .arg(columnCount));
        html.append(subTitle);
        html.append("</td></tr>");
    }

    //循环写入字段名,字段名占一行,居中显示
    if (columnCount > 0) {
        html.append("<tr>");
        for (int i = 0; i < columnCount; i++) {
            html.append(QString("<td width='%1' bgcolor='lightgray' align='center' style='vertical-align:middle;'>")
                .arg(columnWidths.at(i)));
            html.append(columnNames.at(i));
            html.append("</td>");
        }
        html.append("</tr>");
    }

    //循环一行行构建数据
    for (int i = 0; i < rowCount; i++) {
        QStringList value = content.at(i).split(":");
        html.append("<tr>");

        if (check) {
            if (value.at(value.count() - 1) == "历史记录") {
                for (int j = 0; j < columnCount; j++) {
                    html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>"
                        "<font color='red'>").arg(columnWidths.at(j)));
                    html.append(value.at(j));
                    html.append("</font></td>");
                }
            }
            else {
                for (int j = 0; j < columnCount; j++) {
                    html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                        .arg(columnWidths.at(j)));
                    html.append(value.at(j));
                    html.append("</td>");
                }
            }
        }
        else {
            for (int j = 0; j < columnCount; j++) {
                html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                    .arg(columnWidths.at(j)));
                html.append(value.at(j));
                html.append("</td>");
            }
        }

        html.append("</tr>");
    }

    html.append("</table>");

    //调用打印机打印PDF
    QPrinter printer;


    //设置纸张规格
    printer.setPageSize(pageSize);
    //设置横向纵向及页边距
    if (landscape) {
        //Orientation：纸张方向，有QPrinter::Portrait(纵向)和QPrinter::Landscape(横向)
        printer.setOrientation(QPrinter::Landscape);
        printer.setPageMargins(10, 10, 10, 11, QPrinter::Millimeter);
    }
    else {
        printer.setOrientation(QPrinter::Portrait);
        printer.setPageMargins(10, 10, 10, 150, QPrinter::Millimeter);
    }
    //设置输出格式为PDF
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::A4);
    //设置输出文件保存位置
    printer.setOutputFileName(fileName);

    QTextDocument textDocument;
    textDocument.setHtml(html.join(""));
    textDocument.print(&printer);
    textDocument.end();
}

void operationPDF::SavePdf(QString fileName, QString title, QList<QString> columnNames,
    QList<int> columnWidths, QStringList subTitle1, QStringList subTitle2,
    QStringList content, bool landscape, bool check, QPrinter::PageSize pageSize)
{
    //计算列数
    int columnCount = columnNames.count();

    //清空原有数据,确保每次都是新的数据
    html.clear();

    //表格开始
    html.append("<table border='0.5' cellspacing='0' cellpadding='3'>");

    //标题占一行,居中显示
    if (title.length() > 0) {
        html.append(QString("<tr><td align='center' style='vertical-align:middle;font-weight:bold;' colspan='%1'>")
            .arg(columnCount));
        html.append(title);
        html.append("</td></tr>");
    }

    //循环添加副标题/字段名/内容
    int count = content.count();
    for (int i = 0; i < count; i++) {
        //加个空行隔开
        html.append(QString("<tr><td colspan='%1'>").arg(columnCount));
        html.append("</td></tr>");

        //副标题1
        if (subTitle1.count() > 0 && subTitle1.count() > i) {
            if (subTitle1.at(i).length() > 0) {
                html.append(QString("<tr><td align='left' style='vertical-align:middle;' colspan='%1'>")
                    .arg(columnCount));
                html.append(subTitle1.at(i));
                html.append("</td></tr>");
            }
        }

        //副标题2
        if (subTitle2.count() > 0 && subTitle2.count() > i) {
            if (subTitle2.at(i).length() > 0) {
                html.append(QString("<tr><td align='left' style='vertical-align:middle;' colspan='%1'>")
                    .arg(columnCount));
                html.append(subTitle2.at(i));
                html.append("</td></tr>");
            }
        }

        //逐个添加字段名称
        if (columnCount > 0) {
            html.append("<tr>");
            for (int i = 0; i < columnCount; i++) {
                html.append(QString("<td width='%1' bgcolor='lightgray' align='center' style='vertical-align:middle;'>")
                    .arg(columnWidths.at(i)));
                html.append(columnNames.at(i));
                html.append("</td>");
            }
            html.append("</tr>");
        }

        QStringList list = content.at(i).split(";");

        //逐个添加数据
        int rowCount = list.count();
        for (int j = 0; j < rowCount; j++) {
            html.append("<tr>");

            QString temp = list.at(j);
            QStringList value = temp.split("|");
            int valueCount = value.count();

            if (check) {
                //如果是历史记录则文字红色
                if (value.at(valueCount - 1) == "1") {
                    for (int k = 0; k < valueCount - 1; k++) {
                        html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>"
                            "<font color='red'>").arg(columnWidths.at(k)));
                        html.append(value.at(k));
                        html.append("</font></td>");
                    }
                }
                else {
                    for (int k = 0; k < valueCount - 1; k++) {
                        html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                            .arg(columnWidths.at(k)));
                        html.append(value.at(k));
                        html.append("</td>");
                    }
                }
            }
            else {
                for (int k = 0; k < valueCount; k++) {
                    html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                        .arg(columnWidths.at(k)));
                    html.append(value.at(k));
                    html.append("</td>");
                }
            }

            html.append("</tr>");
        }
    }

    html.append("</table>");

    //调用打印机打印PDF
    QPrinter printer;
    //设置纸张规格
    printer.setPageSize(pageSize);
    //设置横向纵向及页边距
    if (landscape) {
        printer.setOrientation(QPrinter::Landscape);
        printer.setPageMargins(10, 10, 10, 11, QPrinter::Millimeter);
    }
    else {
        printer.setOrientation(QPrinter::Portrait);
        printer.setPageMargins(10, 10, 10, 15, QPrinter::Millimeter);
    }
    //设置输出格式为PDF
    printer.setOutputFormat(QPrinter::PdfFormat);
    //设置输出文件保存位置
    printer.setOutputFileName(fileName);

    QTextDocument textDocument;
    textDocument.setHtml(html.join(""));
    textDocument.print(&printer);
    textDocument.end();
}