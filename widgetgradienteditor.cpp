#include "widgetgradienteditor.h"

#include <QPainter>
#include <QDebug>
#include <QColor>
#include <QMouseEvent>
#include <QColorDialog>

WidgetGradientEditor::WidgetGradientEditor(QWidget *parent)
    : QWidget(parent),
      mPadding(0.1),
      mSpreadMode(QGradient::PadSpread)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setToolTip("");
    setFocusPolicy(Qt::StrongFocus);
    mRectGradient = QRect(QPoint(0,0), QPoint(0,0));
    setMinimumSize(parent->width() - 10, 25);
    setMouseTracking(true);

    slotReset();
}

const QMap<float, QColor> WidgetGradientEditor::getGradient() const
{
    QMap<float, QColor> gradientStops;
    for(int i=0;i<mMarkers.size();i++)
        gradientStops.insert(mMarkers[i].position, mMarkers[i].color);
    return gradientStops;
}

void WidgetGradientEditor::setGradient(const QMap<float, QColor> stops)
{
    if(stops.size() < 2)
    {
        slotReset();
        return;
    }

    mMarkers.clear();
    QMapIterator<float, QColor> i(stops);
    while (i.hasNext())
    {
        i.next();
        mMarkers.append(GradientMarker(i.key(), i.value()));
    }
}

const QString WidgetGradientEditor::gradientToString(const QMap<float, QColor> stops)
{
    QString config;
    QMapIterator<float, QColor> i(stops);
    while (i.hasNext())
    {
        i.next();
        config.append(
                    QString("%1%2,%3,%4,%5")
                    .arg(i.hasPrevious() ? ":":"")
                    .arg(i.key())
                    .arg(i.value().red())
                    .arg(i.value().green())
                    .arg(i.value().blue()));
    }
    return config;
}

QMap<float, QColor> WidgetGradientEditor::stringToGradient(const QString config)
{
    QMap<float, QColor> gradientStops;
    const QStringList stops = config.split(":", QString::SkipEmptyParts);

    for(int i=0;i<stops.size();i++)
    {
        const QStringList props = stops.at(i).split(",");
        if(props.size() == 4) gradientStops.insert(props.at(0).toFloat(), QColor(props.at(1).toInt(), props.at(2).toInt(), props.at(3).toInt()));
    }

    return gradientStops;
}

void WidgetGradientEditor::slotReset(const Preset &preset)
{
    mMarkerIsReadyToMove = false;
    mMarkerHasBeenMoved = false;
    mMarkers.clear();

    // do not emit a changed signal for every single marker!
    blockSignals(true);

    if(preset == PresetJet)
    {
        slotAddMarker(QColor(255,000,000), 1.00);
        slotAddMarker(QColor(255,255,000), 0.75);
        slotAddMarker(QColor(000,255,000), 0.50);
        slotAddMarker(QColor(000,255,255), 0.25);
        slotAddMarker(QColor(000,000,255), 0.00);
    }
    else if(preset == PresetJetDark)
    {
        slotAddMarker(QColor(255,128,000), 0.75);
        slotAddMarker(QColor(255,000,000), 1.00);
        slotAddMarker(QColor(000,128,255), 0.25);
        slotAddMarker(QColor(000,128,000), 0.50);
        slotAddMarker(QColor(000,000,255), 0.00);
    }
    else if(preset == PresetEarth)
    {
        slotAddMarker(QColor(000,128,255), 0.25);
        slotAddMarker(QColor(000,128,000), 0.50);
    }

    blockSignals(false);
    emit gradientChanged(getGradient());
    update();
}

void WidgetGradientEditor::slotAddMarker(const QColor &color, float position, const bool isMovedByMouse)
{
    GradientMarker marker;
    marker.color = color;
    marker.position = position;

    if(isMovedByMouse)
    {
        marker.hasFocus = true;
        mMarkerIsReadyToMove = true;

        // clear all other marker's focus!
        for(int i=0;i<mMarkers.size();i++)
        {
            Q_ASSERT(!mMarkers[i].hasFocus);
            mMarkers[i].hasFocus = false;
            qDebug() << "addMarker: marker" << i << "has focus:" << mMarkers[i].hasFocus;
        }
    }

    mMarkers.append(marker);
    emit gradientChanged(getGradient());
}

void WidgetGradientEditor::removeMarker(int index)
{
    if(mMarkers.size() <= 2 || mMarkers.size() <= index)
    {
        return;
    }
    mMarkers.removeAt(index);
    update();
    emit gradientChanged(getGradient());
}

void WidgetGradientEditor::paintEvent(QPaintEvent *)
{
    if(mRectView.size().isNull()  || mRectView.size().isEmpty() || mRectView.topLeft() == mRectView.bottomRight())
    {
        mRectView = QRect(QPoint(0,0), QPoint(width(), height()));
        mRectGradient = mRectView.adjusted(
                    width() * mPadding, // left padding
                    0, // top padding
                    -width() * mPadding, // right padding
                    0); // bottom padding
    }
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setRenderHint(QPainter::Antialiasing);

    QPoint gradStart = QPoint(mRectGradient.topLeft ().x(), mRectGradient.bottomLeft ().y()/2);
    QPoint gradStop  = QPoint(mRectGradient.topRight().x(), mRectGradient.bottomRight().y()/2);
    QLinearGradient gradient(gradStart, gradStop);
    gradient.setSpread(mSpreadMode);

    for(int i=0;i<mMarkers.size();i++)
    {
        const GradientMarker& marker = mMarkers.at(i);
        gradient.setColorAt(marker.position, marker.color);
    }

    painter.fillRect(mRectView.adjusted(0, 1, 0, -12), QBrush(gradient)); // 12 px bottom padding - leave room for sliders!
    painter.setPen(QPen(palette().color(QPalette::WindowText)));
    painter.drawLine(mRectGradient.topLeft(), mRectGradient.bottomLeft());
    painter.drawLine(mRectGradient.topRight(), mRectGradient.bottomRight());

    painter.setPen(QPen(palette().color(QPalette::WindowText)));
    QPen pen;
    pen.setWidth(3);
    for(int i=0;i<mMarkers.size();i++)
    {
        /*painter.drawPie(
                    QRect(
                        mRectGradient.left() + mMarkers[i].position*mRectGradient.width() - 8,
                        height()-17,
                        16, 16),
                    -45*16, // start angle * 16
                    -90*16); // span angle * 16*/
        pen.setColor(mMarkers[i].color);
        painter.setPen(pen);
        painter.drawEllipse(QRect(mRectGradient.left() + mMarkers[i].position*mRectGradient.width() - 2, height()-7, 4, 4));
        painter.setPen(QPen(palette().color(QPalette::WindowText)));
        painter.drawEllipse(QRect(mRectGradient.left() + mMarkers[i].position*mRectGradient.width() - 4, height()-9, 8, 8));
    }

    painter.end();
}

void WidgetGradientEditor::resizeEvent(QResizeEvent * event)
{
    mRectView = QRect();
    update();
}

void WidgetGradientEditor::mousePressEvent(QMouseEvent *event)
{
    dragStart = event->pos();
    //qDebug() << "viewrect:" << viewRect;
    for(int i=0;i<mMarkers.size();i++)
    {
        GradientMarker& marker = mMarkers[i];//it.value();
        const int pixelPosOfMarker = mRectGradient.left() + marker.position * mRectGradient.width();
        if(abs(dragStart.x() - pixelPosOfMarker) < 9)
        {
            if(event->button() == Qt::LeftButton)
            {
                marker.hasFocus = true;
                mMarkerIsReadyToMove = true;
                mMarkerHasBeenMoved = false;
            }
            else
            {
                removeMarker(i);
                update();
                emit gradientChanged(getGradient());
                return;
            }
        }
        else
        {
            marker.hasFocus = false;
        }
    }

    if(!mMarkerIsReadyToMove && event->button() == Qt::LeftButton)
    {
        // We did not find a marker whose poly covers the clicked point, so the user doesn't want to move a marker. Add a marker here!
        const float  markerPos = (float)(event->pos().x() - mRectGradient.left())/(mRectGradient.width() - 5);
        qDebug() << "adding marker at" << markerPos;
        //mMarkerIsReadyToMove = true;

        QColor colorSelected = QColorDialog::getColor(QColor(((float)qrand())/RAND_MAX*255, (float)qrand()/RAND_MAX*255, (float)qrand()/RAND_MAX*255), this, "Select step color");

        if(colorSelected.isValid()) slotAddMarker(colorSelected, markerPos/*, mMarkerIsReadyToMove*/);
    }

    update();
}

void WidgetGradientEditor::mouseMoveEvent(QMouseEvent *event)
{
    if(!mMarkerIsReadyToMove) return;

    for(int i=0;i<mMarkers.size();i++)
    {
        GradientMarker& marker = mMarkers[i];
        if(marker.hasFocus)
        {
            const int pixelPosOfMarker = mRectGradient.left() + marker.position * mRectGradient.width();
            const float dPos = (float)(event->pos().x() - dragStart.x())/((qreal)mRectGradient.width());

            // We always set this true. If the user wants to drag a slider at 0.0 to the left or at 1.0 to the right, it won't work.
            // But we shouldn't show a color-dialog after releasing the mouse!
            mMarkerHasBeenMoved = true;

            if(marker.position + dPos > 1 || marker.position + dPos < 0) break; // do not move beyond borders
            if(dPos > 0 && event->pos().x() < pixelPosOfMarker) break; // sync mouse cursor with slider before moving
            if(dPos < 0 && event->pos().x() > pixelPosOfMarker) break; // sync mouse cursor with slider before moving

            marker.position += dPos;
            emit gradientChanged(getGradient());
            break;
        }
    }
    dragStart = event->pos();
    update();
}

void WidgetGradientEditor::mouseReleaseEvent(QMouseEvent *)
{
    mMarkerIsReadyToMove = false;

    if(mMarkerHasBeenMoved)
    {
        // make sure that marker doesn't completely overlap with neighbors!
    }
    else
    {
        GradientMarker& marker = mMarkers[0];
        int index = -1;
        for(int i=0;i<mMarkers.size();i++)
        {
            if(mMarkers[i].hasFocus) index = i;
        }

        if(index != -1)
        {
            // A marker was clicked/released without being moved. Change color!
            QColor newColor = QColorDialog::getColor(marker.color, this, "Select step color");
            if(newColor.isValid())
            {
                qDebug() << "marker 1" << marker.color;
                mMarkers[index].color = newColor;
                qDebug() << "marker 2" << mMarkers[index].color;
                update();
                emit gradientChanged(getGradient());
            }
        }
    }

}
