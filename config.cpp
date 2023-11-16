#include "config.h"
#include <QSettings>

Config::Config(QObject *parent) : QObject{parent} {}

Config::Config(const Config &o)
    : m_lang(o.m_lang), m_langList(o.m_langList), m_geom(o.m_geom), m_font(o.m_font) {}

Config &Config::operator=(const Config &o) {
  if (this != &o) {
    m_lang = o.m_lang;
    m_langList = o.m_langList;
    m_geom = o.m_geom;
    m_font=o.m_font;
  }
  return *this;
}

void Config::load() {
  QSettings settings(AUTHOR, APPNAME);
  settings.beginGroup(GENERAL);
  m_lang = settings.value(LANG).toString();
  QVariantList list = settings.value(LANGLIST, QVariantList()).toList();
  for (const QVariant &item : list) {
    m_langList << item.toInt();
  }
  m_geom = settings.value(GEOM).toByteArray();
  m_font.fromString(settings.value(FONT).toString());
  settings.endGroup();
}

void Config::save() {
  QSettings settings(AUTHOR, APPNAME);
  settings.beginGroup(GENERAL);
  settings.setValue(LANG, m_lang);
  QVariantList lst;
  for (int i : m_langList) {
    lst << i;
  }
  settings.setValue(LANGLIST, lst);
  settings.setValue(GEOM, m_geom);
  settings.setValue(FONT, m_font.toString());
  settings.endGroup();
}

// getters and setters
QString Config::lang() const { return m_lang; }

void Config::setLang(const QString &newLang) { m_lang = newLang; }

QByteArray Config::geom() const { return m_geom; }

void Config::setGeom(const QByteArray &newGeom) { m_geom = newGeom; }

QList<int> Config::langList() const { return m_langList; }

void Config::setLangList(const QList<int> &newLangList) {
  m_langList = newLangList;
}

QFont Config::font() const { return m_font; }

void Config::setFont(const QFont &newFont) { m_font = newFont; }
