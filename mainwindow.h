#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "langtable.h"
#include "numbertable.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void setLang();
  void updateLang();
  void saveItem();
  void updateItem();
  void about();

private:
  Ui::MainWindow *ui;
  NumberTable m_numberTable;
  QList<NumberTable> m_numberTableList;
  QSqlDatabase m_db;
  LangTable m_langTable;
  Config m_config;

  QList<QTableWidget *> m_tableWidget;

  void initLang();
  void showTable(const int &lang_id);

};
#endif // MAINWINDOW_H
