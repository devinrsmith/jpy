#include <jni.h>
#include <Python.h>

#include "jpy_module.h"
#include "jpy_jtype.h"
#include "jpy_jobj.h"
#include "jpy_conv.h"


PyObject* PyLib_getAttributeObject(JNIEnv* jenv, PyObject* pyValue, jstring jName);
PyObject* PyLib_callAndReturnObject(JNIEnv *jenv, PyObject* pyValue, jboolean isMethodCall, jstring jName, jint argCount, jobjectArray jArgs, jobjectArray jParamClasses);


// Note code in this file is formatted according to the header generated by javah. This makes it easier
// to follow up changes in the header.

/*
 * Class:     org_jpy_python_PyLib
 * Method:    isInitialized
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_jpy_python_PyLib_isInterpreterInitialized
  (JNIEnv* jenv, jclass jLibClass)
{
    int retCode;
    retCode = Py_IsInitialized();
    printf("Java_org_jpy_python_PyLib_isInterpreterInitialized: retCode=%d\n", retCode);
    return retCode != 0;
}

/*
 * Class:     org_jpy_python_PyLib
 * Method:    initialize
 * Signature: ([Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL Java_org_jpy_python_PyLib_initializeInterpreter
  (JNIEnv* jenv, jclass jLibClass, jobjectArray jOptions, jboolean debug)
{
    printf("Java_org_jpy_python_PyLib_initializeInterpreter: debug=%d\n", debug);
    if (!Py_IsInitialized()) {
        //Py_SetProgramName("java");
        Py_Initialize();
    }
    JPy_SetDebug(debug);
    if (JPy_Module == NULL) {
        PyObject* pyModule;

        pyModule = PyImport_ImportModule("jpy");
        if (pyModule == NULL) {
            printf("Java_org_jpy_python_PyLib_initializeInterpreter: pyModule == NULL :-(\n");
            // todo: throw exception from last python error
            return JNI_FALSE;
        }
    }
    return JNI_TRUE;
}
  

/*
 * Class:     org_jpy_python_PyLib
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_jpy_python_PyLib_destroyInterpreter
  (JNIEnv* jenv, jclass jLibClass)
{
    printf("Java_org_jpy_python_PyLib_destroyInterpreter called\n");
    if (Py_IsInitialized()) {
        Py_Finalize();
    }
}

/*
 * Class:     org_jpy_python_PyLib
 * Method:    execScript
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jint JNICALL Java_org_jpy_python_PyLib_execScript
  (JNIEnv* jenv, jclass jLibClass, jstring jScript)
{
    const char* scriptChars;
    int retCode;

    scriptChars = (*jenv)->GetStringUTFChars(jenv, jScript, NULL);
    printf("Java_org_jpy_python_PyLib_execScript: script='%s'\n", scriptChars);
    retCode = PyRun_SimpleString(scriptChars);
    (*jenv)->ReleaseStringUTFChars(jenv, jScript, scriptChars);
    return retCode;
}


/*
 * Class:     org_jpy_python_PyLib
 * Method:    decref
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_jpy_python_PyLib_decref
  (JNIEnv* jenv, jclass jLibClass, jlong objId)
{
    printf("Java_org_jpy_python_PyLib_decref: objId=%p\n", (PyObject*) objId);
    Py_DECREF((PyObject*) objId);
}


/*
 * Class:     org_jpy_python_PyLib
 * Method:    getIntValue
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_jpy_python_PyLib_getIntValue
  (JNIEnv* jenv, jclass jLibClass, jlong objId)
{
    PyObject* pyObject;
    pyObject = (PyObject*) objId;
    return (jint) PyLong_AsLong(pyObject);
}

/*
 * Class:     org_jpy_python_PyLib
 * Method:    getDoubleValue
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_org_jpy_python_PyLib_getDoubleValue
  (JNIEnv* jenv, jclass jLibClass, jlong objId)
{
    PyObject* pyObject;
    pyObject = (PyObject*) objId;
    return (jdouble) PyFloat_AsDouble(pyObject);
}

/*
 * Class:     org_jpy_python_PyLib
 * Method:    getStringValue
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_jpy_python_PyLib_getStringValue
  (JNIEnv* jenv, jclass jLibClass, jlong objId)
{
    PyObject* pyObject;
    jstring jString;

    pyObject = (PyObject*) objId;

    if (JPy_AsJString(jenv, pyObject, &jString) < 0) {
        // todo: create exception
        printf("Java_org_jpy_python_PyLib_getStringValue: error: failed to convert Python object to Java String\n");
        return NULL;
    }

    return jString;
}

/*
 * Class:     org_jpy_python_PyLib
 * Method:    getObjectValue
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_jpy_python_PyLib_getObjectValue
  (JNIEnv* jenv, jclass jLibClass, jlong objId)
{
    PyObject* pyObject;
    jobject jObject;

    pyObject = (PyObject*) objId;

    if (JObj_Check(pyObject)) {
        jObject = ((JPy_JObj*) pyObject)->objectRef;
    } else {
        if (JPy_AsJObject(jenv, pyObject, &jObject) < 0) {
            // todo: create exception
            printf("Java_org_jpy_python_PyLib_getObjectValue: error: failed to convert Python object to Java Object\n");
            return NULL;
        }
    }

    return jObject;
}


/*
 * Class:     org_jpy_python_PyLib
 * Method:    getModule
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_jpy_python_PyLib_importModule
  (JNIEnv* jenv, jclass jLibClass, jstring jName)
{
    PyObject* pyName;
    PyObject* pyModule;
    const char* nameChars;

    nameChars = (*jenv)->GetStringUTFChars(jenv, jName, NULL);
    printf("Java_org_jpy_python_PyLib_importModule: name='%s'\n", nameChars);
    /* Note: pyName is a new reference */
    pyName = PyUnicode_FromString(nameChars);
    /* Note: pyModule is a new reference */
    pyModule = PyImport_Import(pyName);
    if (pyModule == NULL) {
        // todo: create exception
        printf("Java_org_jpy_python_PyLib_importModule: error: module not found '%s'\n", nameChars);
        goto error;
    }
error:
    Py_DECREF(pyName);
    (*jenv)->ReleaseStringUTFChars(jenv, jName, nameChars);
    return (jlong) pyModule;
}




/*
 * Class:     org_jpy_python_PyLib
 * Method:    getAttributeValue
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_jpy_python_PyLib_getAttributeObject
  (JNIEnv* jenv, jclass jLibClass, jlong objId, jstring jName)
{
    PyObject* pyObject;
    PyObject* pyValue;

    pyObject = (PyObject*) objId;

    pyValue = PyLib_getAttributeObject(jenv, pyObject, jName);

    return (jlong) pyValue;
}

/*
 * Class:     org_jpy_python_PyLib
 * Method:    getAttributeValue
 * Signature: (JLjava/lang/String;Ljava/lang/Class;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_jpy_python_PyLib_getAttributeValue
  (JNIEnv* jenv, jclass jLibClass, jlong objId, jstring jName, jclass jValueClass)
{
    PyObject* pyObject;
    PyObject* pyValue;
    jobject jReturnValue;

    pyObject = (PyObject*) objId;

    pyValue = PyLib_getAttributeObject(jenv, pyObject, jName);
    if (pyValue == NULL) {
        return NULL;
    }

    if (JPy_AsJObjectWithClass(jenv, pyValue, &jReturnValue, jValueClass) < 0) {
        // todo: create exception
        printf("Java_org_jpy_python_PyLib_getAttributeValue: error: failed to convert attribute value\n");
        return NULL;
    }

    return jReturnValue;
}


/*
 * Class:     org_jpy_python_PyLib
 * Method:    setAttributeValue
 * Signature: (JLjava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_org_jpy_python_PyLib_setAttributeValue
  (JNIEnv* jenv, jclass jLibClass, jlong objId, jstring jName, jobject jValue, jclass jValueClass)
{
    PyObject* pyObject;
    const char* nameChars;
    PyObject* pyValue;
    JPy_JType* valueType;

    pyObject = (PyObject*) objId;

    nameChars = (*jenv)->GetStringUTFChars(jenv, jName, NULL);
    printf("Java_org_jpy_python_PyLib_setAttributeValue: objId=%p, name='%s', jValue=%p, jValueClass=%p\n", pyObject, nameChars, jValue, jValueClass);

    if (jValueClass != NULL) {
        valueType = JType_GetType(jenv, jValueClass, JNI_FALSE);
    } else {
        valueType = NULL;
    }

    if (valueType != NULL) {
        pyValue = JPy_FromJObjectWithType(jenv, jValue, valueType);
    } else {
        pyValue = JPy_FromJObject(jenv, jValue);
    }

    if (pyValue == NULL) {
        // todo: create exception
        printf("Java_org_jpy_python_PyLib_setAttributeValue: error: attribute '%s': Java object not convertible\n", nameChars);
        goto error;
    }

    if (PyObject_SetAttrString(pyObject, nameChars, pyValue) < 0) {
        // todo: create exception
        printf("Java_org_jpy_python_PyLib_setAttributeValue: error: PyObject_SetAttrString failed on attribute '%s'\n", nameChars);
        goto error;
    }

error:
    (*jenv)->ReleaseStringUTFChars(jenv, jName, nameChars);
    // todo: on error, throw Java exception
}


/*
 * Class:     org_jpy_python_PyLib
 * Method:    call
 * Signature: (JZLjava/lang/String;I[Ljava/lang/Object;[Ljava/lang/Class;Ljava/lang/Class;)Ljava/lang/Object;
 */
JNIEXPORT jlong JNICALL Java_org_jpy_python_PyLib_callAndReturnObject
  (JNIEnv *jenv, jclass jLibClass, jlong objId, jboolean isMethodCall, jstring jName, jint argCount, jobjectArray jArgs, jobjectArray jParamClasses)
{
    PyObject* pyObject;
    PyObject* pyReturnValue;

    pyObject = (PyObject*) objId;

    pyReturnValue = PyLib_callAndReturnObject(jenv, pyObject, isMethodCall, jName, argCount, jArgs, jParamClasses);

    return (jlong) pyReturnValue;
}


/*
 * Class:     org_jpy_python_PyLib
 * Method:    callAndReturnValue
 * Signature: (JZLjava/lang/String;I[Ljava/lang/Object;[Ljava/lang/Class;Ljava/lang/Class;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_jpy_python_PyLib_callAndReturnValue
  (JNIEnv *jenv, jclass jLibClass, jlong objId, jboolean isMethodCall, jstring jName, jint argCount, jobjectArray jArgs, jobjectArray jParamClasses, jclass jReturnClass)
{
    PyObject* pyObject;
    PyObject* pyReturnValue;
    jobject jReturnValue;

    pyObject = (PyObject*) objId;

    pyReturnValue = PyLib_callAndReturnObject(jenv, pyObject, isMethodCall, jName, argCount, jArgs, jParamClasses);
    if (pyReturnValue == NULL) {
        return NULL;
    }

    if (JPy_AsJObjectWithClass(jenv, pyReturnValue, &jReturnValue, jReturnClass) < 0) {
        // todo: create exception
        printf("Java_org_jpy_python_PyLib_callAndReturnValue: error: failed to convert attribute value\n");
        return NULL;
    }

    return jReturnValue;
}




////////////////////////////////////////////////////////////////////////////////////
// Helpers that also throw Java exceptions


PyObject* PyLib_getAttributeObject(JNIEnv* jenv, PyObject* pyObject, jstring jName)
{
    PyObject* pyValue;
    const char* nameChars;

    nameChars = (*jenv)->GetStringUTFChars(jenv, jName, NULL);
    printf("PyLib_getAttributeObject: objId=%p, name='%s'\n", pyObject, nameChars);
    /* Note: pyValue is a new reference */
    pyValue = PyObject_GetAttrString(pyObject, nameChars);
    if (pyValue == NULL) {
        // todo: create exception
        printf("PyLib_getAttributeObject: error: attribute not found '%s'\n", nameChars);
        goto error;
    }
error:
    (*jenv)->ReleaseStringUTFChars(jenv, jName, nameChars);
    return pyValue;
}


PyObject* PyLib_callAndReturnObject(JNIEnv *jenv, PyObject* pyObject, jboolean isMethodCall, jstring jName, jint argCount, jobjectArray jArgs, jobjectArray jParamClasses)
{
    PyObject* pyCallable;
    PyObject* pyArgs;
    PyObject* pyArg;
    PyObject* pyReturnValue;
    const char* nameChars;
    jint i;
    jobject jArg;
    jclass jParamClass;
    JPy_JType* paramType;

    pyReturnValue = NULL;

    nameChars = (*jenv)->GetStringUTFChars(jenv, jName, NULL);

    printf("PyLib_callAndReturnObject: objId=%p, isMethodCall=%d, name='%s', argCount=%d\n", pyObject, isMethodCall, nameChars, argCount);

    pyArgs = NULL;

    // Note: pyCallable is a new reference
    pyCallable = PyObject_GetAttrString(pyObject, nameChars);
    if (pyCallable == NULL) {
        // todo: create exception
        printf("PyLib_callAndReturnObject: error: function or method not found: '%s'\n", nameChars);
        goto error;
    }

    if (!PyCallable_Check(pyCallable)) {
        // todo: create exception
        printf("PyLib_callAndReturnObject: error: object is not callable: '%s'\n", nameChars);
        goto error;
    }

    pyArgs = PyTuple_New(argCount);
    for (i = 0; i < argCount; i++) {
        jArg = (*jenv)->GetObjectArrayElement(jenv, jArgs, i);

        if (jParamClasses != NULL) {
            jParamClass = (*jenv)->GetObjectArrayElement(jenv, jParamClasses, i);
        } else {
            jParamClass = NULL;
        }

        if (jParamClass != NULL) {
            paramType = JType_GetType(jenv, jParamClass, JNI_FALSE);
            if (paramType == NULL) {
                // todo: create exception
                printf("PyLib_callAndReturnObject: error: callable '%s': argument %d: failed to retrieve type\n", nameChars, i);
                goto error;
            }
            pyArg = JPy_FromJObjectWithType(jenv, jArg, paramType);
        } else {
            pyArg = JPy_FromJObject(jenv, jArg);
        }

        if (pyArg == NULL) {
            // todo: create exception
            printf("PyLib_callAndReturnObject: error: callable '%s': argument %d: failed to convert Java into Python object\n", nameChars, i);
            goto error;
        }

        // pyArg reference stolen here
        PyTuple_SetItem(pyArgs, i, pyArg);
    }

    // Check why: for some reason, we don't need the following code to invoke object methods.
    /*
    if (isMethodCall) {
        PyObject* pyMethod;

        pyMethod = PyMethod_New(pyCallable, pyObject);
        if (pyMethod == NULL) {
            // todo: create out of memory exception
            printf("PyLib_callAndReturnObject: error: callable '%s': no memory\n", nameChars);
            goto error;
        }
        Py_DECREF(pyCallable);
        pyCallable = pyMethod;
    }
    */

    pyReturnValue = PyObject_CallObject(pyCallable, argCount > 0 ? pyArgs : NULL);
    if (pyReturnValue == NULL) {
        // todo: create exception
        printf("PyLib_callAndReturnObject: error: callable '%s': call returned NULL\n", nameChars);
        goto error;
    }

    Py_INCREF(pyReturnValue);

error:
    (*jenv)->ReleaseStringUTFChars(jenv, jName, nameChars);
    Py_DECREF(pyCallable);
    Py_DECREF(pyArgs);

    // todo: on error, throw Java exception

    return pyReturnValue;
}
