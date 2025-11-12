#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "catalogue.h"

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(Catalogue* cat, QWidget* parent=nullptr);
    int selectedUserId() const { return selectedId_; }

private slots:
    void onAccept();

private:
    Catalogue* cat_;
    QLineEdit* nameEdit_;
    QLabel*    msg_;
    int selectedId_ = -1;
};

#endif // LOGINDIALOG_H
