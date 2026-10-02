#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

// Starts a page with its title and returns the layout, so each page only has
// to add the widgets that come below the title.
inline QVBoxLayout* startPage(QWidget* page, const QString& title) {
    page->setObjectName("page");

    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(10);

    auto* label = new QLabel(title, page);
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
    auto* feedback = new QLabel(layout->parentWidget());
    feedback->setObjectName("feedback");
    feedback->setWordWrap(true);
    layout->addWidget(feedback);
    return feedback;
}

// A white panel that groups one section of a page. Returns the layout to fill
// with the section's content.
inline QVBoxLayout* addCard(QVBoxLayout* layout, const QString& title) {
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
