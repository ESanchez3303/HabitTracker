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
namespace Ui { class Habbit_tracker; }
QT_END_NAMESPACE


class Habbit_tracker : public QMainWindow{
    Q_OBJECT

private slots:
    void onCheckmarkToggled(int state);


public:
    Habbit_tracker(QWidget *parent = nullptr);
    ~Habbit_tracker();

private:
    // Variables =====================================================================================================================
    Ui::Habbit_tracker *ui;
    vector<habit> allHabits;


    // FILES ==========================================================================================================================
    string const filesPath = "habits";
    string const themesPath = "themes";
    string const currentThemeFileName = themesPath + "/selectedTheme.txt";
    string const defaultColorsFileName = themesPath + "/default.txt";
    string targetThemeFileName = themesPath + "/default.txt";


    // TIME KEEPING ==================================================================================================================
    QTimer *dayCheckTimer;
    QDate currentDate;
    QString dateFormat = "MM/dd/yyyy";
    int dayCheckerInterval = 60000; // 60 seconds (60 * 1000ms)


    // Frame Control Functions ======================================================================================================
    void switchFrame(QFrame* target);



    // Main Frame Functions ===========================================================================================================
    // ----------------------------
    bool    const showGrid = false;
    int     const checkBox_width  = 30;
    int     const checkBox_height = 30;
    int     const initRowCount = 9;
    int     const rowHeight = 50;
    // -----------------------------


    void M_addHabbitButtonClicked();
    void M_removeButtonClicked();
    void M_cancelButtonClicked();
    void M_confirmButtonClicked();
    void M_viewHistoryButtonClicked();
    void M_themeButtonClicked();
    void M_settingsButtonClicked();



    // Add Frame Functions ============================================================================================================
    bool showCappT_A = false;
    bool showNumT_A = false;
    void A_cancelButtonClicked();
    void A_saveButtonClicked();




    // History Frame Functions =========================================================================================================
    int habitIndex = 0;
    string currentTab = "";
    void H_backButtonClicked();
    void H_arrowClicked();
    void spanButtonClicked();
    void updateSpanDisplay(QDate spanStart, QDate spanEnd, int habitIndex);
    void scrollButtonClicked();
    void displayNextPreviousButtonClicked();





    // Theme Frame Functions =========================================================================================================
    QString T_main_darker_color, T_main_lighter_color, T_text_color;
    QString T_button_color, T_button_select_color, T_button_disab_color;
    QString T_keyboard_color, T_cancel_button_color, T_save_button_color;
    QString T_current_day_color, T_iT_checked_color, T_not_checked_color;
    QString T_month_header_color, T_week_header_color, T_complete_color, T_other_dayT_color;
    QString T_remove_item_selec_color;
    QString T_background_image;
    bool showCapps_T = false;
    bool showNums_T = false;
    void T_backButtonClicked();
    void T_keyboardToggleButtonClicked();
    void T_resetButtonClicked();
    void T_renameButtonClicked();
    void loadThemesIntoBox();
    void T_addThemeButtonClicked();
    void T_displayRadioButtonToggled();
    void T_displayMainButtonClicked();
    void T_savedThemeBoxIndexChanged();
    void paintDemo();
    void settingsChangeThemeButtonClicked();
    void T_setDefaultButtonClicked();
    void T_deleteButtonClicked();
    void T_duplicateButtonClicked();
    void T_saveThemeButtonClicked();
    void T_setThemeButtonClicked();
    void T_backgroundButtonClicked();
    void T_backSelectionConfirmButtonClicked();
    void T_backgroundSelectionScrollButtonClicked();
    void T_scrollButtonClicked();



    // Settings Frame Functions =========================================================================================================
    void S_backButtonClicked();



    // Helper Functions ===============================================================================================================
    bool validString(QString input);
    void loadHabits();
    QColor stringToColor(QString input);



    // Keybaord Functions ===============================================================================================================
    void setNumbers();
    void setCapps();
    void insertKey();



    // Theme Functions ================================================================================================================
    QString main_darker_color, main_lighter_color, text_color;
    QString button_color, button_select_color, button_disab_color;
    QString keyboard_color, cancel_button_color, save_button_color;
    QString current_day_color, iT_checked_color, not_checked_color;
    QString month_header_color,week_header_color,complete_color,other_dayT_color;
    QString remove_item_selec_color;
    QString background_image;
    QStringList const possible_backgrounds = {
        "none",
        "background-image:url(:/backgrounds/images/blue_stars.png)",
        "background-image:url(:/backgrounds/images/hearts.png)",
        "background-image:url(:/backgrounds/images/leaves.png)",
        "background-image:url(:/backgrounds/images/uniformSquiggles.png)",
        "background-image:url(:/backgrounds/images/coolHalloween.png)",
        "background-image:url(:/backgrounds/images/orangePumpkins.png)",
        "background-image:url(:/backgrounds/images/peepingPumpkins.png)",
        "background-image:url(:/backgrounds/images/nutCrackers.png)",
        "background-image:url(:/backgrounds/images/hohoho.png)",
        "background-image:url(:/backgrounds/images/cuteChristmas.png)",
        "background-image:url(:/backgrounds/images/bowCherry.png)",
        "background-image:url(:/backgrounds/images/trippy.png)",
        "background-image:url(:/backgrounds/images/colorfullSmiley.png)",
    };

    void paintTheme();        // Paints all the objects their colors set in variables

    void loadColorsFromFile(); // Reads information from file saved in targetThemeFileName



};
#endif // HABBIT_TRACKER_H





















