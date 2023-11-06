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
  void setSelectedLang();
  void updateLang();
  void createItem();
  void updateItem();
  void about();

private:
  Ui::MainWindow *ui;
  QList<NumberTable> m_tableList;
  QSqlDatabase m_db;
  LangTable m_langTable;
  Config m_config;

  QList<QTableWidget *> m_tableWidget;

  void initLang();
  void showTable();
};
#endif // MAINWINDOW_H
