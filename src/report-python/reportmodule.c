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
#include "internal_libreport.h"


PyObject *ReportError;

struct module_state {
        PyObject *error;
};

static PyMethodDef module_methods[] = {
    /* method_name, func, flags, doc_string */
    /* for include/report/dump_dir.h */
    { "dd_opendir"                , p_dd_opendir              , METH_VARARGS },
    { "dd_create"                 , p_dd_create               , METH_VARARGS },
    { "delete_dump_dir"           , p_delete_dump_dir         , METH_VARARGS },
    /* for include/report/report.h */
    { "report_problem_in_dir"     , p_report_problem_in_dir   , METH_VARARGS },
    { "report_problem_in_memory"  , p_report_problem_in_memory, METH_VARARGS },
    { "report_problem"            , p_report_problem          , METH_VARARGS },
    { NULL }
};
#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_pyreport",  
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
PyInit__pyreport(void)

#else
#define INITERROR return


#ifndef PyMODINIT_FUNC /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
init_pyreport(void)
#endif
{
    if (PyType_Ready(&p_problem_data_type) < 0)
    {
        printf("PyType_Ready(&p_problem_data_type) < 0\n");
        INITERROR;
    }
    if (PyType_Ready(&p_dump_dir_type) < 0)
    {
        printf("PyType_Ready(&p_dump_dir_type) < 0\n");
        INITERROR;
    }
    if (PyType_Ready(&p_run_event_state_type) < 0)
    {
        printf("PyType_Ready(&p_run_event_state_type) < 0\n");
        INITERROR;
    }
#if PY_MAJOR_VERSION >= 3
    PyObject *m = PyModule_Create(&moduledef);
#else
    PyObject *m = Py_InitModule("_pyreport", module_methods);
    //m = Py_InitModule3("_pyreport", module_methods, "Python wrapper for libreport");
#endif
    if (m == NULL)
        INITERROR;

    /* init the exception object */
    ReportError = PyErr_NewException((char*) "_pyreport.error", NULL, NULL);
    Py_INCREF(ReportError);
    PyModule_AddObject(m, "error", ReportError);

    /* init type objects and constants */
    /* for include/report/problem_data.h */
    Py_INCREF(&p_problem_data_type);
    PyModule_AddObject(m, "problem_data", (PyObject *)&p_problem_data_type);
    PyModule_AddObject(m, "CD_FLAG_BIN"          , Py_BuildValue("i", CD_FLAG_BIN          ));
    PyModule_AddObject(m, "CD_FLAG_TXT"          , Py_BuildValue("i", CD_FLAG_TXT          ));
    PyModule_AddObject(m, "CD_FLAG_ISEDITABLE"   , Py_BuildValue("i", CD_FLAG_ISEDITABLE   ));
    PyModule_AddObject(m, "CD_FLAG_ISNOTEDITABLE", Py_BuildValue("i", CD_FLAG_ISNOTEDITABLE));
    /* for include/report/dump_dir.h */
    Py_INCREF(&p_dump_dir_type);
    PyModule_AddObject(m, "dump_dir", (PyObject *)&p_dump_dir_type);
    PyModule_AddObject(m, "DD_FAIL_QUIETLY_ENOENT"             , Py_BuildValue("i", DD_FAIL_QUIETLY_ENOENT             ));
    PyModule_AddObject(m, "DD_FAIL_QUIETLY_EACCES"             , Py_BuildValue("i", DD_FAIL_QUIETLY_EACCES             ));
    PyModule_AddObject(m, "DD_OPEN_READONLY"                   , Py_BuildValue("i", DD_OPEN_READONLY                   ));
    PyModule_AddObject(m, "DD_LOAD_TEXT_RETURN_NULL_ON_FAILURE", Py_BuildValue("i", DD_LOAD_TEXT_RETURN_NULL_ON_FAILURE));
    /* for include/report/run_event.h */
    Py_INCREF(&p_run_event_state_type);
    PyModule_AddObject(m, "run_event_state", (PyObject *)&p_run_event_state_type);
    /* for include/report/report.h */
    PyModule_AddObject(m, "LIBREPORT_NOWAIT"     , Py_BuildValue("i", LIBREPORT_NOWAIT     ));
    PyModule_AddObject(m, "LIBREPORT_WAIT"       , Py_BuildValue("i", LIBREPORT_WAIT       ));
    PyModule_AddObject(m, "LIBREPORT_GETPID"     , Py_BuildValue("i", LIBREPORT_GETPID     ));
    PyModule_AddObject(m, "LIBREPORT_RELOAD_DATA", Py_BuildValue("i", LIBREPORT_RELOAD_DATA));
    PyModule_AddObject(m, "LIBREPORT_DEL_DIR"    , Py_BuildValue("i", LIBREPORT_DEL_DIR    ));
    PyModule_AddObject(m, "LIBREPORT_RUN_CLI"    , Py_BuildValue("i", LIBREPORT_RUN_CLI    ));
    PyModule_AddObject(m, "LIBREPORT_RUN_NEWT"   , Py_BuildValue("i", LIBREPORT_RUN_NEWT  ));
    PyModule_AddObject(m, "EXIT_CANCEL_BY_USER", Py_BuildValue("i", EXIT_CANCEL_BY_USER));
#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
