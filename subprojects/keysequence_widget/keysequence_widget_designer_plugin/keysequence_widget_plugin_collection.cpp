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
#include "keysequence_widget_plugin_collection.h"
#include "keysequence_widget_plugin.h"
#include "keysequence_listwidget_plugin.h"

KeySequence_Widget_Plugin_Collection::KeySequence_Widget_Plugin_Collection(QObject *parent) :
    QObject(parent)
{
    widgets.push_back(new KeySequence_Widget_Plugin(this));
    widgets.push_back(new KeySequence_ListWidget_Plugin(this));
}

QList<QDesignerCustomWidgetInterface *> KeySequence_Widget_Plugin_Collection::customWidgets() const
{
    return widgets;
}
