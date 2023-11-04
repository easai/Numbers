#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "aboutdialog.h"
#include "config.h"

#include <QtAlgorithms>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setWindowIcon(QIcon("://images/numbers-favicon.ico"));
  connect(ui->comboBox, &QComboBox::currentIndexChanged, this,
          &MainWindow::setLang);
  connect(ui->pushButton_save, &QPushButton::clicked, this,
          &MainWindow::saveItem);
  connect(ui->action_About_Numbers, &QAction::triggered, this,
          &MainWindow::about);
  connect(ui->action_Quit, &QAction::triggered, this, &QApplication::quit);
  m_db = QSqlDatabase::addDatabase("QODBC", "linguistics");
  m_db.setDatabaseName("linguistics");
  initLang();
  m_config.load();
  restoreGeometry(m_config.geom());

  QString defaultLang = m_config.lang();
  ui->comboBox->setCurrentText(defaultLang);
  connect(ui->tableWidget, &QTableWidget::cellChanged, this,
          &MainWindow::updateItem);
}

MainWindow::~MainWindow() {
  m_config.setGeom(saveGeometry());
  m_config.save();
  delete ui;
}

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
  m_config.setLang(lang);
  showTable(lang_id);
}

void MainWindow::saveItem() {
  QString num = ui->lineEdit_num->text();
  QString exp = ui->lineEdit_exp->text();
  QString lang = ui->comboBox->currentText();
  int lang_id = m_langTable.get(lang);
  m_numberTable.createItem(&m_db, num.toInt(), exp, lang_id, lang);
  showTable(lang_id);
}

void MainWindow::updateItem() {
  QList<QTableWidgetItem *> lst = ui->tableWidget->selectedItems();
  for (QTableWidgetItem *pItem : lst) {
    QString exp = pItem->text();
    int row = pItem->row();
    QTableWidgetItem *pNum = ui->tableWidget->item(row, 0);
    int num = pNum->text().toInt();
    int lang_id = m_langTable.get(m_config.lang());
    m_numberTable.updateItem(&m_db, num, exp, lang_id);
  }
}

void MainWindow::showTable(const int &lang_id) {
  int nItems = m_numberTable.retrieve(&m_db, lang_id);
  ui->tableWidget->setRowCount(0);
  if (nItems <= 0) {
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

void MainWindow::about() {
  AboutDialog *dlg = new AboutDialog(this);
  dlg->exec();
}
