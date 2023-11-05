#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "aboutdialog.h"
#include "config.h"

#include <QInputDialog>
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
          &MainWindow::setSelectedLang);
  connect(ui->pushButton_save, &QPushButton::clicked, this,
          &MainWindow::createItem);
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
    connect(pTableWidget->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::updateLang);
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

void MainWindow::setSelectedLang() {
  QString lang = ui->comboBox->currentText();
  int lang_id = m_langTable.get(lang);
  m_config.setLang(lang);
  NumberTable table;
  table.retrieve(&m_db, lang_id);
  m_numberTableList.clear();
  m_numberTableList.append(table);
  showTable(lang_id);
}

void MainWindow::updateLang() {
  QStringList lst = m_langTable.keys();
  lst.sort();
  bool ok;
  QString lang = QInputDialog::getItem(this, tr("Select language"), tr("en"),
                                       lst, 0, false, &ok);
  if (ok) {
    int lang_id = m_langTable.get(lang);
    NumberTable table;
    table.retrieve(&m_db, lang_id);
    int tabIndex = ui->tabWidget->currentIndex();
    QTableWidget *pTableWidget = m_tableWidget[tabIndex];
    int col = pTableWidget->horizontalHeader()->currentIndex().column();
    if (m_numberTableList.size() <= col - 1) {
      m_numberTableList.append(table);
    } else {
      m_numberTableList.replace(col - 1, table);
    }
    int selected_id = m_langTable.get(this->m_config.lang());
    showTable(selected_id);
  }
}

void MainWindow::createItem() {
  if (m_numberTableList.size() == 0) {
    return;
  }
  QString num = ui->lineEdit_num->text();
  QString exp = ui->lineEdit_exp->text();
  QString lang = ui->comboBox->currentText();
  int lang_id = m_langTable.get(lang);
  m_numberTableList[0].createItem(&m_db, num.toInt(), exp, lang_id, lang);
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
    int col = pItem->column();
    NumberTable table = m_numberTableList[col - 1];
    int lang_id = table.lang_id();
    if (table.contains(num)) {
      table.updateItem(&m_db, num, exp, lang_id);
    } else {
      QString lang = m_langTable.getEn(lang_id);
      table.createItem(&m_db, num, exp, lang_id, lang);
    }
  }
}

void MainWindow::showTable(const int &lang_id) {
  if (m_numberTableList.size() == 0 ||
      m_numberTableList[0].keys().size() == 0) {
    return;
  }
  NumberTable numberTable = m_numberTableList[0];
  QList<int> keyList = numberTable.keys();
  std::sort(keyList.begin(), keyList.end());

  // set header
  QStringList header;
  header << "Number";
  for (const NumberTable& table : m_numberTableList) {
    QString lang = "";
    int lang_id = table.lang_id();
    lang = m_langTable.getEn(lang_id);
    header << lang;
  }
  header << "";

  for (QTableWidget *pTableWidget : m_tableWidget) {
    pTableWidget->setRowCount(0);
    pTableWidget->setColumnCount(header.count());
    pTableWidget->setHorizontalHeaderLabels(header);
    pTableWidget->verticalHeader()->setVisible(false);
  }

  // set table
  QTableWidget *pTableWidget;
  for (int i = 0; i < keyList.count(); i++) {
    int key = keyList.at(i);
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
    int col = 0;
    for (const NumberTable& table : m_numberTableList) {
      if (table.contains(key)) {
        QString exp = table.get(key);
        pTableWidget->setItem(row, ++col, new QTableWidgetItem(exp));
      }
    }
  }
}

void MainWindow::about() {
  AboutDialog *dlg = new AboutDialog(this);
  dlg->exec();
}
