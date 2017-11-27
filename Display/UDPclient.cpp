//UDP Client: receives packets from the server
#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <Python.h>
#include "client.h"

Client client;

// Ensures that wrapper functions and initializes global variables
bool setUpUDPclient() {
	return true;
}

// Closes UDP client 
bool closeUDPclient() {
	try {
		client.closeSocket();
		return true;
	} catch (int e) {
		return false;
	}
	
}

// Wrapper function for setUpUDPclient()
static PyObject* UDPclient_setUpUDPclient(PyObject *self, PyObject *args) {	
	return Py_BuildValue("b", setUpUDPclient());
}

// Wrapper function used to receive packets from UDP client
static PyObject* UDPclient_pollUDPclient(PyObject *self, PyObject *args) {
	client.receiveMessage();
	client.json_message.printJson();
	// return sensor values as a list of strings
	return Py_BuildValue("[s, s, s, s]", client.json_message.getSOCValue().c_str(), client.json_message.getOriValue().c_str(), client.json_message.getLumValue().c_str(), client.json_message.getGPSValue().c_str());
}

// Wrapper function for closeUDPclient()
static PyObject* UDPclient_closeUDPclient(PyObject *self, PyObject *args) {	
	return Py_BuildValue("b", closeUDPclient());
}

// Function list for Python wrapper
static PyMethodDef UDPclientMethods[] = {	// FUNCTION LIST
	{"setUpUDPclient", (PyCFunction) UDPclient_setUpUDPclient},
	{"pollUDPclient", (PyCFunction) UDPclient_pollUDPclient},
	{"closeUDPclient", (PyCFunction) UDPclient_closeUDPclient},
	{NULL, NULL, 0, NULL}
};

// Initializes Python library
PyMODINIT_FUNC initUDPclient() {
	Py_InitModule("UDPclient", UDPclientMethods);
}