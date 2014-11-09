#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

#include "rangeslider.h"
#include "widgetgradienteditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    WidgetGradientEditor* mGradientEditor;

    RangeSlider* mSliderRange;
    FloatingRangeSlider* mSliderFloatingRange;
    FloatingGradientRangeSlider* mSliderFloatingGradientRange;

private slots:
    void slotPrintSliders();
};

#endif // MAINWINDOW_H
