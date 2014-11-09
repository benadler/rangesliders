#include "rangeslider.h"

#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QKeyEvent>

RangeSlider::RangeSlider(const int rangeMin, const int rangeMax, const int valueLo, const int valueHi) :
    mValueLo(valueLo),
    mValueHi(valueHi),
    mSizeSingleStep(1),
    mSizePageStep(10),
    mMouseMovementMode(Disabled)
{
    setOrientation(Qt::Horizontal);
    setRange(rangeMin, rangeMax);
    setFocusPolicy(Qt::StrongFocus);
}

void RangeSlider::setOrientation(const Qt::Orientation orientation)
{
    mOrientation = orientation;

    // Determine size of slider handle
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    if(mOrientation == Qt::Horizontal)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        mSliderHandleSize = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this).size();
    }
    else
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        mSliderHandleSize = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this).size();
    }

    updateGeometry();
    update();
}

void RangeSlider::setValueLo(int valueLo)
{
    valueLo = qBound(mMinimum, valueLo, mMaximum);
    if(valueLo <= mValueHi)
    {
        mValueLo = valueLo;
    }
    else
    {
        mValueLo = mValueHi;
        mValueHi = valueLo;
        emit valueHiChanged(mValueHi);
    }
    emit valueLoChanged(mValueLo);
    update();
}

void RangeSlider::setValueHi(int valueHi)
{
    valueHi = qBound(mMinimum, valueHi, mMaximum);
    if(valueHi >= mValueLo)
    {
        mValueHi = valueHi;
    }
    else
    {
        mValueHi = mValueLo;
        mValueLo = valueHi;
        emit valueLoChanged(mValueLo);
    }
    emit valueHiChanged(mValueHi);
    update();
}

void RangeSlider::setMinimum(const int min)
{
    setRange(min, maximum());
}

void RangeSlider::setMaximum(const int max)
{
    setRange(minimum(), max);
}

void RangeSlider::setRange(const int min, const int max)
{
    int oldMin = mMinimum;
    int oldMax = mMaximum;
    mMinimum = qMin(min, max);
    mMaximum = qMax(min, max);
    if (oldMin != mMinimum || oldMax != mMaximum)
    {
        setValueLo(mValueLo); // re-bound
        setValueHi(mValueHi); // re-bound
        emit rangeChanged(mMinimum, mMaximum);
        update();
    }
}

QSize RangeSlider::sizeHint() const
{
    return (mOrientation == Qt::Horizontal ? QSize(150, 20) : QSize(20, 150));
}

QSize RangeSlider::minimumSizeHint() const
{
    return (mOrientation == Qt::Horizontal ? QSize(30, 20) : QSize(20, 30));
}

QRect RangeSlider::rectContainingBothSliders()
{
    const QRect contRect(rect());
    if(mOrientation == Qt::Horizontal)
    {
        int valRangePixels = contRect.width() - mSliderHandleSize.width();
        int pixelPosOfThumbRectLeft = (float)valRangePixels / (mMaximum - mMinimum) * (mValueLo - mMinimum);
        int pixelPosOfThumbRectRight = (float)valRangePixels / (mMaximum - mMinimum) * (mValueHi - mMinimum) + mSliderHandleSize.width();

        return QRect(
                    contRect.x() + pixelPosOfThumbRectLeft, // left
                    contRect.y(), // top
                    pixelPosOfThumbRectRight - pixelPosOfThumbRectLeft, // width
                    contRect.height()); // height
    }
    else
    {
        double valRange = contRect.height() - mSliderHandleSize.height();
        int up = (1.0 - mValueHi) * valRange;
        int down = (1.0 - mValueLo) * valRange; down += mSliderHandleSize.height();
        return QRect(contRect.x(), contRect.y() + up, contRect.width(), down - up);
    }
}

void RangeSlider::mousePressEvent(QMouseEvent * e)
{
    QRect rectBetweenHandles(rectContainingBothSliders());

    int lengthOfRectBetweenHandles, pixelsIntoRectBetweenHandles;

    if(mOrientation == Qt::Horizontal)
    {
        lengthOfRectBetweenHandles = rectBetweenHandles.width();
        pixelsIntoRectBetweenHandles = e->pos().x() - rectBetweenHandles.left();
    }
    else
    {
        lengthOfRectBetweenHandles = rectBetweenHandles.height();
        pixelsIntoRectBetweenHandles = rectBetweenHandles.top() + lengthOfRectBetweenHandles - e->pos().y();
    }

    if(pixelsIntoRectBetweenHandles < 0 || pixelsIntoRectBetweenHandles > lengthOfRectBetweenHandles)
        return;

    mDragStartPosition = e->pos();

    mDragStartValueLo = mValueLo;
    mDragStartValueHi = mValueHi;

    if(pixelsIntoRectBetweenHandles < mSliderHandleSize.width())
    {
        mMouseMovementMode = MoveLo;
    }
    else if(pixelsIntoRectBetweenHandles >= lengthOfRectBetweenHandles - mSliderHandleSize.width())
    {
        mMouseMovementMode = MoveHi;
    }
    else
    {
        mMouseMovementMode = MoveBoth;
    }

    update();
}

void RangeSlider::mouseMoveEvent(QMouseEvent * e)
{
    if(!e->buttons()) return;

    if(mMouseMovementMode != Disabled)
    {
        const QPoint distanceMoved = e->pos() - mDragStartPosition;

        float delta = mOrientation == Qt::Horizontal ? distanceMoved.x() : distanceMoved.y();
        delta /= (float)(rect().width() - mSliderHandleSize.width()) / (mMaximum - mMinimum);

        switch(mMouseMovementMode)
        {
        case MoveBoth:
            setValueLo(mDragStartValueLo + delta);
            setValueHi(mDragStartValueHi + delta);
            break;
        case MoveHi:
            setValueHi(mDragStartValueHi + delta);
            break;
        case MoveLo:
            setValueLo(mDragStartValueLo + delta);
            break;
        }
    }
}


void RangeSlider::mouseReleaseEvent(QMouseEvent * e)
{
    Q_UNUSED(e);
    mMouseMovementMode = Disabled;
}

void RangeSlider::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Right:
        setValueLo(mValueLo + mSizeSingleStep);
        setValueHi(mValueHi + mSizeSingleStep);
        break;
    case Qt::Key_Down:
    case Qt::Key_Left:
        setValueLo(mValueLo - mSizeSingleStep);
        setValueHi(mValueHi - mSizeSingleStep);
        break;
    case Qt::Key_PageUp:
        setValueLo(mValueLo + mSizePageStep);
        setValueHi(mValueLo + mSizePageStep);
        break;
    case Qt::Key_PageDown:
        setValueLo(mValueLo - mSizePageStep);
        setValueHi(mValueLo - mSizePageStep);
        break;
    default:
        return QWidget::keyPressEvent(e);
    }
}

void RangeSlider::initStyleOption(QStyleOptionSlider *option) const
{
    if (!option) return;

    option->initFrom(this);
    option->subControls = QStyle::SC_None;
    option->activeSubControls = QStyle::SC_None;
    option->orientation = orientation();
    option->maximum = mMaximum;
    option->minimum = mMinimum;
    option->tickPosition = QSlider::NoTicks;
    option->tickInterval = 0;
    option->upsideDown = false;
    option->direction = Qt::LeftToRight;
    option->singleStep = mSizeSingleStep;
    option->pageStep = mSizePageStep;
    if(orientation() == Qt::Horizontal) option->state |= QStyle::State_Horizontal;
}

int RangeSlider::valueDistanceToPixelDistance(const int valueDistance)
{
    const int pixelDistance = rect().width() * ((float)valueDistance / (mMaximum - mMinimum));
    return pixelDistance;
}

void RangeSlider::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QStyleOptionSlider opt;
    initStyleOption(&opt);

    // draw whole-length groove using default palette
    opt.subControls = QStyle::SC_SliderGroove;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

    // draw rectangle between sliders - this gets fucked up for negative minima!
    opt.sliderPosition = 100;
    opt.sliderValue = 100;
    opt.minimum = 1;
    opt.maximum = 100;
    opt.direction = Qt::RightToLeft;

    // buggy: when moving the left side, we get strange graphics for negative values. Yes, please send a patch!
    opt.rect.setLeft(opt.rect.left() + valueDistanceToPixelDistance(mValueLo - mMinimum));
    opt.rect.setRight(opt.rect.right() - valueDistanceToPixelDistance(mMaximum - mValueHi));
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

    // Now draw handles
    initStyleOption(&opt);
    opt.subControls = QStyle::SC_SliderHandle;

    // Left handle
    opt.sliderPosition = mValueLo;
    opt.sliderValue = mValueLo;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

    // Right handle
    opt.sliderPosition = mValueHi;
    opt.sliderValue = mValueHi;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

//    p.setPen(QPen(Qt::green));
//    p.drawRect(rectContainingBothSliders());
}






















FloatingRangeSlider::FloatingRangeSlider(const int initialRangeMin, const int initialRangeMax, const int valueLo, const int valueHi, const float padding) :
    RangeSlider(initialRangeMin, initialRangeMax, valueLo, valueHi),
//    mInitialRangeMin(initialRangeMin),
//    mInitialRangeMax(initialRangeMax),
    mPadding(qBound(0.0f, padding, 0.2f))
{
    mPropertyAnimationMin = new QPropertyAnimation(this, "minimum");
    mPropertyAnimationMin->setDuration(200);
    mPropertyAnimationMin->setEasingCurve(QEasingCurve::OutQuart);

    mPropertyAnimationMax = new QPropertyAnimation(this, "maximum");
    mPropertyAnimationMax->setDuration(200);
    mPropertyAnimationMin->setEasingCurve(QEasingCurve::OutQuart);
}

void FloatingRangeSlider::mouseReleaseEvent(QMouseEvent* e)
{
    const int currentRange = mMaximum - mMinimum;

    // If we're close to the minimum, decrease minimum!
    const float relativePositionLo = (float)(mValueLo - mMinimum) / (mMaximum - mMinimum);
    if(relativePositionLo < mPadding)
    {
        qDebug() << "FloatingRangeSlider::mouseReleaseEvent(): starting animation at relativepos" << relativePositionLo;
        mPropertyAnimationMin->setStartValue(mMinimum);
        mPropertyAnimationMin->setEndValue(mMinimum - (currentRange * 0.2f));
        mPropertyAnimationMin->start();
    }
    else if(relativePositionLo > 0.5f - mPadding)
    {
        qDebug() << "FloatingRangeSlider::mouseReleaseEvent(): starting animation at relativepos" << relativePositionLo;
        mPropertyAnimationMin->setStartValue(mMinimum);
        mPropertyAnimationMin->setEndValue(mMinimum + (currentRange * 0.2f));
        mPropertyAnimationMin->start();
    }

    const float relativePositionHi = (float)(mValueHi - mMinimum) / (mMaximum - mMinimum);
    if(relativePositionHi > (1.0f - mPadding))
    {
        qDebug() << "FloatingRangeSlider::mouseReleaseEvent(): starting animation at relativepos" << relativePositionHi;
        mPropertyAnimationMax->setStartValue(mMaximum);
        mPropertyAnimationMax->setEndValue(mMaximum + (currentRange * 0.2));
        mPropertyAnimationMax->start();
    }
    else if(relativePositionHi < (0.5 + mPadding))
    {
        qDebug() << "FloatingRangeSlider::mouseReleaseEvent(): starting animation at relativepos" << relativePositionHi;
        mPropertyAnimationMax->setStartValue(mMaximum);
        mPropertyAnimationMax->setEndValue(mMaximum - (currentRange * 0.2));
        mPropertyAnimationMax->start();
    }

    RangeSlider::mouseReleaseEvent(e);
}












FloatingGradientRangeSlider::FloatingGradientRangeSlider(const int initialRangeMin, const int initialRangeMax, const int valueLo, const int valueHi, const float padding) :
    FloatingRangeSlider(initialRangeMin, initialRangeMax, valueLo, valueHi, padding)
{

}


void FloatingGradientRangeSlider::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QStyleOptionSlider opt;
    initStyleOption(&opt);

    // draw whole-length groove using default palette
    opt.subControls = QStyle::SC_SliderGroove;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

    QLinearGradient gradient(rectContainingBothSliders().topLeft(), rectContainingBothSliders().topRight());
    gradient.setSpread(QGradient::PadSpread);

    QMapIterator<float, QColor> i(mColorMap);
    while(i.hasNext()){
        i.next();
        gradient.setColorAt(i.key(), i.value());
    }

    const QRect groove = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    p.fillRect(groove.adjusted(0, 1, 0, -1), QBrush(gradient));

    // Now draw handles
    initStyleOption(&opt);
    opt.subControls = QStyle::SC_SliderHandle;

    // Left handle
    opt.sliderPosition = mValueLo;
    opt.sliderValue = mValueLo;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

    // Right handle
    opt.sliderPosition = mValueHi;
    opt.sliderValue = mValueHi;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);
}
