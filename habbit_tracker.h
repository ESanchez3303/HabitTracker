#ifndef HABBIT_TRACKER_H
#define HABBIT_TRACKER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Habbit_tracker;
}
QT_END_NAMESPACE

class Habbit_tracker : public QMainWindow
{
    Q_OBJECT

public:
    Habbit_tracker(QWidget *parent = nullptr);
    ~Habbit_tracker();

private:
    Ui::Habbit_tracker *ui;
};
#endif // HABBIT_TRACKER_H
