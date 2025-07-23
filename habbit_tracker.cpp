#include "habbit_tracker.h"
#include "./ui_habbit_tracker.h"

Habbit_tracker::Habbit_tracker(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Habbit_tracker)
{
    ui->setupUi(this);
}

Habbit_tracker::~Habbit_tracker()
{
    delete ui;
}
