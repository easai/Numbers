#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "aboutdialog.h"
#include "config.h"

#include <QtAlgorithms>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  m_tableWidget.append(ui->tableWidget_10);
  m_tableWidget.append(ui->tableWidget_19);
  m_tableWidget.append(ui->tableWidget_99);
  m_tableWidget.append(ui->tableWidget_100);
  m_tableWidget.append(ui->tableWidget_1000);
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
  for (QTableWidget *pTableWidget : m_tableWidget) {
    connect(pTableWidget, &QTableWidget::cellChanged, this,
            &MainWindow::updateItem);
  }
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
  int tabIndex = ui->tabWidget->currentIndex();
  QTableWidget *pTableWidget = m_tableWidget[tabIndex];
  QList<QTableWidgetItem *> lst = pTableWidget->selectedItems();
  for (QTableWidgetItem *pItem : lst) {
    QString exp = pItem->text();
    int row = pItem->row();
    QTableWidgetItem *pNum = pTableWidget->item(row, 0);
    int num = pNum->text().toInt();
    int lang_id = m_langTable.get(m_config.lang());
    m_numberTable.updateItem(&m_db, num, exp, lang_id);
  }
}

void MainWindow::showTable(const int &lang_id) {
  int nItems = m_numberTable.retrieve(&m_db, lang_id);

  if (nItems <= 0) {
    return;
  }
  QList<int> keyList = m_numberTable.keys();
  std::sort(keyList.begin(), keyList.end());

  QStringList header;
  header << "Number"
         << "Lang";

  for (QTableWidget *pTableWidget : m_tableWidget) {
    pTableWidget->setRowCount(0);
    pTableWidget->setColumnCount(header.count());
    pTableWidget->setHorizontalHeaderLabels(header);
    pTableWidget->verticalHeader()->setVisible(false);
  }

  QTableWidget *pTableWidget;
  for (int i = 0; i < keyList.count(); i++) {
    int key = keyList.at(i);
    QString val = m_numberTable.get(key);

    if (key <= 10) {
      pTableWidget = ui->tableWidget_10;
    } else if (key < 20) {
      pTableWidget = ui->tableWidget_19;
    } else if (key < 100) {
      pTableWidget = ui->tableWidget_99;
    } else if (key < 1000) {
      pTableWidget = ui->tableWidget_100;
    } else {
      pTableWidget = ui->tableWidget_1000;
    }
    int row = pTableWidget->rowCount();
    pTableWidget->insertRow(row);
    QTableWidgetItem *targetItem =
        new QTableWidgetItem(QVariant(key).toString());
    pTableWidget->setItem(row, 0, targetItem);
    QTableWidgetItem *descItem = new QTableWidgetItem(val);
    pTableWidget->setItem(row, 1, descItem);
  }
}

void MainWindow::about() {
  AboutDialog *dlg = new AboutDialog(this);
  dlg->exec();
}
