/**

\file

\author Mattia Basaglia

\section License

Copyright (C) 2014  Mattia Basaglia

This is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "keysequence_listwidget_plugin.h"
#include "keysequence_listwidget.h"


KeySequence_ListWidget_Plugin::KeySequence_ListWidget_Plugin(QObject *parent) :
    QObject(parent), initialized(false)
{
}

void KeySequence_ListWidget_Plugin::initialize(QDesignerFormEditorInterface *)
{
    initialized = true;
}

bool KeySequence_ListWidget_Plugin::isInitialized() const
{
    return initialized;
}

QWidget *KeySequence_ListWidget_Plugin::createWidget(QWidget *parent)
{
    return new KeySequence_ListWidget(parent);
}

QString KeySequence_ListWidget_Plugin::name() const
{
    return "KeySequence_ListWidget";
}

QString KeySequence_ListWidget_Plugin::group() const
{
    return "Input Widgets";
}

QIcon KeySequence_ListWidget_Plugin::icon() const
{
    return QIcon::fromTheme("configure-shortcuts");
}

QString KeySequence_ListWidget_Plugin::toolTip() const
{
    return "KeySequence List Widget";
}

QString KeySequence_ListWidget_Plugin::whatsThis() const
{
    return "A widget to edit several Key Sequences";
}

bool KeySequence_ListWidget_Plugin::isContainer() const
{
    return false;
}

QString KeySequence_ListWidget_Plugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"KeySequence_ListWidget\" name=\"keysequence_listwidget\">\n"
           " </widget>\n"
           "</ui>\n";
}

QString KeySequence_ListWidget_Plugin::includeFile() const
{
    return "keysequence_listwidget.hpp";
}

