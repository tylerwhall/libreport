/*
    Copyright (C) 2010  Abrt team.
    Copyright (C) 2010  RedHat inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <Python.h>

#include "common.h"

struct module_state {
        PyObject *error;
};

static PyMethodDef module_methods[] = {
    /* method_name, func, flags, doc_string */
    /* for include/client.h */
    { "alert"                     , p_alert                   , METH_VARARGS },
    { "ask"                       , p_ask                     , METH_VARARGS },
    { "ask_password"              , p_ask_password            , METH_VARARGS },
    { "ask_yes_no"                , p_ask_yes_no              , METH_VARARGS },
    { "ask_yes_no_yesforever"     , p_ask_yes_no_yesforever   , METH_VARARGS },
    { NULL }
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_reportclient",  
        NULL,
        sizeof(struct module_state),
        module_methods,
        NULL,
        NULL,
        NULL,
        NULL
};

#define INITERROR return NULL

PyObject *
PyInit__reportclient(void)

#else
#define INITERROR return

#ifndef PyMODINIT_FUNC /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
init_reportclient(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *m = PyModule_Create(&moduledef);
#else
    PyObject *m = Py_InitModule("_reportclient", module_methods);
#endif
    if (m == NULL)
        INITERROR;

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
