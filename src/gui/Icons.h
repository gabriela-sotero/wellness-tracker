#pragma once

#include <QBuffer>
#include <QByteArray>
#include <QColor>
#include <QImageReader>
#include <QPixmap>
#include <QSize>
#include <QString>

// Icons are embedded SVG snippets rendered through the same image-format plugin
// the badge artwork already uses, so no extra Qt module or asset file is needed.
// "currentColor" inside a snippet is replaced by the color passed to svgPixmap.
namespace icons {

inline constexpr const char* home = R"(<g fill="none" stroke="currentColor" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><path d="M3.5 11.2 12 4l8.5 7.2"/><path d="M5.8 9.6V19a1.2 1.2 0 0 0 1.2 1.2h3.2v-5.4h3.6v5.4H17a1.2 1.2 0 0 0 1.2-1.2V9.6"/></g>)";

inline constexpr const char* log = R"(<g fill="none" stroke="currentColor" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><rect x="5" y="4.5" width="14" height="16.5" rx="3"/><path d="M9.2 4.5V3.6h5.6v.9"/><path d="m9 13.2 2.3 2.3 4-4.4"/></g>)";

inline constexpr const char* profile = R"(<g fill="none" stroke="currentColor" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="8" r="4"/><path d="M4.6 20.4c.9-3.9 4-5.9 7.4-5.9s6.5 2 7.4 5.9"/></g>)";

inline constexpr const char* logout = R"(<g fill="none" stroke="currentColor" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><path d="M9.5 4H7a3 3 0 0 0-3 3v10a3 3 0 0 0 3 3h2.5"/><path d="M15 8l4 4-4 4"/><path d="M19 12H9.5"/></g>)";

inline constexpr const char* lock = R"(<rect x="5" y="10.5" width="14" height="10.5" rx="3" fill="currentColor"/><path d="M8.5 10.5V8a3.5 3.5 0 0 1 7 0v2.5" fill="none" stroke="currentColor" stroke-width="2.6" stroke-linecap="round"/>)";

inline constexpr const char* star = R"(<path d="M12 2.8l2.8 5.9 6.4.8-4.7 4.4 1.2 6.4L12 17.2l-5.7 3.1 1.2-6.4-4.7-4.4 6.4-.8z" fill="currentColor" stroke="currentColor" stroke-width="1.2" stroke-linejoin="round"/>)";

inline constexpr const char* check = R"(<path d="m5 12.5 4.5 4.5L19 7.5" fill="none" stroke="currentColor" stroke-width="3.4" stroke-linecap="round" stroke-linejoin="round"/>)";

inline constexpr const char* flame = R"(<path d="M12 2c.5 3.2 4.5 5.2 4.5 10a4.5 4.5 0 0 1-9 0c0-2 1-3.2 2-4.2.2 1.4.9 2.2 1.7 2.7C11 8 10.5 5 12 2z" fill="currentColor"/>)";

inline constexpr const char* bolt = R"(<path d="M13.5 2 5 13.5h6L9.5 22 19 9.8h-6.2z" fill="currentColor" stroke="currentColor" stroke-width="1" stroke-linejoin="round"/>)";
inline constexpr const char* drop = R"(<path d="M12 3c3 4.2 6 7 6 11a6 6 0 0 1-12 0c0-4 3-6.8 6-11z" fill="currentColor"/>)";
inline constexpr const char* meal = R"(<path d="M3 11h18a9 9 0 0 1-18 0z" fill="currentColor"/><path d="M8 7.5c0-1.2 1-1.8 1-3M12 7.5c0-1.2 1-1.8 1-3M16 7.5c0-1.2 1-1.8 1-3" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>)";
inline constexpr const char* dumbbell = R"(<g fill="none" stroke="currentColor" stroke-width="2.6" stroke-linecap="round" stroke-linejoin="round"><path d="M7 7v10M17 7v10M4 9.5v5M20 9.5v5M7 12h10"/></g>)";
inline constexpr const char* moon = R"(<path d="M21 12.8A9 9 0 1 1 11.2 3a7 7 0 0 0 9.8 9.8z" fill="currentColor"/>)";

// Full-color mark for the welcome page (drawn on a 96x100 canvas).
inline constexpr const char* emblem = R"(<circle cx="48" cy="56" r="40" fill="#005c4d"/><circle cx="48" cy="48" r="40" fill="#007c68"/><path d="M48 22c8 11 17 18 17 29a17 17 0 0 1-34 0c0-11 9-18 17-29z" fill="#ffffff"/><path d="M41 53a7 7 0 0 0 6 7" fill="none" stroke="#007c68" stroke-width="3.2" stroke-linecap="round"/>)";

// Renders a snippet at the given logical size, sharp on high-DPI screens.
inline QPixmap svgPixmap(
    const QString& body,
    const QColor& color,
    int width,
    int height,
    const QString& viewBox = QString("0 0 24 24")
) {
    constexpr qreal ratio = 2.0;

    QString colored = body;
    colored.replace("currentColor", color.name());
    const QString svg = QString(
        "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%1\">%2</svg>"
    ).arg(viewBox, colored);

    QByteArray data = svg.toUtf8();
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer, "svg");
    reader.setScaledSize(QSize(
        static_cast<int>(width * ratio), static_cast<int>(height * ratio)
    ));
    QPixmap pixmap = QPixmap::fromImage(reader.read());
    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}

// Square convenience overload for the 24x24 icon set.
inline QPixmap svgPixmap(const QString& body, const QColor& color, int size) {
    return svgPixmap(body, color, size, size);
}

}  // namespace icons
