#include "WaveBackgroundWidget.h"

WaveBackgroundWidget::WaveBackgroundWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAutoFillBackground(false);

    waves = {
        { QColor( 20, 100, 200,  60),  22.0, 380.0,  2.0, 0.0 },
        { QColor( 30, 120, 220,  80),  16.0, 260.0, 3.0, 1.6 },
        { QColor( 10,  80, 180,  45),  28.0, 540.0,  1.0, 3.1 }
    };

    elapsed.start();

    timer = new QTimer(this);
    timer->setInterval(16);
    connect(timer, &QTimer::timeout, this, &WaveBackgroundWidget::onTick);
    timer->start();
}

WaveBackgroundWidget::~WaveBackgroundWidget() {
    if (timer) {
        timer->stop();      // belt-and-suspenders
    }
}

void WaveBackgroundWidget::onTick() {
    const double dt = elapsed.restart() / 1000.0;
    for (auto& w : waves) w.phase += w.speed * dt;
    update();
}

void WaveBackgroundWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setClipRect(rect());

    const int W = width();
    const int H = height();
    if (W <= 0 || H <= 0) return;

    p.fillRect(rect(), QColor(240, 240, 240, 255));

    for (const auto& w : waves) {
        QPainterPath path;
        const double yMid = H * 0.65;
        const double amp  = w.amplitude;
        const double k    = (2.0 * M_PI) / w.wavelength;

        path.moveTo(0, yMid + amp * std::sin(k * 0 + w.phase));
        const int step = 6;
        for (int x = step; x <= W; x += step) {
            const double y = yMid + amp * std::sin(k * x + w.phase);
            path.lineTo(x, y);
        }

        QPainterPath fillPath = path;
        fillPath.lineTo(W, H);
        fillPath.lineTo(0, H);
        fillPath.closeSubpath();

        QColor fillC = w.color;
        fillC.setAlpha(std::max(20, w.color.alpha() - 40));
        p.fillPath(fillPath, fillC);

        QPen pen(w.color, 2.0);
        p.setPen(pen);
        p.drawPath(path);
    }
}