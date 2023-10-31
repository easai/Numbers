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
      m_table.insert(en, id.toInt());
    }
  }
  db->close();
}

QList<QString> LangTable::keys() { return m_table.keys(); }

int LangTable::get(const QString &key) {
  int val = 0;
  QHash<QString, int>::iterator i = m_table.find(key);
  if (i != m_table.end() && i.key() == key) {
    val = i.value();
  }
  return val;
}

QHash<QString, int> LangTable::table() const { return m_table; }
