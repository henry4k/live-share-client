/**

@author Mattia Basaglia

@section License

    Copyright (C) 2013-2014 Mattia Basaglia

    This is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef KEYSEQUENCE_WIDGET_PLUGIN_COLLECTION_HPP
#define KEYSEQUENCE_WIDGET_PLUGIN_COLLECTION_HPP

#include <QDesignerCustomWidgetCollectionInterface>

class KeySequence_Widget_Plugin_Collection : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "mattia.basaglia.KeySequence_Widget_Plugin")
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    explicit KeySequence_Widget_Plugin_Collection(QObject *parent = 0);

    QList<QDesignerCustomWidgetInterface*> customWidgets() const;

   private:
       QList<QDesignerCustomWidgetInterface*> widgets;
    
};

#endif // KEYSEQUENCE_WIDGET_PLUGIN_COLLECTION_HPP
