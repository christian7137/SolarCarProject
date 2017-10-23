#include <iostream>
#include <Python.h>
#include <math.h>

bool setUpUDPclient() {
	try {
		std::cout << "I GOT SET UP YEEEE" << std::endl;
		return true;
	} catch {
		return false;
	}
}

static PyObject* UDPclient_setUpUDPclient(PyObject *self, PyObject *args) {	
	return Py_BuildValue("b", setUpUDPclient());
}


static PyMethodDef UDPclientMethods[] = {	// FUNCTION LIST
	{"setUpUDPclient", (PyCFunction) UDPclient_setUpUDPclient},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initUDPclient() {	// INITIALIZE MODULE
	Py_InitModule("UDPclient", UDPclientMethods);
}