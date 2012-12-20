#include "searchbar.h"
#include "ui_searchbar.h"

#include <QSettings>
#include <QCompleter>
#include <QLineEdit>
#include <QKeyEvent>

SearchBar::SearchBar(QString settingsPrefix,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBar)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QStringList searches;

   ui->setupUi(this);

   m_settingsPrefix = settingsPrefix;

   ui->replaceText->addItem("");

   settings.beginGroup(m_settingsPrefix);
   searches = settings.value("SearchTextHistory").toStringList();
   ui->caseSensitive->setChecked(settings.value("CaseSensitive",QVariant(false)).toBool());
   ui->regex->setChecked(settings.value("RegularExpression",QVariant(false)).toBool());
   ui->direction->setChecked(settings.value("Direction",QVariant(true)).toBool());
   ui->replaceText->addItems(settings.value("ReplaceTextHistory").toStringList());
   settings.endGroup();

   ui->searchText->addItem("");
   foreach ( QString search, searches )
   {
      ui->searchText->addItem(search);
   }
   ui->searchText->completer()->setCompletionMode(QCompleter::PopupCompletion);

   ui->searchText->installEventFilter(this);

   ui->replaceText->completer()->setCompletionMode(QCompleter::PopupCompletion);

   ui->close->hide();
}

SearchBar::~SearchBar()
{
   delete ui;
}

bool SearchBar::eventFilter(QObject* object, QEvent *event)
{
   // Only allow ESC to close if close button is shown.
   if ( ui->close->isVisible() )
   {
      if ( (object == ui->searchText) &&
           (event->type() == QEvent::KeyPress) )
      {
         QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
         if ( keyEvent->key() == Qt::Key_Escape )
         {
            hide();
            return true;
         }
      }
   }
   return false;
}

void SearchBar::focusInEvent(QFocusEvent */*event*/)
{
   ui->searchText->setFocus();
   ui->searchText->lineEdit()->setSelection(0,ui->searchText->currentText().length());
}

void SearchBar::showCloseButton(bool show)
{
   if ( show )
   {
      ui->close->show();
   }
   else
   {
      ui->close->hide();
   }
}

QString SearchBar::currentSearchText()
{
   return ui->searchText->currentText();
}

bool    SearchBar::isCaseSensitive()
{
   return ui->caseSensitive->isChecked();
}

bool    SearchBar::isRegularExpression()
{
   return ui->regex->isChecked();
}

bool    SearchBar::searchIsDown()
{
   return ui->direction->isChecked();
}

void SearchBar::on_searchText_activated(QString search)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QStringList items;

   settings.beginGroup(m_settingsPrefix);
   if ( !search.isEmpty() )
   {
      items = settings.value("SearchTextHistory").toStringList();
      if ( !items.contains(search) )
      {
         if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
         {
            items.removeAt(0);
         }
         items.append(search);
      }
      settings.setValue("SearchTextHistory",QVariant(items));

      emit snapTo("SearchBar,"
                  +QString::number(ui->caseSensitive->isChecked())+","
                  +QString::number(ui->regex->isChecked())+","
                  +QString::number(ui->direction->isChecked())+","
                  +search);
   }
   settings.endGroup();
}

void SearchBar::on_searchText_editTextChanged(QString search)
{
   if ( !search.isEmpty() )
   {
      emit snapTo("SearchBar,"
                  +QString::number(ui->caseSensitive->isChecked())+","
                  +QString::number(ui->regex->isChecked())+","
                  +QString::number(ui->direction->isChecked())+","
                  +search);
   }
   else
   {
      ui->searchText->lineEdit()->setStyleSheet("QLineEdit { background: white; color: black }");
   }
}

void SearchBar::on_caseSensitive_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   settings.beginGroup(m_settingsPrefix);
   settings.setValue("CaseSensitive",QVariant(checked));
   settings.endGroup();
}

void SearchBar::on_regex_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   settings.beginGroup(m_settingsPrefix);
   settings.setValue("RegularExpression",QVariant(checked));
   settings.endGroup();
}

void SearchBar::on_direction_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   settings.beginGroup(m_settingsPrefix);
   settings.setValue("Direction",QVariant(checked));
   settings.endGroup();
}

void SearchBar::activateMe(QString item)
{
   show();
   setFocus();
   if ( !item.isEmpty() )
   {
      ui->searchText->setEditText(item);
      ui->searchText->lineEdit()->setSelection(0,ui->searchText->currentText().length());
   }
}

void SearchBar::hintMe(bool found)
{
   if ( found )
   {
      ui->searchText->lineEdit()->setStyleSheet("QLineEdit { background: white; color: black }");
   }
   else
   {
      ui->searchText->lineEdit()->setStyleSheet("QLineEdit { background: #ff8080; color: white }");
   }
}

void SearchBar::on_close_clicked()
{
   hide();
}

void SearchBar::on_findForReplace_clicked()
{
   emit snapTo("SearchBar,"
               +QString::number(isCaseSensitive())+","
               +QString::number(isRegularExpression())+","
               +QString::number(searchIsDown())+","
               +currentSearchText());
}

void SearchBar::on_replace_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QStringList items;

   settings.beginGroup("Search");

   if ( (ui->replaceText->findText(ui->replaceText->currentText(),Qt::MatchExactly|Qt::MatchCaseSensitive) == -1) )
   {
      ui->replaceText->addItem(ui->replaceText->currentText());
      items = settings.value("ReplaceTextHistory").toStringList();
      if ( !items.contains(ui->replaceText->currentText()) )
      {
         if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
         {
            items.removeAt(0);
         }
         items.append(ui->replaceText->currentText());
      }
      settings.setValue("ReplaceTextHistory",QVariant(items));
   }

   settings.endGroup();

   emit replaceText(QString("SearchBar,"
                            +QString::number(ui->caseSensitive->isChecked())+","
                            +QString::number(ui->regex->isChecked())+","
                            +QString::number(ui->direction->isChecked())+","
                            +ui->searchText->currentText()),
                    ui->replaceText->currentText(),false);
}

void SearchBar::on_replaceAll_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QStringList items;

   settings.beginGroup("Search");

   if ( (ui->replaceText->findText(ui->replaceText->currentText(),Qt::MatchExactly|Qt::MatchCaseSensitive) == -1) )
   {
      ui->replaceText->addItem(ui->replaceText->currentText());
      items = settings.value("ReplaceTextHistory").toStringList();
      if ( !items.contains(ui->replaceText->currentText()) )
      {
         if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
         {
            items.removeAt(0);
         }
         items.append(ui->replaceText->currentText());
      }
      settings.setValue("ReplaceTextHistory",QVariant(items));
   }

   settings.endGroup();

   emit replaceText(QString("SearchBar,"
                            +QString::number(ui->caseSensitive->isChecked())+","
                            +QString::number(ui->regex->isChecked())+","
                            +QString::number(ui->direction->isChecked())+","
                            +ui->searchText->currentText()),
                    ui->replaceText->currentText(),true);
}
