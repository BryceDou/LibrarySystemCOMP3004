#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "catalogue.h"
#include <memory>  // make sure this is included here

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create and seed the in-memory dataset
    data_.reset(new catalogue());   // works even without make_unique
    data_->seed_default_data();
}

MainWindow::~MainWindow()
{
    delete ui; // unique_ptr auto-destroys data_
}
