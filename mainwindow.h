#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>   // std::unique_ptr

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class catalogue;    // forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_searchEdit_textChanged(const QString &text); // QLineEdit::textChanged(const QString &)
    void on_typeFilter_currentIndexChanged(int index);   // QComboBox::currentIndexChanged(int)
    void on_statusFilter_currentIndexChanged(int index); // QComboBox::currentIndexChanged(int)
    void on_clearFilterBtn_clicked();                    // QPushButton::clicked()
    void on_actionAbout_triggered();   // QAction::triggered() auto-connect

    void on_tableItems_itemSelectionChanged();   // when selection changes
      void on_borrowBtn_clicked();
      void on_returnBtn_clicked();
      void on_placeHoldBtn_clicked();   // optional, can be stub for now
      void on_cancelHoldBtn_clicked();  // optional, can be stub for now


private:
    Ui::MainWindow *ui;
    std::unique_ptr<catalogue> data_;  // in-memory data model

    void populateItemsTable();   // <— declare it here

    void applyFilters(); // Rebuild table based on search & filters

    void on_actiontrrigered();

    int currentSelectedId() const;                 // -1 if nothing selected
    void updateButtons();                          // enable/disable buttons
    void refreshTableKeepingSelection(int keepId); // repaint and reselect
};

#endif // MAINWINDOW_H
