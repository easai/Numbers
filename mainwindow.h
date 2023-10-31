#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "numbertable.h"
#include "langtable.h"
#include <QMainWindow>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void setLang();

private:
    Ui::MainWindow *ui;
    NumberTable m_numberTable;
    QSqlDatabase m_db;
    LangTable m_langTable;

    void initLang();

    void showTable(const int &lang_id);
};
#endif // MAINWINDOW_H
