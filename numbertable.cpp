#include "numbertable.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>

NumberTable::NumberTable(QObject *parent) : QObject{parent} {}

int NumberTable::retrieve(QSqlDatabase *db, int lang_id) {
  m_table.clear();
  if (!db->open()) {
    qInfo() << db->lastError().text();
    return 0;
  }
  QSqlQuery query(*db);
  QString sql = "SELECT id, value, expression, kana FROM `numbers` WHERE "
                "lang_id=:lang_id";
  query.prepare(sql);
  query.bindValue(":lang_id", lang_id);
  if (!query.exec()) {
    qInfo() << db->lastError().text();
    qInfo() << query.lastError().text();
  } else {
    while (query.next()) {
      QString id = query.value(0).toString();
      QString val = query.value(1).toString();
      QString exp = query.value(2).toString();
      QString ipa = query.value(3).toString();
      m_table.insert(val.toInt(), exp);
    }
  }
  db->close();
  return m_table.size();
}

QHash<int, QString> NumberTable::table() const { return m_table; }

QList<int> NumberTable::keys() { return m_table.keys(); }

QString NumberTable::get(int key) {
  QString val = "";
  QHash<int, QString>::iterator i = m_table.find(key);
  if (i != m_table.end() && i.key() == key) {
    val = i.value();
  }
  return val;
}

QString NumberTable::saveItem(QSqlDatabase *db, int num, const QString& exp, int lang_id, const QString& lang)
{
  if (!db->open()) {
    qInfo() << db->lastError().text();
    return 0;
  }
  QSqlQuery query(*db);
  QString sql = "INSERT INTO `numbers` (value, expression, lang_id, language) VALUES (:num, :exp, :lang_id, :lang)";
  query.prepare(sql);
  query.bindValue(":lang_id", lang_id);
  query.bindValue(":num", num);
  query.bindValue(":exp", exp);
  query.bindValue(":lang", lang);
  if (!query.exec()) {
    qInfo() << db->lastError().text();
    qInfo() << query.lastError().text();
  }
  db->close();
}
