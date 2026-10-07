#pragma once

#include <QDate>
#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QHideEvent>
#include <QLocale>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QShowEvent>
#include <QToolTip>
#include <QVariantAnimation>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <vector>

#include "Icons.h"

// The home screen's day path, in chronological order from top to bottom like
// Duolingo's: finished days sit above, today is the highlighted node, and the
// days still to come are locked below. Scrolling up walks back through the
// history (down to the day the account was created); scrolling down reveals
// more locked days. Painting is done by hand because the path can reach
// hundreds of nodes, and drawing only the visible ones is far lighter than a
// widget per node.
class PathView : public QWidget {
public:
    // Called when the user taps today's node or a past day's node.
    std::function<void(const QDate&)> onDayClicked;
    // Supplies how many of the four daily goals were met on a past date, or -1
    // when nothing is known for it. Answers are cached per day.
    std::function<int(const QDate&)> goalsMetOn;

    explicit PathView(QWidget* parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);

        lockPixmap = icons::svgPixmap(icons::lock, QColor("#a3b3ae"), 28);
        starPixmap = icons::svgPixmap(icons::star, QColor("#ffffff"), 34);
        checkPixmap = icons::svgPixmap(icons::check, QColor("#ffffff"), 34);

        // Gentle loop that makes the speech bubble above today's node bob.
        bobAnimation = new QVariantAnimation(this);
        bobAnimation->setStartValue(0.0);
        bobAnimation->setEndValue(1.0);
        bobAnimation->setDuration(1400);
        bobAnimation->setLoopCount(-1);
        QObject::connect(bobAnimation, &QVariantAnimation::valueChanged, this, [this] {
            const int top = ys[pastDays] - NodeRadius - 110;
            update(0, top, width(), 110);
        });

        rebuild();
    }

    // Starts over centered on today. Nothing before `earliest` (the day the
    // account was created) is ever shown; an invalid date means no history.
    void reset(const QDate& earliest) {
        baseDate = QDate::currentDate();
        const qint64 available = earliest.isValid() ? earliest.daysTo(baseDate) : 0;
        maxPast = static_cast<int>(std::clamp<qint64>(available, 0, 3650));
        pastDays = std::min(static_cast<int>(InitialPast), maxPast);
        futureDays = InitialFuture;
        goalsCache.clear();
        rebuild();
    }

    // More history above the days already shown.
    void extendPast(int moreDays) {
        pastDays = std::min(pastDays + moreDays, maxPast);
        rebuild();
    }

    // More locked days below the ones already shown.
    void extendFuture(int moreDays) {
        futureDays = std::min(futureDays + moreDays, static_cast<int>(MaxFuture));
        rebuild();
    }

    bool canExtendPast() const {
        return pastDays < maxPast;
    }

    bool canExtendFuture() const {
        return futureDays < MaxFuture;
    }

    // Vertical position of today's node inside the widget, from the top.
    int todayY() const {
        return ys[pastDays];
    }

    // Goals met today (0 to 4) drive the progress ring around today's node.
    void setTodayGoals(int goalsMet) {
        todayGoals = std::clamp(goalsMet, 0, 4);
        update();
    }

    QSize sizeHint() const override {
        return QSize(400, contentHeight);
    }

protected:
    void showEvent(QShowEvent* event) override {
        QWidget::showEvent(event);
        bobAnimation->start();
    }

    void hideEvent(QHideEvent* event) override {
        QWidget::hideEvent(event);
        bobAnimation->stop();
    }

    void leaveEvent(QEvent* event) override {
        QWidget::leaveEvent(event);
        setHovered(-1);
        unsetCursor();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        const int node = nodeAt(localPosition(event));
        setHovered(node);
        // Today and past days open their summary; locked days only explain themselves.
        if (node >= 0 && node <= pastDays) {
            setCursor(Qt::PointingHandCursor);
        } else {
            unsetCursor();
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() != Qt::LeftButton) {
            return;
        }
        const int node = nodeAt(localPosition(event));
        if (node < 0) {
            return;
        }
        if (node <= pastDays) {
            if (onDayClicked) {
                onDayClicked(dateAt(node));
            }
        } else {
            QToolTip::showText(
                mapToGlobal(localPosition(event).toPoint()),
                "Unlocks on " + english.toString(dateAt(node), "dddd, MMM d"),
                this
            );
        }
    }

    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
        painter.fillRect(event->rect(), QColor("#ffffff"));

        // Only nodes near the repainted area are drawn.
        const QRect visible = event->rect().adjusted(0, -160, 0, 160);

        paintRoad(painter, visible);
        paintWeekHeaders(painter, visible);

        const int total = static_cast<int>(ys.size());
        for (int i = 0; i < total; ++i) {
            const QPointF c = center(i);
            if (c.y() < visible.top() || c.y() > visible.bottom()) {
                continue;
            }
            if (i == pastDays) {
                paintToday(painter, c);
            } else if (i < pastDays) {
                paintPast(painter, c, goalsFor(i));
            } else {
                paintLocked(painter, c);
            }
            paintDayLabel(painter, c, i);
        }

        const QPointF todayCenter = center(pastDays);
        if (todayCenter.y() >= visible.top() && todayCenter.y() <= visible.bottom()) {
            paintBubble(painter, todayCenter);
        }
    }

private:
    // Sizes are fixed; the enum keeps them usable as plain ints in expressions.
    enum : int {
        NodeRadius = 36,
        RingGap = 12,
        Step = 104,
        HeaderGap = 58,
        TopPad = 130,
        BottomPad = 120,
        InitialPast = 14,
        InitialFuture = 21,
        MaxFuture = 366
    };

    QLocale english{QLocale::English};
    QDate baseDate = QDate::currentDate();
    // Days shown before today, days shown after it, and how far back history may go.
    int pastDays = 0;
    int futureDays = InitialFuture;
    int maxPast = 0;
    int todayGoals = 0;
    int hovered = -1;
    int contentHeight = 0;
    // Center y of each node, top to bottom. Index pastDays is today.
    std::vector<int> ys;
    // Goals met per day offset from today (negative for past days).
    std::map<int, int> goalsCache;

    QVariantAnimation* bobAnimation = nullptr;
    QPixmap lockPixmap;
    QPixmap starPixmap;
    QPixmap checkPixmap;

    static QPointF localPosition(const QMouseEvent* event) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        return event->position();
#else
        return event->localPos();
#endif
    }

    QDate dateAt(int index) const {
        return baseDate.addDays(index - pastDays);
    }

    void rebuild() {
        const int total = pastDays + 1 + futureDays;
        ys.assign(total, 0);
        int y = TopPad;
        for (int i = 0; i < total; ++i) {
            if (i > 0) {
                y += Step;
                // A new week gets extra room for its header pill.
                if (dateAt(i).dayOfWeek() == Qt::Monday) {
                    y += HeaderGap;
                }
            }
            ys[i] = y;
        }
        contentHeight = y + BottomPad;
        setMinimumHeight(contentHeight);
        updateGeometry();
        update();
    }

    // The zigzag is keyed to the offset from today, so today is always at the
    // center and adding days at either end never reshapes the nodes already there.
    QPointF center(int index) const {
        const qreal amplitude = std::min<qreal>(84.0, width() * 0.2);
        return QPointF(
            width() / 2.0 + amplitude * std::sin((index - pastDays) * 0.9),
            ys[index]
        );
    }

    int nodeAt(const QPointF& position) const {
        const int total = static_cast<int>(ys.size());
        for (int i = 0; i < total; ++i) {
            const QPointF c = center(i);
            const qreal reach = NodeRadius + (i == pastDays ? RingGap : 0) + 4;
            if (std::hypot(position.x() - c.x(), position.y() - c.y()) <= reach) {
                return i;
            }
        }
        return -1;
    }

    void setHovered(int node) {
        if (hovered != node) {
            hovered = node;
            update();
        }
    }

    int goalsFor(int index) {
        const int offset = index - pastDays;
        const auto cached = goalsCache.find(offset);
        if (cached != goalsCache.end()) {
            return cached->second;
        }
        const int goals = goalsMetOn ? goalsMetOn(dateAt(index)) : -1;
        goalsCache[offset] = goals;
        return goals;
    }

    void paintRoad(QPainter& painter, const QRect& visible) {
        painter.setPen(QPen(QColor("#e9f0ed"), 14, Qt::SolidLine, Qt::RoundCap));
        painter.setBrush(Qt::NoBrush);
        const int total = static_cast<int>(ys.size());
        for (int i = 0; i + 1 < total; ++i) {
            const QPointF from = center(i);
            const QPointF to = center(i + 1);
            if (to.y() < visible.top() || from.y() > visible.bottom()) {
                continue;
            }
            const qreal middle = (from.y() + to.y()) / 2.0;
            QPainterPath curve(from);
            curve.cubicTo(QPointF(from.x(), middle), QPointF(to.x(), middle), to);
            painter.drawPath(curve);
        }
    }

    void paintWeekHeaders(QPainter& painter, const QRect& visible) {
        QFont headerFont = font();
        headerFont.setPixelSize(12);
        headerFont.setBold(true);
        painter.setFont(headerFont);
        const QFontMetrics metrics(headerFont);

        const int total = static_cast<int>(ys.size());
        for (int i = 1; i < total; ++i) {
            const QDate date = dateAt(i);
            if (date.dayOfWeek() != Qt::Monday) {
                continue;
            }
            const qreal y = (center(i - 1).y() + center(i).y()) / 2.0;
            if (y < visible.top() || y > visible.bottom()) {
                continue;
            }

            const QString text = "WEEK OF " + english.toString(date, "MMM d").toUpper();
            const qreal pillWidth = metrics.horizontalAdvance(text) + 36;
            const QRectF pill(width() / 2.0 - pillWidth / 2.0, y - 15, pillWidth, 30);

            painter.setPen(QPen(QColor("#e3eae8"), 2));
            painter.setBrush(QColor("#ffffff"));
            painter.drawRoundedRect(pill, 15, 15);
            painter.setPen(QColor("#7a8f89"));
            painter.drawText(pill, Qt::AlignCenter, text);
        }
    }

    // Duolingo-style node: a darker disc offset below the face gives it depth.
    void paintDisc(QPainter& painter, const QPointF& c, const QColor& face, const QColor& edge) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(edge);
        painter.drawEllipse(c + QPointF(0, 7), NodeRadius, NodeRadius);
        painter.setBrush(face);
        painter.drawEllipse(c, NodeRadius, NodeRadius);
    }

    void paintLocked(QPainter& painter, const QPointF& c) {
        paintDisc(painter, c, QColor("#e9eeec"), QColor("#cbd6d2"));
        painter.drawPixmap(QPointF(c.x() - 14, c.y() - 14), lockPixmap);
    }

    // A finished day: all four goals is a solid check, anything less shows how
    // far it got, and a day with nothing recorded stays grey.
    void paintPast(QPainter& painter, const QPointF& c, int goals) {
        if (goals >= 4) {
            paintDisc(painter, c, QColor("#007c68"), QColor("#005c4d"));
            painter.drawPixmap(QPointF(c.x() - 17, c.y() - 17), checkPixmap);
            return;
        }

        const bool some = goals > 0;
        paintDisc(painter, c, QColor(some ? "#b9e3da" : "#e9eeec"),
                  QColor(some ? "#8fc7ba" : "#cbd6d2"));

        QFont scoreFont = font();
        scoreFont.setPixelSize(15);
        scoreFont.setBold(true);
        painter.setFont(scoreFont);
        painter.setPen(QColor(some ? "#005c4d" : "#8fa39d"));
        painter.drawText(
            QRectF(c.x() - NodeRadius, c.y() - NodeRadius, NodeRadius * 2, NodeRadius * 2),
            Qt::AlignCenter,
            goals < 0 ? QString("-") : QString("%1/4").arg(goals)
        );
    }

    void paintToday(QPainter& painter, const QPointF& c) {
        const qreal ringRadius = NodeRadius + RingGap;
        const QRectF ring(c.x() - ringRadius, c.y() - ringRadius, ringRadius * 2, ringRadius * 2);

        // Track, then the arc of goals met so far, starting at 12 o'clock.
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor("#e3eae8"), 7, Qt::SolidLine, Qt::RoundCap));
        painter.drawEllipse(ring);
        if (todayGoals > 0) {
            painter.setPen(QPen(QColor("#007c68"), 7, Qt::SolidLine, Qt::RoundCap));
            painter.drawArc(ring, 90 * 16, -static_cast<int>(360.0 * 16.0 * todayGoals / 4.0));
        }

        const bool done = todayGoals >= 4;
        if (done) {
            paintDisc(painter, c, QColor("#ffc800"), QColor("#d9a400"));
        } else if (hovered == pastDays) {
            paintDisc(painter, c, QColor("#0a8c77"), QColor("#005c4d"));
        } else {
            paintDisc(painter, c, QColor("#007c68"), QColor("#005c4d"));
        }
        painter.drawPixmap(QPointF(c.x() - 17, c.y() - 17), done ? checkPixmap : starPixmap);
    }

    void paintDayLabel(QPainter& painter, const QPointF& c, int index) {
        const QDate date = dateAt(index);
        const bool today = index == pastDays;
        const bool past = index < pastDays;
        const bool labelOnRight = c.x() <= width() / 2.0 + 1;
        const qreal margin = NodeRadius + (today ? RingGap : 0) + 18;
        const qreal boxWidth = 110;
        const QRectF top(
            labelOnRight ? c.x() + margin : c.x() - margin - boxWidth, c.y() - 22, boxWidth, 22
        );
        const QRectF bottom = top.translated(0, 22);
        const Qt::Alignment alignment =
            (labelOnRight ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter;

        QFont strong = font();
        strong.setPixelSize(14);
        strong.setBold(true);
        painter.setFont(strong);
        painter.setPen(QColor(today ? "#007c68" : (past ? "#5f817a" : "#8fa39d")));
        painter.drawText(top, alignment, today ? QString("Today") : english.toString(date, "dddd"));

        QFont light = font();
        light.setPixelSize(12);
        painter.setFont(light);
        painter.setPen(QColor(today ? "#5f817a" : "#a3b3ae"));
        painter.drawText(bottom, alignment, english.toString(date, "MMM d"));
    }

    void paintBubble(QPainter& painter, const QPointF& c) {
        const QString text = todayGoals == 0
            ? QString("START")
            : (todayGoals >= 4 ? QString("DONE!") : QString("KEEP GOING"));

        QFont bubbleFont = font();
        bubbleFont.setPixelSize(13);
        bubbleFont.setBold(true);
        painter.setFont(bubbleFont);

        constexpr double fullTurn = 6.283185307179586;
        const qreal lift = 4.0 * std::sin(bobAnimation->currentValue().toDouble() * fullTurn);
        const qreal boxWidth = QFontMetrics(bubbleFont).horizontalAdvance(text) + 36;
        const qreal base = c.y() - (NodeRadius + RingGap) - 14 + lift;
        const QRectF box(c.x() - boxWidth / 2.0, base - 34, boxWidth, 34);

        QPolygonF tip;
        tip << QPointF(c.x() - 9, base - 1) << QPointF(c.x() + 9, base - 1)
            << QPointF(c.x(), base + 9);
        QPainterPath shape;
        shape.addRoundedRect(box, 14, 14);
        QPainterPath pointer;
        pointer.addPolygon(tip);
        pointer.closeSubpath();
        shape = shape.united(pointer);

        painter.setPen(QPen(QColor("#e3eae8"), 2));
        painter.setBrush(QColor("#ffffff"));
        painter.drawPath(shape);
        painter.setPen(QColor("#007c68"));
        painter.drawText(box, Qt::AlignCenter, text);
    }
};
