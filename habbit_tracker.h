#ifndef HABBIT_TRACKER_H
#define HABBIT_TRACKER_H

#include <QMainWindow>
#include <QFrame>
#include <QRadioButton>
#include <filesystem>

#include "habitClass.cpp"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class Habbit_tracker;
}
QT_END_NAMESPACE

class Habbit_tracker : public QMainWindow{
    Q_OBJECT




public:
    Habbit_tracker(QWidget *parent = nullptr);
    ~Habbit_tracker();

private:
    // Variables
    Ui::Habbit_tracker *ui;
    int habitCount = 0;
    vector<habit> allHabits;
    string filesPath = "/habits";


    // Frame Control Functions:
    void switchFrame(QFrame* target);


    // Main Frame Functions
    void M_addHabbitButtonClicked();


    // Add Frame Functions
    bool isCapps = true;
    void setCapps();
    void insertKey();
    void A_cancelButtonClicked();
    void A_saveButtonClicked();


    // Helper Functions:
    bool validString(QString &input);
    void loadHabits();
};
#endif // HABBIT_TRACKER_H
