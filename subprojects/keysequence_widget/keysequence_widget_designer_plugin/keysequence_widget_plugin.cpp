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
#include "keysequence_widget_plugin.h"
#include "keysequence_widget.h"

KeySequence_Widget_Plugin::KeySequence_Widget_Plugin(QObject *parent) :
    QObject(parent), initialized(false)
{
}

void KeySequence_Widget_Plugin::initialize(QDesignerFormEditorInterface *)
{
    initialized = true;
}

bool KeySequence_Widget_Plugin::isInitialized() const
{
    return initialized;
}

QWidget *KeySequence_Widget_Plugin::createWidget(QWidget *parent)
{
    return new KeySequence_Widget(parent);
}

QString KeySequence_Widget_Plugin::name() const
{
    return "KeySequence_Widget";
}

QString KeySequence_Widget_Plugin::group() const
{
    return "Input Widgets";
}

QIcon KeySequence_Widget_Plugin::icon() const
{
    return QIcon::fromTheme("configure-shortcuts");
}

QString KeySequence_Widget_Plugin::toolTip() const
{
    return "KeySequence Widget";
}

QString KeySequence_Widget_Plugin::whatsThis() const
{
    return "A widget to edit Key Sequences";
}

bool KeySequence_Widget_Plugin::isContainer() const
{
    return false;
}

QString KeySequence_Widget_Plugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"KeySequence_Widget\" name=\"keysequence_widget\">\n"
           " </widget>\n"
           "</ui>\n";
}

QString KeySequence_Widget_Plugin::includeFile() const
{
    return "keysequence_widget.hpp";
}


