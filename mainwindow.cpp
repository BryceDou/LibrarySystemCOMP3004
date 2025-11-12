#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "catalogue.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QTableWidgetItem>
#include <memory>  // make sure this is included here

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateButtons();

    // Create and seed the in-memory dataset
    data_.reset(new catalogue());   // works even without make_unique
    updateButtons();
    data_->seed_default_data();
    updateButtons();

    ui->tableItems->setColumnCount(6);
    ui->tableItems->setHorizontalHeaderLabels({"ID","Title","Creator","Type","Status","Due"});
    ui->tableItems->horizontalHeader()->setStretchLastSection(true);

    // finally fill the table
    populateItemsTable();
}

void MainWindow::populateItemsTable() {
    const auto &items = data_->items;
    ui->tableItems->setRowCount(items.size());
    for (int r = 0; r < items.size(); ++r) {
        const item &it = items[r];
        ui->tableItems->setItem(r, 0, new QTableWidgetItem(QString::number(it.id)));
        ui->tableItems->setItem(r, 1, new QTableWidgetItem(it.title));
        ui->tableItems->setItem(r, 2, new QTableWidgetItem(it.creator));
        ui->tableItems->setItem(r, 3, new QTableWidgetItem(
            it.type == item_type::fiction    ? "Fiction" :
            it.type == item_type::nonfiction ? "Non-Fiction" :
            it.type == item_type::magazine   ? "Magazine" :
            it.type == item_type::movie      ? "Movie" : "Video Game"));
        ui->tableItems->setItem(r, 4, new QTableWidgetItem(
            it.status == availability::available ? "Available" : "Checked out"));
        ui->tableItems->setItem(r, 5, new QTableWidgetItem(
            it.due.isValid() ? it.due.toString("yyyy-MM-dd") : ""));
    }
    ui->tableItems->resizeColumnsToContents();
}

void MainWindow::applyFilters() {
    // Read filters
    const QString q = ui->searchEdit->text().trimmed().toLower();
    const int typeIdx = ui->typeFilter->currentIndex();   // 0=All
    const int statIdx = ui->statusFilter->currentIndex(); // 0=All

    // Rebuild filtered list
    QVector<item> filtered;
    filtered.reserve(data_->items.size());
    for (const auto &it : data_->items) {
        // text match
        bool textOk = q.isEmpty()
            || it.title.toLower().contains(q)
            || it.creator.toLower().contains(q);

        // type match
        bool typeOk = true;
        if (typeIdx > 0) {
            item_type want =
                (typeIdx==1)? item_type::fiction :
                (typeIdx==2)? item_type::nonfiction :
                (typeIdx==3)? item_type::magazine :
                (typeIdx==4)? item_type::movie :
                              item_type::videogame;
            typeOk = (it.type == want);
        }

        // status match
        bool statOk = true;
        if (statIdx > 0) {
            availability want = (statIdx==1)? availability::available
                                            : availability::checked_out;
            statOk = (it.status == want);
        }

        if (textOk && typeOk && statOk) filtered.push_back(it);
    }

    // Paint table
    ui->tableItems->setRowCount(filtered.size());
    for (int r=0; r<filtered.size(); ++r) {
        const item &it = filtered[r];
        ui->tableItems->setItem(r, 0, new QTableWidgetItem(QString::number(it.id)));
        ui->tableItems->setItem(r, 1, new QTableWidgetItem(it.title));
        ui->tableItems->setItem(r, 2, new QTableWidgetItem(it.creator));
        QString typeStr =
            (it.type==item_type::fiction)    ? "Fiction" :
            (it.type==item_type::nonfiction) ? "Non-Fiction" :
            (it.type==item_type::magazine)   ? "Magazine" :
            (it.type==item_type::movie)      ? "Movie" : "Video Game";
        ui->tableItems->setItem(r, 3, new QTableWidgetItem(typeStr));
        ui->tableItems->setItem(r, 4, new QTableWidgetItem(
            (it.status==availability::available) ? "Available" : "Checked out"));
        ui->tableItems->setItem(r, 5, new QTableWidgetItem(
            it.due.isValid()? it.due.toString("yyyy-MM-dd") : ""));
    }
    ui->tableItems->resizeColumnsToContents();
    updateButtons();
}

int MainWindow::currentSelectedId() const {
    int row = ui->tableItems->currentRow();
    if (row < 0) return -1;
    bool ok = false;
    int id = ui->tableItems->item(row, 0)->text().toInt(&ok);
    return ok ? id : -1;
}

// Enable/disable buttons depending on selection and status.
void MainWindow::updateButtons() {
    int id = currentSelectedId();
    bool hasSel = (id >= 0);

    // default states
    bool canBorrow = false;
    bool canReturn = false;
    bool canPlaceHold = false;
    bool canCancelHold = false; // we will keep it false for now

    if (hasSel) {
        // Find the item in memory (simple linear search).
        const auto &items = data_->items;
        for (const auto &it : items) {
            if (it.id == id) {
                canBorrow = (it.status == availability::available);
                canReturn = (it.status == availability::checked_out);
                // If you later implement a real hold queue, set these properly:
                canPlaceHold = (it.status == availability::checked_out);
                break;
            }
        }
    }

    ui->borrowBtn->setEnabled(canBorrow);
    ui->returnBtn->setEnabled(canReturn);
    ui->placeHoldBtn->setEnabled(canPlaceHold);
    ui->cancelHoldBtn->setEnabled(canCancelHold);

    // Update label with selected info
    if (hasSel) {
        ui->selectedInfo->setText(QString("Selected ID: %1").arg(id));
    } else {
        ui->selectedInfo->setText("");
    }
}

// Refill table (by applying current filters) and reselect item if still visible.
void MainWindow::refreshTableKeepingSelection(int keepId) {
    applyFilters(); // this repaints the table according to current filters

    if (keepId < 0) { updateButtons(); return; }

    // Try to reselect the same id if it is still visible under current filters.
    for (int r = 0; r < ui->tableItems->rowCount(); ++r) {
        bool ok = false;
        int id = ui->tableItems->item(r, 0)->text().toInt(&ok);
        if (ok && id == keepId) {
            ui->tableItems->setCurrentCell(r, 0);
            break;
        }
    }
    updateButtons();
}

// When table selection changes, update buttons immediately.
void MainWindow::on_tableItems_itemSelectionChanged() {
    updateButtons();
}

// Borrow: set status to checked_out and assign a due date (+14 days).
void MainWindow::on_borrowBtn_clicked() {
    int id = currentSelectedId();
    if (id < 0) return;

    // Find the item and modify it.
    for (auto &it : data_->items) {
        if (it.id == id) {
            if (it.status == availability::checked_out) {
                QMessageBox::warning(this, "Borrow", "Item already checked out.");
                return;
            }
            it.status = availability::checked_out;
            it.due = QDate::currentDate().addDays(14);
            statusBar()->showMessage(QString("Borrowed item %1 (due %2)")
                                     .arg(id).arg(it.due.toString("yyyy-MM-dd")), 3000);
            refreshTableKeepingSelection(id);
            return;
        }
    }
}

// Return: set status to available and clear due date.
void MainWindow::on_returnBtn_clicked() {
    int id = currentSelectedId();
    if (id < 0) return;

    for (auto &it : data_->items) {
        if (it.id == id) {
            if (it.status == availability::available) {
                QMessageBox::information(this, "Return", "Item is already available.");
                return;
            }
            it.status = availability::available;
            it.due = QDate(); // invalid date = clear
            statusBar()->showMessage(QString("Returned item %1").arg(id), 3000);
            refreshTableKeepingSelection(id);
            return;
        }
    }
}

// Place Hold / Cancel Hold (stubs for now). You can extend with a hold queue later.
void MainWindow::on_placeHoldBtn_clicked() {
    int id = currentSelectedId();
    if (id < 0) return;
    QMessageBox::information(this, "Hold", QString("Placed hold for item %1 (stub).").arg(id));
    updateButtons();
}

void MainWindow::on_cancelHoldBtn_clicked() {
    int id = currentSelectedId();
    if (id < 0) return;
    QMessageBox::information(this, "Hold", QString("Canceled hold for item %1 (stub).").arg(id));
    updateButtons();
}

void MainWindow::on_searchEdit_textChanged(const QString&) { applyFilters(); }
void MainWindow::on_typeFilter_currentIndexChanged(int)     { applyFilters(); }
void MainWindow::on_statusFilter_currentIndexChanged(int)   { applyFilters(); }
void MainWindow::on_clearFilterBtn_clicked() {
    ui->searchEdit->clear();
    ui->typeFilter->setCurrentIndex(0);
    ui->statusFilter->setCurrentIndex(0);
    applyFilters();
}

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this, "About",
        "Library Demo\nCOMP3004 Group Project");
}

MainWindow::~MainWindow()
{
    delete ui; // unique_ptr auto-destroys data_
}
