#include "logindialog.h"
#include <QHBoxLayout>

LoginDialog::LoginDialog(Catalogue* cat, QWidget* parent)
    : QDialog(parent), cat_(cat)
{
    setWindowTitle("HinLIBS — Sign in");
    auto* layout = new QVBoxLayout(this);
    auto* label  = new QLabel("Enter your name (e.g., Alice, Bob, Carmen, Diego, Eva, Liam, Sara):", this);
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setPlaceholderText("Name");
    msg_ = new QLabel(this);
    msg_->setStyleSheet("color:#a00;");

    auto* btns = new QHBoxLayout();
    auto* ok   = new QPushButton("Continue", this);
    auto* cancel = new QPushButton("Cancel", this);
    btns->addStretch();
    btns->addWidget(ok);
    btns->addWidget(cancel);

    layout->addWidget(label);
    layout->addWidget(nameEdit_);
    layout->addWidget(msg_);
    layout->addLayout(btns);

    connect(ok, &QPushButton::clicked, this, &LoginDialog::onAccept);
    connect(cancel, &QPushButton::clicked, this, &LoginDialog::reject);
    nameEdit_->setFocus();
}

void LoginDialog::onAccept() {
    User u = cat_->getUserByName(nameEdit_->text());
if (u.id == 0) {  // ID=0 means user not found in your DB-backed design
    msg_->setText("User not found. Try one of the seeded names.");
    return;
}
selectedId_ = u.id;
    accept();
}
