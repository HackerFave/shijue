#ifndef NBASESWITCHBUTTON_H
#define NBASESWITCHBUTTON_H



#include <QObject>
#include <QWidget>
#include <QColor>
#include <QPainter>


/**
 * @brief The NBaseSwitchButton class switch button
 */
class NBaseSwitchButton : public QWidget
{
    Q_OBJECT
public:
    explicit NBaseSwitchButton(QWidget *parent = 0);
    void setChecked(bool stat);
	bool isChecked();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void drawPressBackground(QPainter *painter);
    void drawPressSlide(QPainter *painter);

signals:
     void checkedChanged(bool checked);
private:
    QColor mOffBgcBegin;
    QColor mOffBgcEnd;
    QColor mOnBgcBegin;
    QColor mOnBgcEnd;
    QColor mSlideONCBegin;
    QColor mSlideONCEnd;
    QColor mSlidOFFCBegin;
    QColor mSlidOFFCEnd;
    bool mState;
    bool mMouseDown;
    bool mMouseMove;
    bool mMouseUp;
    QPoint firstPoint;
    QPoint lastPoint;
};

#endif // NBASESWITCHBUTTON_H
