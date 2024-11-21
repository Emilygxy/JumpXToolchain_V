#pragma once

#include <QDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class TextEditDialog : public QDialog {
    Q_OBJECT

public:
    TextEditDialog(QWidget* parent = nullptr) : QDialog(parent) {
        mPlainTextEdit = new QPlainTextEdit(this);
        mPlainTextEdit->setPlaceholderText("Enter text here...");

        QPushButton* okButton = new QPushButton("OK", this);
        QPushButton* cancelButton = new QPushButton("Cancel", this);

        connect(okButton, &QPushButton::clicked, this, &TextEditDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &TextEditDialog::reject);

        QLabel* labelHint;
        labelHint = new QLabel();
        labelHint->setText("Format: \nframe;R;G;B;A \n25;255;255;255;255");
        labelHint->setMaximumSize(QSize(512, 512));
        labelHint->setAlignment(Qt::AlignLeft);

        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(labelHint);
        mainLayout->addWidget(mPlainTextEdit);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);
        setWindowTitle("Text Editor Dialog");
    }

    ~TextEditDialog() {
        delete mPlainTextEdit;
        mPlainTextEdit = nullptr;
    }
    
    QString getText() const {
        return mPlainTextEdit->toPlainText();
    }

    void setCacheText(const QString& placeholderText)
    {
        mPlainTextEdit->setPlaceholderText(placeholderText);
    }

private:
    QPlainTextEdit* mPlainTextEdit{ nullptr };
};
