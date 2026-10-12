#pragma once
#include <QWidget>
#include <QVector>
#include <QColor>
#include <QElapsedTimer>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

class WaveBackgroundWidget : public QWidget {
    Q_OBJECT
public:
    explicit WaveBackgroundWidget(QWidget* parent = nullptr);
    ~WaveBackgroundWidget() override;

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void onTick();

private:
    struct Wave {
        QColor color;
        double amplitude;
        double wavelength;
        double speed;   // radians/sec
        double phase;
    };

    QVector<Wave> waves;
    QElapsedTimer elapsed;
    QTimer* timer {nullptr};  // child-owned; safer during teardown
};