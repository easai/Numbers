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

int NumberTable::retrieve(QSqlDatabase *db) {
  m_table.clear();
  if (m_lang_id == -1) {
    return 0;
  }
  if (!db->open()) {
    qInfo() << db->lastError().text();
    return 0;
  }
  QSqlQuery query(*db);
  QString sql = "SELECT value, expression, kana FROM `numbers` WHERE "
                "lang_id=:lang_id";
  query.prepare(sql);
  query.bindValue(":lang_id", m_lang_id);
  if (!query.exec()) {
    qInfo() << db->lastError().text();
    qInfo() << query.lastError().text();
  } else {
    int idx = -1;
    while (query.next()) {
      idx = -1;
      QString val = query.value(++idx).toString();
      QString exp = query.value(++idx).toString();
      QString ipa = query.value(++idx).toString();
      m_table.insert(val.toInt(), exp);
    }
  }
  db->close();
  return m_table.size();
}

QHash<int, QString> NumberTable::table() const { return m_table; }

QList<int> NumberTable::keys() { return m_table.keys(); }

QString NumberTable::getExp(int key) const { return m_table[key]; }

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

bool NumberTable::contains(int key) const { return m_table.contains(key); }

int NumberTable::lang_id() const { return m_lang_id; }

void NumberTable::setLang_id(int newLang_id) { m_lang_id = newLang_id; }
