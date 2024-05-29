#include <Python.h>
#include "band_api.h"


static PyObject *
posix_error(void)
{
  return PyErr_SetFromErrno(PyExc_OSError);
}


static PyObject *
py_band_create(PyObject *self, PyObject *args)
{
  int instrument;
  int status;
  
  if (!PyArg_ParseTuple(args, "i", &instrument))
    return NULL;
  
  status = band_create(instrument);
  
  if (status < 0)
    return posix_error();
  
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
py_band_join(PyObject *self, PyObject *args)
{
  int instrument;
  int pid;
  int status;

  if (!PyArg_ParseTuple(args, "ii", &pid, &instrument))
    return NULL;

  status = band_join(pid, instrument);
  
  if (status < 0) {
    return posix_error();
  }
  
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
py_band_play(PyObject *self, PyObject *args)
{
  int instrument;
  unsigned char note;
  int status;

  if (!PyArg_ParseTuple(args, "ib", &instrument, &note))
    return NULL;
  
  status = band_play(instrument, note);
  
  if (status < 0) {
    return posix_error();
  }

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject * py_band_listen(PyObject *self, PyObject *args)
{
  unsigned char chord[4];
  int pid;
  int status;
  
  if (!PyArg_ParseTuple(args, "i", &pid))
    return NULL;
  
  status = band_listen(pid, &chord);
  if (status < 0) {
    return posix_error();
  }
  
  return Py_BuildValue("BBBB", chord[0], chord[1], chord[2], chord[3]);
}


static PyMethodDef msgMethods[] = {
  {"band_create",  py_band_create, METH_VARARGS,
   "Create a new band with us playing some instrument.\nExample:\nband_create(1) # we're the guitarist"},
  {"band_join",  py_band_join, METH_VARARGS,
   "Joins an existing band, using a member's PID.\nExample:\nband_join(123, 0) # try to join as singer"},
  {"band_play",  py_band_play, METH_VARARGS,
   "Play a note in our band.\nExample:\nband_play(23) # Note should be 0-255"},
  {"band_listen",  py_band_listen, METH_VARARGS,
   "Listen to a chord of PID's band. Returns a tuple of 4 notes\nExample:\nsing, guitar, bass, drums = band_listen(123)"},
  {NULL, NULL, 0, NULL} 
};


void
initpyBand(void)
{
  (void) Py_InitModule("pyBand", msgMethods);
}
