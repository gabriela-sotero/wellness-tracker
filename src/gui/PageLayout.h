#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

// Starts a page with its title and returns the layout, so each page only has
// to add the widgets that come below the title.
inline QVBoxLayout* startPage(
    QWidget* page,
    const QString& title,
    int maxWidth = 480
) {
    // Each page keeps its own bounded column inside the stacked page widget.
    // Outer stretches center that column without stretching its internal groups.
    page->setObjectName("page");

    auto* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addStretch();

    auto* centered = new QHBoxLayout;
    centered->setContentsMargins(0, 0, 0, 0);
    centered->addStretch();

    auto* column = new QWidget(page);
    column->setMaximumWidth(maxWidth);
    column->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    centered->addWidget(column);
    centered->addStretch();
    pageLayout->addLayout(centered);
    pageLayout->addStretch();

    auto* layout = new QVBoxLayout(column);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(10);

    auto* label = new QLabel(title, column);
    label->setObjectName("title");
    label->setWordWrap(true);
    layout->addWidget(label);

    return layout;
}

// Adds a labelled text field and returns it.
inline QLineEdit* addField(
    QVBoxLayout* layout,
    const QString& label,
    bool password = false
) {
    // The returned input is also stored by the page for validation and clearing.
    QWidget* page = layout->parentWidget();
    layout->addWidget(new QLabel(label, page));

    auto* field = new QLineEdit(page);
    field->setAccessibleName(label);
    if (password) {
        field->setEchoMode(QLineEdit::Password);
    }
    layout->addWidget(field);

    return field;
}

// Adds the red label each page uses to report errors.
inline QLabel* addFeedback(QVBoxLayout* layout) {
    // Reuse a named label so stylesheets can format feedback consistently.
    auto* feedback = new QLabel(layout->parentWidget());
    feedback->setObjectName("feedback");
    feedback->setWordWrap(true);
    layout->addWidget(feedback);
    return feedback;
}

// A white panel that groups one section of a page. Returning its inner layout
// lets callers add content without managing the card widget itself.
inline QVBoxLayout* addCard(QVBoxLayout* layout, const QString& title) {
    // Return the card's inner layout so callers can add content without exposing
    // the card widget's construction details.
    auto* card = new QWidget(layout->parentWidget());
    card->setObjectName("card");
    // Without this a plain QWidget ignores the stylesheet background.
    card->setAttribute(Qt::WA_StyledBackground, true);

    auto* inner = new QVBoxLayout(card);
    inner->setContentsMargins(16, 13, 16, 13);
    inner->setSpacing(5);

    if (!title.isEmpty()) {
        auto* label = new QLabel(title, card);
        label->setObjectName("cardTitle");
        inner->addWidget(label);
    }

    layout->addWidget(card);
    return inner;
}

// A name on the left and its value on the right. Returns the value label, which
// the page fills in when it has data.
inline QLabel* addCardRow(QVBoxLayout* card, const QString& name) {
    // Keep the caption and value aligned in one reusable horizontal row.
    QWidget* parent = card->parentWidget();

    auto* label = new QLabel(name, parent);
    label->setObjectName("muted");
    auto* value = new QLabel(parent);
    value->setObjectName("value");

    auto* row = new QHBoxLayout;
    row->setContentsMargins(0, 0, 0, 0);
    row->addWidget(label);
    row->addStretch();
    row->addWidget(value);
    card->addLayout(row);

    return value;
}
