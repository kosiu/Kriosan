/***************************************************************************
 *   Copyright (C) 2006-2008 by Tomasz Ziobrowski                          *
 *   http://www.3electrons.com                                             *
 *   e-mail: t.ziobrowski@3electrons.com                                   *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QPaintEvent> 
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSvgItem>  
#include <QScriptEngine> 
#include <QSvgRenderer> 
#include <QCoreApplication> 
#include <QFile>
#include "scriptwidget.h" 
#include "item.h" 

Item::Item(QGraphicsItem * i,QObject * parent):QObject(parent), item(i) { ; }
QRectF Item::boundingRect() const         { return item->boundingRect(); } 
void Item::scale(qreal sx, qreal sy)      { item->scale(sx,sy); } 
void Item::scale(qreal x)                 { item->scale(x,x);  }
void Item::rotate(qreal angle)            { item->rotate(angle);}
void Item::translate(qreal dx, qreal dy)  { item->translate(dx,dy);}
void Item::setPos(qreal x,qreal y)        { item->setPos(x,y);  }
void Item::moveBy(qreal dx, qreal dy)     { item->moveBy(dx,dy);}
void Item::resetTransform()               { item->resetTransform();}
void Item::setVisible(bool v)             { item->setVisible(v);}
void Item::hide()                         { item->hide();       }
void Item::show()                         { item->show();       }

Item::~Item()
{
   qDebug("Deleting item %s",qPrintable(objectName()));
   item->scene()->removeItem(item);
   delete item; 
  
}

/*---------------------------------------------------------------------------------------------------------------
                                                      P U B L I C
---------------------------------------------------------------------------------------------------------------*/

QScriptEngine * ScriptWidget::m_engine = NULL; 

ScriptWidget::ScriptWidget(QWidget * parent):QGraphicsView(parent)
{
  init();
}

ScriptWidget::ScriptWidget(const QString & objectName, QWidget * parent):QGraphicsView(parent)
{
  init();
  setObjectName(objectName); 
}

ScriptWidget::~ScriptWidget()
{
  clearSvgItems();
} 


QString ScriptWidget::script() const
{
  return m_script;
}


QString ScriptWidget::svgFile() const
{
  return m_svgFile;
}

QVariant ScriptWidget::getPValue() const
{
  return m_values[m_currentProperty];
}

QString ScriptWidget::getPName() const
{
   return m_currentProperty;
}

QStringList ScriptWidget::PNames() const 
{
  return m_values.keys();
}

/*---------------------------------------------------------------------------------------------------------------
                                                 P U B L I C    S L O T S 
---------------------------------------------------------------------------------------------------------------*/
void ScriptWidget::setScript(const QString & s)
{
  m_script = s;
  qDebug("ScriptWidget::setScript() %s",qPrintable(m_script)); 
  try 
  {
   QFile file(m_script); 
   if (file.open( QIODevice::ReadOnly ))
   {
      QString src = QString(file.readAll());
      QScriptValue widget  = m_engine->newQObject(this);
      m_engine->globalObject().setProperty(objectName(),widget);
      m_engine->evaluate(src);
      QScriptValue v = m_engine->uncaughtException(); 

      if (v.isValid()) 
      qDebug("Error %s",qPrintable(v.toString()));
   }
  }
  catch (std::exception & e) 
  {
     qDebug("Wyjatek %s",e.what()); 
  }
}// setScript 

void ScriptWidget::setSvgFile(const QString & file)
{
  qDebug("Setting property %s",qPrintable(file));
  m_svgFile = file;
  loadSvgFile(file);
}

void ScriptWidget::resetItems()
{
  m_items.clear();
}

void ScriptWidget::addItem(const QString & item) 
{
  m_items.insert(item);
}

void ScriptWidget::loadSvgFile(const QString & file) 
{
  
  if (!file.isEmpty())
  {
      clearSvgItems();
      qDebug("Loading file %s",qPrintable(file)); 
      m_renderer = new QSvgRenderer(file);
      int zBuffer = 0; 
      foreach (QString itemName, m_items)
      {
        QGraphicsSvgItem * g = new QGraphicsSvgItem();
        Item * item = new Item(g,this);  
        item->setObjectName(itemName); 
        g->setElementId(itemName); 
        g->setSharedRenderer(m_renderer); 
        g->setZValue(zBuffer++); 
        m_scene->addItem(g); 
        qDebug("item %s",qPrintable(itemName));
      }
      update(); 
  }
}// loadSvgFile 


void ScriptWidget::setPName(const QString & name)
{
  m_currentProperty = name; 
}

void ScriptWidget::setPValue(int p) 
{
  setPValue(QVariant(p));
}

void ScriptWidget::setPValue(double p)
{
 setPValue(QVariant(p));
}

void ScriptWidget::setPValue(const QString & p)
{ 
  setPValue(QVariant(p));
}

void ScriptWidget::setPValue(const QVariant & p)
{
  if (m_values[m_currentProperty] != p) 
  {
    m_values[m_currentProperty] = p; 
    emit evaluate(); 
  }
}

/*---------------------------------------------------------------------------------------------------------------
                                                    P R O T E C T E D 
---------------------------------------------------------------------------------------------------------------*/

void ScriptWidget::init()
{
  if (!m_engine) 
    m_engine = new QScriptEngine(QCoreApplication::instance());

  m_scene = new QGraphicsScene(this); 
  m_renderer = NULL;
  setScene(m_scene);

  qDebug("ScriptWidget::init()");

  /* Setting default settings */
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
  setInteractive(false);
  setFrameShape(QFrame::NoFrame);

  QPalette p(palette());
  p.setBrush( QPalette::Base, Qt::NoBrush );
  setPalette(p); 
}

void ScriptWidget::clearSvgItems()
{
  foreach(QGraphicsItem * item, m_scene->items()) 
  {
    QGraphicsSvgItem * i = dynamic_cast<QGraphicsSvgItem *>(item); 
    if (i)
     delete findChild<Item*>( i->elementId());
  }

  if (m_renderer) 
   delete m_renderer; 
}


void ScriptWidget::resizeEvent ( QResizeEvent * )
{
  if (sceneRect().width() > 0 && sceneRect().height() > 0 ) 
  fitInView(sceneRect()); 
}


