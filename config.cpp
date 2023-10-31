#include "config.h"
#include <QSettings>

Config::Config(QObject *parent) : QObject{parent} {}

Config::Config(const Config &o) : m_lang(o.m_lang) {}

Config &Config::operator=(const Config &o) {
  if (this != &o) {
    m_lang = o.m_lang;
  }
  return *this;
}

void Config::load() {
  QSettings settings(AUTHOR, APPNAME);
  settings.beginGroup(GENERAL);
  m_lang = settings.value(LANG).toString();
  settings.endGroup();
}

void Config::save() {
  QSettings settings(AUTHOR, APPNAME);
  settings.beginGroup(GENERAL);
  settings.setValue(LANG, m_lang);
  settings.endGroup();
}

void Config::setLang(const QString &newLang) { m_lang = newLang; }

QString Config::lang() const { return m_lang; }
