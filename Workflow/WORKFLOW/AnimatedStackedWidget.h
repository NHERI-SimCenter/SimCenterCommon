#pragma once
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QTimer>

class AnimatedStackedWidget : public QStackedWidget {
    Q_OBJECT
public:
    explicit AnimatedStackedWidget(QWidget* parent=nullptr)
        : QStackedWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }

    void setDuration(int ms) { durationMs = ms; }
    void setEasing(const QEasingCurve& c) { easing = c; }

public slots:
    void setCurrentIndexAnimated(int targetIndex, int directionHint = 0) {
        // directionHint: -1 = slide top->bottom, +1 = bottom->top, 0 = auto by index
        if (animating) return;
        if (targetIndex == currentIndex()) return;
        if (targetIndex < 0 || targetIndex >= count()) return;

        QWidget* curr = currentWidget();
        QWidget* next = widget(targetIndex);
        if (!curr || !next) { setCurrentIndex(targetIndex); return; }

        animating = true;

        // Ensure both pages fill the same rect
        const QRect area = rect();
        const int w = area.width();
        const int h = area.height();

        // Decide direction (default: increasing index slides left)
        int dir = directionHint;
        if (dir == 0) dir = (targetIndex > currentIndex()) ? -1 : +1; // -1 => next enters from right

        // Geometry setup
        QPoint offNext = QPoint(0, -dir * w);       // if dir = +1, next starts at +w (from top), if -1, starts at -w (bottom)
        QPoint offCurr = QPoint(0, dir * w);      // current leaves opposite way

        next->setGeometry(QRect(offNext, QSize(w, h)));
        next->show();
        next->raise();

        // Disable interactions during animation
        // curr->setEnabled(false);
        // next->setEnabled(false);

        // Animations
        auto* animOut = new QPropertyAnimation(curr, "pos");
        animOut->setDuration(durationMs);
        animOut->setEasingCurve(easing);
        animOut->setStartValue(QPoint(0,0));
        animOut->setEndValue(offCurr);

        auto* animIn = new QPropertyAnimation(next, "pos");
        animIn->setDuration(durationMs);
        animIn->setEasingCurve(easing);
        animIn->setStartValue(offNext);
        animIn->setEndValue(QPoint(0,0));

        auto* group = new QParallelAnimationGroup(this);
        group->addAnimation(animOut);
        group->addAnimation(animIn);

        // Clean up and finalize
        connect(group, &QParallelAnimationGroup::finished, this, [=](){
            setCurrentIndex(targetIndex);
            // Reset geometry so layout reflows correctly
            curr->move(0,0);
            next->setGeometry(area);

            curr->setEnabled(true);
            next->setEnabled(true);

            group->deleteLater();
            animating = false;
        });

        group->start(QAbstractAnimation::DeleteWhenStopped);
    }

protected:
    void resizeEvent(QResizeEvent* e) override {
        // Keep current page sized to the widget; prevents visual gaps on resize
        if (QWidget* cw = currentWidget())
            cw->setGeometry(rect());
        QStackedWidget::resizeEvent(e);
    }

private:
    int durationMs = 180;                     // snappy but smooth
    QEasingCurve easing = QEasingCurve::OutCubic;
    bool animating = false;
};
