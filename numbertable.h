#ifndef NUMBERTABLE_H
#define NUMBERTABLE_H

#include <QHash>
#include <QObject>
#include <QSqlDatabase>

class NumberTable : public QObject {
public:
  NumberTable(QObject *parent = nullptr);
  NumberTable(const NumberTable &);
  NumberTable &operator=(const NumberTable &);

  int retrieve(QSqlDatabase *db, int lang_id);

  QHash<int, QString> table() const;

  QList<int> keys();
  QString get(int key) const;
  void createItem(QSqlDatabase *db, int num, const QString &exp, int lang_id,
                  const QString &lang);
  void updateItem(QSqlDatabase *db, int num, const QString &exp, int lang_id);
  bool contains(int key) const;

  // Getters/Setters
  int lang_id() const;
  void setLang_id(int newLang_id);

private:
  int m_lang_id=-1;
  QHash<int, QString> m_table;
};

#endif // NUMBERTABLE_H
