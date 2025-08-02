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
#include <fstream>
#include <QRegularExpression>
#include <QColorDialog>


using namespace std;

// CONSTRUCTOR AND DESTRUCTOR
Habbit_tracker::~Habbit_tracker(){
    delete ui;
}

Habbit_tracker::Habbit_tracker(QWidget *parent): QMainWindow(parent), ui(new Ui::Habbit_tracker){
    ui->setupUi(this);
    QApplication::setStyle("Fusion");                               // Setting the style be portable
    setFixedSize(1024, 600);                                        // Setting Size of window
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);  // Removing the sizing gadget



    // Raising frames
    ui->M_removeFrame->raise();
    ui->S_keyboard->raise();
    ui->S_backgroundSelectionFrame->raise();


    // Moving frames where they are suppose to be
    ui->S_keyboard->move(QPoint(50,7));
    ui->S_backgroundSelectionFrame->move(QPoint(4,4));


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


    // Setting up the table for the backgrounds in settings
    ui->S_backgroundSelectionBox->clear();
    string backTempString;
    for(auto &item:possible_backgrounds){
        if(item == "none"){
            ui->S_backgroundSelectionBox->addItem("NONE");
            continue;
        }
        backTempString = item.toStdString();
        backTempString = backTempString.substr(backTempString.find("images/") + 7);
        backTempString = backTempString.substr(0,backTempString.size()-1);
        ui->S_backgroundSelectionBox->addItem(QString::fromStdString(backTempString));
    }


    // Setting up the display table for the settings
    QCheckBox* displayTableCompleted   = new QCheckBox(this);
    displayTableCompleted->setChecked(true);
    displayTableCompleted->setDisabled(true);
    QWidget* DTCompletedContainer = new QWidget();
    QHBoxLayout* DTCompletedLayout = new QHBoxLayout(DTCompletedContainer);
    DTCompletedLayout->addWidget(displayTableCompleted);
    DTCompletedLayout->setAlignment(Qt::AlignCenter);
    DTCompletedLayout->setContentsMargins(0, 0, 0, 0);
    DTCompletedLayout->setSpacing(0);
    ui->S_displayTable->setCellWidget(0,0,DTCompletedContainer);
    DTCompletedContainer->setStyleSheet("background: transparent;");
    DTCompletedContainer->setAttribute(Qt::WA_TranslucentBackground);


    QCheckBox* displayTableIncompleted = new QCheckBox(this);
    displayTableIncompleted->setChecked(false);
    displayTableIncompleted->setDisabled(true);
    QWidget* DTIncompletedContainer = new QWidget();
    QHBoxLayout* DTIncompletedLayout = new QHBoxLayout(DTIncompletedContainer);
    DTIncompletedLayout->addWidget(displayTableIncompleted);
    DTIncompletedLayout->setAlignment(Qt::AlignCenter);
    DTIncompletedLayout->setContentsMargins(0, 0, 0, 0);
    DTIncompletedLayout->setSpacing(0);
    ui->S_displayTable->setCellWidget(1,0,DTIncompletedContainer);
    DTIncompletedContainer->setStyleSheet("background: transparent;");
    DTIncompletedContainer->setAttribute(Qt::WA_TranslucentBackground);


    QCheckBox* displayTableCurrent = new QCheckBox(this);
    displayTableCurrent->setChecked(false);
    displayTableCurrent->setDisabled(true);
    QWidget* DTCurrentContainer = new QWidget();
    QHBoxLayout* DTCurrentLayout = new QHBoxLayout(DTCurrentContainer);
    DTCurrentLayout->addWidget(displayTableCurrent);
    DTCurrentLayout->setAlignment(Qt::AlignCenter);
    DTCurrentLayout->setContentsMargins(0, 0, 0, 0);
    DTCurrentLayout->setSpacing(0);
    ui->S_displayTable->setCellWidget(2,0,DTCurrentContainer);
    DTCurrentContainer->setStyleSheet("background: transparent;");
    DTCurrentContainer->setAttribute(Qt::WA_TranslucentBackground);




    // Connecting all the keyboard keys and Setting their Focus Policy   ------ ADDING KEYBOARD
    QList<QPushButton*> keys = ui->A_keyboard->findChildren<QPushButton*>();
    for (auto &key : keys) {
        connect(key, &QPushButton::clicked, this, &Habbit_tracker::insertKey);
        key->setFocusPolicy(Qt::NoFocus);
    }

    // Connecting all the keyboard keys and Setting their Focus Policy    ----- SETTINGS KEYBOARD
    QList<QPushButton*> keys_S = ui->S_keyboard->findChildren<QPushButton*>();
    for (auto &key : keys_S) {
        connect(key, &QPushButton::clicked, this, &Habbit_tracker::insertKey);
        key->setFocusPolicy(Qt::NoFocus);
    }

    // Setting no focus to other things
    ui->A_keyboard->setFocusPolicy(Qt::NoFocus);
    ui->A_cancelButton->setFocusPolicy(Qt::NoFocus);
    ui->A_saveButton->setFocusPolicy(Qt::NoFocus);
    ui->M_habitTable->setFocusPolicy(Qt::NoFocus);
    ui->H_spanDisplay->setFocusPolicy(Qt::NoFocus);
    ui->A_keyboard->setFocusPolicy(Qt::NoFocus);
    ui->S_keyboardToggleButton->setFocusPolicy(Qt::NoFocus);
    ui->S_displayTable->setFocusPolicy(Qt::NoFocus);



    // Connections
    connect(ui->M_addHabitButton, &QPushButton::clicked, this, &Habbit_tracker::M_addHabbitButtonClicked);
    connect(ui->M_removeButton, &QPushButton::clicked, this, &Habbit_tracker::M_removeButtonClicked);
    connect(ui->M_cancelButton, &QPushButton::clicked, this, &Habbit_tracker::M_cancelButtonClicked);
    connect(ui->M_confirmButton, &QPushButton::clicked, this, &Habbit_tracker::M_confirmButtonClicked);
    connect(ui->M_viewHistoryButton, &QPushButton::clicked, this, &Habbit_tracker::M_viewHistoryButtonClicked);
    connect(ui->M_settingsButton, &QPushButton::clicked, this, &Habbit_tracker::M_settingsButtonClicked);

    connect(ui->A_cancelButton, &QPushButton::clicked, this, &Habbit_tracker::A_cancelButtonClicked);
    connect(ui->A_saveButton, &QPushButton::clicked, this, &Habbit_tracker::A_saveButtonClicked);

    connect(ui->H_backButton, &QPushButton::clicked, this, &Habbit_tracker::H_backButtonClicked);
    connect(ui->H_downArrow, &QPushButton::clicked, this, &Habbit_tracker::H_arrowClicked);
    connect(ui->H_upArrow, &QPushButton::clicked, this, &Habbit_tracker::H_arrowClicked);
    connect(ui->H_weekButton, &QPushButton::clicked, this, &Habbit_tracker::spanButtonClicked);
    connect(ui->H_monthButton, &QPushButton::clicked, this, &Habbit_tracker::spanButtonClicked);
    connect(ui->H_yearButton, &QPushButton::clicked, this, &Habbit_tracker::spanButtonClicked);
    connect(ui->H_scrollDownButton, &QPushButton::clicked, this, &Habbit_tracker::scrollButtonClicked);
    connect(ui->H_scrollUpButton, &QPushButton::clicked, this, &Habbit_tracker::scrollButtonClicked);
    connect(ui->H_displayPreviousButton, &QPushButton::clicked, this, &Habbit_tracker::displayNextPreviousButtonClicked);
    connect(ui->H_displayNextButton, &QPushButton::clicked, this, &Habbit_tracker::displayNextPreviousButtonClicked);

    connect(ui->S_backButton, &QPushButton::clicked, this, &Habbit_tracker::S_backButtonClicked);
    connect(ui->S_addThemeButton, &QPushButton::clicked, this, &Habbit_tracker::S_addThemeButtonClicked);
    connect(ui->S_keyboardToggleButton, &QPushButton::clicked, this, &Habbit_tracker::S_keyboardToggleButtonClicked);
    connect(ui->S_savedThemesBox,&QListWidget::currentRowChanged, this, &Habbit_tracker::S_savedThemeBoxIndexChanged);
    connect(ui->S_displayRadioButton, &QRadioButton::toggled, this, &Habbit_tracker::S_displayRadioButtonToggled);
    connect(ui->S_displayMainButton, &QPushButton::clicked, this, &Habbit_tracker::S_displayMainButtonClicked);
    connect(ui->S_resetButton, &QPushButton::clicked, this, &Habbit_tracker::S_resetButtonClicked);
    connect(ui->S_renameButton, &QPushButton::clicked, this, &Habbit_tracker::S_renameButtonClicked);
    connect(ui->S_deleteButton, &QPushButton::clicked, this, &Habbit_tracker::S_deleteButtonClicked);
    connect(ui->S_duplicateButton, &QPushButton::clicked, this, &Habbit_tracker::S_duplicateButtonClicked);
    connect(ui->S_saveThemeButton, &QPushButton::clicked, this, &Habbit_tracker::S_saveThemeButtonClicked);
    connect(ui->S_setThemeButton, &QPushButton::clicked, this, &Habbit_tracker::S_setThemeButtonClicked);
    connect(ui->S_backgroundButton, &QPushButton::clicked, this, &Habbit_tracker::S_backgroundButtonClicked);
    connect(ui->S_setDefaultButton, &QPushButton::clicked, this, &Habbit_tracker::S_setDefaultButtonClicked);
    connect(ui->S_backgroundSelectionScrollUp, &QPushButton::clicked, this, &Habbit_tracker::S_backgroundSelectionScrollButtonClicked);
    connect(ui->S_backgroundSelectionScrollDown, &QPushButton::clicked, this, &Habbit_tracker::S_backgroundSelectionScrollButtonClicked);
    connect(ui->S_backgroundSelectionConfirmButton, &QPushButton::clicked, this, &Habbit_tracker::S_backSelectionConfirmButtonClicked);

    QList<QPushButton*> themeRecolorButtons = ui->S_selectedThemeFrame->findChildren<QPushButton*>();
    for (auto &button : themeRecolorButtons) {
        if(button->text() == "Select Background") continue;
        if(button->text() == "V" ||  button->text() == "^") continue;
        connect(button, &QPushButton::clicked, this, &Habbit_tracker::settingsChangeThemeButtonClicked);
    }




    // Setting up the day checker timer to keep track of what day it is
    dayCheckTimer = new QTimer(this);
    currentDate = QDate::currentDate();
    //currentDate = QDate(2025, 7, 27); // <---- Sunday
    connect(dayCheckTimer, &QTimer::timeout, this, [=]() mutable {
        QDate now = QDate::currentDate();
        //QDate now = QDate(2025, 7, 28); // <--- Monday | TESTING: moving into a new week
        if (now != currentDate) {                    // If now has moved to a different date than saved currentDate
            currentDate = now;                       // Updates the currentday to today to get ready for check of tomorrow
            if (currentDate.dayOfWeek() == 1) {      // If the new day is a MONDAY, then we need to save the previous week
                for(auto &currHabit:allHabits){
                    currHabit.saveWeek(currentDate); // Pushes week into history (vector of weeks), then resets the week bools to 0
                    currHabit.writeToFile();         // Saves the week to its file
                }
            }

            loadHabits();                            // Clears the allHabits and the habitTable and resets again from the files
        }
    });

    dayCheckTimer->start(dayCheckerInterval);       // every <intervalAmount>,  check if we are in a new day



    // Paint all Colors
    loadColorsFromFile();
    paintTheme();


    // Switching to the Main Frame
    switchFrame(ui->M_frame);


}





// FRAME CONTROL FUNCTIONS:
void Habbit_tracker::switchFrame(QFrame* target){
    ui->M_frame->hide();
    ui->A_frame->hide();
    ui->H_frame->hide();
    ui->S_frame->hide();
    if(target == ui->M_frame){
        // Hiding the removing habits frame
        ui->M_removeFrame->hide();

        // Loading and painting all habits from files
        loadHabits();
    }
    else if(target == ui->A_frame){

        target->show(); // Calling this first to be able to use it in the set capps
        // Hiding the numbers
        showNums_A = false;

        // Setting keyboard to Capital Letters
        showCapps_A = false;
        setCapps();



        // Clearing the name text
        ui->A_nameInput->setText("");
        ui->A_nameInput->setFocus();

        // Setting the note back to regular color
        ui->A_note->setStyleSheet("color:rgb" + text_color + ";");

        // Setting the image to sad face
        ui->A_face->setStyleSheet("border-image: url(:/faces/images/nuetralFace.png) 0 0 0 0 stretch stretch;");
    }
    else if(target == ui->H_frame){

        // Reseting the buttons
        ui->H_weekButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
        ui->H_monthButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
        ui->H_yearButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );

        // Resetting the Top Text and the disabled buttons if any
        if(allHabits.size() <= 0){                                                      // If there are no habits, show mess. and disable buttons
            ui->H_habitName->setText("There are currently no habits");
            ui->H_upArrow->setDisabled(true);
            ui->H_downArrow->setDisabled(true);
            ui->H_weekButton->setDisabled(true);
            ui->H_monthButton->setDisabled(true);
            ui->H_yearButton->setDisabled(true);
            ui->H_displayNextButton->setDisabled(true);
            ui->H_displayPreviousButton->setDisabled(true);
            habitIndex = -1;

            // Hiding the scroll since there is nothing
            ui->H_scrollDownButton->hide();
            ui->H_scrollUpButton->hide();

            // Setting date Text to empty
            ui->H_dateSpan->setText("");

        }
        else{                                                                           // If there ARE habits, set to first, and enable buttons
            ui->H_habitName->setText(QString::fromStdString(allHabits[0].getName()));
            ui->H_upArrow->setDisabled(true);                                  // Setting up arrow to be disabled because we are at the top already
            ui->H_downArrow->setDisabled((allHabits.size() == 1 ? true:false)); // If size = 1, then set down to disable, else to false
            ui->H_weekButton->setDisabled(false);
            ui->H_monthButton->setDisabled(false);
            ui->H_yearButton->setDisabled(false);
            habitIndex = 0;

            // Setting the Year button to active
            ui->H_yearButton->setStyleSheet("QPushButton { background-color: rgb" + button_select_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );

            // Setting the sending parameters for the update to show the YEAR
            QDate spanStart = QDate(currentDate.year(), 1, 1);
            QDate spanEnd = QDate(currentDate.year(), 12, 31);
            ui->H_dateSpan->setText(QString::number(spanStart.year()));


            // Setting the current tab
            currentTab = "year";
            // Updating the calendar
            updateSpanDisplay(spanStart, spanEnd, habitIndex);

            // Showing the scroll since we are going into the year
            ui->H_scrollDownButton->show();
            ui->H_scrollUpButton-> show();


        }







    }
    else if(target == ui->S_frame){
        // Stop the timer that checks the current date
        dayCheckTimer->stop();

        // Upload the current themes from theme folder, also hides everything on the right side
        loadThemesIntoBox();

        // Getting the current them from saved target theme file name
        string currentTheme = targetThemeFileName.substr(targetThemeFileName.find('/')+1);
        currentTheme = currentTheme.substr(0,currentTheme.find('.'));

        // Cleaning the underscores and setting them to spaces instead
        string cleanedCurrentTheme = "";
        for(auto& ch:currentTheme)
            cleanedCurrentTheme += (ch == '_' ? ' ' : ch);

        if(cleanedCurrentTheme == "default")
            cleanedCurrentTheme = "Default Theme";

        ui->S_currentTheme->setText("Current Theme: " + QString::fromStdString(cleanedCurrentTheme));

        // Hiding the keyboard until the show keyboard button is pressed and SETTTING UP THE KEYBOARD
        ui->S_keyboard->hide();
        ui->S_keyboardToggleButton->setText("Show\nKeyboard");

        target->show(); // Calling this first to be able to use in set capps
        // Resetting the variables and setting to all capps
        showNums_S = false;
        showCapps_S = false;
        setCapps();



    }
    target->show();
}












// MAIN MENU FUNCTIONS:
void Habbit_tracker::M_addHabbitButtonClicked(){
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

void Habbit_tracker::M_settingsButtonClicked(){
    switchFrame(ui->S_frame);
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

void Habbit_tracker::A_saveButtonClicked() {
    // Get the text from the name input field, returning if the string is empty or invalid
    QString habitName = ui->A_nameInput->text().trimmed();

    if (!validString(habitName)) {
        ui->A_note->setStyleSheet("color:red;");
        ui->A_nameInput->setText("");
        return;
    } else {
        ui->A_note->setStyleSheet("color:rgb" + text_color + ";");
    }


    // CHECKING IF INPUT NAME HAS ALREADY BEEN USED
    for(auto &currHabit:allHabits){
        if(currHabit.getName() == habitName.toStdString()){
            ui->A_note->setStyleSheet("color:red;");
            ui->A_nameInput->setText("");
            return;
        }
    }

    ui->A_note->setStyleSheet("color:rgb" + text_color + ";");


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
        check->setFixedSize(checkBox_width, checkBox_height);
        check->setStyleSheet("QCheckBox::indicator {width: " +
                             QString::fromStdString(to_string(checkBox_width)) + "px; height: " +
                             QString::fromStdString(to_string(checkBox_height) + "px;}"));

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
    if(habitIndex == -1) return;

    // Clearing the spanDisplay since we switched and resetting the spanButtons
    ui->H_spanDisplay->clear();
    ui->H_spanDisplay->clearSpans();
    ui->H_spanDisplay->clearSelection();
    ui->H_spanDisplay->setRowCount(0);
    ui->H_spanDisplay->setColumnCount(0);

    // Reseting the colors
    ui->H_weekButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_monthButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_yearButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );


    // If there was only one, then the two buttons were disabled in the switchframe function
    if(pressedButton == ui->H_upArrow)
        habitIndex--;
    else if(pressedButton == ui->H_downArrow)
        habitIndex++;


    ui->H_habitName->setText(QString::fromStdString(allHabits[habitIndex].getName()));
    ui->H_upArrow  ->setDisabled((habitIndex <= 0 ? true:false));
    ui->H_downArrow->setDisabled((habitIndex >= (allHabits.size()-1) ? true:false));



    // Determine current span
    QString spanText = ui->H_dateSpan->text().trimmed();
    QDate spanStart, spanEnd;

    if (currentTab == "week") {
        // Format: "MM/dd/yyyy - MM/dd/yyyy"
        QStringList parts = spanText.split(" - ");
        if (parts.size() == 2) {
            spanStart = QDate::fromString(parts[0], "MM/dd/yyyy");
            spanEnd = QDate::fromString(parts[1], "MM/dd/yyyy");
            if (spanStart.isValid() && spanEnd.isValid()) {
                updateSpanDisplay(spanStart, spanEnd, habitIndex);
            }
        }
        ui->H_weekButton->setStyleSheet("QPushButton { background-color: rgb" + button_select_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    }
    else if (currentTab == "month"){
        // Format: "Month yyyy"
        QDate date = QDate::fromString("01 " + spanText, "dd MMMM yyyy");
        if (date.isValid()) {
            spanStart = QDate(date.year(), date.month(), 1);
            spanEnd = spanStart.addMonths(1).addDays(-1);
            updateSpanDisplay(spanStart, spanEnd, habitIndex);
        }
        ui->H_monthButton->setStyleSheet("QPushButton { background-color: rgb" + button_select_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    }
    else if (currentTab == "year") {
        // Format: "yyyy"
        int year = spanText.toInt();
        spanStart = QDate(year, 1, 1);
        spanEnd = QDate(year, 12, 31);
        updateSpanDisplay(spanStart, spanEnd, habitIndex);
        ui->H_yearButton->setStyleSheet("QPushButton { background-color: rgb" + button_select_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
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

void Habbit_tracker::spanButtonClicked(){
    QPushButton *pressedButton = qobject_cast<QPushButton*>(sender());
    if (!pressedButton) return;
    if(habitIndex == -1) return;

    // NOTE: If history is empty then it will still paint everything, but it will not print any green

    // Reseting the colors for all the buttons
    ui->H_weekButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_monthButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_yearButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );

    // Setting current pressed to the color of button_select_color which is what makes it look "active"
    pressedButton->setStyleSheet("QPushButton { background-color: rgb" + button_select_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );

    QDate spanStart;
    QDate spanEnd;

    // Setting the color to be set as currently in this tab
    if(pressedButton == ui->H_weekButton){
        spanStart = currentDate.addDays(1 - currentDate.dayOfWeek());
        spanEnd = spanStart.addDays(6);

        ui->H_dateSpan->setText(spanStart.toString(dateFormat) + " - " + spanEnd.toString(dateFormat));
        currentTab = "week";
    }
    else if(pressedButton == ui->H_monthButton){
        spanStart = QDate(currentDate.year(), currentDate.month(), 1);
        spanEnd = spanStart.addMonths(1).addDays(-1);

        QString monthYear = spanStart.toString("MMMM yyyy");
        ui->H_dateSpan->setText(monthYear);

        currentTab = "month";
    }
    else if(pressedButton == ui->H_yearButton){
        spanStart = QDate(currentDate.year(), 1, 1);
        spanEnd = QDate(currentDate.year(), 12, 31);

        ui->H_dateSpan->setText(QString::number(spanStart.year()));

        currentTab = "year";
    }



    updateSpanDisplay(spanStart,spanEnd, habitIndex);

    // Showing or Hiding the arrows if the year button is the one clicked
    if(pressedButton == ui->H_yearButton){
        ui->H_scrollDownButton->show();
        ui->H_scrollUpButton->show();
    }
    else{
        ui->H_scrollDownButton->hide();
        ui->H_scrollUpButton->hide();
    }

    // Setting the next and previous buttons to be enabled
    ui->H_displayNextButton->setDisabled(false);
    ui->H_displayPreviousButton->setDisabled(false);
}

void Habbit_tracker::displayNextPreviousButtonClicked(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;


    bool isNext = false;

    if (button == ui->H_displayNextButton) {
        isNext = true;
    } else if (button == ui->H_displayPreviousButton) {
        isNext = false;
    } else {
        return; // Unknown button, do nothing
    }

    QString text = ui->H_dateSpan->text();
    QDate spanStart, spanEnd;

    // Check if it's a weekly format like "Jul 21 - Jul 27"
    if (currentTab == "week") {
        QStringList parts = text.split("-");
        if (parts.size() == 2) {
            QDate start = QDate::fromString(parts[0].trimmed(), dateFormat);
            if (start.isValid()) {
                spanStart = start.addDays(isNext ? 7 : -7);
                spanEnd = spanStart.addDays(6);
            }
        }
        ui->H_dateSpan->setText(spanStart.toString(dateFormat) + " - " + spanEnd.toString(dateFormat));
    }
    // Check if it's a monthly format like "January 2025"
    else if (currentTab == "month") {
        QDate start = QDate::fromString(text, "MMMM yyyy");
        if (start.isValid()) {
            spanStart = isNext ? start.addMonths(1) : start.addMonths(-1);
            spanEnd = spanStart.addMonths(1).addDays(-1);
        }
        QString monthYear = spanStart.toString("MMMM yyyy");
        ui->H_dateSpan->setText(monthYear);
    }
    // Otherwise assume it's a year like "2025"
    else if (currentTab == "year"){
        bool ok;
        int year = text.toInt(&ok);
        if (ok && year >= 1000 && year <= 9999) {
            int newYear = isNext ? year + 1 : year - 1;
            spanStart = QDate(newYear, 1, 1);
            spanEnd = QDate(newYear, 12, 31);
        }
        ui->H_dateSpan->setText(QString::number(spanStart.year()));
    }
    else{
        QMessageBox::critical(this, "ERORR", "Error in next/previous, currentTab is: " + QString::fromStdString(currentTab));
        return;
    }


    if (!spanStart.isValid() || !spanEnd.isValid())
        return;

    updateSpanDisplay(spanStart, spanEnd, habitIndex);
}

void Habbit_tracker::updateSpanDisplay(QDate spanStart, QDate spanEnd, int habitIndex){
    if (habitIndex < 0 || habitIndex >= allHabits.size()) return;

    habit& currentHabit = allHabits[habitIndex];

    // Clear the table
    ui->H_spanDisplay->clear();
    int rowCount = ui->H_spanDisplay->rowCount();
    int colCount = ui->H_spanDisplay->columnCount();
    for (int r = 0; r < rowCount; ++r) {
        for (int c = 0; c < colCount; ++c) {
            if (ui->H_spanDisplay->rowSpan(r, c) > 1 || ui->H_spanDisplay->columnSpan(r, c) > 1) {
                ui->H_spanDisplay->setSpan(r, c, 1, 1);
            }
        }
    }


    int columns = 0;
    int rows = 1;
    QColor completedColor_ = stringToColor(complete_color);
    QColor otherDaysColor_ = stringToColor(other_days_color);

    vector<historyWeek> allHistory = currentHabit.getHistory();
    allHistory.push_back(currentHabit.getCurrentWeek());

    if (currentTab == "week") {
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
        for (auto& hw : allHistory) {
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
                item->setBackground(completedColor_);
            } else {
                item->setBackground(otherDaysColor_);
            }
            ui->H_spanDisplay->setItem(0, i, item);
        }
    }




    else if (currentTab == "month") {
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
        for (const auto& hw : allHistory) {
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
                    item->setBackground(otherDaysColor_);
                }
                else {
                    QDate currentDay = spanStart.addDays(dayNumber - 1);
                    item = new QTableWidgetItem(QString::number(dayNumber));
                    item->setTextAlignment(Qt::AlignCenter);

                    if (progressMap.count(currentDay) && progressMap[currentDay]) {
                        item->setBackground(completedColor_);
                    }
                    else {
                        item->setBackground(otherDaysColor_);
                    }

                    dayNumber++;
                }

                ui->H_spanDisplay->setItem(row, col, item);
            }
        }
    }



    else if (currentTab == "year"){
        ui->H_spanDisplay->clear();

        // === Configurable colors ===

        QColor monthHeaderColor = stringToColor(month_header_color);    // Month Header
        QColor weekdayHeaderColor = stringToColor(week_header_color);   // Slightly darker for weekdays
        QColor spacerColor = stringToColor(week_header_color);         // Space Color (right now same color as the background)
        QColor dateBackgroundColor = stringToColor(other_days_color); // Default white for days
        QColor completedColor = stringToColor(complete_color);          // Green for completed

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
        for (const auto& hw : allHistory) {
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
                        item->setBackground(dateBackgroundColor);
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
    else{
        QMessageBox::critical(this, "ERORR", "Error in next/previous, updateSpanDisplay is: " + QString::fromStdString(currentTab));
        return;
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





// SETTINGS FRAME FUNCTIONS:
void Habbit_tracker::S_backButtonClicked(){
    // Start up the timer again
    dayCheckTimer->start(dayCheckerInterval);
    switchFrame(ui->M_frame);
}

void Habbit_tracker::S_keyboardToggleButtonClicked(){
    if(ui->S_keyboard->isVisible()){
        ui->S_keyboard->hide();
        ui->S_keyboardToggleButton->setText("Show\nKeyboard");
    }
    else{
        ui->S_keyboard->show();
        ui->S_keyboardToggleButton->setText("Hide\nKeyboard");
    }
}

void Habbit_tracker::S_resetButtonClicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Resetting Changes", "Are you sure you want to delete changes made to this theme?",
                                  QMessageBox::Yes | QMessageBox::No);

    // Returning if user decided to cancel the reset
    if (reply == QMessageBox::No) {
        return;
    }

    S_savedThemeBoxIndexChanged();
}

void Habbit_tracker::S_renameButtonClicked(){
    string savingName = ui->S_renameInput->text().toStdString();


    // Checking if the name did not change at all
    if(ui->S_renameInput->text() == ui->S_savedThemesBox->currentItem()->text())
        return;

    // Checking if the format is correct and if the string is empty
    if(!validString(QString::fromStdString(savingName))){
        QMessageBox::information(this, "Name is invalid.","Please fix name format. Name has to have at least 1 character. Valid Characters: Numbers, Letters, Spaces.");
        ui->S_renameInput->setText(ui->S_savedThemesBox->currentItem()->text());
        return;
    }

    // Making sure they cant rename it to default
    if(savingName == "default"){
        QMessageBox::information(this, "Name Cannot Be Default","Theme name cannot be default. This file name is used in the backend for other reasons.");
        ui->S_renameInput->setText(ui->S_savedThemesBox->currentItem()->text());
        return;
    }

    // Making sure they cant rename it to selectedTheme
    if(savingName == "selectedTheme"){
        QMessageBox::information(this, "Name Cannot Be selectedTheme","Theme name cannot be selectedTheme. This file name is used in the backend for other reasons.");
        ui->S_renameInput->setText(ui->S_savedThemesBox->currentItem()->text());
        return;
    }

    // Checking if this name has been used before
    int currentRow = ui->S_savedThemesBox->currentRow();
    int count = ui->S_savedThemesBox->count();

    for (int i = 0; i < count; ++i) {
        if (i == currentRow) continue; // Skip the currently selected item

        QListWidgetItem* item = ui->S_savedThemesBox->item(i);
        if (item->text().toLower() == QString::fromStdString(savingName).toLower()) {
            QMessageBox::information(this, "Theme Name is Used.",
                                     "This theme name has already been used. Please choose a different name.");
            ui->S_renameInput->setText(ui->S_savedThemesBox->currentItem()->text());
            return;
        }
    }

    // Changing the name of the theme file now that it passed all checks
    string savingFileName = "";
    for(auto &ch:savingName)
        savingFileName += (ch == ' ' ? '_':ch);
    savingFileName = themesPath + "/" + savingFileName + ".txt";


    // Getting original file name
    string originalName = ui->S_savedThemesBox->currentItem()->text().toStdString();
    string originalFileName = "";
    for(auto& ch:originalName)
        originalFileName += (ch == ' ' ? '_':ch);
    originalFileName = themesPath + "/" + originalFileName + ".txt";


    // Rename the file
    try {
        filesystem::rename(originalFileName, savingFileName);
    } catch (const filesystem::filesystem_error& e) {
        QMessageBox::critical(this,"ERORR RENAMING FILE", "Could not rename the file. Please contact ema.");
        ui->S_renameInput->setText(ui->S_savedThemesBox->currentItem()->text());
        return;
    }

    // Reload the themes box
    loadThemesIntoBox();

}

void Habbit_tracker::loadThemesIntoBox(){
    // Making sure that the folder exists
    if (!filesystem::exists(themesPath)) {
        QMessageBox::critical(this, "ERROR", "Theme folder does not exist. Please contact ema for a reset.");
        S_backButtonClicked();
        return;
    }


    ui->S_savedThemesBox->clear();
    // Adding all the files into the theme box after parsing/cleaning their names
    for (const auto& entry : filesystem::recursive_directory_iterator(themesPath)) {
        if (entry.is_regular_file()) {
            string habitNameWith_ = entry.path().filename().string();
            habitNameWith_ = habitNameWith_.substr(0, habitNameWith_.length() - 4);

            string habitName = "";
            for (auto& ch : habitNameWith_) {
                habitName += (ch == '_') ? ' ' : ch;
            }

            // Checking if its the selectedTheme file
            if(habitName == "selectedTheme" || habitName == "default")
                continue;

            ui->S_savedThemesBox->addItem(QString::fromStdString(habitName));
        }
    }


    // Setting the index to -1 so that nothing is being selected at the moment
    ui->S_savedThemesBox->setCurrentRow(-1);


    // Showing scroll buttons when they are needed and hidding when not
    if(ui->S_savedThemesBox->count() > 9){
        ui->S_scrollUpButton->show();
        ui->S_scrollDownButton->show();
    }else{
        ui->S_scrollUpButton->hide();
        ui->S_scrollDownButton->hide();
    }

    // Hiding the whole selected theme section (rigth section)
    ui->S_selectedThemeTitle->hide();
    ui->S_selectedThemeFrame->hide();
    ui->S_selectedThemeDemoTitle->hide();
    ui->S_selectedThemeDemoFrame->hide();
    ui->S_saveThemeButton->hide();
    ui->S_setThemeButton->hide();
    ui->S_resetButton->hide();
    ui->S_backgroundSelectionFrame->hide();


    // Disabeling the buttons that would need an index from the themeselectionbox
    ui->S_renameButton->setDisabled(true);
    ui->S_deleteButton->setDisabled(true);
    ui->S_duplicateButton->setDisabled(true);
    ui->S_renameInput->setDisabled(true);

    // Cleaning the input for the rename section
    ui->S_renameInput->setText("");

}

void Habbit_tracker::S_addThemeButtonClicked(){
    // Catching when the new theme name is invalid
    if(!validString(ui->S_addThemeInput->text())){
        QMessageBox::information(this, "Name is invalid.","Please fix name format. Name has to have at least 1 character. Valid Characters: Numbers, Letters, Spaces.");
        ui->S_addThemeInput->setText("");
        return;
    }


    // Name was valid, replacing the spaces with underscores
    string currentThemeName = ui->S_addThemeInput->text().toStdString();
    string cleanedThemeName = "";
    for(auto& ch: currentThemeName)
        cleanedThemeName += (ch == ' ' ? '_':ch);


    // Catching when the name is already used
    if(!ui->S_savedThemesBox->findItems(QString::fromStdString(currentThemeName), Qt::MatchFixedString).isEmpty()){
        QMessageBox::information(this, "Theme Name is Used.","This theme name has already been used. Please choose a different name.");
        ui->S_addThemeInput->setText("");
        return;
    }


    // Catching when the name is "Default" since we never want to rewrite that file
    if(cleanedThemeName == "default"){
        QMessageBox::information(this, "Name Cannot Be Default","Theme name cannot be default. This file name is used in the backend for other reasons.");
        ui->S_addThemeInput->setText("");
        return;
    }

    // Making sure they cant rename it to selectedTheme
    if(cleanedThemeName == "selectedTheme"){
        QMessageBox::information(this, "Name Cannot Be selectedTheme","Theme name cannot be selectedTheme. This file name is used in the backend for other reasons.");
        ui->S_addThemeInput->setText("");
        return;
    }


    // Make a copy of the default file and name it to the cleanedThemeName
    ofstream newThemeFile(themesPath + "/" + cleanedThemeName + ".txt");
        newThemeFile << "#WARNING: Do not put spaces inside the ()" << endl;
        newThemeFile << endl;
        newThemeFile << "#Main Colors:" << endl;
        newThemeFile << "main_darker_color=(255,255,187)" << endl;
        newThemeFile << "main_lighter_color=(253,255,222)" << endl;
        newThemeFile << "text_color=(0,0,0)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Buttons Colors:" << endl;
        newThemeFile << "button_color=(255,233,176)" << endl;
        newThemeFile << "button_select_color=(255,171,69)" << endl;
        newThemeFile << "button_disab_color=(160,160,160)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Add Habit Colors:" << endl;
        newThemeFile << "keyboard_color=(255,170,0)" << endl;
        newThemeFile << "cancel_button_color=(255,126,126)" << endl;
        newThemeFile << "save_button_color=(165,255,171)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Main Display Colors:" << endl;
        newThemeFile << "current_day_color=(255,203,160)" << endl;
        newThemeFile << "is_checked_color=(168,230,163)" << endl;
        newThemeFile << "not_checked_color=(245,163,163)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Calendar Colors:" << endl;
        newThemeFile << "month_header_color=(255,171,69)" << endl;
        newThemeFile << "week_header_color=(230,230,230)" << endl;
        newThemeFile << "complete_color=(100,200,100)" << endl;
        newThemeFile << "other_days_color=(255,255,255)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Remove Habit Colors:" << endl;
        newThemeFile << "remove_item_selec_color=(255,203,160)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Background Image:" << endl;
        newThemeFile << "background_image=none" << endl;
    newThemeFile.close();

    // Resetting the input section for the new theme
    ui->S_addThemeInput->setText("");

    // Sending to loadThemes so that it can be loaded
    loadThemesIntoBox();
}

void Habbit_tracker::S_displayRadioButtonToggled(){
    if(ui->S_displayRadioButton->isChecked()){
        ui->S_displayMainButton->setDisabled(true);
    }
    else{
        ui->S_displayMainButton->setDisabled(false);
    }
}

void Habbit_tracker::S_displayMainButtonClicked(){
    if(ui->S_displayMainButton->isChecked()){
        ui->S_displayMainButton->setStyleSheet("QPushButton { background-color: rgb" + T_button_select_color + "; } QPushButton:disabled { background-color: rgb" + T_button_disab_color + ";}" );
    }else{
        ui->S_displayMainButton->setStyleSheet("QPushButton { background-color: rgb" + T_button_color + "; } QPushButton:disabled { background-color: rgb" + T_button_disab_color + ";}" );
    }
}

void Habbit_tracker::S_savedThemeBoxIndexChanged(){
    // Catching when the index changes to nothing
    if(ui->S_savedThemesBox->currentRow() < 0){
        // Reset the saved theme box, will also hide the right section, and clear everything from the screen
        loadThemesIntoBox();
        return;
    }

    // Hiding again the background selection frame
    ui->S_backgroundSelectionFrame->hide();

    // Attempting to open the file with the given name
    // Making theme name into the file name
    string themeName = ui->S_savedThemesBox->currentItem()->text().toStdString();
    string themeFileName = "";
    for(auto& ch:themeName){
        themeFileName += (ch == ' ' ? '_':ch);
    }

    themeFileName = themesPath + "/" + themeFileName + ".txt";

    ifstream themeFile(themeFileName);
    if(!themeFile){
        QMessageBox::critical(this,"ERROR","Could not open the selected file. Please contact ema.");
        loadThemesIntoBox();
        return;
    }






    QString errorString = "";
    string tempString = "";
    string holdingFullString = "";
    // Saving all the file variables into the above temp. variables
    try{
        tempString = "[none]";
        int count = 1;

        while(getline(themeFile, tempString)){
            holdingFullString = tempString;
            if(tempString.empty())              // Skipping empty lines
                continue;
            if(tempString[0] == '#')            // Skipping lines with # in front
                continue;


            tempString = tempString.substr(tempString.find('=') + 1); // Removing everything up to inclusive: "="
            switch(count){
            case 1 :T_main_darker_color       = QString::fromStdString(tempString);
            case 2 :T_main_lighter_color      = QString::fromStdString(tempString);
            case 3: T_text_color              = QString::fromStdString(tempString);
            case 4 :T_button_color            = QString::fromStdString(tempString);
            case 5 :T_button_select_color     = QString::fromStdString(tempString);
            case 6 :T_button_disab_color      = QString::fromStdString(tempString);
            case 7 :T_keyboard_color          = QString::fromStdString(tempString);
            case 8 :T_cancel_button_color     = QString::fromStdString(tempString);
            case 9 :T_save_button_color       = QString::fromStdString(tempString);
            case 10:T_current_day_color       = QString::fromStdString(tempString);
            case 11:T_is_checked_color        = QString::fromStdString(tempString);
            case 12:T_not_checked_color       = QString::fromStdString(tempString);
            case 13:T_month_header_color      = QString::fromStdString(tempString);
            case 14:T_week_header_color       = QString::fromStdString(tempString);
            case 15:T_complete_color          = QString::fromStdString(tempString);
            case 16:T_other_days_color        = QString::fromStdString(tempString);
            case 17:T_remove_item_selec_color = QString::fromStdString(tempString);
            case 18:T_background_image        = QString::fromStdString(tempString);
            }

            count++;
            if(count == 19) // Not checking the background image since we already checked that at the end of this function
                continue;


            // Validating the string that was saved | Format (x[xx],x[xx],x[xx]) x be 1-3 characters
            if(tempString.empty()){
                errorString = "<string was empty>";
                throw 1;
            }

            if(tempString[0] != '('){
                errorString = "missing (";
                throw 1;
            }



            if(tempString.find(',') == tempString.npos){
                errorString = "missing first comma";
                throw 1;
            }

            errorString = "converting FIRST number";
            int tmp = stoi(tempString.substr(1,tempString.find(',')));
            tempString = tempString.substr(tempString.find(',') + 1);
            errorString = "";


            if(tempString.find(',') == tempString.npos){
                errorString = "missing second comma";
                throw 1;
            }
            errorString = "converting SECOND number";
            tmp = stoi(tempString.substr(0,tempString.find(',')));
            tempString = tempString.substr(tempString.find(',') + 1);
            errorString = "";


            if(tempString.find(')') == tempString.npos){
                errorString = "missing )";
                throw 1;
            }

            errorString = "converting THIRD number";
            tmp = stoi(tempString.substr(0,tempString.find(')')));
            tempString = tempString.substr(tempString.find(')') + 1);
            errorString = "";

            if(!tempString.empty()){
                errorString = "string was not empty after reading.";
                throw 1;
            }


        }
        // Checking if background_image is a valid image:
        if(!possible_backgrounds.contains(T_background_image)){
            errorString = "background image not found";
            throw 1;
        }

        themeFile.close();

    }
    catch(...){    // IF ANYTHING GOES WRONG DURING READING, close and do not change anything in screen
        themeFile.close();
        QMessageBox::critical(this, "Theme file Error", "Theme file was corrupted, Please contact ema. ERROR: " + errorString +
                              ", TMPSTRING: [" + QString::fromStdString(tempString) + "], FULLSTRING: [" + QString::fromStdString(holdingFullString) + "]");
        loadThemesIntoBox();
        return;
    }



    // Setting the renaming text
    ui->S_renameInput->setText(QString::fromStdString(themeName));

    // Enabling the buttons
    ui->S_renameButton->setDisabled(false);
    ui->S_duplicateButton->setDisabled(false);
    ui->S_deleteButton->setDisabled(false);
    ui->S_renameInput->setDisabled(false);

    // Showing the right side
    ui->S_selectedThemeTitle->show();
    ui->S_selectedThemeFrame->show();
    ui->S_selectedThemeDemoTitle->show();
    ui->S_selectedThemeDemoFrame->show();
    ui->S_saveThemeButton->show();
    ui->S_setThemeButton->show();
    ui->S_resetButton->show();

    paintDemo(); // This paints the demo with the saved information

}

void Habbit_tracker::paintDemo(){
    // UPDATING THE DISPLAY DEMO ----------------------------------------------------------------------------------------------------
    // Setting the main background color
    ui->S_selectedThemeDemoFrame->setStyleSheet("background-color: rgb" + T_main_lighter_color + "; color:rgb" + T_text_color + ";");
    ui->S_selectedThemeDemoTitle->setStyleSheet("background-color: rgb" + T_main_darker_color + "; color:rgb" + T_text_color + ";");

    // Starting the demo with the radio button for the disabled to be inactive
    ui->S_displayRadioButton->setChecked(false);

    // Painting the display sections of the theme information (easy ones)
    ui->S_displayCancelButton->setStyleSheet("background-color: rgb" + T_cancel_button_color + "; background-image: none;"); // Cancel Buttton
    ui->S_displaySaveButton->setStyleSheet("background-color: rgb" + T_save_button_color + ";");                             // Save Button
    ui->S_displayKeyboardButton->setStyleSheet("background-color: rgb" + T_keyboard_color + ";");                            // Keyboard Button
    ui->S_displaySelection->setStyleSheet("QListWidget { background-color: rgb" + T_main_darker_color + "; }"                // Selection List
                                                                                                        "QListWidget::item:selected { background-color: rgb" + T_remove_item_selec_color + "; color:rgb" + text_color + ";}"
                                                                        "QListWidget::item:hover { background-color: rgb" + T_remove_item_selec_color + "; }");
    ui->S_displayMainButton->setStyleSheet("QPushButton { background-color: rgb" + T_button_color + "; } QPushButton:disabled { background-color: rgb" + T_button_disab_color + "}"); // Main Button (when is enabled)

    // Painting the background image into the displayBackgroundImage
    string backgroundUrl_string = T_background_image.toStdString();
    if(backgroundUrl_string == "none"){ // If the image is none, then set it to the image of none
        ui->S_displayBackground->setStyleSheet("border-image:url(:/none/images/none.png);"); // Background Image
        ui->S_backgroundButton->setText("NONE");
    }
    else{ // If there is an actual background image selected, then parse for it and set the displaybackground using BORDER-IMAGE to show the full image
        backgroundUrl_string = backgroundUrl_string.substr(backgroundUrl_string.find(':')+1);
        ui->S_displayBackground->setStyleSheet("border-image:" + QString::fromStdString(backgroundUrl_string)); // Background Image

        // Getting only the name of the file with the .png tag at the end for cleaner presentation
        backgroundUrl_string = backgroundUrl_string.substr(backgroundUrl_string.find("images/") + 7);
        backgroundUrl_string = backgroundUrl_string.substr(0,backgroundUrl_string.size()-1);
        ui->S_backgroundButton->setText(QString::fromStdString(backgroundUrl_string));
    }


    // Painting the display section of the QTableWidget ------------------------------------------------------

    // Chaning the colors of the checkmarks that represent the ones in the home screen
    QWidget* cellWidget1 = ui->S_displayTable->cellWidget(0,0);
    cellWidget1->setStyleSheet("background-color: rgb" + T_is_checked_color + ";");

    QWidget* cellWidget2 = ui->S_displayTable->cellWidget(1,0);
    cellWidget2->setStyleSheet("background-color: rgb" + T_not_checked_color + ";");

    QWidget* cellWidget3 = ui->S_displayTable->cellWidget(2,0);
    cellWidget3->setStyleSheet("background-color: rgb" + T_current_day_color + ";");



    // Changing the colors of the calendar section
    QTableWidgetItem* calendarComplete = ui->S_displayTable->item(0, 1);
    QTableWidgetItem* calendarIncomplete= ui->S_displayTable->item(1, 1);
    QTableWidgetItem* calendarMonth = ui->S_displayTable->item(3, 1);
    QTableWidgetItem* calendarWeekday = ui->S_displayTable->item(4, 1);

    calendarComplete->setBackground(stringToColor(T_complete_color));
    calendarIncomplete->setBackground(stringToColor(T_other_days_color));
    calendarMonth->setBackground(stringToColor(T_month_header_color));
    calendarWeekday->setBackground(stringToColor(T_week_header_color));

    // Changing the background image on the table and setting the grid lines variable
    ui->S_displayTable->setStyleSheet("background-color: rgb" + T_main_lighter_color + ";" + T_background_image + ";");
    ui->S_displayTable->setShowGrid(showGrid);

}

void Habbit_tracker::settingsChangeThemeButtonClicked(){
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    // If the button pressed was the background image button, then show something different and return
    if(clickedButton == ui->S_backgroundButton){
        // Show the list of the avaliable images
        return;
    }


    // Show the message that gets the color from the user and save that
    QColor inputColor = QColorDialog::getColor(Qt::white,this,"Select a color.");
    if(!inputColor.isValid()){
        QMessageBox::critical(this,"No Color Chosen","There was no color chosen from input.");
        return;
    }


    // Parse the saved color to get it in the format of (x[xx],x[xx],x[xx]) -- (x,x,x) ; x ranges from lit. amount of 1-3
    int r = inputColor.red();
    int g = inputColor.green();
    int b = inputColor.blue();
    QString inputColorQString = QString("(%1,%2,%3)").arg(r).arg(g).arg(b);



    // Finding out which button is pressed and setting the screen display accordingly
    if(clickedButton == ui->S_mainDarkerButton)               T_main_darker_color = inputColorQString;
    else if(clickedButton == ui->S_mainLighterButton)         T_main_lighter_color = inputColorQString;
    else if(clickedButton == ui->S_textColorButton)           T_text_color = inputColorQString;
    else if(clickedButton == ui->S_allButtonsButton)          T_button_color = inputColorQString;
    else if(clickedButton == ui->S_selectedButtonsButton)     T_button_select_color = inputColorQString;
    else if(clickedButton == ui->S_disableButtonsButton)      T_button_disab_color = inputColorQString;
    else if(clickedButton == ui->S_keyboardButton)            T_keyboard_color = inputColorQString;
    else if(clickedButton == ui->S_cancelButton)              T_cancel_button_color = inputColorQString;
    else if(clickedButton == ui->S_saveButton)                T_save_button_color = inputColorQString;
    else if(clickedButton == ui->S_itemSelectedButton)        T_remove_item_selec_color = inputColorQString;
    else if(clickedButton == ui->S_homeCurrentButton)         T_current_day_color = inputColorQString;
    else if(clickedButton == ui->S_homeCompletedButton)       T_is_checked_color = inputColorQString;
    else if(clickedButton == ui->S_homeIncompletedButton)     T_not_checked_color = inputColorQString;
    else if(clickedButton == ui->S_calendarCompletedButton)   T_complete_color = inputColorQString;
    else if(clickedButton == ui->S_calendarIncompletedButton) T_other_days_color = inputColorQString;
    else if(clickedButton == ui->S_calendarMonthButton)       T_month_header_color = inputColorQString;
    else if(clickedButton == ui->S_calendarWeekdayButton)     T_week_header_color = inputColorQString;



    // Paint the demo with the new information, since some information interlaps with other widgets
    paintDemo();
}

void Habbit_tracker::S_setDefaultButtonClicked(){
    // Confirm with the user that we are going to switch the current set theme
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Setting Program Theme", "Are you sure you want to set the theme to 'DEFAULT THEME'?",
                                  QMessageBox::Yes | QMessageBox::No);

    // Returning if user decided to cancel the the setting to default
    if (reply == QMessageBox::No) {
        return;
    }

    // Saving the new current theme
    ofstream newSelectedTheme(currentThemeFileName);
    newSelectedTheme << defaultColorsFileName;
    newSelectedTheme.close();

    // Loading the data from the file
    loadColorsFromFile();

    // Re-painting the program
    paintTheme();

    // Re-sending back into this frame to fix up everything
    switchFrame(ui->S_frame);

}

void Habbit_tracker::S_deleteButtonClicked(){
    // Just in case to make sure that we have selected an item
    if(ui->S_savedThemesBox->currentRow() == -1){
        return;
    }

    // Confirm with the user that we are going to delete the selected theme
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Deleting theme", "Are you sure you want to delete '" + ui->S_savedThemesBox->currentItem()->text() + "'?",
                                  QMessageBox::Yes | QMessageBox::No);

    // Returning if user decided to cancel the deletion
    if (reply == QMessageBox::No) {
        return;
    }

    // Making the file Name
    string themeName = ui->S_savedThemesBox->currentItem()->text().toStdString();
    string themeFileName = "";
    for(auto&ch:themeName)
        themeFileName += (ch==' ' ? '_':ch);
    themeFileName = themesPath + "/" + themeFileName + ".txt";


    // Checking if we are removing the current theme that we have set
    if(targetThemeFileName == themeFileName){
        ofstream resettingThemeFile(currentThemeFileName);
        resettingThemeFile << defaultColorsFileName;
        resettingThemeFile.close();
        QMessageBox::information(this,"Setting theme to default.","Removed theme was the set theme. Changing set theme back to default theme");
        loadColorsFromFile();
        paintTheme();
    }

    // Removing the file
    if(filesystem::exists(themeFileName))
        filesystem::remove(themeFileName);

    // Reloading the themes from the files
    switchFrame(ui->S_frame);
}

void Habbit_tracker::S_duplicateButtonClicked(){
    if(ui->S_savedThemesBox->currentRow() < 0)
        return;

    // Getting the name of the current theme in the qlistwidget that is selected
    string themeName = ui->S_savedThemesBox->currentItem()->text().toStdString();
    string themeFileName = "";
    for(auto &ch:themeName)
        themeFileName += (ch == ' ' ? '_':ch);


    // Adding this to the end of the file name to show its a duplicate
    string timestamp = QDateTime::currentDateTime().toString("MddyyHmmss").toStdString();
    string duplicateFileName = themesPath + "/" + themeFileName + "_DUPLI_" + timestamp + ".txt";

    // Making the file name an actual path of the original file
    themeFileName = themesPath + "/" + themeFileName +  ".txt";

    // Copying everything from original file to the new file
    ifstream originalFile(themeFileName);
    ofstream newFile(duplicateFileName);
    try{
        if(!originalFile) throw 0;
        if(!newFile) throw 0;

        string tempString;
        while(getline(originalFile,tempString))
            newFile << tempString << endl;

        originalFile.close();
        newFile.close();
    }
    catch(...){
        if(originalFile) originalFile.close();
        if(newFile) newFile.close();
        QMessageBox::critical(this,"ERROR DUPLICATING","Could not duplicate file, contact ema.");
        return;
    }

    // Reloading the themes box to grab new box
    loadThemesIntoBox();
}

void Habbit_tracker::S_saveThemeButtonClicked(){
    // This function ONLY saves the theme from the temp values into the theme file
    if(ui->S_savedThemesBox->currentRow() < 0)
        return;

    // Getting the filename of the theme
    string themeName = ui->S_savedThemesBox->currentItem()->text().toStdString();
    string themeFileName = "";
    for(auto& ch : themeName)
        themeFileName += (ch == ' ' ? '_':ch);
    themeFileName = themesPath + "/" + themeFileName + ".txt";



    // Rewritting the file that is selected using the T_ values that have been using for the demo
    ofstream themeFile(themeFileName);
    if(!themeFile){
        QMessageBox::critical(this,"ERROR SAVING FILE", "There was an error in saving the file, please conctact ema.");
        switchFrame(ui->S_frame);
        return;
    }

    // Writting the file
    themeFile << "#WARNING: Do not put spaces inside the ()" << endl;
    themeFile << endl;
    themeFile << "#Main Colors:" << endl;
    themeFile << "main_darker_color=" << T_main_darker_color.toStdString() << endl;
    themeFile << "main_lighter_color=" << T_main_lighter_color.toStdString() << endl;
    themeFile << "text_color=" << T_text_color.toStdString() << endl;
    themeFile << endl;
    themeFile << "#Buttons Colors:" << endl;
    themeFile << "button_color=" << T_button_color.toStdString() << endl;
    themeFile << "button_select_color=" << T_button_select_color.toStdString() << endl;
    themeFile << "button_disab_color=" << T_button_disab_color.toStdString() << endl;
    themeFile << endl;
    themeFile << "#Add Habit Colors:" << endl;
    themeFile << "keyboard_color=" << T_keyboard_color.toStdString() << endl;
    themeFile << "cancel_button_color=" << T_cancel_button_color.toStdString() << endl;
    themeFile << "save_button_color=" << T_save_button_color.toStdString() << endl;
    themeFile << endl;
    themeFile << "#Main Display Colors:" << endl;
    themeFile << "current_day_color=" << T_current_day_color.toStdString() << endl;
    themeFile << "is_checked_color=" << T_is_checked_color.toStdString() << endl;
    themeFile << "not_checked_color=" << T_not_checked_color.toStdString() << endl;
    themeFile << endl;
    themeFile << "#Calendar Colors:" << endl;
    themeFile << "month_header_color=" << T_month_header_color.toStdString() << endl;
    themeFile << "week_header_color=" << T_week_header_color.toStdString() << endl;
    themeFile << "complete_color=" << T_complete_color.toStdString() << endl;
    themeFile << "other_days_color=" << T_other_days_color.toStdString() << endl;
    themeFile << endl;
    themeFile << "#Remove Habit Colors:" << endl;
    themeFile << "remove_item_selec_color=" << T_remove_item_selec_color.toStdString() << endl;
    themeFile << endl;
    themeFile << "#Background Image:" << endl;
    themeFile << "background_image=" << T_background_image.toStdString() << endl;

    themeFile.close();

    QMessageBox::information(this, "Successfully Saved Theme.", "Theme has been saved into files.");

    if(themeFileName == targetThemeFileName){
        loadColorsFromFile();
        paintTheme();
    }

}

void Habbit_tracker::S_setThemeButtonClicked(){
    // Saving the theme to the file
    S_saveThemeButtonClicked();

    // Setting the target theme file
    ofstream themeSelectionFile(currentThemeFileName);
    if(!themeSelectionFile){
        QMessageBox::critical(this, "ERROR OPENING THEME SELEC FILE.","Could not open theme selection file. Please restart program.");
        return;
    }

    // Getting the new target file
    string themeName = ui->S_savedThemesBox->currentItem()->text().toStdString();
    string themeFileName = "";
    for(auto& ch : themeName)
        themeFileName += (ch == ' ' ? '_':ch);
    themeFileName = themesPath + "/" + themeFileName + ".txt";


    // Writting the new selected theme into the currentThemeFileName file
    themeSelectionFile << themeFileName << endl;
    themeSelectionFile.close();

    // Loading theme from file (which is the file above now)
    loadColorsFromFile();

    // Repainting the program (using the target theme)
    paintTheme();

    // Resetting completely the frame
    switchFrame(ui->S_frame);
}

void Habbit_tracker::S_backgroundButtonClicked(){
    ui->S_backgroundSelectionFrame->show();
    int indexOfSelectedBackground = 0;
    for(auto &item : possible_backgrounds){
        if(item == T_background_image)
            break;
        indexOfSelectedBackground++;
    }

    ui->S_backgroundSelectionBox->setCurrentRow(indexOfSelectedBackground);
}

void Habbit_tracker::S_backSelectionConfirmButtonClicked(){
    if(ui->S_backgroundSelectionBox->currentRow() == -1)
        return;

    // Checking if the chosen value was none
    QString userChoice = ui->S_backgroundSelectionBox->currentItem()->text();
    if(userChoice == "NONE")
        T_background_image = "none"; // Setting the temp value to "none"
    else
        T_background_image = "background-image:url(:/backgrounds/images/" + userChoice + ")"; // Setting the temp value for the background

    // Painting the demo again
    paintDemo();

    // Hiding again the background selection frame
    ui->S_backgroundSelectionFrame->hide();
}

void Habbit_tracker::S_backgroundSelectionScrollButtonClicked(){
    QObject* btn = sender();
    if (!btn) return;

    int currentValue = ui->S_backgroundSelectionBox->verticalScrollBar()->value();
    int step = 2;


    if (btn == ui->S_backgroundSelectionScrollDown) {
        ui->S_backgroundSelectionBox->verticalScrollBar()->setValue(currentValue + step);
    } else if (btn == ui->S_backgroundSelectionScrollUp) {
        ui->S_backgroundSelectionBox->verticalScrollBar()->setValue(currentValue - step);
    }
}









// HELPER FUNCTIONS:
bool Habbit_tracker::validString(QString input) {
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

void Habbit_tracker::loadHabits() {
    ui->M_habitTable->clear();
    allHabits.clear();

    if (!filesystem::exists(filesPath)) {
        filesystem::create_directories(filesPath);
    }

    for (const auto& entry : filesystem::recursive_directory_iterator(filesPath)) {
        if (entry.is_regular_file()) {
            string habitNameWith_ = entry.path().filename().string();
            habitNameWith_ = habitNameWith_.substr(0, habitNameWith_.length() - 4);

            string habitName = "";
            for (auto& ch : habitNameWith_) {
                habitName += (ch == '_') ? ' ' : ch;
            }

            habit newHabit(habitName, filesPath);
            string currentFileResults = newHabit.makeFromFile();

            if (!currentFileResults.empty()) {
                QMessageBox::critical(this, "ERROR: Will not add [" + QString::fromStdString(habitName) + "]",
                                      QString::fromStdString(currentFileResults));
            } else {
                allHabits.push_back(newHabit);
            }
        }
    }


    int habitCount = 0;
    for (auto& currHabit : allHabits) {
        array<bool, 7> week = currHabit.getWeek();
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(currHabit.getName()));
        ui->M_habitTable->setItem(habitCount, 0, nameItem);

        for (int col = 1; col <= 7; ++col) {
            QCheckBox* check = new QCheckBox(this);
            check->setChecked(week[col - 1]);

            check->setFixedSize(checkBox_width, checkBox_height);
            check->setStyleSheet("QCheckBox::indicator {width: " +
                                 QString::fromStdString(to_string(checkBox_width)) + "px; height: " +
                                 QString::fromStdString(to_string(checkBox_height) + "px;}"));

            connect(check, &QCheckBox::stateChanged, this, &Habbit_tracker::onCheckmarkToggled);

            QWidget* container = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(container);
            layout->addWidget(check);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            ui->M_habitTable->setCellWidget(habitCount, col, container);
        }
        habitCount++;
    }



    QString colorString = main_lighter_color;
    colorString.remove('(');
    colorString.remove(')');

    QStringList rgbList = colorString.split(',');

    if (rgbList.size() == 3) {
        int r = rgbList[0].trimmed().toInt();
        int g = rgbList[1].trimmed().toInt();
        int b = rgbList[2].trimmed().toInt();

        QColor habitBackgroundColor(r, g, b);
        int colCount = ui->M_habitTable->columnCount();

        for (int row = 0; row < initRowCount; ++row) {
            for (int col = 0; col < colCount; ++col) {
                if (col == 0) {
                    QTableWidgetItem* item = ui->M_habitTable->item(row, col);
                    if (!item) {
                        item = new QTableWidgetItem();
                        item->setTextAlignment(Qt::AlignCenter);
                        ui->M_habitTable->setItem(row, col, item);
                    }
                    item->setBackground(habitBackgroundColor);
                } else {
                    QWidget* cellWidget = ui->M_habitTable->cellWidget(row, col);
                    if (!cellWidget) {
                        cellWidget = new QWidget();
                        QHBoxLayout* layout = new QHBoxLayout(cellWidget);
                        layout->setContentsMargins(0, 0, 0, 0);
                        layout->setSpacing(0);
                        layout->addStretch();
                        ui->M_habitTable->setCellWidget(row, col, cellWidget);
                    }
                    cellWidget->setStyleSheet("background: transparent;");
                    cellWidget->setAttribute(Qt::WA_TranslucentBackground);
                }
            }
        }
    }

    for (int row = 0; row < ui->M_habitTable->rowCount(); ++row) {
        ui->M_habitTable->setRowHeight(row, rowHeight);
    }



    int todaysDay = currentDate.dayOfWeek();
    for (int row = 0; row < ui->M_habitTable->rowCount(); ++row) {
        for (int col = 1; col <= 7; ++col) {
            QWidget* cellWidget = ui->M_habitTable->cellWidget(row, col);
            if (cellWidget) {
                QCheckBox* check = cellWidget->findChild<QCheckBox*>();
                if (check) {
                    bool isToday = (col == todaysDay);
                    bool isPast = (col < todaysDay);

                    if (isPast) {
                        QString color = (check->isChecked() ? is_checked_color : not_checked_color);
                        cellWidget->setStyleSheet("background-color: rgb" + color + ";");
                    } else if (isToday) {
                        cellWidget->setStyleSheet("background-color: rgb" + current_day_color + ";");
                    } else {
                        check->setDisabled(true);
                    }
                }
            }
        }
    }

    ui->M_monLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + main_darker_color + ";");
    ui->M_tueLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + main_darker_color + ";");
    ui->M_wedLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + main_darker_color + ";");
    ui->M_thuLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + main_darker_color + ";");
    ui->M_friLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + main_darker_color + ";");
    ui->M_satLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + main_darker_color + ";");
    ui->M_sunLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + main_darker_color + ";");

    switch (todaysDay) {
    case 1: ui->M_monLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 2: ui->M_tueLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 3: ui->M_wedLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 4: ui->M_thuLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 5: ui->M_friLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 6: ui->M_satLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 7: ui->M_sunLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    }

    // ui->M_date->setText(currentDate.toString("dddd, MM/dd/yyyy"));  //Sunday, 07/27/2025
    ui->M_date->setText(currentDate.toString("dddd: MMMM d, yyyy"));  //sunday, july 27, 2025
}

QColor Habbit_tracker::stringToColor(QString input){
    string inputString = input.toStdString();
    int red = 0;
    int green = 0;
    int blue = 0;


    red = stoi(inputString.substr(1,inputString.find(',')-1));
        inputString = inputString.substr(inputString.find(',')+1);
    green = stoi(inputString.substr(0,inputString.find(',')));
        inputString = inputString.substr(inputString.find(',')+1);
    blue = stoi(inputString.substr(0,inputString.size()-1));

    QColor rValue = QColor(red,green,blue);
    return rValue;
}








// KEYBOARD FUNCTIONS:
void Habbit_tracker::setCapps(){
    if(ui->A_frame->isVisible()){
        // When starting, isCapps is true. So it will enter the first section
        if(showCapps_A){
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
        showCapps_A = !showCapps_A;
        showNums_A = false;
    }
    else if(ui->S_frame->isVisible()){
        // When starting, isCapps is true. So it will enter the first section
        if(showCapps_S){
            ui->key_Q_S->setText("Q"); ui->key_A_S->setText("A"); ui->key_Z_S->setText("Z");
            ui->key_W_S->setText("W"); ui->key_S_S->setText("S"); ui->key_X_S->setText("X");
            ui->key_E_S->setText("E"); ui->key_D_S->setText("D"); ui->key_C_S->setText("C");
            ui->key_R_S->setText("R"); ui->key_F_S->setText("F"); ui->key_V_S->setText("V");
            ui->key_T_S->setText("T"); ui->key_G_S->setText("G"); ui->key_B_S->setText("B");
            ui->key_Y_S->setText("Y"); ui->key_H_S->setText("H"); ui->key_N_S->setText("N");
            ui->key_U_S->setText("U"); ui->key_J_S->setText("J"); ui->key_M_S->setText("M");
            ui->key_I_S->setText("I"); ui->key_K_S->setText("K");
            ui->key_O_S->setText("O"); ui->key_L_S->setText("L");
            ui->key_P_S->setText("P");
        }
        else{
            ui->key_Q_S->setText("q"); ui->key_A_S->setText("a"); ui->key_Z_S->setText("z");
            ui->key_W_S->setText("w"); ui->key_S_S->setText("s"); ui->key_X_S->setText("x");
            ui->key_E_S->setText("e"); ui->key_D_S->setText("d"); ui->key_C_S->setText("c");
            ui->key_R_S->setText("r"); ui->key_F_S->setText("f"); ui->key_V_S->setText("v");
            ui->key_T_S->setText("t"); ui->key_G_S->setText("g"); ui->key_B_S->setText("b");
            ui->key_Y_S->setText("y"); ui->key_H_S->setText("h"); ui->key_N_S->setText("n");
            ui->key_U_S->setText("u"); ui->key_J_S->setText("j"); ui->key_M_S->setText("m");
            ui->key_I_S->setText("i"); ui->key_K_S->setText("k");
            ui->key_O_S->setText("o"); ui->key_L_S->setText("l");
            ui->key_P_S->setText("p");
        }

        // Change state of capps locked
        showCapps_S = !showCapps_S;
        showNums_S = false;
    }
}

void Habbit_tracker::setNumbers(){
    if(ui->A_frame->isVisible()){
        if(!showNums_A){
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
            if(!showCapps_A){
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

        showNums_A = !showNums_A;
    }else{
        if(!showNums_S){
            ui->key_Q_S->setText("1");
            ui->key_W_S->setText("2");
            ui->key_E_S->setText("3");
            ui->key_R_S->setText("4");
            ui->key_T_S->setText("5");
            ui->key_Y_S->setText("6");
            ui->key_U_S->setText("7");
            ui->key_I_S->setText("8");
            ui->key_O_S->setText("9");
            ui->key_P_S->setText("0");
        }
        else{
            if(!showCapps_S){
                ui->key_Q_S->setText("Q");
                ui->key_W_S->setText("W");
                ui->key_E_S->setText("E");
                ui->key_R_S->setText("R");
                ui->key_T_S->setText("T");
                ui->key_Y_S->setText("Y");
                ui->key_U_S->setText("U");
                ui->key_I_S->setText("I");
                ui->key_O_S->setText("O");
                ui->key_P_S->setText("P");
            }
            else{
                ui->key_Q_S->setText("q");
                ui->key_W_S->setText("w");
                ui->key_E_S->setText("e");
                ui->key_R_S->setText("r");
                ui->key_T_S->setText("t");
                ui->key_Y_S->setText("y");
                ui->key_U_S->setText("u");
                ui->key_I_S->setText("i");
                ui->key_O_S->setText("o");
                ui->key_P_S->setText("p");
            }
        }

        showNums_S = !showNums_S;
    }
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

    if(keyText == "Hide Keyboard"){
        return;
    }


    // Handle normal key input (A–Z)
    if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
        lineEdit->insert(keyText);
    }
}













// COLOR FUNCTIONS:
void Habbit_tracker::paintTheme(){

    // Main Frame ===========================================================================================
    // Frame:
    ui->M_frame->setStyleSheet("background-color: rgb" + main_darker_color + "; color:rgb" + text_color + ";");

    // Week Days Labels
    ui->M_monLabel->setAutoFillBackground(false);
    ui->M_monLabel->setStyleSheet("color:rgb" + text_color + "; background-color: rgb" + main_darker_color + ";");
    ui->M_tueLabel->setStyleSheet("color:rgb" + text_color + "; background-color: rgb" + main_darker_color + ";");
    ui->M_wedLabel->setStyleSheet("color:rgb" + text_color + "; background-color: rgb" + main_darker_color + ";");
    ui->M_thuLabel->setStyleSheet("color:rgb" + text_color + "; background-color: rgb" + main_darker_color + ";");
    ui->M_friLabel->setStyleSheet("color:rgb" + text_color + "; background-color: rgb" + main_darker_color + ";");
    ui->M_satLabel->setStyleSheet("color:rgb" + text_color + "; background-color: rgb" + main_darker_color + ";");
    ui->M_sunLabel->setStyleSheet("color:rgb" + text_color + "; background-color: rgb" + main_darker_color + ";");

    // Week Day Current Day Label
    switch(currentDate.dayOfWeek()){
    case 1: ui->M_monLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 2: ui->M_tueLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 3: ui->M_wedLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 4: ui->M_thuLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 5: ui->M_friLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 6: ui->M_satLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    case 7: ui->M_sunLabel->setStyleSheet("color:rgb" + text_color + "; background-color:rgb" + current_day_color + ";"); break;
    }

    // Buttons
    ui->M_removeButton->setStyleSheet("background-color: rgb" + button_color + ";");
    ui->M_addHabitButton->setStyleSheet("background-color: rgb" + button_color + ";");
    ui->M_viewHistoryButton->setStyleSheet("background-color: rgb" + button_color + ";");
    ui->M_settingsButton->setStyleSheet("background-color: rgb" + button_color + ";");

    // Other Frames:
    ui->M_buttonFrame->setStyleSheet("background-color: rgb" + main_darker_color + ";");
    ui->M_buttonFrame_2->setStyleSheet("background-color: rgb" + main_darker_color + ";");
    ui->M_buttonFrame_3->setStyleSheet("background-color: rgb" + main_darker_color + ";");
    ui->M_line->setStyleSheet("background-color: rgb" + main_darker_color + ";");

    // Date Text
    ui->M_dateTitle->setStyleSheet("background-color: rgb" + main_darker_color + ";");
    ui->M_date->setStyleSheet("background-color: rgb" + main_darker_color + ";");

    // Habit Table
    ui->M_habitTable->setStyleSheet("background-color: rgb" + main_lighter_color + ";" + background_image + ";");


    // Removing Frame:
    ui->M_removeFrame->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->M_cancelButton->setStyleSheet("background-color: rgb" + cancel_button_color + "; background-image: none;");
    ui->M_confirmButton->setStyleSheet("background-color: rgb" + save_button_color + ";");
    ui->M_removeList->setStyleSheet("background-color: rgb" + main_darker_color + ";");
    ui->M_removeTitle->setStyleSheet("background-color: rgb" + main_darker_color + ";");
    ui->M_removeList->setStyleSheet("QListWidget { background-color: rgb" + main_darker_color + "; }"
                                    "QListWidget::item { padding: 5px; }"
                                    "QListWidget::item:selected { background-color: rgb" + remove_item_selec_color + "; color:rgb" + text_color + "; }"
                                    "QListWidget::item:hover { background-color: rgb" + remove_item_selec_color + "; }");
    ui->M_removeBackground->setStyleSheet(background_image);



    // Adding Frame ===========================================================================================
    // Frame
    ui->A_frame->setStyleSheet("background-color: rgb" + main_darker_color + "; color:rgb" + text_color + ";");

    // Title
    ui->A_title->setStyleSheet("background-color: rgb" + main_darker_color + ";");

    // Other Frames:
    ui->A_frame1  ->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->A_keyboard->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

    // Buttons:
    ui->A_cancelButton->setStyleSheet("background-color: rgb" + cancel_button_color + ";");
    ui->A_saveButton->setStyleSheet("background-color: rgb" + save_button_color + ";");

    // Text Input
    ui->A_nameInput->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

    // Keyboard Buttons:
    QList<QPushButton*> keys_A = ui->A_keyboard->findChildren<QPushButton*>();
    for (auto &key : keys_A) {
        key->setStyleSheet("background-color: rgb" + keyboard_color + ";");
    }



    // History Frame ===========================================================================================
    // Frame:
    ui->H_frame->setStyleSheet("background-color: rgb" + main_darker_color + "; color:rgb" + text_color + ";");

    // Other Frames:
    ui->H_frame1->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->H_frame1_2->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->H_frame1_3->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->H_background->setStyleSheet(background_image);


    // Buttons:
    ui->H_upArrow->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->H_downArrow->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->H_backButton->setStyleSheet("background-color: rgb" + button_color + ";");
    ui->H_scrollDownButton->setStyleSheet("background-color: rgb" + button_color + ";");
    ui->H_scrollUpButton->setStyleSheet("background-color: rgb" + button_color + ";");
    ui->H_weekButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_monthButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_yearButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_displayNextButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );
    ui->H_displayPreviousButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + ";}" );


    // Span Display:
    ui->H_spanDisplay->setStyleSheet("background-color: rgb" + main_lighter_color + ";");


    // Settings Frame ===========================================================================================
    // Frame:
    ui->S_frame->setStyleSheet("background-color: rgb" + main_darker_color + "; color:rgb" + text_color + ";");

    // Other Frames:
    ui->frame_2->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_3->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->S_savedThemeBoxTitle->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->S_selectedThemeTitle->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->S_selectedThemeFrame->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_6->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_7->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->S_backgroundSelectionFrame->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_4->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

    // Text Boxes
    ui->S_renameInput->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->S_addThemeInput->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

    // Buttons:
    ui->S_backButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_scrollUpButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_scrollDownButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_renameButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_deleteButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_duplicateButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_addThemeButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_saveThemeButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_setThemeButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_mainDarkerButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_mainLighterButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_allButtonsButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_selectedButtonsButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_disableButtonsButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_keyboardButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_cancelButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_saveButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_itemSelectedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_homeCurrentButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_homeCompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_homeIncompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_calendarCompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_calendarIncompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_calendarMonthButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_calendarWeekdayButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_backgroundButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_setDefaultButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_resetButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_backgroundSelectionConfirmButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_backgroundSelectionScrollDown->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_backgroundSelectionScrollUp->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_keyboardToggleButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->S_textColorButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");

    // Saved Theme box:
    ui->S_savedThemesBox->setStyleSheet("QListWidget { background-color: rgb" + main_lighter_color + "; }"
                                        "QListWidget::item { padding: 5px; }"
                                        "QListWidget::item:selected { background-color: rgb" + remove_item_selec_color + "; color:rgb" + text_color + ";}"
                                        "QListWidget::item:hover { background-color: rgb" + remove_item_selec_color + "; }");

    // Possible background box:
    ui->S_backgroundSelectionBox->setStyleSheet("QListWidget { background-color: rgb" + main_lighter_color + "; }"
                                                "QListWidget::item:selected { background-color: rgb" + remove_item_selec_color + "; color:rgb" + text_color + ";}"
                                                "QListWidget::item:hover { background-color: rgb" + remove_item_selec_color + "; }");


    // Keyboard Colors
    QList<QPushButton*> keys_S = ui->S_keyboard->findChildren<QPushButton*>();
    for (auto &key : keys_S) {
        key->setStyleSheet("background-color: rgb" + keyboard_color + ";");
    }
    ui->S_keyboard->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

}

void Habbit_tracker::loadColorsFromFile(){

    // Making path if path for colors if it does not exists
    if(!filesystem::exists(themesPath)){
        filesystem::create_directories(themesPath);
    }


    // Reading from the current theme file that says what file to use for the colors
    ifstream themeFile(currentThemeFileName);
    if(!themeFile){                                      // If there is no theme file, then make a new one with the default.txt being target
        ofstream newThemeFile(currentThemeFileName);     // Make the file
        newThemeFile << defaultColorsFileName;           // Write the const default file name
        targetThemeFileName = defaultColorsFileName;     // Set the target theme to const default file name
        newThemeFile.close();                            // Close
    }else{
        string tmp;
        getline(themeFile, tmp);   // Get text from current theme file
        targetThemeFileName = tmp; // Set the target theme to be that theme file name
        themeFile.close();         // Close
    }


    // Check if target theme is the default AND if it does not exist, THEN MAKE THE DEFAULT FILE
    if(targetThemeFileName == defaultColorsFileName){
        ifstream colorFile(defaultColorsFileName);
        if(!colorFile){
            // Write new file with default colors
            ofstream newColorFile(defaultColorsFileName);
                newColorFile << "#WARNING: Do not put spaces inside the ()" << endl;
                newColorFile << endl;
                newColorFile << "#Main Colors:" << endl;
                newColorFile << "main_darker_color=(255,255,187)" << endl;
                newColorFile << "main_lighter_color=(253,255,222)" << endl;
                newColorFile << "text_color=(0,0,0)" << endl;
                newColorFile << endl;
                newColorFile << "#Buttons Colors:" << endl;
                newColorFile << "button_color=(255,233,176)" << endl;
                newColorFile << "button_select_color=(255,171,69)" << endl;
                newColorFile << "button_disab_color=(160,160,160)" << endl;
                newColorFile << endl;
                newColorFile << "#Add Habit Colors:" << endl;
                newColorFile << "keyboard_color=(255,170,0)" << endl;
                newColorFile << "cancel_button_color=(255,126,126)" << endl;
                newColorFile << "save_button_color=(165,255,171)" << endl;
                newColorFile << endl;
                newColorFile << "#Main Display Colors:" << endl;
                newColorFile << "current_day_color=(255,203,160)" << endl;
                newColorFile << "is_checked_color=(168,230,163)" << endl;
                newColorFile << "not_checked_color=(245,163,163)" << endl;
                newColorFile << endl;
                newColorFile << "#Calendar Colors:" << endl;
                newColorFile << "month_header_color=(255,171,69)" << endl;
                newColorFile << "week_header_color=(230,230,230)" << endl;
                newColorFile << "complete_color=(100,200,100)" << endl;
                newColorFile << "other_days_color=(255,255,255)" << endl;
                newColorFile << endl;
                newColorFile << "#Remove Habit Colors:" << endl;
                newColorFile << "remove_item_selec_color=(255,203,160)" << endl;
                newColorFile << endl;
                newColorFile << "#Background Image:" << endl;
                newColorFile << "background_image=none" << endl;
            newColorFile.close();

            // Setting variables to default colors
            main_darker_color  = "(255,255,187)";
            main_lighter_color = "(253,255,222)";
            text_color         = "(0,0,0)";
            button_color       = "(255,233,176)";
            button_select_color= "(255,171,69)";
            button_disab_color = "(160,160,160)";
            keyboard_color     = "(255,170,0)";
            cancel_button_color= "(255,126,126)";
            save_button_color  = "(165,255,171)";
            current_day_color  = "(255,203,160)";
            is_checked_color   = "(168,230,163)";
            not_checked_color  = "(245,163,163)";
            month_header_color = "(255,171,69)";
            week_header_color  = "(230,230,230)";
            complete_color     = "(100,200,100)";
            other_days_color   = "(255,255,255)";
            remove_item_selec_color = "(255,203,160)";
            background_image = "none";

            return;
        }
        else
            colorFile.close();
    }


    // READING FROM THE TARGET THEME FILE (Ofc if it was not written above ^)



    ifstream colorFile(targetThemeFileName);
    try{
        if(!colorFile)   // If file was not able to be opened, then throw into the catch
            throw 0;

        string tempString;
        int count = 1;

        while(getline(colorFile, tempString)){
            if(tempString.empty())              // Skipping empty lines
                continue;
            if(tempString[0] == '#')            // Skipping lines with # in front
                continue;


            tempString = tempString.substr(tempString.find('=') + 1); // Removing everything up to inclusive: "="
            switch(count){
            case 1 :main_darker_color       = QString::fromStdString(tempString);
            case 2 :main_lighter_color      = QString::fromStdString(tempString);
            case 3 :text_color              = QString::fromStdString(tempString);
            case 4 :button_color            = QString::fromStdString(tempString);
            case 5 :button_select_color     = QString::fromStdString(tempString);
            case 6 :button_disab_color      = QString::fromStdString(tempString);
            case 7 :keyboard_color          = QString::fromStdString(tempString);
            case 8 :cancel_button_color     = QString::fromStdString(tempString);
            case 9 :save_button_color       = QString::fromStdString(tempString);
            case 10:current_day_color       = QString::fromStdString(tempString);
            case 11:is_checked_color        = QString::fromStdString(tempString);
            case 12:not_checked_color       = QString::fromStdString(tempString);
            case 13:month_header_color      = QString::fromStdString(tempString);
            case 14:week_header_color       = QString::fromStdString(tempString);
            case 15:complete_color          = QString::fromStdString(tempString);
            case 16:other_days_color        = QString::fromStdString(tempString);
            case 17:remove_item_selec_color = QString::fromStdString(tempString);
            case 18:background_image        = QString::fromStdString(tempString);
            }

            count++;
            if(count == 19) // Not checking the background image since we already checked that at the end of this function
                continue;


            // Validating the string that was saved | Format (x[xx],x[xx],x[xx]) x be 1-3 characters
            if(tempString.empty()){
                throw 1;
            }
            if(tempString[0] != '('){
                throw 1;
            }



            if(tempString.find(',') == tempString.npos){
                throw 1;
            }
            int tmp = stoi(tempString.substr(1,tempString.find(',')));
            tempString = tempString.substr(tempString.find(',') + 1);



            if(tempString.find(',') == tempString.npos){
                throw 1;
            }
            tmp = stoi(tempString.substr(0,tempString.find(',')));
            tempString = tempString.substr(tempString.find(',') + 1);



            if(tempString.find(')') == tempString.npos){
                cout << "ERROR 2" << endl;
                throw 1;
            }
            tmp = stoi(tempString.substr(0,tempString.find(')')));
            tempString = tempString.substr(tempString.find(')') + 1);

            if(!tempString.empty())
                throw 1;


        }
        colorFile.close();  // Close the file


        // Checking if background_image is a valid image:
        if(!possible_backgrounds.contains(background_image)){
            throw 1;
        }

    }
    catch(...){    // IF ANYTHING GOES WRONG DURING READING
        colorFile.close();
        QMessageBox::critical(this, "Theme file Error", "Theme file was corrupted, Please check file. Setting theme to default. File"
                                                        ":" + QString::fromStdString(targetThemeFileName));
        if(filesystem::exists(currentThemeFileName))   // Remove the current theme so when the loadColors is called again it can make it
            filesystem::remove(currentThemeFileName);
        if(filesystem::exists(defaultColorsFileName))  // Remove the default colors file so that the loadColords can make it again
            filesystem::remove(defaultColorsFileName);

        loadColorsFromFile();                          // Call this function again to make default file
        return;
    }




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


















