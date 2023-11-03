#include "config.h"
#include <QSettings>

Config::Config(QObject *parent) : QObject{parent} {}

Config::Config(const Config &o) : m_lang(o.m_lang), m_geom(o.m_geom) {}

Config &Config::operator=(const Config &o) {
  if (this != &o) {
    m_lang = o.m_lang;
    m_geom = o.m_geom;
  }
  return *this;
}

void Config::load() {
  QSettings settings(AUTHOR, APPNAME);
  settings.beginGroup(GENERAL);
  m_lang = settings.value(LANG).toString();
  m_geom = settings.value(GEOM).toByteArray();
  settings.endGroup();
}

void Config::save() {
  QSettings settings(AUTHOR, APPNAME);
  settings.beginGroup(GENERAL);
  settings.setValue(LANG, m_lang);
  settings.setValue(GEOM, m_geom);
  settings.endGroup();
}

// getters and setters
QString Config::lang() const { return m_lang; }

void Config::setLang(const QString &newLang) { m_lang = newLang; }

QByteArray Config::geom() const { return m_geom; }

void Config::setGeom(const QByteArray &newGeom) { m_geom = newGeom; }
