#include "habbit_tracker.h"
#include "./ui_habbit_tracker.h"
#include <QKeyEvent>

Habbit_tracker::~Habbit_tracker(){
    delete ui;
}



Habbit_tracker::Habbit_tracker(QWidget *parent): QMainWindow(parent), ui(new Ui::Habbit_tracker){
    ui->setupUi(this);
    QApplication::setStyle("Fusion");                               // Setting the style be portable
    setFixedSize(1024, 600);                                        // Setting Size of window
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);  // Removing the sizing gadget


    // Setting up the Habit Tracker Table
    ui->M_habitTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->M_habitTable->horizontalHeader()->setStretchLastSection(false);
    ui->M_habitTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->M_habitTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->M_habitTable->setColumnWidth(0, 254);
    for (int col = 1; col < ui->M_habitTable->columnCount(); ++col) {
        ui->M_habitTable->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    }

    ui->M_habitTable->setShowGrid(true);
    ui->M_habitTable->setGridStyle(Qt::SolidLine);
    ui->M_habitTable->setRowCount(12);


    // Connecting all the keyboard keys and Setting their Focus Policy
    QList<QPushButton*> keys = ui->A_keyboard->findChildren<QPushButton*>();
    for (const auto &key : keys) {
        connect(key, &QPushButton::clicked, this, &Habbit_tracker::insertKey);
        key->setFocusPolicy(Qt::NoFocus);
    }
    // Setting the rest of the buttons and frames to have no focus either
    ui->A_keyboard->setFocusPolicy(Qt::NoFocus);
    ui->A_cancelButton->setFocusPolicy(Qt::NoFocus);
    ui->A_saveButton->setFocusPolicy(Qt::NoFocus);





    // Connections
    connect(ui->M_addHabitButton, &QPushButton::clicked, this, &Habbit_tracker::M_addHabbitButtonClicked);
    connect(ui->A_cancelButton, &QPushButton::clicked, this, &Habbit_tracker::A_cancelButtonClicked);




    // Switching to the Main Frame
    switchFrame(ui->M_frame);
}





// FRAME CONTROL FUNCTIONS:
void Habbit_tracker::switchFrame(QFrame* target){
    ui->M_frame->hide();
    ui->A_frame->hide();
    if(target == ui->M_frame){

    }
    else if(target == ui->A_frame){
        // Setting keyboard to Capital Letters
        isCapps = true;
        setCapps();

        // Clearing the name text
        ui->A_nameInput->setText("");
        ui->A_nameInput->setFocus();
    }
    target->show();
}




// MAIN MENU FUNCTIONS:
void Habbit_tracker::M_addHabbitButtonClicked(){
    switchFrame(ui->A_frame);
}






// ADD HABIT FUNCTIONS:
void Habbit_tracker::A_cancelButtonClicked(){
    switchFrame(ui->M_frame);
}
void Habbit_tracker::setCapps(){
    // When starting, isCapps is true. So it will enter the first section
    if(isCapps){
        ui->key_Q->setText("Q"); ui->key_A->setText("A"); ui->key_Z->setText("Z");
        ui->key_W->setText("W"); ui->key_S->setText("S"); ui->key_X->setText("X");
        ui->key_E->setText("E"); ui->key_D->setText("D"); ui->key_C->setText("C");
        ui->key_R->setText("R"); ui->key_F->setText("F"); ui->key_V->setText("V");
        ui->key_T->setText("T"); ui->key_G->setText("G"); ui->key_B->setText("B");
        ui->key_Y->setText("Y"); ui->key_H->setText("H"); ui->key_N->setText("N");
        ui->key_U->setText("U"); ui->key_J->setText("J"); ui->key_M->setText("M");
        ui->key_I->setText("I"); ui->key_K->setText("K");
        ui->key_O->setText("O"); ui->key_L->setText("L");
        ui->key_P->setText("P");
    }
    else{

        ui->key_Q->setText("q"); ui->key_A->setText("a"); ui->key_Z->setText("z");
        ui->key_W->setText("w"); ui->key_S->setText("s"); ui->key_X->setText("x");
        ui->key_E->setText("e"); ui->key_D->setText("d"); ui->key_C->setText("c");
        ui->key_R->setText("r"); ui->key_F->setText("f"); ui->key_V->setText("v");
        ui->key_T->setText("t"); ui->key_G->setText("g"); ui->key_B->setText("b");
        ui->key_Y->setText("y"); ui->key_H->setText("h"); ui->key_N->setText("n");
        ui->key_U->setText("u"); ui->key_J->setText("j"); ui->key_M->setText("m");
        ui->key_I->setText("i"); ui->key_K->setText("k");
        ui->key_O->setText("o"); ui->key_L->setText("l");
        ui->key_P->setText("p");
    }

    // Change state of capps locked
    isCapps = !isCapps;
}
void Habbit_tracker::insertKey(){
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString keyText = btn->text();
    QWidget* widget = QApplication::focusWidget();


    if (keyText == "Backspace") {
        if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
            QString text = lineEdit->text();
            text.chop(1);
            lineEdit->setText(text);
        }
        return;
    }

    if(keyText == "Space Bar"){
        if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
            QString text = lineEdit->text();
            text = text + " ";
            lineEdit->setText(text);
        }
        return;
    }

    if(keyText == "Capps Lock"){
        setCapps();
        return;
    }

    // Handle normal key input (A–Z)
    if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
        lineEdit->insert(keyText);
    }
}


void Habbit_tracker::addHabit(){
    //int row = ui->M_habitTable->rowCount();
    //ui->M_habitTable->insertRow(row);
    //
    //// Add a QLineEdit for the habit name
    //QLineEdit *habitNameEdit = new QLineEdit(this);
    //ui->M_habitTable->setCellWidget(row, 0, habitNameEdit);
    //
    //// Add radio buttons for each day
    //for (int col = 1; col <= 7; ++col) {
    //    QRadioButton *radio = new QRadioButton(this);
    //
    //    // Wrap it in a centered layout
    //    QWidget *container = new QWidget(this);
    //    QHBoxLayout *layout = new QHBoxLayout(container);
    //    layout->addWidget(radio);
    //    layout->setAlignment(Qt::AlignCenter);
    //    layout->setContentsMargins(0, 0, 0, 0);
    //    container->setLayout(layout);
    //
    //    ui->M_habitTable->setCellWidget(row, col, container);
    //}
}
