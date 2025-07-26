#include "habbit_tracker.h"
#include "./ui_habbit_tracker.h"
#include <QKeyEvent>
#include <QRadioButton>
#include <QHBoxLayout>
#include <filesystem>
#include <iostream>
#include <QCheckBox>
#include <QThread>
#include <QDate>
#include <QMessageBox>
#include <QTimer>
#include <QColor>
#include <QScrollBar>


using namespace std;

Habbit_tracker::~Habbit_tracker(){
    delete ui;
}



Habbit_tracker::Habbit_tracker(QWidget *parent): QMainWindow(parent), ui(new Ui::Habbit_tracker){
    ui->setupUi(this);
    QApplication::setStyle("Fusion");                               // Setting the style be portable
    setFixedSize(1024, 600);                                        // Setting Size of window
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);  // Removing the sizing gadget


    // Setting up the habit list for removal apperances
    ui->M_removeList->setStyleSheet(R"(
    QListWidget {
        background: rgb(251, 251, 184);
    }

    QListWidget::item {
        padding: 5px;
    }

    QListWidget::item:selected {
        background-color: rgb(255, 203, 160);
        color: black;
    }

    QListWidget::item:hover {
        background-color: rgb(255, 203, 160);
    }
    )");
    ui->M_removeFrame->raise(); // Raising since we are going to be messing with the frames in the ui a lot





    // Setting up the Habit Tracker Table
    ui->M_habitTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->M_habitTable->horizontalHeader()->setStretchLastSection(false);
    ui->M_habitTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->M_habitTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->M_habitTable->setColumnWidth(0, 254);
    for (int col = 1; col < ui->M_habitTable->columnCount(); ++col) {
        ui->M_habitTable->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    }

    ui->M_habitTable->setShowGrid(showGrid);
    ui->M_habitTable->setGridStyle(Qt::SolidLine);
    ui->M_habitTable->setRowCount(initRowCount);


    // Connecting all the keyboard keys and Setting their Focus Policy
    QList<QPushButton*> keys = ui->A_keyboard->findChildren<QPushButton*>();
    for (auto &key : keys) {
        connect(key, &QPushButton::clicked, this, &Habbit_tracker::insertKey);
        key->setFocusPolicy(Qt::NoFocus);
    }

    // Setting no focus to other things
    ui->A_keyboard->setFocusPolicy(Qt::NoFocus);
    ui->A_cancelButton->setFocusPolicy(Qt::NoFocus);
    ui->A_saveButton->setFocusPolicy(Qt::NoFocus);
    ui->M_habitTable->setFocusPolicy(Qt::NoFocus);
    ui->H_spanDisplay->setFocusPolicy(Qt::NoFocus);


    // Connections
    connect(ui->M_addHabitButton, &QPushButton::clicked, this, &Habbit_tracker::M_addHabbitButtonClicked);
    connect(ui->M_removeButton, &QPushButton::clicked, this, &Habbit_tracker::M_removeButtonClicked);
    connect(ui->M_cancelButton, &QPushButton::clicked, this, &Habbit_tracker::M_cancelButtonClicked);
    connect(ui->M_confirmButton, &QPushButton::clicked, this, &Habbit_tracker::M_confirmButtonClicked);
    connect(ui->M_viewHistoryButton, &QPushButton::clicked, this, &Habbit_tracker::M_viewHistoryButtonClicked);

    connect(ui->A_cancelButton, &QPushButton::clicked, this, &Habbit_tracker::A_cancelButtonClicked);
    connect(ui->A_saveButton, &QPushButton::clicked, this, &Habbit_tracker::A_saveButtonClicked);

    connect(ui->H_backButton, &QPushButton::clicked, this, &Habbit_tracker::H_backButtonClicked);
    connect(ui->H_downArrow, &QPushButton::clicked, this, &Habbit_tracker::H_arrowClicked);
    connect(ui->H_upArrow, &QPushButton::clicked, this, &Habbit_tracker::H_arrowClicked);
    connect(ui->H_lastWeekButton, &QPushButton::clicked, this, &Habbit_tracker::spanButtonClicked);
    connect(ui->H_monthButton, &QPushButton::clicked, this, &Habbit_tracker::spanButtonClicked);
    connect(ui->H_yearButton, &QPushButton::clicked, this, &Habbit_tracker::spanButtonClicked);
    connect(ui->H_scrollDownButton, &QPushButton::clicked, this, &Habbit_tracker::scrollButtonClicked);
    connect(ui->H_scrollUpButton, &QPushButton::clicked, this, &Habbit_tracker::scrollButtonClicked);





    // Setting up the day checker timer to keep track of what day it is
    dayCheckTimer = new QTimer(this);
    currentDate = QDate::currentDate();
    //currentDate = QDate(2025, 7, 27); // <---- TESTING
    connect(dayCheckTimer, &QTimer::timeout, this, [=]() mutable {
        QDate now = QDate::currentDate();
        //QDate now = QDate(2025, 7, 28); // <--- TESTING
        if (now != currentDate) {                    // If now has moved to a different date than saved currentDate
            if (currentDate.dayOfWeek() == 7) {      // If it was sunday, we need to save all past week and set new week up
                for(auto &currHabit:allHabits){
                    currHabit.saveWeek();            // Pushes week into history (vector of weeks), then resets the week bools to 0
                    currHabit.writeToFile();         // Saves the week to its file
                }
            }
            currentDate = now;                       // Updates the currentday to today to get ready for check of tomorrow
            loadHabits();                            // Clears the allHabits and the habitTable and resets again from the files
        }
    });

    dayCheckTimer->start(dayCheckerInterval);                     // every minute check if we are in a new day

    // Switching to the Main Frame
    switchFrame(ui->M_frame);
}





// FRAME CONTROL FUNCTIONS:
void Habbit_tracker::switchFrame(QFrame* target){
    ui->M_frame->hide();
    ui->A_frame->hide();
    ui->H_frame->hide();
    if(target == ui->M_frame){
        // Hiding the removing habits frame
        ui->M_removeFrame->hide();

        // Loading and painting all habits from files
        loadHabits();
    }
    else if(target == ui->A_frame){

        // Hiding the numbers
        showNums = false;

        // Setting keyboard to Capital Letters
        showCapps = false;
        setCapps();



        // Clearing the name text
        ui->A_nameInput->setText("");
        ui->A_nameInput->setFocus();

        // Setting the note back to regular color
        ui->A_note->setStyleSheet("color:black;");

        // Setting the image to sad face
        ui->A_face->setStyleSheet("border-image: url(:/faces/images/nuetralFace.png) 0 0 0 0 stretch stretch;");
    }
    else if(target == ui->H_frame){
        // Resetting the Top Text and the disabled buttons if any
        if(allHabits.size() <= 0){                                                      // If there are no habits, show mess. and disable buttons
            ui->H_habitName->setText("There are currently no habits");
            ui->H_upArrow->setDisabled(true);
            ui->H_downArrow->setDisabled(true);
            ui->H_lastWeekButton->setDisabled(true);
            ui->H_monthButton->setDisabled(true);
            ui->H_yearButton->setDisabled(true);
            historyIndex = -1;
        }
        else{                                                                           // If there ARE habits, set to first, and enable buttons
            ui->H_habitName->setText(QString::fromStdString(allHabits[0].getName()));
            ui->H_upArrow->setDisabled(true);     // Setting up arrow to be disabled because we are at the top already
            ui->H_downArrow->setDisabled((allHabits.size() == 1 ? true:false)); // If size = 1, then set down to disable, else to false
            ui->H_lastWeekButton->setDisabled(false);
            ui->H_monthButton->setDisabled(false);
            ui->H_yearButton->setDisabled(false);
            historyIndex = 0;
        }

        // Resetting the date span text
        ui->H_dateSpan->setText("Please select a span.");

        // Setting up the history buttons apperance when disabled and enabled
        ui->H_upArrow->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");
        ui->H_downArrow->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");
        ui->H_lastWeekButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");
        ui->H_monthButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");
        ui->H_yearButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");

        // Clearing the span display
        ui->H_spanDisplay->clear();
        ui->H_spanDisplay->clearSpans();
        ui->H_spanDisplay->clearSelection();
        ui->H_spanDisplay->setRowCount(0);
        ui->H_spanDisplay->setColumnCount(0);
    }
    target->show();
}








// MAIN MENU FUNCTIONS:
void Habbit_tracker::M_addHabbitButtonClicked(){
    ui->M_removeFrame->hide();
    switchFrame(ui->A_frame);
}

void Habbit_tracker::M_removeButtonClicked(){
    // If remove frame is already visible, "close" the window and return
    if(ui->M_removeFrame->isVisible()){
        ui->M_removeFrame->hide();
        return;
    }


    // Setting up the remove list
    ui->M_removeList->clear();
    for(auto &currHabit:allHabits){
        ui->M_removeList->addItem(QString::fromStdString(currHabit.getName()));
    }


    // Making frame visible
    ui->M_removeFrame->show();
}

void Habbit_tracker::M_cancelButtonClicked(){
    ui->M_removeFrame->hide();
}

void Habbit_tracker::M_confirmButtonClicked(){
    // If there is nothing selected
    if(ui->M_removeList->selectedItems().count() != 1){
        return;
    }

    // Getting the name of the Target Habit
    string targetHabit = ui->M_removeList->currentItem()->text().toStdString();

    // Confirming with user that they want to remove this habit
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Confirm Deletion", "Are you sure you want to remove: " + QString::fromStdString(targetHabit),
                                  QMessageBox::Yes | QMessageBox::No);

    // Returning if user decided to cancel
    if (reply == QMessageBox::No) {
        return;
    }

    // Saving the target file to remove and removing habit from all habits
    int currIndex = 0;
    for(auto &currHabit:allHabits){
        if(currHabit.getName() == targetHabit){
            targetHabit = currHabit.getFileName();    // Saving file name to targetHabit
            allHabits.erase(allHabits.begin()+currIndex);
            break;
        }
        currIndex++;
    }



    if (filesystem::exists(targetHabit)) {
        filesystem::remove(targetHabit);
    }

    switchFrame(ui->M_frame);
}


void Habbit_tracker::M_viewHistoryButtonClicked(){
    ui->M_removeFrame->hide();
    switchFrame(ui->H_frame);
}




// ADD HABIT FUNCTIONS:
void Habbit_tracker::A_cancelButtonClicked(){
    ui->A_face->setStyleSheet("border-image: url(:/faces/images/sadFace.png) 0 0 0 0 stretch stretch;");
    QApplication::processEvents();   // Forcing the stylesheet to update before going to sleep for the second
    QThread::sleep(1);
    switchFrame(ui->M_frame);
}

void Habbit_tracker::setCapps(){
    // When starting, isCapps is true. So it will enter the first section
    if(showCapps){
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
    showCapps = !showCapps;
    showNums = false;
}

void Habbit_tracker::setNumbers(){
    if(!showNums){
        ui->key_Q->setText("1");
        ui->key_W->setText("2");
        ui->key_E->setText("3");
        ui->key_R->setText("4");
        ui->key_T->setText("5");
        ui->key_Y->setText("6");
        ui->key_U->setText("7");
        ui->key_I->setText("8");
        ui->key_O->setText("9");
        ui->key_P->setText("0");
    }
    else{
        if(!showCapps){
            ui->key_Q->setText("Q");
            ui->key_W->setText("W");
            ui->key_E->setText("E");
            ui->key_R->setText("R");
            ui->key_T->setText("T");
            ui->key_Y->setText("Y");
            ui->key_U->setText("U");
            ui->key_I->setText("I");
            ui->key_O->setText("O");
            ui->key_P->setText("P");
        }
        else{
            ui->key_Q->setText("q");
            ui->key_W->setText("w");
            ui->key_E->setText("e");
            ui->key_R->setText("r");
            ui->key_T->setText("t");
            ui->key_Y->setText("y");
            ui->key_U->setText("u");
            ui->key_I->setText("i");
            ui->key_O->setText("o");
            ui->key_P->setText("p");
        }
    }

    showNums = !showNums;
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

    if(keyText == "Caps Lock"){
        setCapps();
        return;
    }

    if(keyText == "Num."){
        setNumbers();
        return;
    }


    // Handle normal key input (A–Z)
    if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
        lineEdit->insert(keyText);
    }
}

void Habbit_tracker::A_saveButtonClicked() {
    // Get the text from the name input field, returning if the string is empty or invalid
    QString habitName = ui->A_nameInput->text().trimmed();

    if (!validString(habitName)) {
        ui->A_note->setStyleSheet("color:red;");
        ui->A_nameInput->setText("");
        return;
    } else {
        ui->A_note->setStyleSheet("color:black;");
    }


    // CHECKING IF INPUT NAME HAS ALREADY BEEN USED
    for(auto &currHabit:allHabits){
        if(currHabit.getName() == habitName.toStdString()){
            ui->A_note->setStyleSheet("color:red;");
            ui->A_nameInput->setText("");
            return;
        }
    }

    ui->A_note->setStyleSheet("color:black;");


    // WAITING FOR 1 SECOND TO SHOW THE FACE THEN ADDING:
    ui->A_face->setStyleSheet("border-image: url(:/faces/images/happyFace.png) 0 0 0 0 stretch stretch;");
    QApplication::processEvents();   // Forcing the stylesheet to update before going to sleep for the second
    QThread::sleep(1);



    // ADDING
    int habitCount = allHabits.size();
    // Only insert a new row if we're past the initial number of rows
    if (habitCount >= ui->M_habitTable->rowCount()) {
        ui->M_habitTable->insertRow(habitCount);
    }

    // SETTING THE HABIT IN THE GRAPH
    QTableWidgetItem *nameItem = new QTableWidgetItem(habitName);
    ui->M_habitTable->setItem(habitCount, 0, nameItem);

    // Add radio buttons for each day (columns 1 to 7)
    for (int col = 1; col <= 7; ++col) {
        QCheckBox *check = new QCheckBox(this);
        check->setStyleSheet(checkBoxStyleSheet);

        connect(check, &QCheckBox::stateChanged, this, &Habbit_tracker::onCheckmarkToggled);
        QWidget *container = new QWidget(this);
        QHBoxLayout *layout = new QHBoxLayout(container);
        layout->addWidget(check);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        container->setLayout(layout);

        ui->M_habitTable->setCellWidget(habitCount, col, container);
    }


    // ADDING TO THE VECTOR OF ALL HABITS
    habit newHabit(habitName.toStdString(), filesPath); // Making new Habit object
    newHabit.writeToFile();                             // Saving new habit to its path
    allHabits.push_back(newHabit);                      // Adding new habit to the vector


    switchFrame(ui->M_frame);          // Go back to main frame
}












// HISTORY FRAME FUNCTIONS:
void Habbit_tracker::H_backButtonClicked(){
    switchFrame(ui->M_frame);
}

void Habbit_tracker::H_arrowClicked(){
    QPushButton *pressedButton = qobject_cast<QPushButton*>(sender());
    if (!pressedButton) return;
    if(historyIndex == -1) return;

    // Clearing the spanDisplay since we switched and resetting the spanButtons
    ui->H_spanDisplay->clear();
    ui->H_spanDisplay->clearSpans();
    ui->H_spanDisplay->clearSelection();
    ui->H_spanDisplay->setRowCount(0);
    ui->H_spanDisplay->setColumnCount(0);
    ui->H_lastWeekButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; })");
    ui->H_monthButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");
    ui->H_yearButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");

    pressedButton->setStyleSheet(R"(
            QPushButton { background-color: rgb(255, 171, 69); }
            QPushButton:disabled { background-color: #a0a0a0; })");


    // If there was only one, then the two buttons were disabled in the switchframe function
    if(pressedButton == ui->H_upArrow){
        historyIndex--;
        ui->H_habitName->setText(QString::fromStdString(allHabits[historyIndex].getName()));
    }
    else if(pressedButton == ui->H_downArrow){
        historyIndex++;
        ui->H_habitName->setText(QString::fromStdString(allHabits[historyIndex].getName()));
    }
    ui->H_upArrow  ->setDisabled((historyIndex <= 0 ? true:false));
    ui->H_downArrow->setDisabled((historyIndex >= (allHabits.size()-1) ? true:false));
}

void Habbit_tracker::spanButtonClicked(){
    QPushButton *pressedButton = qobject_cast<QPushButton*>(sender());
    if (!pressedButton) return;
    if(historyIndex == -1) return;
    if(allHabits[historyIndex].getHistorySize() == 0){   // If history is empty return
        ui->H_dateSpan->setText("Not Enough Data to Generate.");
        return;
    }

    // Reseting the colors for all the buttons
    ui->H_lastWeekButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; })");
    ui->H_monthButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");
    ui->H_yearButton->setStyleSheet(R"(
        QPushButton { background-color: rgb(255, 233, 176); }
        QPushButton:disabled { background-color: #a0a0a0; } )");

    pressedButton->setStyleSheet(R"(
            QPushButton { background-color: rgb(255, 171, 69); }
            QPushButton:disabled { background-color: #a0a0a0; })");

    QDate spanStart;
    QDate spanEnd;

    // Setting the color to be set as currently in this tab
    if(pressedButton == ui->H_lastWeekButton){
        int currentDayOfWeek = currentDate.dayOfWeek();
        spanStart = currentDate.addDays(-currentDayOfWeek - 6);
        spanEnd = spanStart.addDays(6);
    }
    else if(pressedButton == ui->H_monthButton){
        spanStart = QDate(currentDate.year(), currentDate.month(), 1);
        spanEnd = spanStart.addMonths(1).addDays(-1);
    }
    else if(pressedButton == ui->H_yearButton){
        spanStart = QDate(currentDate.year(), 1, 1);
        spanEnd = QDate(currentDate.year(), 12, 31);
    }

    ui->H_dateSpan->setText(spanStart.toString(dateFormat) + "-" + spanEnd.toString(dateFormat));
    updateSpanDisplay(spanStart,spanEnd,historyIndex, pressedButton);
}

void Habbit_tracker::updateSpanDisplay(QDate spanStart, QDate spanEnd, int habitIndex, QPushButton* pressedButton){
    if (habitIndex < 0 || habitIndex >= allHabits.size()) return;

    habit& currentHabit = allHabits[habitIndex];

    // Clear the table
    ui->H_spanDisplay->clear();
    int rowCount = ui->H_spanDisplay->rowCount();
    int colCount = ui->H_spanDisplay->columnCount();
    for (int r = 0; r < rowCount; ++r) {
        for (int c = 0; c < colCount; ++c) {
            ui->H_spanDisplay->setSpan(r, c, 1, 1);
        }
    }


    int columns = 0;
    int rows = 1;

    if (pressedButton == ui->H_lastWeekButton) {
        ui->H_spanDisplay->setShowGrid(true);
        ui->H_spanDisplay->horizontalHeader()->setVisible(true);
        columns = 7;
        rows = 1;

        ui->H_spanDisplay->setColumnCount(columns);
        ui->H_spanDisplay->setRowCount(rows);

        // Set headers
        QStringList headers = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        ui->H_spanDisplay->setHorizontalHeaderLabels(headers);

        // Find matching week
        historyWeek* targetWeek = nullptr;
        for (auto& hw : currentHabit.getHistory()) {
            if (hw.start == spanStart && hw.end == spanEnd) {
                targetWeek = &hw;
                break;
            }
        }

        for (int i = 0; i < 7; ++i) {
            QTableWidgetItem* item = new QTableWidgetItem();
            QDate currentDay = spanStart.addDays(i);
            item->setText(QString::number(currentDay.day()));
            item->setTextAlignment(Qt::AlignCenter);
            if (targetWeek && targetWeek->values[i]) {
                item->setBackground(Qt::green);
            } else {
                item->setBackground(Qt::white);
            }
            ui->H_spanDisplay->setItem(0, i, item);
        }
    }




    else if (pressedButton == ui->H_monthButton) {
         ui->H_spanDisplay->setShowGrid(true);
        ui->H_spanDisplay->horizontalHeader()->setVisible(true);
        int totalDays = spanStart.daysTo(spanEnd) + 1;
        columns = 7;

        // Calculate number of rows needed to display the month properly
        int startDayOffset = spanStart.dayOfWeek() - 1; // Monday=1 -> offset=0
        rows = ((totalDays + startDayOffset + columns - 1) / columns); // Ceiling division

        ui->H_spanDisplay->setColumnCount(columns);
        ui->H_spanDisplay->setRowCount(rows);

        QStringList headers = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        ui->H_spanDisplay->setHorizontalHeaderLabels(headers);

        // Build progressMap as before
        std::map<QDate, bool> progressMap;
        for (const auto& hw : currentHabit.getHistory()) {
            if (!(hw.end < spanStart || hw.start > spanEnd)) {
                for (int i = 0; i < 7; ++i) {
                    QDate day = hw.start.addDays(i);
                    if (day >= spanStart && day <= spanEnd) {
                        progressMap[day] = hw.values[i];
                    }
                }
            }
        }

        int dayNumber = 1;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < columns; ++col) {
                int cellIndex = row * columns + col;

                QTableWidgetItem* item = nullptr;

                if (cellIndex < startDayOffset || dayNumber > totalDays) {
                    item = new QTableWidgetItem("");
                    item->setFlags(Qt::NoItemFlags);
                }
                else {
                    QDate currentDay = spanStart.addDays(dayNumber - 1);
                    item = new QTableWidgetItem(QString::number(dayNumber));
                    item->setTextAlignment(Qt::AlignCenter);

                    if (progressMap.count(currentDay) && progressMap[currentDay]) {
                        item->setBackground(Qt::green);
                    }
                    else {
                        item->setBackground(Qt::white);
                    }

                    dayNumber++;
                }

                ui->H_spanDisplay->setItem(row, col, item);
            }
        }
    }



    else {
        ui->H_spanDisplay->clear();

        // === Configurable colors ===
        QColor monthHeaderColor = QColor(255, 171, 69);     // Month Header
        QColor weekdayHeaderColor = QColor(230, 230, 230);  // Slightly darker for weekdays
        QColor spacerColor = QColor(214, 209, 158);         // Space Color
        QColor dateBackgroundColor = QColor(255, 255, 255); // Default white for days
        QColor completedColor = QColor(100, 200, 100);      // Green for completed

        // === Layout config: 3×4 grid of months ===
        const int monthsPerRow = 3;
        const int monthsPerCol = 4;
        const int monthsCount = 12;

        const int monthCols = 7;
        const int monthRows = 7; // 1 label + 1 header + 5 week rows

        const int spacerCols = 1;
        const int spacerRows = 1;

        int totalCols = monthsPerRow * monthCols + (monthsPerRow - 1) * spacerCols;
        int totalRows = monthsPerCol * monthRows + (monthsPerCol - 1) * spacerRows;

        ui->H_spanDisplay->setColumnCount(totalCols);
        ui->H_spanDisplay->setRowCount(totalRows);

        ui->H_spanDisplay->horizontalHeader()->setVisible(false);
        ui->H_spanDisplay->verticalHeader()->setVisible(false);
        ui->H_spanDisplay->setShowGrid(false);

        QStringList dayNames = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

        int year = spanStart.year();
        habit& currentHabit = allHabits[habitIndex];

        // Build progress map
        std::map<QDate, bool> progressMap;
        for (const auto& hw : currentHabit.getHistory()) {
            for (int i = 0; i < 7; ++i) {
                QDate day = hw.start.addDays(i);
                if (day.year() == year) {
                    progressMap[day] = hw.values[i];
                }
            }
        }

        for (int m = 0; m < monthsCount; ++m) {
            int month = m + 1;

            int monthRow = m / monthsPerRow;
            int monthCol = m % monthsPerRow;
            int baseRow = monthRow * (monthRows + spacerRows);
            int baseCol = monthCol * (monthCols + spacerCols);

            // === Month name ===
            QString monthName = QLocale().monthName(month, QLocale::LongFormat);
            QTableWidgetItem* monthLabel = new QTableWidgetItem(monthName);
            monthLabel->setTextAlignment(Qt::AlignCenter);
            monthLabel->setBackground(monthHeaderColor);
            if (monthCols > 1) {
                ui->H_spanDisplay->setSpan(baseRow, baseCol, 1, monthCols);
            }
            ui->H_spanDisplay->setItem(baseRow, baseCol, monthLabel);

            // === Weekday headers ===
            for (int d = 0; d < 7; ++d) {
                QTableWidgetItem* dayHeader = new QTableWidgetItem(dayNames[d]);
                dayHeader->setTextAlignment(Qt::AlignCenter);
                dayHeader->setFlags(Qt::NoItemFlags);
                dayHeader->setBackground(weekdayHeaderColor);
                ui->H_spanDisplay->setItem(baseRow + 1, baseCol + d, dayHeader);
            }

            // === Day cells ===
            QDate firstDay(year, month, 1);
            int daysInMonth = firstDay.daysInMonth();
            int startDayOffset = firstDay.dayOfWeek() - 1; // 0-indexed from Mon

            int dayNum = 1;
            for (int row = 0; row < 6; ++row) {
                for (int col = 0; col < 7; ++col) {
                    int cellIndex = row * 7 + col;
                    QTableWidgetItem* item = nullptr;

                    if (cellIndex < startDayOffset || dayNum > daysInMonth) {
                        item = new QTableWidgetItem("");
                        item->setFlags(Qt::NoItemFlags);
                    } else {
                        QDate currentDay(year, month, dayNum);
                        item = new QTableWidgetItem(QString::number(dayNum));
                        item->setTextAlignment(Qt::AlignCenter);

                        if (progressMap.count(currentDay) && progressMap[currentDay]) {
                            item->setBackground(completedColor);
                        } else {
                            item->setBackground(dateBackgroundColor);
                        }
                        dayNum++;
                    }

                    ui->H_spanDisplay->setItem(baseRow + 2 + row, baseCol + col, item);
                }
            }
        }

        // === Fill vertical spacers ===
        for (int row = 0; row < totalRows; ++row) {
            for (int col = monthCols; col < totalCols; col += (monthCols + spacerCols)) {
                QTableWidgetItem* spacerItem = new QTableWidgetItem("");
                spacerItem->setFlags(Qt::NoItemFlags);
                spacerItem->setBackground(spacerColor);
                ui->H_spanDisplay->setItem(row, col, spacerItem);
            }
        }

        // === Fill horizontal spacers ===
        for (int col = 0; col < totalCols; ++col) {
            for (int row = monthRows; row < totalRows; row += (monthRows + spacerRows)) {
                QTableWidgetItem* spacerItem = new QTableWidgetItem("");
                spacerItem->setFlags(Qt::NoItemFlags);
                spacerItem->setBackground(spacerColor);
                ui->H_spanDisplay->setItem(row, col, spacerItem);
            }
        }

        columns = totalCols;
        rows = totalRows;
    }




    // NOW: Calculate and set column widths and row heights

    // Get available size for the table viewport (excluding headers)
    QSize viewportSize = ui->H_spanDisplay->viewport()->size();

    // Calculate width per column (divide viewport width by number of columns)
    int columnWidth = viewportSize.width() / columns;

    // Calculate height per row (divide viewport height by number of rows)
    int rowHeight = viewportSize.height() / rows;

    // Apply sizes to columns and rows
    for (int c = 0; c < columns; ++c) {
        ui->H_spanDisplay->setColumnWidth(c, columnWidth);
    }
    for (int r = 0; r < rows; ++r) {
        ui->H_spanDisplay->setRowHeight(r, rowHeight);
    }
}


void Habbit_tracker::scrollButtonClicked(){
    QObject* btn = sender();
    if (!btn) return;

    int currentValue = ui->H_spanDisplay->verticalScrollBar()->value();
    int step = 2;

    if (btn == ui->H_scrollDownButton) {
        ui->H_spanDisplay->verticalScrollBar()->setValue(currentValue + step);
    } else if (btn == ui->H_scrollUpButton) {
        ui->H_spanDisplay->verticalScrollBar()->setValue(currentValue - step);
    }
}











// HELPER FUNCTIONS:
bool Habbit_tracker::validString(QString &input) {
    // Step 1: Remove leading spaces
    int firstCharIndex = 0;
    while (firstCharIndex < input.length() && input[firstCharIndex].isSpace()) {
        ++firstCharIndex;
    }

    QString trimmed = input.mid(firstCharIndex);

    // Step 2: Return false if it's empty after trimming
    if (trimmed.isEmpty())
        return false;

    // Step 3: Validate characters: must be only letters, digits, or spaces
    bool hasLetterOrDigit = false;
    for (QChar ch : trimmed) {
        if (ch.isLetterOrNumber()) {
            hasLetterOrDigit = true;
        } else if (!ch.isSpace()) {
            return false; // invalid character found
        }
    }

    return hasLetterOrDigit;
}

void Habbit_tracker::loadHabits(){
    ui->M_habitTable->clear();
    allHabits.clear();

    // Make folder if the folder does not exist
    if(!filesystem::exists(filesPath)){
        filesystem::create_directories(filesPath);
    }


    // Looking through all the folders in the filesPath folder, for every entry get the name and load it into the vector
    for (const auto& entry : filesystem::recursive_directory_iterator(filesPath)) {
        if (entry.is_regular_file()) {
            // Getting the name in <name_name.txt> and fixing to <name_name>
            string habitNameWith_ = entry.path().filename().string(); // e.g., "<name_name>.txt"
            habitNameWith_ = habitNameWith_.substr(0, habitNameWith_.length() - 4); // Remove ".txt"

            // Fixing <name_name> to <name name>
            string habitName = "";
            for(auto &ch:habitNameWith_){
                if(ch == '_'){
                    habitName += ' ';
                }
                else
                    habitName += ch;
            }


            // Making and loading object using name and filesPath
            habit newHabit(habitName, filesPath);
            newHabit.makeFromFile();

            // Copying the object into the vector
            allHabits.push_back(newHabit);
        }
    }


    //// Loading from already filled vector into the habit table
    int habitCount = 0;
    for(auto &currHabit:allHabits){
        array<bool,7> week = currHabit.getWeek();
        QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(currHabit.getName()));
        ui->M_habitTable->setItem(habitCount, 0, nameItem);

        // Add check box buttons for each day (columns 1 to 7)
        for (int col = 1; col <= 7; ++col) {
            QCheckBox *check = new QCheckBox(this);
            if (week[col - 1])
                check->setChecked(true);
            else
                check->setChecked(false);

            check->setStyleSheet(checkBoxStyleSheet);

            connect(check, &QCheckBox::stateChanged, this, &Habbit_tracker::onCheckmarkToggled);
            QWidget *container = new QWidget(this);
            QHBoxLayout *layout = new QHBoxLayout(container);
            layout->addWidget(check);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            container->setLayout(layout);

            ui->M_habitTable->setCellWidget(habitCount, col, container);
        }
        habitCount++;
    }

    // SETTING ROW HEIGHT: ============================================================================================
    for (int row = 0; row < ui->M_habitTable->rowCount(); ++row) {
        ui->M_habitTable->setRowHeight(row, rowHeight);
    }


    // VISUALS (PAINTING) =============================================================================================
    // Update current day
    int todaysDay = currentDate.dayOfWeek();


    // SETTING THE PAST DAYS PAINT
    for (int row = 0; row < ui->M_habitTable->rowCount(); ++row) {

        for (int col = 1; col <= 7; ++col) {
            QWidget *cellWidget = ui->M_habitTable->cellWidget(row, col);
            if (cellWidget) {
                QCheckBox *check = cellWidget->findChild<QCheckBox*>();
                if (check) {
                    bool isToday = (col == todaysDay);
                    bool isPast = (col < todaysDay);

                    if (isPast) {
                        // Color past days
                        QString color = (check->isChecked() ? isCheckedColor : notCheckedColor);
                        cellWidget->setStyleSheet("background-color: " + color + ";");
                    } else if (isToday) {
                        // Color current day
                        cellWidget->setStyleSheet("background-color: " + currentDayColor + ";");
                    } else {
                        // Future days — no color
                        cellWidget->setStyleSheet("");
                        check->setDisabled(true);
                    }
                }
            }
        }
    }


    // SETTTING THE CURRENT DAY PAINT:

    // Reseting the top sections to normal looks
    ui->M_monLabel->setStyleSheet("color:black; background-color:rgb(255, 255, 187)");
    ui->M_tueLabel->setStyleSheet("color:black; background-color:rgb(255, 255, 187)");
    ui->M_wedLabel->setStyleSheet("color:black; background-color:rgb(255, 255, 187)");
    ui->M_thuLabel->setStyleSheet("color:black; background-color:rgb(255, 255, 187)");
    ui->M_friLabel->setStyleSheet("color:black; background-color:rgb(255, 255, 187)");
    ui->M_satLabel->setStyleSheet("color:black; background-color:rgb(255, 255, 187)");
    ui->M_sunLabel->setStyleSheet("color:black; background-color:rgb(255, 255, 187)");


    // Setting the current date to the currentDayColor that we define in class
    switch(todaysDay){
    case 1: ui->M_monLabel->setStyleSheet("color:black; background-color:" + currentDayColor); break;
    case 2: ui->M_tueLabel->setStyleSheet("color:black; background-color:" + currentDayColor); break;
    case 3: ui->M_wedLabel->setStyleSheet("color:black; background-color:" + currentDayColor); break;
    case 4: ui->M_thuLabel->setStyleSheet("color:black; background-color:" + currentDayColor); break;
    case 5: ui->M_friLabel->setStyleSheet("color:black; background-color:" + currentDayColor); break;
    case 6: ui->M_satLabel->setStyleSheet("color:black; background-color:" + currentDayColor); break;
    case 7: ui->M_sunLabel->setStyleSheet("color:black; background-color:" + currentDayColor); break;
    }

    // Setting the date on the mainpage
    ui->M_date->setText(currentDate.toString());
}







// SLOTS FUNCTIONS:
void Habbit_tracker::onCheckmarkToggled(int state){
    QCheckBox *check = qobject_cast<QCheckBox*>(sender());                   // Saving the sender
    if (!check)
        return;

    // Loop through the table to find where the checkbox is
    for (int row = 0; row < ui->M_habitTable->rowCount(); ++row) {           // Looping through the rows
        for (int col = 1; col <= 7; ++col) {                                 // Looping through the cols
            QWidget *cellWidget = ui->M_habitTable->cellWidget(row, col);    // Casting the cellWidget
            if (cellWidget) {
                QCheckBox *cellCheck = cellWidget->findChild<QCheckBox*>();  // Grabbing the cell checkbox
                if (cellCheck == check) {                                    // If this is the sender saved above
                    allHabits[row].setDay(col - 1, state == Qt::Checked);    // Set the day
                    allHabits[row].writeToFile(); // Save update to file     // Save the file
                    loadHabits();
                    return;
                }
            }
        }
    }

}



















