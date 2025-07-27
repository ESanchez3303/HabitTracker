#ifndef HABBIT_TRACKER_H
#define HABBIT_TRACKER_H

#include <QMainWindow>
#include <QFrame>
#include <QRadioButton>
#include "habitClass.h"
#include <QDate>
#include <QTime>
#include <QPushButton>


using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class Habbit_tracker;
}
QT_END_NAMESPACE

class Habbit_tracker : public QMainWindow{
    Q_OBJECT

private slots:
    void onCheckmarkToggled(int state);


public:
    Habbit_tracker(QWidget *parent = nullptr);
    ~Habbit_tracker();

private:
    // Variables
    Ui::Habbit_tracker *ui;
    vector<habit> allHabits;
    string filesPath = "habits";
    string colorsFileName = "colors.txt";
    QTimer *dayCheckTimer;
    QDate currentDate;
    int habitIndex = 0;
    QString dateFormat = "MM/dd/yyyy";
    int dayCheckerInterval = 60000; // 60 seconds (60 * 1000ms)


    // Changable Variables: ==========================================
        // Main Display Grid -----------------------------------
        bool showGrid = true;
        QString checkBox_width  = "30";
        QString checkBox_height = "30";
        int initRowCount = 9;
        int rowHeight = 50;


        // COLORS ----------------------------------------------
        //Main Colors:
        QString main_darker_color  = "(255,255,187)";
        QString main_lighter_color = "(253,255,222)";

        //Buttons Colors:
        QString button_color       = "(255,233,176)";
        QString button_select_color= "(255,171,69)";
        QString button_disab_color = "(160,160,160)";

        //Add Habit Colors:
        QString keyboard_color     = "(255,170,0)";
        QString cancel_button_color= "(255,126,126)";
        QString save_button_color  = "(165,255,171)";

        //Main Display Colors:
        QString current_day_color  = "(255,203,160)";
        QString is_checked_color   = "(168,230,163)";
        QString not_checked_color  = "(245,163,163)";

        //Calendar Colors:
        QString month_header_color = "(255,171,69)";
        QString week_header_color  = "(230,230,230)";
        QString complete_color     = "(100,200,100)";
        QString other_days_color = "(255,255,255)";

        // Remove Habit Colors:
        QString remove_item_selec_color = "(255,203,160)";

    // ===============================================================




    // Frame Control Functions:
    void switchFrame(QFrame* target);




    // Main Frame Functions
    void M_addHabbitButtonClicked();
    void M_removeButtonClicked();
    void M_cancelButtonClicked();
    void M_confirmButtonClicked();
    void M_viewHistoryButtonClicked();



    // Add Frame Functions
    bool showCapps = false;
    bool showNums = false;
    void setNumbers();
    void setCapps();
    void insertKey();
    void A_cancelButtonClicked();
    void A_saveButtonClicked();



    // History Frame Functions:
    string currentTab = "";
    void H_backButtonClicked();
    void H_arrowClicked();
    void spanButtonClicked();
    void updateSpanDisplay(QDate spanStart, QDate spanEnd, int habitIndex);
    void scrollButtonClicked();
    void displayNextPreviousButtonClicked();


    // Helper Functions:
    bool validString(QString &input);
    void loadHabits();
    QColor stringToColor(QString input);


    // Color Functions:
    void paintColors();
    void writeColorsToFile();  // Writes current colors in program mem. to file
    void loadColorsFromFile(); // Reads from file and sets colors in program mem. | makes new file if no file exists
    void resetColors();        // Removes the colors file and calls load function to make new file and set variables
};
#endif // HABBIT_TRACKER_H





















