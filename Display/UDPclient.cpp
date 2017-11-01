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

bool setUpUDPclient() {
	return true;
}

bool closeUDPclient() {
	try {
		client.closeSocket();
		return true;
	} catch (int e) {
		return false;
	}
	
}

static PyObject* UDPclient_setUpUDPclient(PyObject *self, PyObject *args) {	
	return Py_BuildValue("b", setUpUDPclient());
}

static PyObject* UDPclient_pollUDPclient(PyObject *self, PyObject *args) {
	// receive packet
	// get timestamp
	// get sensor value
	client.receiveMessage();
	client.json_message.printJson();
	return Py_BuildValue("[s, s, s, s]", client.json_message.getSOCValue(), client.json_message.getOriValue(), client.json_message.getLumValue(), client.json_message.getGPSValue());
}

static PyObject* UDPclient_closeUDPclient(PyObject *self, PyObject *args) {	
	return Py_BuildValue("b", closeUDPclient());
}

static PyMethodDef UDPclientMethods[] = {	// FUNCTION LIST
	{"setUpUDPclient", (PyCFunction) UDPclient_setUpUDPclient},
	{"pollUDPclient", (PyCFunction) UDPclient_pollUDPclient},
	{"closeUDPclient", (PyCFunction) UDPclient_closeUDPclient},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initUDPclient() {	// INITIALIZE MODULE
	Py_InitModule("UDPclient", UDPclientMethods);
}