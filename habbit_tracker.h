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
    QTimer *dayCheckTimer;
    QDate currentDate;
    int historyIndex = 0;
    QString dateFormat = "MM/dd/yyyy";


    // Changable Variables: =======================================
    QString checkBoxStyleSheet =
        "QCheckBox::indicator {width: 30px; height: 30px;}";
    bool showGrid = true;
    int initRowCount = 9;
    int rowHeight = 50;

    QString notCheckedColor = "#f5a3a3";
    QString isCheckedColor  = "#a8e6a3";
    QString currentDayColor = "#ffcba0";

    int dayCheckerInterval = 60000; // 60 seconds (60 * 1000ms)
    // ============================================================


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
    void H_backButtonClicked();
    void H_arrowClicked();
    void spanButtonClicked();
    void updateSpanDisplay(QDate spanStart, QDate spanEnd, int habitIndex, QPushButton* pressedButton);
    void scrollButtonClicked();


    // Helper Functions:
    bool validString(QString &input);
    void loadHabits();
};
#endif // HABBIT_TRACKER_H
