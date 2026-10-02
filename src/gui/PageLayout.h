#pragma once

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
