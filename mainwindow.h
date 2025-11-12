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

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent=nullptr);
    ~MainWindow() override = default;

private slots:
    void borrowItem();
    void returnItem();
    void placeHold();
    void cancelHold();
    void onSelectionChanged();
    void onLogout();

private:
    void buildUi();
    void loginFlow();  // show login, set active_
    void setActiveUser(int uid);
    void refreshAll();
    void refreshItemsTable();
    void refreshDetails();
    void refreshAccountPanels();
    void updateButtons();

    static QString extra1Header(ItemType t);
    static QString extra2Header(ItemType t);
    static QString extra1Value(const Item& it);
    static QString extra2Value(const Item& it);

    // Data
    Catalogue cat_;
    User* active_ = nullptr;

    // Widgets
    QLabel* banner_ = nullptr;
    QTableWidget* itemsTbl_ = nullptr;
    QPushButton *btnBorrow_=nullptr, *btnReturn_=nullptr, *btnHold_=nullptr, *btnCancelHold_=nullptr;

    // Details
    QLabel *detTitle_=nullptr, *detCreator_=nullptr, *detType_=nullptr, *detStatus_=nullptr, *detDue_=nullptr, *detExtra1_=nullptr, *detExtra2_=nullptr;

    // Account panels
    QTableWidget* loansTbl_=nullptr;
    QTableWidget* holdsTbl_=nullptr;
};

#endif // MAINWINDOW_H
