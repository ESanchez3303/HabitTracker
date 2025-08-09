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
    ui->T_keyboard->raise();
    ui->T_backgroundSelectionFrame->raise();


    // Moving frames where they are suppose to be
    ui->T_keyboard->move(QPoint(50,7));
    ui->T_backgroundSelectionFrame->move(QPoint(4,4));


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


    // Setting up the table for the backgrounds images in settings
    ui->T_backgroundSelectionBox->clear();
    string backTempString;
    for(auto &item:possible_backgrounds){
        if(item == "none"){
            ui->T_backgroundSelectionBox->addItem("NONE");
            continue;
        }
        backTempString = item.toStdString();
        backTempString = backTempString.substr(backTempString.find("images/") + 7);
        backTempString = backTempString.substr(0,backTempString.size()-1);
        ui->T_backgroundSelectionBox->addItem(QString::fromStdString(backTempString));
    }


    // Setting up the table for the display
    ui->T_displayTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->T_displayTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

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
    ui->T_displayTable->setCellWidget(0,0,DTCompletedContainer);
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
    ui->T_displayTable->setCellWidget(1,0,DTIncompletedContainer);
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
    ui->T_displayTable->setCellWidget(2,0,DTCurrentContainer);
    DTCurrentContainer->setStyleSheet("background: transparent;");
    DTCurrentContainer->setAttribute(Qt::WA_TranslucentBackground);




    // Connecting all the keyboard keys and Setting their Focus Policy   ------ ADDING KEYBOARD
    QList<QPushButton*> keys = ui->A_keyboard->findChildren<QPushButton*>();
    for (auto &key : keys) {
        connect(key, &QPushButton::clicked, this, &Habbit_tracker::insertKey);
        key->setFocusPolicy(Qt::NoFocus);
    }

    // Connecting all the keyboard keys and Setting their Focus Policy    ----- SETTINGS KEYBOARD
    QList<QPushButton*> keyT_S = ui->T_keyboard->findChildren<QPushButton*>();
    for (auto &key : keyT_S) {
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
    ui->T_keyboardToggleButton->setFocusPolicy(Qt::NoFocus);
    ui->T_displayTable->setFocusPolicy(Qt::NoFocus);
    ui->S_currentThemesBox->setFocusPolicy(Qt::NoFocus);



    // Connections
    connect(ui->M_addHabitButton, &QPushButton::clicked, this, &Habbit_tracker::M_addHabbitButtonClicked);
    connect(ui->M_removeButton, &QPushButton::clicked, this, &Habbit_tracker::M_removeButtonClicked);
    connect(ui->M_cancelButton, &QPushButton::clicked, this, &Habbit_tracker::M_cancelButtonClicked);
    connect(ui->M_confirmButton, &QPushButton::clicked, this, &Habbit_tracker::M_confirmButtonClicked);
    connect(ui->M_viewHistoryButton, &QPushButton::clicked, this, &Habbit_tracker::M_viewHistoryButtonClicked);
    connect(ui->M_themeButton, &QPushButton::clicked, this, &Habbit_tracker::M_themeButtonClicked);
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

    connect(ui->T_backButton, &QPushButton::clicked, this, &Habbit_tracker::T_backButtonClicked);
    connect(ui->T_addThemeButton, &QPushButton::clicked, this, &Habbit_tracker::T_addThemeButtonClicked);
    connect(ui->T_keyboardToggleButton, &QPushButton::clicked, this, &Habbit_tracker::T_keyboardToggleButtonClicked);
    connect(ui->T_savedThemesBox,&QListWidget::currentRowChanged, this, &Habbit_tracker::T_savedThemeBoxIndexChanged);
    connect(ui->T_displayRadioButton, &QRadioButton::toggled, this, &Habbit_tracker::T_displayRadioButtonToggled);
    connect(ui->T_displayMainButton, &QPushButton::clicked, this, &Habbit_tracker::T_displayMainButtonClicked);
    connect(ui->T_resetButton, &QPushButton::clicked, this, &Habbit_tracker::T_resetButtonClicked);
    connect(ui->T_renameButton, &QPushButton::clicked, this, &Habbit_tracker::T_renameButtonClicked);
    connect(ui->T_deleteButton, &QPushButton::clicked, this, &Habbit_tracker::T_deleteButtonClicked);
    connect(ui->T_duplicateButton, &QPushButton::clicked, this, &Habbit_tracker::T_duplicateButtonClicked);
    connect(ui->T_saveThemeButton, &QPushButton::clicked, this, &Habbit_tracker::T_saveThemeButtonClicked);
    connect(ui->T_setThemeButton, &QPushButton::clicked, this, &Habbit_tracker::T_setThemeButtonClicked);
    connect(ui->T_backgroundButton, &QPushButton::clicked, this, &Habbit_tracker::T_backgroundButtonClicked);
    connect(ui->T_setDefaultButton, &QPushButton::clicked, this, &Habbit_tracker::T_setDefaultButtonClicked);
    connect(ui->T_backgroundSelectionScrollUp, &QPushButton::clicked, this, &Habbit_tracker::T_backgroundSelectionScrollButtonClicked);
    connect(ui->T_backgroundSelectionScrollDown, &QPushButton::clicked, this, &Habbit_tracker::T_backgroundSelectionScrollButtonClicked);
    connect(ui->T_backgroundSelectionConfirmButton, &QPushButton::clicked, this, &Habbit_tracker::T_backSelectionConfirmButtonClicked);
    connect(ui->T_backButton, &QPushButton::clicked, this, &Habbit_tracker::T_backButtonClicked);
    connect(ui->T_scrollDownButton, &QPushButton::clicked, this, &Habbit_tracker::T_scrollButtonClicked);
    connect(ui->T_scrollUpButton, &QPushButton::clicked, this, &Habbit_tracker::T_scrollButtonClicked);


    connect(ui->S_backButton, &QPushButton::clicked, this, &Habbit_tracker::S_backButtonClicked);
    connect(ui->S_updateCycleButton, &QPushButton::clicked, this, &Habbit_tracker::S_updateCycleButtonClicked);


    QList<QPushButton*> themeRecolorButtons = ui->T_selectedThemeFrame->findChildren<QPushButton*>();
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
    ui->T_frame->hide();
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
        showNumT_A = false;

        // Setting keyboard to Capital Letters
        showCappT_A = false;
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
    else if(target == ui->T_frame){
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

        ui->T_currentTheme->setText("Current Theme: " + QString::fromStdString(cleanedCurrentTheme));

        // Hiding the keyboard until the show keyboard button is pressed and SETTTING UP THE KEYBOARD
        ui->T_keyboard->hide();
        ui->T_keyboardToggleButton->setText("Show\nKeyboard");

        target->show(); // Calling this first to be able to use in set capps
        // Resetting the variables and setting to all capps
        showNums_T = false;
        showCapps_T = false;
        setCapps();
    }
    else if(target == ui->S_frame){
        // Resetting everything in settings page
        ui->S_tabsWindow->setCurrentIndex(0);

        // Setting everything in settings page
        S_loadTheme();
        S_loadCurrentCycle();
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

void Habbit_tracker::M_themeButtonClicked(){
    switchFrame(ui->T_frame);
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

void Habbit_tracker::M_settingsButtonClicked(){
    switchFrame(ui->S_frame);
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
    QColor otherDaysColor_ = stringToColor(other_dayT_color);

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
        QColor dateBackgroundColor = stringToColor(other_dayT_color); // Default white for days
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





// THEME FRAME FUNCTIONS:
void Habbit_tracker::T_backButtonClicked(){
    // Start up the timer again
    dayCheckTimer->start(dayCheckerInterval);
    switchFrame(ui->M_frame);
}

void Habbit_tracker::T_keyboardToggleButtonClicked(){
    if(ui->T_keyboard->isVisible()){
        ui->T_keyboard->hide();
        ui->T_keyboardToggleButton->setText("Show\nKeyboard");
    }
    else{
        ui->T_keyboard->show();
        ui->T_keyboardToggleButton->setText("Hide\nKeyboard");
    }
}

void Habbit_tracker::T_resetButtonClicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Resetting Changes", "Are you sure you want to delete changes made to this theme?",
                                  QMessageBox::Yes | QMessageBox::No);

    // Returning if user decided to cancel the reset
    if (reply == QMessageBox::No) {
        return;
    }

    T_savedThemeBoxIndexChanged();
}

void Habbit_tracker::T_renameButtonClicked(){
    string savingName = ui->T_renameInput->text().toStdString();


    // Checking if the name did not change at all
    if(ui->T_renameInput->text() == ui->T_savedThemesBox->currentItem()->text())
        return;

    // Checking if the format is correct and if the string is empty
    if(!validString(QString::fromStdString(savingName))){
        QMessageBox::information(this, "Name is invalid.","Please fix name format. Name has to have at least 1 character. Valid Characters: Numbers, Letters, Spaces.");
        ui->T_renameInput->setText(ui->T_savedThemesBox->currentItem()->text());
        return;
    }

    // Making sure they cant rename it to default
    if(savingName == "default"){
        QMessageBox::information(this, "Name Cannot Be Default","Theme name cannot be default. This file name is used in the backend for other reasons.");
        ui->T_renameInput->setText(ui->T_savedThemesBox->currentItem()->text());
        return;
    }

    // Making sure they cant rename it to selectedTheme
    if(savingName == "selectedTheme"){
        QMessageBox::information(this, "Name Cannot Be selectedTheme","Theme name cannot be selectedTheme. This file name is used in the backend for other reasons.");
        ui->T_renameInput->setText(ui->T_savedThemesBox->currentItem()->text());
        return;
    }

    // Checking if this name has been used before
    int currentRow = ui->T_savedThemesBox->currentRow();
    int count = ui->T_savedThemesBox->count();

    for (int i = 0; i < count; ++i) {
        if (i == currentRow) continue; // Skip the currently selected item

        QListWidgetItem* item = ui->T_savedThemesBox->item(i);
        if (item->text().toLower() == QString::fromStdString(savingName).toLower()) {
            QMessageBox::information(this, "Theme Name is Used.",
                                     "This theme name has already been used. Please choose a different name.");
            ui->T_renameInput->setText(ui->T_savedThemesBox->currentItem()->text());
            return;
        }
    }

    // Changing the name of the theme file now that it passed all checks
    string savingFileName = "";
    for(auto &ch:savingName)
        savingFileName += (ch == ' ' ? '_':ch);
    savingFileName = themesPath + "/" + savingFileName + ".txt";


    // Getting original file name
    string originalName = ui->T_savedThemesBox->currentItem()->text().toStdString();
    string originalFileName = "";
    for(auto& ch:originalName)
        originalFileName += (ch == ' ' ? '_':ch);
    originalFileName = themesPath + "/" + originalFileName + ".txt";


    // Rename the file
    try {
        filesystem::rename(originalFileName, savingFileName);
    } catch (const filesystem::filesystem_error& e) {
        QMessageBox::critical(this,"ERORR RENAMING FILE", "Could not rename the file. Please contact ema.");
        ui->T_renameInput->setText(ui->T_savedThemesBox->currentItem()->text());
        return;
    }


    // Rewrite the current theme file so that it knows where to look next time we open it
    ofstream selectedThemeFile(currentThemeFileName);
    selectedThemeFile << savingFileName << endl;
    selectedThemeFile.close();

    // Setting up the current theme file variable again
    targetThemeFileName = savingFileName;



    // Reload the themes page
    switchFrame(ui->T_frame);
}

void Habbit_tracker::loadThemesIntoBox(){
    // Making sure that the folder exists
    if (!filesystem::exists(themesPath)) {
        QMessageBox::critical(this, "ERROR", "Theme folder does not exist. Please contact ema for a reset.");
        T_backButtonClicked();
        return;
    }


    ui->T_savedThemesBox->clear();
    // Adding all the files into the theme box after parsing/cleaning their names
    for (const auto& entry : filesystem::recursive_directory_iterator(themesPath)) {
        if (entry.is_regular_file()) {
            string fileNameWith_ = entry.path().filename().string();
            fileNameWith_ = fileNameWith_.substr(0, fileNameWith_.length() - 4);

            // Checking if its the selectedTheme file
            if(fileNameWith_ == "selectedTheme" || fileNameWith_ == "default" || fileNameWith_ == "themeCycle__")
                continue;

            string fixedFileName = "";
            for (auto& ch : fileNameWith_) {
                fixedFileName += (ch == '_') ? ' ' : ch;
            }
            ui->T_savedThemesBox->addItem(QString::fromStdString(fixedFileName));
        }
    }


    // Setting the index to -1 so that nothing is being selected at the moment
    ui->T_savedThemesBox->setCurrentRow(-1);


    // Showing scroll buttons when they are needed and hidding when not
    if(ui->T_savedThemesBox->count() > 7){
        ui->T_scrollUpButton->show();
        ui->T_scrollDownButton->show();
    }else{
        ui->T_scrollUpButton->hide();
        ui->T_scrollDownButton->hide();
    }

    // Hiding the whole selected theme section (rigth section)
    ui->T_selectedThemeTitle->hide();
    ui->T_selectedThemeFrame->hide();
    ui->T_selectedThemeDemoTitle->hide();
    ui->T_selectedThemeDemoFrame->hide();
    ui->T_saveThemeButton->hide();
    ui->T_setThemeButton->hide();
    ui->T_resetButton->hide();
    ui->T_backgroundSelectionFrame->hide();


    // Disabeling the buttons that would need an index from the themeselectionbox
    ui->T_renameButton->setDisabled(true);
    ui->T_deleteButton->setDisabled(true);
    ui->T_duplicateButton->setDisabled(true);
    ui->T_renameInput->setDisabled(true);

    // Cleaning the input for the rename section
    ui->T_renameInput->setText("");

}

void Habbit_tracker::T_addThemeButtonClicked(){
    // Catching when the new theme name is invalid
    if(!validString(ui->T_addThemeInput->text())){
        QMessageBox::information(this, "Name is invalid.","Please fix name format. Name has to have at least 1 character. Valid Characters: Numbers, Letters, Spaces.");
        ui->T_addThemeInput->setText("");
        return;
    }


    // Name was valid, replacing the spaces with underscores
    string currentThemeName = ui->T_addThemeInput->text().toStdString();
    string cleanedThemeName = "";
    for(auto& ch: currentThemeName)
        cleanedThemeName += (ch == ' ' ? '_':ch);


    // Catching when the name is already used
    if(!ui->T_savedThemesBox->findItems(QString::fromStdString(currentThemeName), Qt::MatchFixedString).isEmpty()){
        QMessageBox::information(this, "Theme Name is Used.","This theme name has already been used. Please choose a different name.");
        ui->T_addThemeInput->setText("");
        return;
    }


    // Catching when the name is "Default" since we never want to rewrite that file
    if(cleanedThemeName == "default"){
        QMessageBox::information(this, "Name Cannot Be Default","Theme name cannot be default. This file name is used in the backend for other reasons.");
        ui->T_addThemeInput->setText("");
        return;
    }

    // Making sure they cant rename it to selectedTheme
    if(cleanedThemeName == "selectedTheme"){
        QMessageBox::information(this, "Name Cannot Be selectedTheme","Theme name cannot be selectedTheme. This file name is used in the backend for other reasons.");
        ui->T_addThemeInput->setText("");
        return;
    }

    // Making sure they cant rename it to selectedTheme
    if(cleanedThemeName == "themeCycle__"){
        QMessageBox::information(this, "Name Cannot Be themeCycle__","Theme name cannot be themeCycle__. This file name is used in the backend for other reasons.");
        ui->T_addThemeInput->setText("");
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
        newThemeFile << "iT_checked_color=(168,230,163)" << endl;
        newThemeFile << "not_checked_color=(245,163,163)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Calendar Colors:" << endl;
        newThemeFile << "month_header_color=(255,171,69)" << endl;
        newThemeFile << "week_header_color=(230,230,230)" << endl;
        newThemeFile << "complete_color=(100,200,100)" << endl;
        newThemeFile << "other_dayT_color=(255,255,255)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Remove Habit Colors:" << endl;
        newThemeFile << "remove_item_selec_color=(255,203,160)" << endl;
        newThemeFile << endl;
        newThemeFile << "#Background Image:" << endl;
        newThemeFile << "background_image=none" << endl;
    newThemeFile.close();

    // Resetting the input section for the new theme
    ui->T_addThemeInput->setText("");

    // Sending to loadThemes so that it can be loaded
    loadThemesIntoBox();
}

void Habbit_tracker::T_displayRadioButtonToggled(){
    if(ui->T_displayRadioButton->isChecked()){
        ui->T_displayMainButton->setDisabled(true);
    }
    else{
        ui->T_displayMainButton->setDisabled(false);
    }
}

void Habbit_tracker::T_displayMainButtonClicked(){
    if(ui->T_displayMainButton->isChecked()){
        ui->T_displayMainButton->setStyleSheet("QPushButton { background-color: rgb" + T_button_select_color + "; } QPushButton:disabled { background-color: rgb" + T_button_disab_color + ";}" );
    }else{
        ui->T_displayMainButton->setStyleSheet("QPushButton { background-color: rgb" + T_button_color + "; } QPushButton:disabled { background-color: rgb" + T_button_disab_color + ";}" );
    }
}

void Habbit_tracker::T_savedThemeBoxIndexChanged(){
    // Catching when the index changes to nothing
    if(ui->T_savedThemesBox->currentRow() < 0){
        // Reset the saved theme box, will also hide the right section, and clear everything from the screen
        loadThemesIntoBox();
        return;
    }

    // Hiding again the background selection frame
    ui->T_backgroundSelectionFrame->hide();

    // Attempting to open the file with the given name
    // Making theme name into the file name
    string themeName = ui->T_savedThemesBox->currentItem()->text().toStdString();
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
            case 11:T_iT_checked_color        = QString::fromStdString(tempString);
            case 12:T_not_checked_color       = QString::fromStdString(tempString);
            case 13:T_month_header_color      = QString::fromStdString(tempString);
            case 14:T_week_header_color       = QString::fromStdString(tempString);
            case 15:T_complete_color          = QString::fromStdString(tempString);
            case 16:T_other_dayT_color        = QString::fromStdString(tempString);
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
    ui->T_renameInput->setText(QString::fromStdString(themeName));

    // Enabling the buttons
    ui->T_renameButton->setDisabled(false);
    ui->T_duplicateButton->setDisabled(false);
    ui->T_deleteButton->setDisabled(false);
    ui->T_renameInput->setDisabled(false);

    // Showing the right side
    ui->T_selectedThemeTitle->show();
    ui->T_selectedThemeFrame->show();
    ui->T_selectedThemeDemoTitle->show();
    ui->T_selectedThemeDemoFrame->show();
    ui->T_saveThemeButton->show();
    ui->T_setThemeButton->show();
    ui->T_resetButton->show();

    paintDemo(); // This paints the demo with the saved information

}

void Habbit_tracker::paintDemo(){
    // UPDATING THE DISPLAY DEMO ----------------------------------------------------------------------------------------------------
    // Setting the main background color
    ui->T_selectedThemeDemoFrame->setStyleSheet("background-color: rgb" + T_main_lighter_color + "; color:rgb" + T_text_color + ";");
    ui->T_selectedThemeDemoTitle->setStyleSheet("background-color: rgb" + T_main_darker_color + "; color:rgb" + T_text_color + ";");

    // Starting the demo with the radio button for the disabled to be inactive
    ui->T_displayRadioButton->setChecked(false);

    // Painting the display sections of the theme information (easy ones)
    ui->T_displayCancelButton->setStyleSheet("background-color: rgb" + T_cancel_button_color + "; background-image: none;"); // Cancel Buttton
    ui->T_displaySaveButton->setStyleSheet("background-color: rgb" + T_save_button_color + ";");                             // Save Button
    ui->T_displayKeyboardButton->setStyleSheet("background-color: rgb" + T_keyboard_color + ";");                            // Keyboard Button
    ui->T_displaySelection->setStyleSheet("QListWidget { background-color: rgb" + T_main_darker_color + "; }"                // Selection List
                                                                                                        "QListWidget::item:selected { background-color: rgb" + T_remove_item_selec_color + "; color:rgb" + text_color + ";}"
                                                                        "QListWidget::item:hover { background-color: rgb" + T_remove_item_selec_color + "; }");
    ui->T_displayMainButton->setStyleSheet("QPushButton { background-color: rgb" + T_button_color + "; } QPushButton:disabled { background-color: rgb" + T_button_disab_color + "}"); // Main Button (when is enabled)

    // Painting the background image into the displayBackgroundImage
    string backgroundUrl_string = T_background_image.toStdString();
    if(backgroundUrl_string == "none"){ // If the image is none, then set it to the image of none
        ui->T_displayBackground->setStyleSheet("border-image:url(:/none/images/none.png);"); // Background Image
        ui->T_backgroundButton->setText("NONE");
    }
    else{ // If there is an actual background image selected, then parse for it and set the displaybackground using BORDER-IMAGE to show the full image
        backgroundUrl_string = backgroundUrl_string.substr(backgroundUrl_string.find(':')+1);
        ui->T_displayBackground->setStyleSheet("border-image:" + QString::fromStdString(backgroundUrl_string)); // Background Image

        // Getting only the name of the file with the .png tag at the end for cleaner presentation
        backgroundUrl_string = backgroundUrl_string.substr(backgroundUrl_string.find("images/") + 7);
        backgroundUrl_string = backgroundUrl_string.substr(0,backgroundUrl_string.size()-1);
        ui->T_backgroundButton->setText(QString::fromStdString(backgroundUrl_string));
    }


    // Painting the display section of the QTableWidget ------------------------------------------------------

    // Chaning the colors of the checkmarks that represent the ones in the home screen
    QWidget* cellWidget1 = ui->T_displayTable->cellWidget(0,0);
    cellWidget1->setStyleSheet("background-color: rgb" + T_iT_checked_color + ";");

    QWidget* cellWidget2 = ui->T_displayTable->cellWidget(1,0);
    cellWidget2->setStyleSheet("background-color: rgb" + T_not_checked_color + ";");

    QWidget* cellWidget3 = ui->T_displayTable->cellWidget(2,0);
    cellWidget3->setStyleSheet("background-color: rgb" + T_current_day_color + ";");



    // Changing the colors of the calendar section
    QTableWidgetItem* calendarComplete = ui->T_displayTable->item(0, 1);
    QTableWidgetItem* calendarIncomplete= ui->T_displayTable->item(1, 1);
    QTableWidgetItem* calendarMonth = ui->T_displayTable->item(3, 1);
    QTableWidgetItem* calendarWeekday = ui->T_displayTable->item(4, 1);

    calendarComplete->setBackground(stringToColor(T_complete_color));
    calendarIncomplete->setBackground(stringToColor(T_other_dayT_color));
    calendarMonth->setBackground(stringToColor(T_month_header_color));
    calendarWeekday->setBackground(stringToColor(T_week_header_color));

    // Changing the background image on the table and setting the grid lines variable
    ui->T_displayTable->setStyleSheet("background-color: rgb" + T_main_lighter_color + ";" + T_background_image + ";");
    ui->T_displayTable->setShowGrid(showGrid);

}

void Habbit_tracker::settingsChangeThemeButtonClicked(){
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    // If the button pressed was the background image button, then show something different and return
    if(clickedButton == ui->T_backgroundButton){
        // Show the list of the avaliable images
        return;
    }

    // Setting up the custom colors from the theme that we have right now
    QStringList colorStrings = {
        T_main_darker_color, T_main_lighter_color, T_text_color,
        T_button_color, T_button_select_color, T_button_disab_color,
        T_keyboard_color, T_cancel_button_color, T_save_button_color,
        T_current_day_color, T_iT_checked_color, T_not_checked_color,
        T_month_header_color, T_week_header_color, T_complete_color,
        T_other_dayT_color
    };

    for (int i = 0; i < colorStrings.size() && i < 16; ++i) {
        QString colorStr = colorStrings[i];
        colorStr.remove('(').remove(')'); // Remove parentheses
        QStringList rgbParts = colorStr.split(',');

        if (rgbParts.size() == 3) {
            int r = rgbParts[0].trimmed().toInt();
            int g = rgbParts[1].trimmed().toInt();
            int b = rgbParts[2].trimmed().toInt();

            QColor color(r, g, b);
            QColorDialog::setCustomColor(i, color);
        }
    }

    // Grabbing the color of the item that we are trying to change up. EG: we click on the main darker color then grab that color and save to send into the getColor
    QColor currentColor;
    if(clickedButton == ui->T_mainDarkerButton)               currentColor = stringToColor(T_main_darker_color);
    else if(clickedButton == ui->T_mainLighterButton)         currentColor = stringToColor(T_main_lighter_color);
    else if(clickedButton == ui->T_textColorButton)           currentColor = stringToColor(T_text_color);
    else if(clickedButton == ui->T_allButtonsButton)          currentColor = stringToColor(T_button_color);
    else if(clickedButton == ui->T_selectedButtonsButton)     currentColor = stringToColor(T_button_select_color);
    else if(clickedButton == ui->T_disableButtonsButton)      currentColor = stringToColor(T_button_disab_color);
    else if(clickedButton == ui->T_keyboardButton)            currentColor = stringToColor(T_keyboard_color);
    else if(clickedButton == ui->T_cancelButton)              currentColor = stringToColor(T_cancel_button_color);
    else if(clickedButton == ui->T_saveButton)                currentColor = stringToColor(T_save_button_color);
    else if(clickedButton == ui->T_itemSelectedButton)        currentColor = stringToColor(T_remove_item_selec_color);
    else if(clickedButton == ui->T_homeCurrentButton)         currentColor = stringToColor(T_current_day_color);
    else if(clickedButton == ui->T_homeCompletedButton)       currentColor = stringToColor(T_iT_checked_color);
    else if(clickedButton == ui->T_homeIncompletedButton)     currentColor = stringToColor(T_not_checked_color);
    else if(clickedButton == ui->T_calendarCompletedButton)   currentColor = stringToColor(T_complete_color);
    else if(clickedButton == ui->T_calendarIncompletedButton) currentColor = stringToColor(T_other_dayT_color);
    else if(clickedButton == ui->T_calendarMonthButton)       currentColor = stringToColor(T_month_header_color);
    else if(clickedButton == ui->T_calendarWeekdayButton)     currentColor = stringToColor(T_week_header_color);
    else currentColor = stringToColor("(255,255,255)");  // fallback to white if no match






    // Show the message that gets the color from the user and save that
    QColor inputColor = QColorDialog::getColor(currentColor,this,"Select the color for: " + clickedButton->text());
    if(!inputColor.isValid()){
        return;
    }


    // Parse the saved color to get it in the format of (x[xx],x[xx],x[xx]) -- (x,x,x) ; x ranges from lit. amount of 1-3
    int r = inputColor.red();
    int g = inputColor.green();
    int b = inputColor.blue();
    QString inputColorQString = QString("(%1,%2,%3)").arg(r).arg(g).arg(b);



    // Finding out which button is pressed and setting the screen display accordingly
    if(clickedButton == ui->T_mainDarkerButton)               T_main_darker_color = inputColorQString;
    else if(clickedButton == ui->T_mainLighterButton)         T_main_lighter_color = inputColorQString;
    else if(clickedButton == ui->T_textColorButton)           T_text_color = inputColorQString;
    else if(clickedButton == ui->T_allButtonsButton)          T_button_color = inputColorQString;
    else if(clickedButton == ui->T_selectedButtonsButton)     T_button_select_color = inputColorQString;
    else if(clickedButton == ui->T_disableButtonsButton)      T_button_disab_color = inputColorQString;
    else if(clickedButton == ui->T_keyboardButton)            T_keyboard_color = inputColorQString;
    else if(clickedButton == ui->T_cancelButton)              T_cancel_button_color = inputColorQString;
    else if(clickedButton == ui->T_saveButton)                T_save_button_color = inputColorQString;
    else if(clickedButton == ui->T_itemSelectedButton)        T_remove_item_selec_color = inputColorQString;
    else if(clickedButton == ui->T_homeCurrentButton)         T_current_day_color = inputColorQString;
    else if(clickedButton == ui->T_homeCompletedButton)       T_iT_checked_color = inputColorQString;
    else if(clickedButton == ui->T_homeIncompletedButton)     T_not_checked_color = inputColorQString;
    else if(clickedButton == ui->T_calendarCompletedButton)   T_complete_color = inputColorQString;
    else if(clickedButton == ui->T_calendarIncompletedButton) T_other_dayT_color = inputColorQString;
    else if(clickedButton == ui->T_calendarMonthButton)       T_month_header_color = inputColorQString;
    else if(clickedButton == ui->T_calendarWeekdayButton)     T_week_header_color = inputColorQString;



    // Paint the demo with the new information, since some information interlaps with other widgets
    paintDemo();
}

void Habbit_tracker::T_setDefaultButtonClicked(){
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
    switchFrame(ui->T_frame);

}

void Habbit_tracker::T_deleteButtonClicked(){
    // Just in case to make sure that we have selected an item
    if(ui->T_savedThemesBox->currentRow() == -1){
        return;
    }

    // Confirm with the user that we are going to delete the selected theme
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Deleting theme", "Are you sure you want to delete '" + ui->T_savedThemesBox->currentItem()->text() + "'?",
                                  QMessageBox::Yes | QMessageBox::No);

    // Returning if user decided to cancel the deletion
    if (reply == QMessageBox::No) {
        return;
    }

    // Making the file Name
    string themeName = ui->T_savedThemesBox->currentItem()->text().toStdString();
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

    // Removing the theme from the cycle theme and saving the cycle theme
    int currentIndex = 0;
    for(auto& item : themeCycle){
        if(item == themeFileName){ // If the theme cycle has this theme
            // Remove the theme from the themecycle
            themeCycle.erase(themeCycle.begin() + currentIndex);

            // Writting using everything we have saved WITHOUT the theme that we just deleted
            ofstream rewrittingThemeCycle(themeCycleFileName);
            rewrittingThemeCycle << (cycleThemeActive ? "1" : "0") << endl << cycleSchedule << endl;
            for(auto& item1 : themeCycle){
                rewrittingThemeCycle << item1 << endl;
            }
            rewrittingThemeCycle.close();
            break;
        }
        currentIndex++;
    }

    // Reloading the themes from the files
    switchFrame(ui->T_frame);
}

void Habbit_tracker::T_duplicateButtonClicked(){
    if(ui->T_savedThemesBox->currentRow() < 0)
        return;

    // Getting the name of the current theme in the qlistwidget that is selected
    string themeName = ui->T_savedThemesBox->currentItem()->text().toStdString();
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

void Habbit_tracker::T_saveThemeButtonClicked(){
    // This function ONLY saves the theme from the temp values into the theme file
    if(ui->T_savedThemesBox->currentRow() < 0)
        return;

    // Getting the filename of the theme
    string themeName = ui->T_savedThemesBox->currentItem()->text().toStdString();
    string themeFileName = "";
    for(auto& ch : themeName)
        themeFileName += (ch == ' ' ? '_':ch);
    themeFileName = themesPath + "/" + themeFileName + ".txt";



    // Rewritting the file that is selected using the T_ values that have been using for the demo
    ofstream themeFile(themeFileName);
    if(!themeFile){
        QMessageBox::critical(this,"ERROR SAVING FILE", "There was an error in saving the file, please conctact ema.");
        switchFrame(ui->T_frame);
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
    themeFile << "iT_checked_color=" << T_iT_checked_color.toStdString() << endl;
    themeFile << "not_checked_color=" << T_not_checked_color.toStdString() << endl;
    themeFile << endl;
    themeFile << "#Calendar Colors:" << endl;
    themeFile << "month_header_color=" << T_month_header_color.toStdString() << endl;
    themeFile << "week_header_color=" << T_week_header_color.toStdString() << endl;
    themeFile << "complete_color=" << T_complete_color.toStdString() << endl;
    themeFile << "other_dayT_color=" << T_other_dayT_color.toStdString() << endl;
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

void Habbit_tracker::T_setThemeButtonClicked(){
    // Saving the theme to the file
    T_saveThemeButtonClicked();

    // Setting the target theme file
    ofstream themeSelectionFile(currentThemeFileName);
    if(!themeSelectionFile){
        QMessageBox::critical(this, "ERROR OPENING THEME SELEC FILE.","Could not open theme selection file. Please restart program.");
        return;
    }

    // Getting the new target file
    string themeName = ui->T_savedThemesBox->currentItem()->text().toStdString();
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
    switchFrame(ui->T_frame);
}

void Habbit_tracker::T_backgroundButtonClicked(){
    ui->T_backgroundSelectionFrame->show();
    int indexOfSelectedBackground = 0;
    for(auto &item : possible_backgrounds){
        if(item == T_background_image)
            break;
        indexOfSelectedBackground++;
    }

    ui->T_backgroundSelectionBox->setCurrentRow(indexOfSelectedBackground);
}

void Habbit_tracker::T_backSelectionConfirmButtonClicked(){
    if(ui->T_backgroundSelectionBox->currentRow() == -1)
        return;

    // Checking if the chosen value was none
    QString userChoice = ui->T_backgroundSelectionBox->currentItem()->text();
    if(userChoice == "NONE")
        T_background_image = "none"; // Setting the temp value to "none"
    else
        T_background_image = "background-image:url(:/backgrounds/images/" + userChoice + ")"; // Setting the temp value for the background

    // Painting the demo again
    paintDemo();

    // Hiding again the background selection frame
    ui->T_backgroundSelectionFrame->hide();
}

void Habbit_tracker::T_backgroundSelectionScrollButtonClicked(){
    QObject* btn = sender();
    if (!btn) return;

    int currentValue = ui->T_backgroundSelectionBox->verticalScrollBar()->value();
    int step = 2;


    if (btn == ui->T_backgroundSelectionScrollDown) {
        ui->T_backgroundSelectionBox->verticalScrollBar()->setValue(currentValue + step);
    } else if (btn == ui->T_backgroundSelectionScrollUp) {
        ui->T_backgroundSelectionBox->verticalScrollBar()->setValue(currentValue - step);
    }
}

void Habbit_tracker::T_scrollButtonClicked(){
    QObject* btn = sender();
    if (!btn) return;

    int currentValue = ui->T_savedThemesBox->verticalScrollBar()->value();
    int step = 2;


    if (btn == ui->T_scrollDownButton) {
        ui->T_savedThemesBox->verticalScrollBar()->setValue(currentValue + step);
    } else if (btn == ui->T_scrollUpButton) {
        ui->T_savedThemesBox->verticalScrollBar()->setValue(currentValue - step);
    }
}






// SETTINGS FUNCTIONS:
void Habbit_tracker::S_backButtonClicked(){
    switchFrame(ui->M_frame);
}

void Habbit_tracker::S_loadTheme(){
    // Loading the themes from the vector into the all themes box
    ui->S_allThemesBox->clear();

    // Reading from the folder and loading in
    for (const auto& entry : filesystem::recursive_directory_iterator(themesPath)) {
        if (entry.is_regular_file()) {
            string themeName = entry.path().filename().string();

            if(themeName == "selectedTheme.txt" || themeName == "default.txt" || themeName == "themeCycle__.txt")
                continue;

            // Fxiing name for presentation
            themeName = themeName.substr(0, themeName.find('.'));
            string fixedThemeName = "";
            for(auto &ch : themeName){
                fixedThemeName += (ch == '_' ? ' ' : ch);
            }

            // Adding theme into all themes box
            ui->S_allThemesBox->addItem(QString::fromStdString(fixedThemeName));
        }
    }
}

void Habbit_tracker::S_loadCurrentCycle(){
    // Clear the current cycle box
    ui->S_currentThemesBox->clear();
    themeCycle.clear();

    // Technically we already guanrateed that the path exists, but doing it again just in case
    if (!filesystem::exists(themesPath)) {
        filesystem::create_directories(themesPath);
    }


    // Checking if the file exists, if not then make it
    ifstream testingCycleFile(themeCycleFileName);
    if(!testingCycleFile){                                  // Making general format of the file, the first line will be 0/1 if is activated
        ofstream makingCycleFile(themeCycleFileName);
        makingCycleFile << "0" << endl;                     // Will be set to false at first start
        makingCycleFile << "None" << endl;
        makingCycleFile.close();

        // Setting up page to mode where there is no cycle set up
        ui->S_offRadio->setChecked(true);
        ui->S_currentCycleText->setText("None");
    }
    else
        testingCycleFile.close();

    // Reading from the file
    string tempString = "";
    ifstream readingCycleFile(themeCycleFileName);

    if(!readingCycleFile){ // Final checking if we have the file open before reading
        QMessageBox::critical(this, "ERROR", "Could not open file after making the file. FILE: " + QString::fromStdString(themeCycleFileName));
        return;
    }

    // If there was nothing read then we assume file is corrupted, remove file and remake by calling this function again
    getline(readingCycleFile, tempString);
    if(tempString.empty()){
        readingCycleFile.close();
        if(filesystem::exists(themeCycleFileName))
            filesystem::remove(themeCycleFileName);
        QMessageBox::critical(this, "ERROR", "Cycle file was corrupted. Removing and rebuilding file.");
        S_loadCurrentCycle();
        return;
    }

    // Reading from the file, first line was already gravved and should be (1/0) to show if the theme is active
    try{
        if(tempString != "0" && tempString != "1"){ // Catching when first line was not a 0 or 1
            throw 1;
        }

         // Setting the currentthemeActive bool and display of this value
        cycleThemeActive = (tempString == "0" ? false:true);
        if(!cycleThemeActive)
            ui->S_offRadio->setChecked(true);
        else
            ui->S_onRadio->setChecked(true);

        // Setting the cycle schedule if there is any found
        getline(readingCycleFile, tempString);
        if(tempString != "12 Hours" && tempString != "Daily" && tempString != "Weekly" && tempString != "None"){
            throw 1;
        }
        ui->S_currentCycleText->setText(QString::fromStdString(tempString));
        cycleSchedule = tempString;



        while(getline(readingCycleFile,tempString)){
            // Skiping simple corruption of empty lines
            if(tempString.empty())
                continue;

            // Adding the full string being in form of /themes/something.txt to the theme cycle
            themeCycle.push_back(tempString);

            // Fixing the name for display
            tempString = tempString.substr(tempString.find('/')+1);
            tempString = tempString.substr(0,tempString.find('.'));
            string fixedThemeName = "";
            for(auto& ch : tempString)
                fixedThemeName += (ch == '_' ? ' ' : ch);

            ui->S_currentThemesBox->addItem(QString::fromStdString(fixedThemeName));

        }
        readingCycleFile.close();


    }catch(...){
        readingCycleFile.close();
        QMessageBox::critical(this, "ERROR", "Error reading file. Filename: " + QString::fromStdString(themeCycleFileName));
        return;
    }
}

void Habbit_tracker::S_updateCycleButtonClicked(){
    // Reset the current theme cycle
    themeCycle.clear();

    // Making sure that at least one is chosen
    if(ui->S_allThemesBox->selectedItems().count() <= 1){
        QMessageBox::information(this, "No themes selected.", "Please select at least 2 themes to set up a cycle");
        return;
    }

    // Making sure that a schedule is selected
    if(!ui->S_12HoursRadio->isChecked() && !ui->S_dailyRadio->isChecked() && !ui->S_weeklyRadio->isChecked()){
        QMessageBox::information(this, "No schedule selected.", "Please choose a schedule option to set up cycle");
        return;
    }

    // Grab all the items that are selected and add into the vector
    for (QListWidgetItem* item : ui->S_allThemesBox->selectedItems()) { // detaching is okay as long as computer is fast enough
        // Fixing the text into file name form
        string selected = item->text().toStdString();
        string fixedSelected = "";
        for(auto &ch : selected)
            fixedSelected += (ch == ' ' ? '_' : ch);

        fixedSelected = themesPath + "/" + fixedSelected + ".txt";

        // Adding file name form into the vector
        themeCycle.push_back(fixedSelected);
    }

    // Save the cycle TO FILES using: true for activate, the schedule selected, and vector
    ofstream writtingThemeCycle(themeCycleFileName);
    writtingThemeCycle << "1" << endl;
    if (ui->S_12HoursRadio->isChecked()) { writtingThemeCycle << "12 Hours" << endl;}
    else if (ui->S_dailyRadio->isChecked()) { writtingThemeCycle << "Daily" << endl;}
    else if (ui->S_weeklyRadio->isChecked()) { writtingThemeCycle << "Weekly" << endl;}
    for(auto& item : themeCycle){
        writtingThemeCycle << item << endl;
    }
    writtingThemeCycle.close();

    // Set the active checkmark to true
    ui->S_onRadio->setChecked(true);

    // Clear the selection of the allthemesbox
    ui->S_allThemesBox->clearSelection();


    //------------------------------

    // Call the load current cycle to draw right right side
    S_loadCurrentCycle();
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
                        QString color = (check->isChecked() ? iT_checked_color : not_checked_color);
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
        if(showCappT_A){
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
        showCappT_A = !showCappT_A;
        showNumT_A = false;
    }
    else if(ui->T_frame->isVisible()){
        // When starting, isCapps is true. So it will enter the first section
        if(showCapps_T){
            ui->key_Q_T->setText("Q"); ui->key_A_T->setText("A"); ui->key_Z_T->setText("Z");
            ui->key_W_T->setText("W"); ui->key_T_T->setText("S"); ui->key_X_T->setText("X");
            ui->key_E_T->setText("E"); ui->key_D_T->setText("D"); ui->key_C_T->setText("C");
            ui->key_R_T->setText("R"); ui->key_F_T->setText("F"); ui->key_V_T->setText("V");
            ui->key_T_T->setText("T"); ui->key_G_T->setText("G"); ui->key_B_T->setText("B");
            ui->key_Y_T->setText("Y"); ui->key_H_T->setText("H"); ui->key_N_T->setText("N");
            ui->key_U_T->setText("U"); ui->key_J_T->setText("J"); ui->key_M_T->setText("M");
            ui->key_I_T->setText("I"); ui->key_K_T->setText("K");
            ui->key_O_T->setText("O"); ui->key_L_T->setText("L");
            ui->key_P_T->setText("P");
        }
        else{
            ui->key_Q_T->setText("q"); ui->key_A_T->setText("a"); ui->key_Z_T->setText("z");
            ui->key_W_T->setText("w"); ui->key_T_T->setText("s"); ui->key_X_T->setText("x");
            ui->key_E_T->setText("e"); ui->key_D_T->setText("d"); ui->key_C_T->setText("c");
            ui->key_R_T->setText("r"); ui->key_F_T->setText("f"); ui->key_V_T->setText("v");
            ui->key_T_T->setText("t"); ui->key_G_T->setText("g"); ui->key_B_T->setText("b");
            ui->key_Y_T->setText("y"); ui->key_H_T->setText("h"); ui->key_N_T->setText("n");
            ui->key_U_T->setText("u"); ui->key_J_T->setText("j"); ui->key_M_T->setText("m");
            ui->key_I_T->setText("i"); ui->key_K_T->setText("k");
            ui->key_O_T->setText("o"); ui->key_L_T->setText("l");
            ui->key_P_T->setText("p");
        }

        // Change state of capps locked
        showCapps_T = !showCapps_T;
        showNums_T = false;
    }
}

void Habbit_tracker::setNumbers(){
    if(ui->A_frame->isVisible()){
        if(!showNumT_A){
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
            if(!showCappT_A){
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

        showNumT_A = !showNumT_A;
    }else{
        if(!showNums_T){
            ui->key_Q_T->setText("1");
            ui->key_W_T->setText("2");
            ui->key_E_T->setText("3");
            ui->key_R_T->setText("4");
            ui->key_T_T->setText("5");
            ui->key_Y_T->setText("6");
            ui->key_U_T->setText("7");
            ui->key_I_T->setText("8");
            ui->key_O_T->setText("9");
            ui->key_P_T->setText("0");
        }
        else{
            if(!showCapps_T){
                ui->key_Q_T->setText("Q");
                ui->key_W_T->setText("W");
                ui->key_E_T->setText("E");
                ui->key_R_T->setText("R");
                ui->key_T_T->setText("T");
                ui->key_Y_T->setText("Y");
                ui->key_U_T->setText("U");
                ui->key_I_T->setText("I");
                ui->key_O_T->setText("O");
                ui->key_P_T->setText("P");
            }
            else{
                ui->key_Q_T->setText("q");
                ui->key_W_T->setText("w");
                ui->key_E_T->setText("e");
                ui->key_R_T->setText("r");
                ui->key_T_T->setText("t");
                ui->key_Y_T->setText("y");
                ui->key_U_T->setText("u");
                ui->key_I_T->setText("i");
                ui->key_O_T->setText("o");
                ui->key_P_T->setText("p");
            }
        }

        showNums_T = !showNums_T;
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
    ui->M_themeButton->setStyleSheet("background-color: rgb" + button_color + ";");

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
    QList<QPushButton*> keyT_A = ui->A_keyboard->findChildren<QPushButton*>();
    for (auto &key : keyT_A) {
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
    ui->T_frame->setStyleSheet("background-color: rgb" + main_darker_color + "; color:rgb" + text_color + ";");

    // Other Frames:
    ui->frame_2->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_3->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->T_savedThemeBoxTitle->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->T_selectedThemeTitle->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->T_selectedThemeFrame->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_6->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_7->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->T_backgroundSelectionFrame->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->frame_4->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

    // Text Boxes
    ui->T_renameInput->setStyleSheet("background-color: rgb" + main_lighter_color + ";");
    ui->T_addThemeInput->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

    // Buttons:
    ui->T_backButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_scrollUpButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_scrollDownButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_renameButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_deleteButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_duplicateButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_addThemeButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_saveThemeButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_setThemeButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_mainDarkerButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_mainLighterButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_allButtonsButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_selectedButtonsButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_disableButtonsButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_keyboardButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_cancelButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_saveButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_itemSelectedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_homeCurrentButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_homeCompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_homeIncompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_calendarCompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_calendarIncompletedButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_calendarMonthButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_calendarWeekdayButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_backgroundButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_setDefaultButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_resetButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_backgroundSelectionConfirmButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_backgroundSelectionScrollDown->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_backgroundSelectionScrollUp->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_keyboardToggleButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");
    ui->T_textColorButton->setStyleSheet("QPushButton { background-color: rgb" + button_color + "; } QPushButton:disabled { background-color: rgb" + button_disab_color + "}");

    // Saved Theme box:
    ui->T_savedThemesBox->setStyleSheet("QListWidget { background-color: rgb" + main_lighter_color + "; }"
                                        "QListWidget::item { padding: 5px; }"
                                        "QListWidget::item:selected { background-color: rgb" + remove_item_selec_color + "; color:rgb" + text_color + ";}"
                                        "QListWidget::item:hover { background-color: rgb" + remove_item_selec_color + "; }");

    // Possible background box:
    ui->T_backgroundSelectionBox->setStyleSheet("QListWidget { background-color: rgb" + main_lighter_color + "; }"
                                                "QListWidget::item:selected { background-color: rgb" + remove_item_selec_color + "; color:rgb" + text_color + ";}"
                                                "QListWidget::item:hover { background-color: rgb" + remove_item_selec_color + "; }");


    // Keyboard Colors
    QList<QPushButton*> keyT_S = ui->T_keyboard->findChildren<QPushButton*>();
    for (auto &key : keyT_S) {
        key->setStyleSheet("background-color: rgb" + keyboard_color + ";");
    }
    ui->T_keyboard->setStyleSheet("background-color: rgb" + main_lighter_color + ";");

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
                newColorFile << "iT_checked_color=(168,230,163)" << endl;
                newColorFile << "not_checked_color=(245,163,163)" << endl;
                newColorFile << endl;
                newColorFile << "#Calendar Colors:" << endl;
                newColorFile << "month_header_color=(255,171,69)" << endl;
                newColorFile << "week_header_color=(230,230,230)" << endl;
                newColorFile << "complete_color=(100,200,100)" << endl;
                newColorFile << "other_dayT_color=(255,255,255)" << endl;
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
            iT_checked_color   = "(168,230,163)";
            not_checked_color  = "(245,163,163)";
            month_header_color = "(255,171,69)";
            week_header_color  = "(230,230,230)";
            complete_color     = "(100,200,100)";
            other_dayT_color   = "(255,255,255)";
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
            case 11:iT_checked_color        = QString::fromStdString(tempString);
            case 12:not_checked_color       = QString::fromStdString(tempString);
            case 13:month_header_color      = QString::fromStdString(tempString);
            case 14:week_header_color       = QString::fromStdString(tempString);
            case 15:complete_color          = QString::fromStdString(tempString);
            case 16:other_dayT_color        = QString::fromStdString(tempString);
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


















