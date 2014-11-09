#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QPalette>
#include <QPropertyAnimation>

#include <QStyleOption>

// Warning: only works for horizontal sliders. Vertical must be completed.

class RangeSlider : public QWidget
{
    Q_OBJECT

public:
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)

    enum MouseMovementMode
    {
        Disabled,
        MoveBoth,
        MoveHi,
        MoveLo
    };

    RangeSlider(const int rangeMin, const int rangeMax, const int valueLo, const int valueHi);
    Qt::Orientation orientation() const { return mOrientation; }
    void setOrientation(const Qt::Orientation orientation);
    const int minimum() const {return mMinimum;}
    const int maximum() const {return mMaximum;}
    const int valueLo() const { return mValueLo; }
    const int valueHi() const { return mValueHi; }
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public slots:
    void setMinimum(const int min);
    void setMaximum(const int max);
    void setRange(const int min, const int max);
    void setStepSize(const int f) {mSizeSingleStep = f;}
    void setPageSize(const int f) {mSizePageStep = f;}
    void setValueLo(int valueLo);
    void setValueHi(int valueHi);

signals:
    void valueLoChanged(int valueLo);
    void valueHiChanged(int valueHi);
    void rangeChanged(int lo, int hi);

protected:
    void initStyleOption(QStyleOptionSlider* option) const;
    int valueDistanceToPixelDistance(const int valueDistance);
    QRect rectContainingBothSliders();

    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
    virtual void paintEvent(QPaintEvent*);

    Qt::Orientation mOrientation;
    QSize mSliderHandleSize; // how many pixels is the slider handle wide/high?
    int mMinimum, mMaximum;
    int mValueLo, mValueHi;
    int mSizeSingleStep, mSizePageStep;
    QPoint mDragStartPosition;
    int mDragStartValueLo, mDragStartValueHi;
    MouseMovementMode mMouseMovementMode;
};

class FloatingRangeSlider : public RangeSlider
{
    Q_OBJECT

    float mPadding;
    QPropertyAnimation* mPropertyAnimationMin;
    QPropertyAnimation* mPropertyAnimationMax;

public:
    // Use padding = 0.1 as an example:
    //  - when any slider goes lower than 10% or higher than 90%, we rescale the slider
    //  - when lo slider goes higher than (middle-padding)=40%, we rescale the slider
    //  - when hi slider goes lower than (middle+padding)=60%, we rescale the slider
    FloatingRangeSlider(const int initialRangeMin, const int initialRangeMax, const int valueLo, const int valueHi, const float padding);

protected:
    void mouseReleaseEvent(QMouseEvent*e);
};

class FloatingGradientRangeSlider : public FloatingRangeSlider
{
    Q_OBJECT

    QMap<float, QColor> mColorMap;

public:
    FloatingGradientRangeSlider(const int initialRangeMin, const int initialRangeMax, const int valueLo, const int valueHi, const float padding);

public slots:
    void slotSetColorMap(const QMap<float, QColor>& colorMap)
    {
        mColorMap = colorMap;
        update();
    }

protected:
    void paintEvent(QPaintEvent*);
};

#endif
