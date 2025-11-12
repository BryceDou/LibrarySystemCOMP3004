#include "mainwindow.h"
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    cat_.seedDefaultData();
    buildUi();
    loginFlow();
}

void MainWindow::buildUi() {
    setWindowTitle("HinLIBS");
    auto* tool = addToolBar("Main");
    auto* actLogout = tool->addAction("Logout");
    connect(actLogout, &QAction::triggered, this, &MainWindow::onLogout);

    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);

    banner_ = new QLabel(this);
    banner_->setObjectName("banner");
    banner_->setStyleSheet("#banner{font-weight:600;font-size:16px;padding:8px 4px;}");

    // Items table
    itemsTbl_ = new QTableWidget(this);
    itemsTbl_->setColumnCount(8);
    itemsTbl_->setHorizontalHeaderLabels({"ID","Title","Creator","Type","Status","Due","Extra 1","Extra 2"});
    itemsTbl_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    itemsTbl_->setSelectionBehavior(QAbstractItemView::SelectRows);
    itemsTbl_->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(itemsTbl_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);

    // Actions
    auto* actions = new QHBoxLayout();
    btnBorrow_ = new QPushButton("Borrow");
    btnReturn_ = new QPushButton("Return");
    btnHold_   = new QPushButton("Place Hold");
    btnCancelHold_ = new QPushButton("Cancel Hold");
    actions->addWidget(btnBorrow_);
    actions->addWidget(btnReturn_);
    actions->addWidget(btnHold_);
    actions->addWidget(btnCancelHold_);
    actions->addStretch();

    connect(btnBorrow_, &QPushButton::clicked, this, &MainWindow::borrowItem);
    connect(btnReturn_, &QPushButton::clicked, this, &MainWindow::returnItem);
    connect(btnHold_,   &QPushButton::clicked, this, &MainWindow::placeHold);
    connect(btnCancelHold_, &QPushButton::clicked, this, &MainWindow::cancelHold);

    // Details panel
    auto* detBox = new QGroupBox("Selected Item", this);
    auto* detLay = new QVBoxLayout(detBox);
    detTitle_   = new QLabel("-");
    detCreator_ = new QLabel("-");
    detType_    = new QLabel("-");
    detStatus_  = new QLabel("-");
    detDue_     = new QLabel("-");
    detExtra1_  = new QLabel("-");
    detExtra2_  = new QLabel("-");
    for (auto* l : {detTitle_, detCreator_, detType_, detStatus_, detDue_, detExtra1_, detExtra2_}) l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    detLay->addWidget(detTitle_);
    detLay->addWidget(detCreator_);
    detLay->addWidget(detType_);
    detLay->addWidget(detStatus_);
    detLay->addWidget(detDue_);
    detLay->addWidget(detExtra1_);
    detLay->addWidget(detExtra2_);

    // Account status
    auto* acctBox = new QGroupBox("Account Status", this);
    auto* acctLay = new QHBoxLayout(acctBox);

    loansTbl_ = new QTableWidget(this);
    loansTbl_->setColumnCount(3);
    loansTbl_->setHorizontalHeaderLabels({"Title","Due","Days Left"});
    loansTbl_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    loansTbl_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    holdsTbl_ = new QTableWidget(this);
    holdsTbl_->setColumnCount(2);
    holdsTbl_->setHorizontalHeaderLabels({"Title","Queue Pos"});
    holdsTbl_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    holdsTbl_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    acctLay->addWidget(loansTbl_, 1);
    acctLay->addWidget(holdsTbl_, 1);

    root->addWidget(banner_);
    root->addWidget(itemsTbl_, 3);
    root->addLayout(actions);
    root->addWidget(detBox);
    root->addWidget(acctBox, 2);
    setCentralWidget(central);
}

void MainWindow::loginFlow() {
    while (true) {
        LoginDialog dlg(&cat_, this);
        if (dlg.exec() != QDialog::Accepted) {
            qApp->quit();
            return;
        }
        setActiveUser(dlg.selectedUserId());
        if (active_) break;
    }
    refreshAll();
}

void MainWindow::setActiveUser(int uid) {
    active_ = cat_.findUserById(uid);
    if (!active_) return;
    banner_->setText(QString("Logged in as: <b>%1</b> — <i>%2</i>")
                     .arg(active_->name, toString(active_->type)));
    // Role-based UI
    const bool patron = active_->type == UserType::Patron;
    btnBorrow_->setEnabled(patron);
    btnReturn_->setEnabled(patron);
    btnHold_->setEnabled(patron);
    btnCancelHold_->setEnabled(patron);
}

void MainWindow::refreshAll() {
    refreshItemsTable();
    refreshDetails();
    refreshAccountPanels();
    updateButtons();
}

static int selectedItemId(const QTableWidget* tbl) {
    auto sel = tbl->selectionModel()->selectedRows();
    if (sel.isEmpty()) return -1;
    return tbl->item(sel.first().row(), 0)->text().toInt();
}

QString MainWindow::extra1Header(ItemType t) {
    switch (t) {
        case ItemType::NonFiction: return "Dewey";
        case ItemType::Magazine:   return "Issue";
        case ItemType::Movie:      return "Genre";
        case ItemType::VideoGame:  return "Genre";
        default: return "Extra 1";
    }
}
QString MainWindow::extra2Header(ItemType t) {
    switch (t) {
        case ItemType::Magazine:   return "Published";
        case ItemType::Movie:      return "Rating";
        case ItemType::VideoGame:  return "Rating";
        default: return "Extra 2";
    }
}
QString MainWindow::extra1Value(const Item& it) {
    switch (it.type) {
        case ItemType::NonFiction: return it.dewey;
        case ItemType::Magazine:   return it.issue;
        case ItemType::Movie:
        case ItemType::VideoGame:  return it.genre;
        default: return "";
    }
}
QString MainWindow::extra2Value(const Item& it) {
    switch (it.type) {
        case ItemType::Magazine:   return it.pub.isValid() ? it.pub.toString("yyyy-MM-dd") : "";
        case ItemType::Movie:
        case ItemType::VideoGame:  return it.rating;
        default: return "";
    }
}

void MainWindow::refreshItemsTable() {
    itemsTbl_->setRowCount(0);
    itemsTbl_->clearContents();
    itemsTbl_->setSortingEnabled(false);

    itemsTbl_->setHorizontalHeaderLabels({"ID","Title","Creator","Type","Status","Due","Extra 1","Extra 2"});

    int row=0;
    for (const auto& it : cat_.items) {
        itemsTbl_->insertRow(row);
        auto put = [&](int col, const QString& s){
            auto* item = new QTableWidgetItem(s);
            if (col==0) item->setData(Qt::UserRole, it.id);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            itemsTbl_->setItem(row, col, item);
        };
        put(0, QString::number(it.id));
        put(1, it.title);
        put(2, it.creator);
        put(3, toString(it.type));
        put(4, toString(it.status));
        put(5, it.due.isValid()? it.due.toString("yyyy-MM-dd") : "");
        put(6, extra1Value(it));
        put(7, extra2Value(it));
        row++;
    }
    itemsTbl_->setSortingEnabled(true);
}

void MainWindow::refreshDetails() {
    int id = selectedItemId(itemsTbl_);
    const Item* it = (id>=0) ? cat_.findItem(id) : nullptr;
    if (!it) {
        detTitle_->setText("Title: -");
        detCreator_->setText("Creator: -");
        detType_->setText("Type: -");
        detStatus_->setText("Status: -");
        detDue_->setText("Due: -");
        detExtra1_->setText("Extra 1: -");
        detExtra2_->setText("Extra 2: -");
        return;
    }
    detTitle_->setText("Title: " + it->title);
    detCreator_->setText("Creator: " + it->creator);
    detType_->setText("Type: " + toString(it->type));
    detStatus_->setText("Status: " + toString(it->status));
    detDue_->setText("Due: " + (it->due.isValid()? it->due.toString("yyyy-MM-dd") : "-"));
    detExtra1_->setText(extra1Header(it->type) + ": " + extra1Value(*it));
    detExtra2_->setText(extra2Header(it->type) + ": " + extra2Value(*it));
}

void MainWindow::refreshAccountPanels() {
    loansTbl_->setRowCount(0);
    holdsTbl_->setRowCount(0);
    if (!active_) return;

    // Loans
    int r=0;
    for (int itemId : active_->loans) {
        const Item* it = cat_.findItem(itemId);
        if (!it) continue;
        loansTbl_->insertRow(r);
        auto put=[&](int c, const QString&s){
            auto* cell=new QTableWidgetItem(s);
            cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
            loansTbl_->setItem(r,c,cell);
        };
        put(0, it->title);
        put(1, it->due.isValid()? it->due.toString("yyyy-MM-dd") : "");
        int daysLeft = QDate::currentDate().daysTo(it->due);
        put(2, QString::number(daysLeft));
        r++;
    }

    // Holds
    r=0;
    for (int itemId : active_->holds) {
        const Item* it = cat_.findItem(itemId);
        if (!it) continue;
        int pos = it->holdQueue.indexOf(active_->id);
        holdsTbl_->insertRow(r);
        auto put=[&](int c, const QString&s){
            auto* cell=new QTableWidgetItem(s);
            cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
            holdsTbl_->setItem(r,c,cell);
        };
        put(0, it->title);
        put(1, (pos>=0? QString::number(pos+1) : "-"));
        r++;
    }
}

void MainWindow::updateButtons() {
    const bool patron = active_ && active_->type == UserType::Patron;
    int id = selectedItemId(itemsTbl_);
    const Item* it = (id>=0) ? cat_.findItem(id) : nullptr;

    bool canBorrow=false, canReturn=false, canHold=false, canCancelHold=false;

    if (patron && it) {
        const bool hasLoanCap = active_->loans.size() < 3;
        const bool isAvailable = it->status == Availability::Available;
        const bool isCheckedOut = it->status == Availability::CheckedOut;
        const bool isBorrower = it->borrowerId == active_->id;
        const bool inHoldQ = it->holdQueue.contains(active_->id);

        // Borrow: available AND loan cap AND (no holds OR you're first in line)
        bool queueClear = it->holdQueue.isEmpty() || (it->holdQueue.first() == active_->id);
        canBorrow = isAvailable && hasLoanCap && queueClear;

        // Return: only if you borrowed it
        canReturn = isCheckedOut && isBorrower;

        // Place hold: only if checked out and not already in queue
        canHold = isCheckedOut && !inHoldQ;

        // Cancel hold: only if you are in queue
        canCancelHold = inHoldQ;
    }

    btnBorrow_->setEnabled(canBorrow);
    btnReturn_->setEnabled(canReturn);
    btnHold_->setEnabled(canHold);
    btnCancelHold_->setEnabled(canCancelHold);
}

void MainWindow::borrowItem() {
    if (!active_ || active_->type != UserType::Patron) return;
    int id = selectedItemId(itemsTbl_);
    Item* it = cat_.findItem(id);
    if (!it) return;

    if (active_->loans.size() >= 3) {
        QMessageBox::warning(this,"Limit","Maximum active loans is 3.");
        return;
    }
    if (it->status != Availability::Available) {
        QMessageBox::warning(this,"Unavailable","Item is not available.");
        return;
    }
    if (!it->holdQueue.isEmpty() && it->holdQueue.first() != active_->id) {
        QMessageBox::information(this,"On Hold","Another patron is first in the hold queue.");
        return;
    }

    it->status = Availability::CheckedOut;
    it->borrowerId = active_->id;
    it->due = QDate::currentDate().addDays(14);
    active_->addLoan(it->id);

    // If user was first in queue, remove that hold
    if (!it->holdQueue.isEmpty() && it->holdQueue.first() == active_->id) {
        it->holdQueue.pop_front();
        active_->removeHold(it->id);
    }

    refreshAll();
}

void MainWindow::returnItem() {
    if (!active_ || active_->type != UserType::Patron) return;
    int id = selectedItemId(itemsTbl_);
    Item* it = cat_.findItem(id);
    if (!it) return;

    if (it->borrowerId != active_->id) {
        QMessageBox::warning(this, "Not your loan", "You can only return items you borrowed.");
        return;
    }

    it->status = Availability::Available;
    it->borrowerId = -1;
    it->due = QDate();
    active_->removeLoan(it->id);

    refreshAll();
}

void MainWindow::placeHold() {
    if (!active_ || active_->type != UserType::Patron) return;
    int id = selectedItemId(itemsTbl_);
    Item* it = cat_.findItem(id);
    if (!it) return;

    if (it->status != Availability::CheckedOut) {
        QMessageBox::information(this, "Hold not allowed", "Holds are only allowed on checked-out items.");
        return;
    }
    if (it->holdQueue.contains(active_->id)) {
        QMessageBox::information(this, "Already queued", "You are already in the hold queue.");
        return;
    }

    it->holdQueue.append(active_->id);
    active_->addHold(it->id);
    int pos = it->holdQueue.size(); // appended at end
    QMessageBox::information(this, "Hold placed", QString("You are #%1 in the queue.").arg(pos));

    refreshAll();
}

void MainWindow::cancelHold() {
    if (!active_ || active_->type != UserType::Patron) return;
    int id = selectedItemId(itemsTbl_);
    Item* it = cat_.findItem(id);
    if (!it) return;

    int idx = it->holdQueue.indexOf(active_->id);
    if (idx < 0) {
        QMessageBox::information(this, "No hold", "You do not have a hold on this item.");
        return;
    }

    it->holdQueue.removeAt(idx);
    active_->removeHold(it->id);
    refreshAll();
}

void MainWindow::onSelectionChanged() {
    refreshDetails();
    updateButtons();
}

void MainWindow::onLogout() {
    // Clear session-specific UI state
    active_ = nullptr;
    banner_->setText("Not signed in");
    loansTbl_->setRowCount(0);
    holdsTbl_->setRowCount(0);
    updateButtons();
    // Show login
    loginFlow();
}

