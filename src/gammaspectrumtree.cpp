/***************************************************************************
 *   Copyright (C) 2007, 2009 by Marcelo Francis Maduar                    *
 *   maduar@gmail.com                                                      *
 *                                                                         *
 *   This file is part of OpenGamma.                                       *
 *                                                                         *
 *   OpenGamma is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   OpenGamma is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with OpenGamma. If not, see <http://www.gnu.org/licenses/>.     *
 ***************************************************************************/

#include <QtGui>
#include <QMessageBox>

#include "gammaspectrumtree.h"

GammaSpectrumTree::GammaSpectrumTree(QIODevice *device, QTreeWidget *tree)
{
    treeWidget = tree;
    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if (!doc.setContent(device, true, &errorStr, &errorLine,
                        &errorColumn)) {
        QMessageBox::warning(0, QObject::tr("DOM Parser"),
                             QObject::tr("Parse error at line %1, "
                                         "column %2:\n%3")
                             .arg(errorLine)
                             .arg(errorColumn)
                             .arg(errorStr));
        return;
    }
    QDomElement root = doc.documentElement();
    if (root.tagName() != "bookindex")
        return;
    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        if (node.toElement().tagName() == "entry")
            parseEntry(node.toElement(), 0);
        node = node.nextSibling();
    }
}

void GammaSpectrumTree::parseEntry(const QDomElement &element,
                             QTreeWidgetItem *parent)
{
     QTreeWidgetItem *item;
     if (parent) {
         item = new QTreeWidgetItem(parent);
     } else {
         item = new QTreeWidgetItem(treeWidget);
     }
     item->setText(0, element.attribute("term"));
     QDomNode node = element.firstChild();
     while (!node.isNull()) {
         if (node.toElement().tagName() == "entry") {
              parseEntry(node.toElement(), item);
         } else if (node.toElement().tagName() == "page") {
              QDomNode childNode = node.firstChild();
              while (!childNode.isNull()) {
                  if (childNode.nodeType() == QDomNode::TextNode) {
                      QString page = childNode.toText().data();
                      QString allPages = item->text(1);
                      if (!allPages.isEmpty())
                          allPages += ", ";
                      allPages += page;
                      item->setText(1, allPages);
                      break;
                  }
                  childNode = childNode.nextSibling();
              }
         }
         node = node.nextSibling();
     }
}


bool GammaSpectrumTree::buildspectrumtree()
{
    /*
    QDomElement root = domDocument.createElement("opengammaspectrum");
    root.setAttribute("version","1.0");
    domDocument.appendChild(root);

    QDomElement tag1 = domDocument.createElement("Greeting");
    tag1.setAttribute("Hello", "Alow");
    root.appendChild(tag1);

    QDomElement tag2 = domDocument.createElement("region");
    tag2.setAttribute("centroid1", 127.1347);
    root.appendChild(tag2);

    QDomElement tag3 = domDocument.createElement("region");
    tag3.setAttribute("centroid2", 127.8899);
    root.appendChild(tag3);

    QString xml = domDocument.toString();

    // preenche a arvore:
    // filles the tree:

    QDomElement child = root.firstChildElement("region");
    while (!child.isNull()) {
        parseFolderElement(child);
        child = child.nextSiblingElement("region");
    }
    */
    return true;
}
