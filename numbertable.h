#ifndef NUMBERTABLE_H
#define NUMBERTABLE_H

#include <QHash>
#include <QObject>
#include <QSqlDatabase>

class NumberTable : public QObject {
public:
  NumberTable(QObject *parent = nullptr);

  int retrieve(QSqlDatabase *db, int lang_id);

  QHash<int, QString> table() const;

  QList<int> keys();
  QString get(int key);
  QString saveItem(QSqlDatabase *db, int num, const QString& exp, int lang_id, const QString& lang);

private:
  QHash<int, QString> m_table;
};

#endif // NUMBERTABLE_H
