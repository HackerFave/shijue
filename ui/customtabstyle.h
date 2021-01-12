#include <QPainter>
#include <QProxyStyle>
#include <qstring.h>
class CustomTabStyle : public QProxyStyle
{
public:
	QSize sizeFromContents(ContentsType type, const QStyleOption *option,
		const QSize &size, const QWidget *widget) const
	{
		
		QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
		if (type == QStyle::CT_TabBarTab) {
			s.transpose();
			s.rwidth() = 150; // 设置每个tabBar中item的大小
			s.rheight() = 50;
		}
		return s;
	}

	void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
	{
		
		if (element == CE_TabBarTabLabel) {
			if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
				QRect allRect = tab->rect;
				if (!QString::compare(tab->text,""))
				{
					painter->save();
					painter->setPen(QColor(0, 0, 0));
					painter->setBrush(QBrush(0x353535));
					painter->drawRect(allRect.adjusted(0, 0, 0, 800));
					painter->restore();
					return;
				}


				if (tab->state & QStyle::State_Selected) {
					painter->save();
					painter->setPen(QColor(0, 0, 0));
					painter->setBrush(QBrush(0x303030));
					painter->drawRect(allRect.adjusted(0, 0, 0,0));
					painter->restore();
				}
				else 
				{
					painter->save();
					painter->setPen(QColor(0, 0, 0));
					painter->setBrush(QBrush(0x353535));
					painter->drawRect(allRect.adjusted(0, 0, 0, 0));
					painter->restore();
				}

				QTextOption option;
				option.setAlignment(Qt::AlignCenter);
				if (tab->state & QStyle::State_Selected) 
				{
					painter->setPen(QColor(255,255,255,204));
				}
				else 
				{
					painter->setPen(QColor(255, 255, 255, 153));
					
				}

				painter->drawText(allRect,tab->text, option);

				if (!tab->icon.isNull())
				{
					tab->icon.paint(painter, allRect.adjusted(20,0,0,0), Qt::AlignLeft | Qt::AlignVCenter);
				}
				
				return;
			}
		}

		if (element == CE_TabBarTab) {
			QProxyStyle::drawControl(element, option, painter, widget);
		}
	}
};
