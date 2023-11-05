#include "langtable.h"
#include <QDebug>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>

LangTable::LangTable(QObject *parent) : QObject{parent} {}

void LangTable::retrieve(QSqlDatabase *db) {
  if (!db->open()) {
    qInfo() << db->lastError().text();
    return;
  }
  QSqlQuery query(*db);
  QString sql = "SELECT id, en FROM `languages`";

  if (!query.exec(sql)) {
    qInfo() << db->lastError().text();
    qInfo() << query.lastError().text();
  } else {
    while (query.next()) {
      QString id = query.value(0).toString();
      QString en = query.value(1).toString();
      m_indexTable.insert(en, id.toInt());
      m_table.insert(id.toInt(), en);
    }
  }
  db->close();
}

QStringList LangTable::keys() { return m_indexTable.keys(); }

int LangTable::get(const QString &key) { return m_indexTable[key]; }

QString LangTable::getEn(int idx) { return m_table[idx]; }

QHash<QString, int> LangTable::table() const { return m_indexTable; }
