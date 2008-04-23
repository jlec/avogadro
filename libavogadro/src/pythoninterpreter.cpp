/**********************************************************************
  PythonInterpreter - Python Internal Interactive Interpreter

  Copyright (C) 2008 Donald Ephraim Curtis

  This file is part of the Avogadro molecular editor project.
  For more information, see <http://avogadro.sourceforge.net/>

  Avogadro is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Avogadro is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
 **********************************************************************/

#include "pythoninterpreter.h"

#include <avogadro/global.h>
#include <avogadro/boost.h>
#include <QString>
#include <QDebug>

namespace Avogadro {

  class PythonInterpreterPrivate {

    public:
      PythonInterpreterPrivate() : molecule(0)
    {
//      static bool initialized = false;

//      if(!initialized)
//      {
        std::cout << "initializing python interpreter\n";
        try {
          PyImport_AppendInittab( const_cast<char *>("Avogadro"), &initAvogadro );

          Py_Initialize();

//          object sys_module( handle<>(PyImport_ImportModule("sys")) );
//          object cStringIO_module( handle<>(PyImport_ImportModule("cStringIO")) );
//          main_module = handle<>(borrowed(PyImport_AddModule("__main__")));

          //        m_main_namespace = main_module.attr("__dict__");

          //        scope(avogadro_module).attr("widget") = ptr(widget);
        } catch( error_already_set ) {
          PyErr_Print();
        }
//        initialized = true;
//      }
    }

      dict local_namespace;
      Molecule *molecule;
  };

  PythonInterpreter::PythonInterpreter() : d(new PythonInterpreterPrivate)
  {
  }

  PythonInterpreter::~PythonInterpreter()
  {
    Py_Finalize();
  }

  void PythonInterpreter::setMolecule(Molecule *molecule)
  {
    d->molecule = molecule;

  }

  object PythonInterpreter::execWrapper(const QString &command, object main, object local)
  {
    try
    {
      return boost::python::exec(command.toAscii().constData(), main, local);
    }
    catch(error_already_set const &)
    {
      PyErr_Print();
    }

    return object();
  }

  object PythonInterpreter::evalWrapper(const QString &command, object main, object local)
  {
    try
    {
      return boost::python::eval(command.toAscii().constData(), main, local);
    }
    catch(error_already_set const &)
    {
      PyErr_Print();
    }

    return object();
  }

  QString PythonInterpreter::eval(const QString &string, object local)
  {
    object main_module = object(( handle<>(borrowed(PyImport_AddModule("__main__")))));
    object main_namespace = main_module.attr("__dict__");

    object result = evalWrapper(string, main_namespace, local);

    QString s;
    try
    {
      s = extract<const char *>(result);
    }
    catch(error_already_set const &)
    {
      return QString();
    }
//    return extract<>(result);
    return s;
  }

  void PythonInterpreter::addSearchPath(const QString &path)
  {
    object main_module = object(( handle<>(borrowed(PyImport_AddModule("__main__")))));
    object main_namespace = main_module.attr("__dict__");
    execWrapper("import sys", main_namespace, main_namespace);
    QString exp("sys.path.insert(0,\"");
    exp.append(path);
    exp.append("\")");
    execWrapper(exp, main_namespace, main_namespace); 
  }

  QString PythonInterpreter::exec(const QString &command, object local)
  {
    object main_module = object(( handle<>(borrowed(PyImport_AddModule("__main__")))));
    object main_namespace = main_module.attr("__dict__");

    object avogadro_module(handle<>(PyImport_ImportModule("Avogadro")) );
    if(d->molecule)
    {
      avogadro_module.attr("molecule") = ptr(d->molecule);
    }

    local["sys"] = object(handle<>(PyImport_ImportModule("sys")));
    local["cStringIO"] = object(handle<>(PyImport_ImportModule("cStringIO")));
    execWrapper("CIO = cStringIO.StringIO()", main_namespace, local);
    execWrapper("sys.stdout=CIO", main_namespace, local);
    execWrapper("sys.stderr=CIO", main_namespace, local);

//    handle<> ignored(( PyRun_String( command.toAscii().constData(), Py_file_input, main_namespace.ptr(), main_namespace.ptr() ) ));
    object ignored = execWrapper(command.toAscii().constData(), main_namespace, local);

    object result = evalWrapper("str(CIO.getvalue())", main_namespace, local);

    QString s;
    try
    {
      s = extract<const char *>(result);
    }
    catch(error_already_set const &)
    {
      return QString();
    }
//    return extract<>(result);
    return s;
  }

  QString PythonInterpreter::exec(const QString &command)
  {
    return exec(command, d->local_namespace);
  }
}
