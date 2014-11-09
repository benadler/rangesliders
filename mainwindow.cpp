#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->hide();
    ui->statusBar->hide();

    setWindowTitle(QString("RangeSliderTest"));

    mSliderRange = new RangeSlider(0, 100, 20, 80);
    connect(mSliderRange, &RangeSlider::valueLoChanged, this, &MainWindow::slotPrintSliders);
    connect(mSliderRange, &RangeSlider::valueHiChanged, this, &MainWindow::slotPrintSliders);
    connect(mSliderRange, &RangeSlider::rangeChanged, this, &MainWindow::slotPrintSliders);
    ui->centralWidget->layout()->addWidget(mSliderRange);

    mSliderFloatingRange = new FloatingRangeSlider(0, 100, 20, 80, 0.1);
    connect(mSliderFloatingRange, &FloatingRangeSlider::valueLoChanged, this, &MainWindow::slotPrintSliders);
    connect(mSliderFloatingRange, &FloatingRangeSlider::valueHiChanged, this, &MainWindow::slotPrintSliders);
    connect(mSliderFloatingRange, &FloatingRangeSlider::rangeChanged, this, &MainWindow::slotPrintSliders);
    ui->centralWidget->layout()->addWidget(mSliderFloatingRange);

    mSliderFloatingGradientRange = new FloatingGradientRangeSlider(0, 100, 20, 80, 0.1);
    connect(mSliderFloatingGradientRange, &FloatingGradientRangeSlider::valueLoChanged, this, &MainWindow::slotPrintSliders);
    connect(mSliderFloatingGradientRange, &FloatingGradientRangeSlider::valueHiChanged, this, &MainWindow::slotPrintSliders);
    connect(mSliderFloatingGradientRange, &FloatingGradientRangeSlider::rangeChanged, this, &MainWindow::slotPrintSliders);
    ui->centralWidget->layout()->addWidget(mSliderFloatingGradientRange);

    ui->centralWidget->layout()->addWidget(new QLabel("\ngradient editor: lmb into the void to create marker, lmb on marker to change color. rmb on marker to delete", this));

    mGradientEditor = new WidgetGradientEditor(this);
    mGradientEditor->slotReset(WidgetGradientEditor::PresetJet);
    ui->centralWidget->layout()->addWidget(mGradientEditor);
    mSliderFloatingGradientRange->slotSetColorMap(mGradientEditor->getGradient());
    connect(mGradientEditor, &WidgetGradientEditor::gradientChanged, mSliderFloatingGradientRange, &FloatingGradientRangeSlider::slotSetColorMap);

    show();
}

void MainWindow::slotPrintSliders()
{
    QString text;
    text.append(QString("simple range slider\n[%1   %2   %3   %4]").arg(mSliderRange->minimum()).arg(mSliderRange->valueLo()).arg(mSliderRange->valueHi()).arg(mSliderRange->maximum()));
    text.append(QString("\n\nfloating range slider for endless range (but requires mousework :)\n[%1   %2   %3   %4]").arg(mSliderFloatingRange->minimum()).arg(mSliderFloatingRange->valueLo()).arg(mSliderFloatingRange->valueHi()).arg(mSliderFloatingRange->maximum()));
    text.append(QString("\n\nfloating gradient range slider - same as above, but with gradient\n[%1   %2   %3   %4]").arg(mSliderFloatingGradientRange->minimum()).arg(mSliderFloatingGradientRange->valueLo()).arg(mSliderFloatingGradientRange->valueHi()).arg(mSliderFloatingGradientRange->maximum()));
    ui->label->setText(text);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
