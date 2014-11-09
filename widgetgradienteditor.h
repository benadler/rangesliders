#ifndef WIDGETGRADIENTEDITOR_H
#define WIDGETGRADIENTEDITOR_H

#include <QWidget>

struct GradientMarker
{
public:

    GradientMarker(const float position, const QColor& color) : hasFocus(false), position(position), color(color) { }

    GradientMarker() : hasFocus(false), position(0.5) { }

    GradientMarker(const GradientMarker &other) :
        hasFocus(other.hasFocus),
        color(other.color),
        position(other.position) { }

    const bool operator<(const GradientMarker& o) const
    {
        return position < o.position;
    }

   float position;
   bool hasFocus;
   QColor color;
};

class WidgetGradientEditor : public QWidget
{
   Q_OBJECT

public:
   explicit WidgetGradientEditor(QWidget *parent = nullptr);

    enum Preset
    {
        PresetEmpty,
        PresetJet,
        PresetJetDark,
        PresetEarth
    };

   void removeMarker(int index);
   const QMap<float, QColor> getGradient() const;
   void setGradient(const QMap<float, QColor> stops);
   static const QString gradientToString(const QMap<float, QColor> stops);
   static QMap<float, QColor> stringToGradient(const QString config);

protected:
   void paintEvent       (QPaintEvent *);
   void resizeEvent(QResizeEvent * event);
   void mousePressEvent  (QMouseEvent *);
   void mouseMoveEvent   (QMouseEvent *);
   void mouseReleaseEvent(QMouseEvent *);

public slots:
   void slotReset(const Preset& preset = PresetEmpty);
   void slotAddMarker(const QColor &color, float position = 0.5, const bool isMovedByMouse = false);

signals:
   void gradientChanged(const QMap<float, QColor>);

private:
   QGradient::Spread mSpreadMode;
   float mPadding; // the padding area on the outer edges is used to repeat/pad the gradient
   bool mMarkerIsReadyToMove;
   bool mMarkerHasBeenMoved;
   QRect mRectView;
   QRect mRectGradient;
   QSize viewSize;
   QPoint dragStart;
   QVector<GradientMarker> mMarkers;
};

#endif // WIDGETGRADIENTEDITOR_H
