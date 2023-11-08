#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "langtable.h"
#include "numbertable.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QTableWidget>

#define JSONTAG "LangList"

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
  void setLangList();
  void setSelectedLang();
  void updateLang();
  void createItem();
  void updateItem();
  void deleteItem();
  void openFile();
  void saveFile();
  void deleteNumber(const QPoint &pos);
  void about();

private:
  Ui::MainWindow *ui;
  QList<NumberTable> m_tableList;
  QSqlDatabase m_db;
  LangTable m_langTable;
  Config m_config;
  QTableWidgetItem* m_current;

  QList<QTableWidget *> m_tableWidget;

  void initLang();
  void showTable();
};
#endif // MAINWINDOW_H
