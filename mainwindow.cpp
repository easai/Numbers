#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QtAlgorithms>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->comboBox, &QComboBox::currentIndexChanged, this,
          &MainWindow::setLang);
  connect(ui->pushButton_save, &QPushButton::clicked, this,
          &MainWindow::saveItem);
  connect(ui->action_Quit, &QAction::triggered, this, &QApplication::quit);
  m_db = QSqlDatabase::addDatabase("QODBC", "linguistics");
  m_db.setDatabaseName("linguistics");
  initLang();
  showTable(1);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initLang() {
  m_langTable.retrieve(&m_db);
  QStringList langList = m_langTable.keys();
  langList.sort();
  for (int i = 0; i < langList.count(); i++) {
    ui->comboBox->addItem(langList.at(i));
  }
}

void MainWindow::setLang() {
  QString lang = ui->comboBox->currentText();
  int lang_id = m_langTable.get(lang);
  showTable(lang_id);
}

void MainWindow::saveItem()
{
  QString num=ui->lineEdit_num->text();
  QString exp=ui->lineEdit_exp->text();
  QString lang = ui->comboBox->currentText();
  int lang_id=m_langTable.get(lang);
  m_numberTable.saveItem(&m_db, num.toInt(), exp, lang_id, lang);
  showTable(lang_id);
}

void MainWindow::showTable(const int &lang_id) {
  int nItems=m_numberTable.retrieve(&m_db, lang_id);
  ui->tableWidget->setRowCount(0);
  if(nItems<=0){
    return;
  }
  QList<int> keyList = m_numberTable.keys();
  std::sort(keyList.begin(), keyList.end());

  QStringList header;
  header << "Number"
         << "Lang";


  ui->tableWidget->setColumnCount(header.count());
  ui->tableWidget->setHorizontalHeaderLabels(header);
  ui->tableWidget->verticalHeader()->setVisible(false);

  for (int i = 0; i < keyList.count(); i++) {
    int key = keyList.at(i);
    QString val = m_numberTable.get(key);

    ui->tableWidget->insertRow(i);
    QTableWidgetItem *targetItem =
        new QTableWidgetItem(QVariant(key).toString());
    ui->tableWidget->setItem(i, 0, targetItem);
    QTableWidgetItem *descItem = new QTableWidgetItem(val);
    ui->tableWidget->setItem(i, 1, descItem);
  }
}
