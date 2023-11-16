#ifndef CONFIG_H
#define CONFIG_H

#include "qfont.h"
#include <QObject>

#define AUTHOR "easai"
#define APPNAME "Numbers"
#define GENERAL "General"
#define LANG "Lang"
#define LANGLIST "LanguageList"
#define GEOM "Geometry"
#define FONT "Font"

class Config : public QObject {
  Q_OBJECT
public:
  explicit Config(QObject *parent = nullptr);
  Config(const Config &);
  Config &operator=(const Config &);

  QString local() const;
  QString url() const;

  void setLocal(const QString &newLocal);
  void setUrl(const QString &newUrl);

  void load();
  void save();

  // getters and setters
  QString lang() const;
  void setLang(const QString &newLang);

  QByteArray geom() const;
  void setGeom(const QByteArray &newGeom);

  QList<int> langList() const;
  void setLangList(const QList<int> &newLangList);

  QFont font() const;
  void setFont(const QFont &newFont);

  signals:
private:
  QString m_lang;
  QList<int> m_langList;
  QByteArray m_geom;
  QFont m_font;
};

#endif // CONFIG_H
