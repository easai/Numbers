#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

#define AUTHOR "easai"
#define APPNAME "Numbers"
#define GENERAL "General"
#define LANG "Lang"

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

  void setLang(const QString &newLang);

  QString lang() const;

signals:
private:
  QString m_lang;
};

#endif // CONFIG_H
