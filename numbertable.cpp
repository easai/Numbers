#include "numbertable.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>

NumberTable::NumberTable(QObject *parent) : QObject{parent} {}

NumberTable::NumberTable(const NumberTable &o)
    : m_table(o.m_table), m_lang_id(o.m_lang_id) {}

NumberTable &NumberTable::operator=(const NumberTable &o) {
  if (this != &o) {
    m_table = o.m_table;
    m_lang_id = o.m_lang_id;
  }
  return *this;
}

int NumberTable::retrieve(QSqlDatabase *db, int lang_id) {
  m_lang_id = lang_id;
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
  return m_table[key];
}

void NumberTable::createItem(QSqlDatabase *db, int num, const QString &exp,
                             int lang_id, const QString &lang) {
  if (!db->open()) {
    qInfo() << db->lastError().text();
    return;
  }
  QSqlQuery query(*db);
  QString sql = "INSERT INTO `numbers` (value, expression, lang_id, language) "
                "VALUES (:num, :exp, :lang_id, :lang)";
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

void NumberTable::updateItem(QSqlDatabase *db, int num, const QString &exp,
                             int lang_id) {
  if (!db->open()) {
    qInfo() << db->lastError().text();
    return;
  }
  QSqlQuery query(*db);
  QString sql = "UPDATE `numbers` SET expression=:exp WHERE lang_id=:lang_id "
                "AND value=:num";
  query.prepare(sql);
  query.bindValue(":lang_id", lang_id);
  query.bindValue(":num", num);
  query.bindValue(":exp", exp);
  if (!query.exec()) {
    qInfo() << db->lastError().text();
    qInfo() << query.lastError().text();
  }
  db->close();
}

bool NumberTable::contains(int key)
{
  return m_table.contains(key);
}

int NumberTable::lang_id() const { return m_lang_id; }

void NumberTable::setLang_id(int newLang_id) { m_lang_id = newLang_id; }
