#pragma once
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QPointer>
#include <QTimer>

class AnimatedStackedWidget : public QStackedWidget {
    Q_OBJECT
public:
    enum SlideAxis { Horizontal, Vertical };

    explicit AnimatedStackedWidget(QWidget* parent=nullptr)
        : QStackedWidget(parent) {}

    void setDuration(int ms) { durationMs = ms; }
    void setEasing(const QEasingCurve& c) { easing = c; }
    void setAxis(SlideAxis a) { axis = a; }

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
        const int w = area.width();
        const int h = area.height();

        // dir: +1 = next enters from right/bottom, -1 = from left/top (for Horizontal/Vertical)
        int dir = -directionHint;
        if (dir == 0) dir = (targetIndex > currentIndex()) ? +1 : -1;

        // Compute offsets by axis
        auto offsetPoint = [&](int s)->QPoint {
            return (axis == Horizontal) ? QPoint(s, 0) : QPoint(0, s);
        };
        const int delta = (axis == Horizontal) ? w : h;

        // Prepare pages
        curr->setGeometry(area);
        next->setGeometry(QRect(area.topLeft() + offsetPoint(dir * delta), area.size()));
        next->show();
        curr->raise();
        next->raise();

        // Use GEOMETRY animations (avoids layout fighting and jitter)
        auto* animOut = new QPropertyAnimation(curr, "geometry");
        animOut->setDuration(durationMs);
        animOut->setEasingCurve(easing);
        animOut->setStartValue(area);
        animOut->setEndValue(QRect(area.topLeft() + offsetPoint(-dir * delta), area.size())); // curr slides out opposite

        auto* animIn = new QPropertyAnimation(next, "geometry");
        animIn->setDuration(durationMs);
        animIn->setEasingCurve(easing);
        animIn->setStartValue(QRect(area.topLeft() + offsetPoint(dir * delta), area.size()));
        animIn->setEndValue(area);

        auto* group = new QParallelAnimationGroup(this);
        group->addAnimation(animOut);
        group->addAnimation(animIn);

        // Guard against accidental deletion mid-anim
        QPointer<QWidget> currGuard = curr;
        QPointer<QWidget> nextGuard = next;

        connect(group, &QParallelAnimationGroup::finished, this, [=]() {
            if (nextGuard) nextGuard->setGeometry(area);
            setCurrentIndex(targetIndex);               // commit the page switch
            if (currGuard) currGuard->move(0, 0);       // reset stray pos
            group->deleteLater();
            animating = false;
        });

        group->start(QAbstractAnimation::DeleteWhenStopped);
    }

protected:
    void resizeEvent(QResizeEvent* e) override {
        if (QWidget* cw = currentWidget())
            cw->setGeometry(rect()); // keep current page fitted
        QStackedWidget::resizeEvent(e);
    }

private:
    int durationMs = 220;
    QEasingCurve easing = QEasingCurve::OutCubic;
    SlideAxis axis = Vertical; // set to Vertical for up/down slides
    bool animating = false;
};