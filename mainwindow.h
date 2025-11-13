#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QAction>

#include "catalogue.h"
#include "logindialog.h"
#include "librarycontroller.h"   // <-- added

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent=nullptr);

private slots:
    // Actions
    void borrowItem();
    void returnItem();
    void placeHold();
    void cancelHold();

    // UI events
    void onSelectionChanged();
    void onLogout();

private:
    // UI construction & refresh
    void buildUi();
    void loginFlow();
    void setActiveUser(int uid);

    void refreshAll();
    void refreshItemsTable();
    void refreshDetails();
    void refreshAccountPanels();
    void updateButtons();

    // Helpers for extra columns/labels
    static QString extra1Header(ItemType t);
    static QString extra2Header(ItemType t);
    static QString extra1Value(const Item& it);
    static QString extra2Value(const Item& it);

    // Data
    Catalogue cat_;
    User* active_ = nullptr;

    // Controller (option a: entities remain public)
    LibraryController* lib_ = nullptr;   // <-- added

    // Widgets
    QLabel* banner_ = nullptr;
    QTableWidget* itemsTbl_ = nullptr;
    QPushButton *btnBorrow_ = nullptr, *btnReturn_ = nullptr, *btnHold_ = nullptr, *btnCancelHold_ = nullptr;

    // Details
    QLabel *detTitle_ = nullptr, *detCreator_ = nullptr, *detType_ = nullptr, *detStatus_ = nullptr,
           *detDue_ = nullptr, *detExtra1_ = nullptr, *detExtra2_ = nullptr;

    // Account panels
    QTableWidget* loansTbl_ = nullptr;
    QTableWidget* holdsTbl_ = nullptr;
};

#endif // MAINWINDOW_H
