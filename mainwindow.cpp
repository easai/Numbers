#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "aboutdialog.h"
#include "config.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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
  connect(ui->pushButton_save, &QPushButton::clicked, this,
          &MainWindow::createItem);
  connect(ui->action_Save, &QAction::triggered, this, &MainWindow::saveFile);
  connect(ui->action_Open, &QAction::triggered, this, &MainWindow::openFile);
  connect(ui->action_About_Numbers, &QAction::triggered, this,
          &MainWindow::about);
  connect(ui->action_Quit, &QAction::triggered, this, &QApplication::quit);
  m_db = QSqlDatabase::addDatabase("QODBC", "linguistics");
  m_db.setDatabaseName("linguistics");
  initLang();
  m_config.load();
  restoreGeometry(m_config.geom());
  m_tableList.clear();
  setLangList();
  showTable();
  QString defaultLang = m_config.lang();
  ui->comboBox->setCurrentText(defaultLang);
  for (QTableWidget *pTableWidget : m_tableWidget) {
    connect(pTableWidget, &QTableWidget::cellChanged, this,
            &MainWindow::updateItem);
    connect(pTableWidget->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::updateLang);
  }
  connect(ui->comboBox, &QComboBox::currentIndexChanged, this,
          &MainWindow::setSelectedLang);
}

MainWindow::~MainWindow() {
  QList<int> langList;
  for (NumberTable table : m_tableList) {
    int lang_id = table.lang_id();
    langList.append(lang_id);
  }
  m_config.setLangList(langList);
  m_config.setGeom(saveGeometry());
  m_config.save();
  delete ui;
}

void MainWindow::setLangList() {
  m_tableList.clear();
  for (int i : m_config.langList()) {
    NumberTable table;
    table.setLang_id(i);
    m_tableList.append(table);
  }
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
  table.setLang_id(lang_id);
  m_tableList.clear();
  m_tableList.append(table);
  showTable();
}

void MainWindow::updateLang() {
  QStringList lst = m_langTable.keys();
  lst.append("-");
  lst.sort();
  bool ok;
  QString lang = QInputDialog::getItem(this, tr("Select language"), tr("en"),
                                       lst, 0, false, &ok);
  if (ok) {
    int tabIndex = ui->tabWidget->currentIndex();
    QTableWidget *pTableWidget = m_tableWidget[tabIndex];
    int col = pTableWidget->horizontalHeader()->currentIndex().column();
    if (lang == "-") {
      if (col - 1 < m_tableList.size()) {
        m_tableList.removeAt(col - 1);
      }
    } else {
      NumberTable table;
      int lang_id = m_langTable.get(lang);
      table.setLang_id(lang_id);
      if (m_tableList.size() <= col - 1) {
        m_tableList.append(table);
      } else {
        m_tableList.replace(col - 1, table);
      }
    }
    showTable();
  }
}

void MainWindow::createItem() {
  if (m_tableList.size() == 0) {
    return;
  }
  QString num = ui->lineEdit_num->text();
  QString exp = ui->lineEdit_exp->text();
  QString lang = ui->comboBox->currentText();
  int lang_id = m_langTable.get(lang);
  m_tableList[0].createItem(&m_db, num.toInt(), exp, lang_id, lang);
  showTable();
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
    NumberTable table = m_tableList[col - 1];
    int lang_id = table.lang_id();
    if (table.contains(num)) {
      table.updateItem(&m_db, num, exp, lang_id);
    } else {
      QString lang = m_langTable.getEn(lang_id);
      table.createItem(&m_db, num, exp, lang_id, lang);
    }
  }
}

void MainWindow::openFile() {
  QString selFilter = tr("JSON Documents(*.json)");
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open language list"), ".",
      tr("JSON Documents(*.json);;All(*.*)"), &selFilter,
      QFileDialog::DontUseCustomDirectoryIcons);
  if (!fileName.isEmpty()) {
    QFile openFile(fileName);
    openFile.open(QIODevice::ReadOnly);
    QByteArray data = openFile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(data));
    QJsonObject jsonObj(jsonDoc.object());
    QJsonArray jsonArr = jsonObj[JSONTAG].toArray();
    QList<int> lst;
    for (auto item : jsonArr) {
      lst.append(item.toInt());
    }
    m_config.setLangList(lst);
    setLangList();
    showTable();
  }
}

void MainWindow::saveFile() {
  QJsonObject jsonObj;
  QJsonArray jsonArr;
  for (const NumberTable table : m_tableList) {
    jsonArr.append(table.lang_id());
  }
  jsonObj[JSONTAG] = jsonArr;
  QJsonDocument jsonDoc(jsonObj);
  QByteArray data(jsonDoc.toJson());

  QString selFilter;
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save language list"), ".", tr("JSON documents(*.json)"),
      &selFilter, QFileDialog::DontUseCustomDirectoryIcons);
  if (!fileName.isEmpty()) {
    QFile saveFile(fileName);
    saveFile.open(QIODevice::WriteOnly);
    saveFile.write(data);
    saveFile.close();
  }
}

void MainWindow::showTable() {
  if (m_tableList.size() == 0) {
    return;
  }

  // set header
  QStringList header;
  header << "Number";
  for (NumberTable &table : m_tableList) {
    QString lang = "";
    int lang_id = table.lang_id();
    table.retrieve(&m_db);
    lang = m_langTable.getEn(lang_id);
    header << lang;
  }
  NumberTable numberTable = m_tableList[0];
  QList<int> keyList = numberTable.keys();
  std::sort(keyList.begin(), keyList.end());

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
    for (int i = 0; i < m_tableList.size(); i++) {
      NumberTable table = m_tableList[i];
      if (table.contains(key)) {
        QString exp = table.getExp(key);
        pTableWidget->setItem(row, i + 1, new QTableWidgetItem(exp));
      }
    }
  }
}

void MainWindow::about() {
  AboutDialog *dlg = new AboutDialog(this);
  dlg->exec();
}
