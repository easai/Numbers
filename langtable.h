#ifndef LANGTABLE_H
#define LANGTABLE_H

#include <QHash>
#include <QObject>
#include <QSqlDatabase>

class LangTable : public QObject {
  Q_OBJECT
public:
  explicit LangTable(QObject *parent = nullptr);

  void retrieve(QSqlDatabase *db);
  QList<QString> keys();
  int get(const QString &key);
  QString getEn(int idx);

  QHash<QString, int> table() const;

signals:

private:
    QHash<QString, int> m_indexTable;
    QHash<int, QString> m_table;
};

#endif // LANGTABLE_H
