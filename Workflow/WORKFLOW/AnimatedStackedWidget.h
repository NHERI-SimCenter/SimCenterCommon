#pragma once
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QPointer>
#include <QScreen>
#include <QApplication>
#include <QPainter>

class AnimatedStackedWidget : public QStackedWidget {
    Q_OBJECT
public:
    enum SlideAxis { Horizontal, Vertical };

    explicit AnimatedStackedWidget(QWidget* parent=nullptr)
        : QStackedWidget(parent) {}

    void setDuration(int ms)            { durationMs = ms; }
    void setEasing(const QEasingCurve& c){ easing = c; }
    void setAxis(SlideAxis a)           { axis = a; }

public slots:
    void setCurrentIndexAnimated(int targetIndex, int directionHint = 0) {
        if (animating) return;
        if (targetIndex == currentIndex()) return;
        if (targetIndex < 0 || targetIndex >= count()) return;

        QWidget* curr = currentWidget();
        QWidget* next = widget(targetIndex);
        if (!curr || !next) { setCurrentIndex(targetIndex); return; }

        animating = true;

        const QRect area = rect();
        const int W = area.width();
        const int H = area.height();

        // +1 => next enters from right (Horizontal) / bottom (Vertical)
        // -1 => next enters from left  (Horizontal) / top   (Vertical)
        int dir = directionHint;
        if (dir == 0) dir = (targetIndex > currentIndex()) ? -1 : +1;

        auto offset = [&](int s){ return (axis == Horizontal) ? QPoint(s, 0) : QPoint(0, s); };
        const int delta = (axis == Horizontal) ? W : H;

        // Make sure 'next' is laid out for a correct snapshot
        next->setGeometry(area);
        next->ensurePolished();

        // HiDPI-aware snapshots
        const qreal dpr = qApp->primaryScreen()->devicePixelRatio();
        // HiDPI-aware snapshots that include native children (Qt3D window container)
        auto grabPage = [&](QWidget* page)->QPixmap {
            // Let any pending 3D frame present before we grab
            qApp->processEvents(QEventLoop::AllEvents, 1);

            // QWidget::grab() captures the widget as seen on screen, including native children
            QPixmap pm = page->grab(); // devicePixelRatio handled by Qt

            // Ensure the pixmap matches our stacked area size
            const QSize logical = pm.size() / pm.devicePixelRatio();
            if (logical != area.size())
                pm = pm.copy(QRect(QPoint(0,0), area.size())); // keep DPR metadata

            return pm;
        };
        QPixmap snapCurr = grabPage(curr);
        QPixmap snapNext = grabPage(next);

        // Overlay that fully covers this stacked widget
        QWidget* overlay = new QWidget(this);
        overlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        overlay->setAttribute(Qt::WA_NoSystemBackground, true);
        overlay->setGeometry(area);
        overlay->show();
        overlay->raise();

        // Simple pixmap painters
        struct Pix : QWidget {
            QPixmap pm;
            explicit Pix(QWidget* p=nullptr): QWidget(p) {
                setAttribute(Qt::WA_TransparentForMouseEvents, true);
            }
            void paintEvent(QPaintEvent*) override { QPainter(this).drawPixmap(0, 0, pm); }
        };

        auto* currPix = new Pix(overlay); currPix->pm = snapCurr;
        currPix->setGeometry(area);
        currPix->show();                     // IMPORTANT

        auto* nextPix = new Pix(overlay); nextPix->pm = snapNext;
        nextPix->setGeometry(QRect(area.topLeft() + offset(-dir * delta), area.size()));
        nextPix->show();                     // IMPORTANT

        // Hide real pages to avoid any double painting underneath
        curr->setVisible(false);
        next->setVisible(false);

        // Animate the OVERLAY widgets (not the real pages)
        auto* animOut = new QPropertyAnimation(currPix, "geometry");
        animOut->setDuration(durationMs);
        animOut->setEasingCurve(easing);
        animOut->setStartValue(currPix->geometry());
        animOut->setEndValue(QRect(area.topLeft() + offset(dir * delta), area.size()));

        auto* animIn  = new QPropertyAnimation(nextPix, "geometry");
        animIn->setDuration(durationMs);
        animIn->setEasingCurve(easing);
        animIn->setStartValue(nextPix->geometry());
        animIn->setEndValue(area);

        auto* group = new QParallelAnimationGroup(this);
        group->addAnimation(animOut);
        group->addAnimation(animIn);

        QPointer<QWidget> nextGuard = next;

        connect(group, &QParallelAnimationGroup::finished, this, [=]() {
            setCurrentIndex(targetIndex);
            if (nextGuard) {
                nextGuard->setGeometry(area);
                nextGuard->setVisible(true);
            }
            overlay->deleteLater();          // removes currPix/nextPix too
            animating = false;
        });

        group->start(QAbstractAnimation::DeleteWhenStopped);
    }

protected:
    void resizeEvent(QResizeEvent* e) override {
        if (QWidget* cw = currentWidget())
            cw->setGeometry(rect());
        QStackedWidget::resizeEvent(e);
    }

private:
    int durationMs = 220;
    QEasingCurve easing = QEasingCurve::OutCubic;
    SlideAxis axis = Vertical;
    bool animating = false;
};