// This code is GPLed by Yan Shoshitaishvili

#include <Python.h>
#include <structmember.h>
#include <libvex.h>

#include "pyvex_enums.h"
#include "pyvex_types.h"
#include "pyvex_macros.h"
#include "pyvex_logging.h"

#ifdef PYVEX_STATIC
	#include "pyvex_static.h"
	#include "pyvex_deepcopy.h"
#endif


///////////////////////
// IRStmt base class //
///////////////////////

PYVEX_NEW(IRStmt)
PYVEX_DEALLOC(IRStmt)
PYVEX_METH_STANDARD(IRStmt)

static int
pyIRStmt_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);
	PyErr_SetString(VexException, "Base IRStmt creation not supported.");
	return -1;
}

PYVEX_ACCESSOR_WRAPPED(IRStmt, IRStmt, self->wrapped, wrapped, IRStmt)
PYVEX_ACCESSOR_ENUM(IRStmt, IRStmt, self->wrapped->tag, tag, IRStmtTag)

static PyGetSetDef pyIRStmt_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmt, wrapped),
	PYVEX_ACCESSOR_DEF(IRStmt, tag),
	{NULL}
};

static PyObject *pyIRStmt_flat(pyIRStmt* self)
{
	if (isFlatIRStmt(self->wrapped)) { Py_RETURN_TRUE; }
	Py_RETURN_FALSE;
}

static PyMethodDef pyIRStmt_methods[] =
{
	PYVEX_METHDEF_STANDARD(IRStmt),
	{"flat", (PyCFunction)pyIRStmt_flat, METH_NOARGS, "Returns true if IRStmt is flat, false otherwise."},
	{NULL}
};

static PyMemberDef pyIRStmt_members[] = { {NULL} };
PYVEX_TYPEOBJECT(IRStmt);

// wrap functionality
PyObject *wrap_IRStmt(IRStmt *i)
{
	PyTypeObject *t = NULL;

	switch (i->tag)
	{
		PYVEX_WRAPCASE(IRStmt, Ist_, NoOp)
		PYVEX_WRAPCASE(IRStmt, Ist_, IMark)
		PYVEX_WRAPCASE(IRStmt, Ist_, AbiHint)
		PYVEX_WRAPCASE(IRStmt, Ist_, Put)
		PYVEX_WRAPCASE(IRStmt, Ist_, PutI)
		PYVEX_WRAPCASE(IRStmt, Ist_, WrTmp)
		PYVEX_WRAPCASE(IRStmt, Ist_, Store)
		PYVEX_WRAPCASE(IRStmt, Ist_, CAS)
		PYVEX_WRAPCASE(IRStmt, Ist_, LLSC)
		PYVEX_WRAPCASE(IRStmt, Ist_, Dirty)
		PYVEX_WRAPCASE(IRStmt, Ist_, MBE)
		PYVEX_WRAPCASE(IRStmt, Ist_, Exit)
		default:
			error("PyVEX: Unknown/unsupported IRStmtTag %s\n", IRStmtTag_to_str(i->tag));
			t = &pyIRStmtType;
	}

	PyObject *args = Py_BuildValue("");
	PyObject *kwargs = Py_BuildValue("{s:O}", "wrap", PyCapsule_New(i, "IRStmt", NULL));
	PyObject *o = PyObject_Call((PyObject *)t, args, kwargs);
	Py_DECREF(args); Py_DECREF(kwargs);
	return (PyObject *)o;
}

/////////////////
// NoOp IRStmt //
/////////////////

static int
pyIRStmtNoOp_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	if (!kwargs) { self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_NoOp()); return 0; }
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	PyErr_SetString(VexException, "Unexpected arguments provided to constructor.");
	return -1;
}

static PyMethodDef pyIRStmtNoOp_methods[] = { {NULL} };
static PyGetSetDef pyIRStmtNoOp_getseters[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(NoOp, IRStmt);

//////////////////
// IMark IRStmt //
//////////////////

static int
pyIRStmtIMark_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	Addr64 addr;
	Int len;
	UChar delta;

	static char *kwlist[] = {"addr", "len", "delta", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Kib", kwlist, &addr, &len, &delta)) return -1;

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_IMark(addr, len, delta));
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtIMark, IRStmt, self->wrapped->Ist.IMark.addr, addr, "K")
PYVEX_ACCESSOR_BUILDVAL(IRStmtIMark, IRStmt, self->wrapped->Ist.IMark.len, len, "i")
PYVEX_ACCESSOR_BUILDVAL(IRStmtIMark, IRStmt, self->wrapped->Ist.IMark.delta, delta, "b")

static PyGetSetDef pyIRStmtIMark_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtIMark, addr),
	PYVEX_ACCESSOR_DEF(IRStmtIMark, len),
	PYVEX_ACCESSOR_DEF(IRStmtIMark, delta),
	{NULL}
};

static PyMethodDef pyIRStmtIMark_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IMark, IRStmt);

////////////////////
// AbiHint IRStmt //
////////////////////

static int
pyIRStmtAbiHint_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	pyIRExpr *base;
	Int len;
	pyIRExpr *nia;

	static char *kwlist[] = {"base", "len", "nia", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OiO", kwlist, &base, &len, &nia)) return -1;
	PYVEX_CHECKTYPE(base, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(nia, pyIRExprType, return -1)

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_AbiHint(base->wrapped, len, nia->wrapped));
	return 0;
}

PYVEX_ACCESSOR_WRAPPED(IRStmtAbiHint, IRStmt, self->wrapped->Ist.AbiHint.base, base, IRExpr)
PYVEX_ACCESSOR_BUILDVAL(IRStmtAbiHint, IRStmt, self->wrapped->Ist.AbiHint.len, len, "i")
PYVEX_ACCESSOR_WRAPPED(IRStmtAbiHint, IRStmt, self->wrapped->Ist.AbiHint.nia, nia, IRExpr)

static PyGetSetDef pyIRStmtAbiHint_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtAbiHint, base),
	PYVEX_ACCESSOR_DEF(IRStmtAbiHint, len),
	PYVEX_ACCESSOR_DEF(IRStmtAbiHint, nia),
	{NULL}
};

static PyMethodDef pyIRStmtAbiHint_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(AbiHint, IRStmt);

////////////////
// Put IRStmt //
////////////////

static int
pyIRStmtPut_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	Int offset;
	pyIRExpr *data;

	static char *kwlist[] = {"offset", "data", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iO", kwlist, &offset, &data)) return -1;
	PYVEX_CHECKTYPE(data, pyIRExprType, return -1)

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_Put(offset, data->wrapped));
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtPut, IRStmt, self->wrapped->Ist.Put.offset, offset, "i")
PYVEX_ACCESSOR_WRAPPED(IRStmtPut, IRStmt, self->wrapped->Ist.Put.data, data, IRExpr)

static PyGetSetDef pyIRStmtPut_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtPut, offset),
	PYVEX_ACCESSOR_DEF(IRStmtPut, data),
	{NULL}
};

static PyMethodDef pyIRStmtPut_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(Put, IRStmt);

/////////////////
// PutI IRStmt //
/////////////////

static int
pyIRStmtPutI_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	pyIRRegArray *descr;
	pyIRExpr *ix;
	Int bias;
	pyIRExpr *data;

	static char *kwlist[] = {"description", "index", "bias", "data", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOiO", kwlist, &descr, &ix, &bias, &data)) return -1;
	PYVEX_CHECKTYPE(descr, pyIRRegArrayType, return -1)
	PYVEX_CHECKTYPE(ix, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(data, pyIRExprType, return -1)

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_PutI(mkIRPutI(descr->wrapped, ix->wrapped, bias, data->wrapped)));
	return 0;
}

PYVEX_ACCESSOR_WRAPPED(IRStmtPutI, IRStmt, self->wrapped->Ist.PutI.details->descr, description, IRRegArray)
PYVEX_ACCESSOR_WRAPPED(IRStmtPutI, IRStmt, self->wrapped->Ist.PutI.details->ix, index, IRExpr)
PYVEX_ACCESSOR_BUILDVAL(IRStmtPutI, IRStmt, self->wrapped->Ist.PutI.details->bias, bias, "i")
PYVEX_ACCESSOR_WRAPPED(IRStmtPutI, IRStmt, self->wrapped->Ist.PutI.details->data, data, IRExpr)

static PyGetSetDef pyIRStmtPutI_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtPutI, description),
	PYVEX_ACCESSOR_DEF(IRStmtPutI, index),
	PYVEX_ACCESSOR_DEF(IRStmtPutI, bias),
	PYVEX_ACCESSOR_DEF(IRStmtPutI, data),
	{NULL}
};

static PyMethodDef pyIRStmtPutI_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(PutI, IRStmt);

//////////////////
// WrTmp IRStmt //
//////////////////

static int
pyIRStmtWrTmp_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	IRTemp tmp;
	pyIRExpr *data;

	static char *kwlist[] = {"tmp", "data", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "IO", kwlist, &tmp, &data)) return -1;
	PYVEX_CHECKTYPE(data, pyIRExprType, return -1)

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_WrTmp(tmp, data->wrapped));
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtWrTmp, IRStmt, self->wrapped->Ist.WrTmp.tmp, tmp, "i")
PYVEX_ACCESSOR_WRAPPED(IRStmtWrTmp, IRStmt, self->wrapped->Ist.WrTmp.data, data, IRExpr)

static PyGetSetDef pyIRStmtWrTmp_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtWrTmp, tmp),
	PYVEX_ACCESSOR_DEF(IRStmtWrTmp, data),
	{NULL}
};

static PyMethodDef pyIRStmtWrTmp_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(WrTmp, IRStmt);

//////////////////
// Store IRStmt //
//////////////////

static int
pyIRStmtStore_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	IREndness endness;
	char *endness_str;
	pyIRExpr *addr;
	pyIRExpr *data;

	static char *kwlist[] = {"endness", "addr", "data", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sOO", kwlist, &endness_str, &addr, &data)) return -1;
	PYVEX_CHECKTYPE(addr, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(data, pyIRExprType, return -1)
	PYVEX_ENUM_FROMSTR(IREndness, endness, endness_str, return -1);

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_Store(endness, addr->wrapped, data->wrapped));
	return 0;
}

PYVEX_ACCESSOR_ENUM(IRStmtStore, IRStmt, self->wrapped->Ist.Store.end, endness, IREndness)
PYVEX_ACCESSOR_WRAPPED(IRStmtStore, IRStmt, self->wrapped->Ist.Store.addr, addr, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRStmtStore, IRStmt, self->wrapped->Ist.Store.data, data, IRExpr)

static PyGetSetDef pyIRStmtStore_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtStore, endness),
	PYVEX_ACCESSOR_DEF(IRStmtStore, addr),
	PYVEX_ACCESSOR_DEF(IRStmtStore, data),
	{NULL}
};

static PyMethodDef pyIRStmtStore_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(Store, IRStmt);

////////////////
// CAS IRStmt //
////////////////

static int
pyIRStmtCAS_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	IRTemp oldHi;
	IRTemp oldLo;
	IREndness endness;
	char *endness_str;
	pyIRExpr *addr;
	pyIRExpr *expdHi;
	pyIRExpr *expdLo;
	pyIRExpr *dataHi;
	pyIRExpr *dataLo;

	static char *kwlist[] = {"oldHi", "oldLo", "endness", "addr", "expdHi", "expdLo", "dataHi", "dataLo", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "IIsOOOOO", kwlist, &oldHi, &oldLo, &endness_str, &addr, &expdHi, &expdLo,
				&dataHi, &dataLo)) return -1;
	PYVEX_CHECKTYPE(expdHi, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(expdLo, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(dataHi, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(dataLo, pyIRExprType, return -1)
	PYVEX_ENUM_FROMSTR(IREndness, endness, endness_str, return -1);

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_CAS(mkIRCAS(oldHi, oldLo, endness, addr->wrapped, expdHi->wrapped, expdLo->wrapped, dataHi->wrapped, dataLo->wrapped)));
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->oldHi, oldHi, "i")
PYVEX_ACCESSOR_BUILDVAL(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->oldLo, oldLo, "i")
PYVEX_ACCESSOR_ENUM(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->end, endness, IREndness)
PYVEX_ACCESSOR_WRAPPED(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->addr, addr, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->expdHi, expdHi, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->expdLo, expdLo, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->dataHi, dataHi, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRStmtCAS, IRStmt, self->wrapped->Ist.CAS.details->dataLo, dataLo, IRExpr)

static PyGetSetDef pyIRStmtCAS_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtCAS, oldHi),
	PYVEX_ACCESSOR_DEF(IRStmtCAS, oldLo),
	PYVEX_ACCESSOR_DEF(IRStmtCAS, endness),
	PYVEX_ACCESSOR_DEF(IRStmtCAS, addr),
	PYVEX_ACCESSOR_DEF(IRStmtCAS, expdHi),
	PYVEX_ACCESSOR_DEF(IRStmtCAS, expdLo),
	PYVEX_ACCESSOR_DEF(IRStmtCAS, dataHi),
	PYVEX_ACCESSOR_DEF(IRStmtCAS, dataLo),
	{NULL}
};

static PyMethodDef pyIRStmtCAS_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(CAS, IRStmt);

/////////////////
// LLSC IRStmt //
/////////////////

static int
pyIRStmtLLSC_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	IREndness endness;
	char *endness_str;
	IRTemp result;
	pyIRExpr *addr;
	pyIRExpr *storedata;

	static char *kwlist[] = {"endness", "result", "addr", "storedata", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sIOO", kwlist, &endness_str, &result, &addr, &storedata)) return -1;
	PYVEX_CHECKTYPE(addr, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(storedata, pyIRExprType, return -1)
	PYVEX_ENUM_FROMSTR(IREndness, endness, endness_str, return -1);

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_LLSC(endness, result, addr->wrapped, storedata->wrapped));
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRStmtLLSC, IRStmt, self->wrapped->Ist.LLSC.result, result, "i")
PYVEX_ACCESSOR_ENUM(IRStmtLLSC, IRStmt, self->wrapped->Ist.LLSC.end, endness, IREndness)
PYVEX_ACCESSOR_WRAPPED(IRStmtLLSC, IRStmt, self->wrapped->Ist.LLSC.addr, addr, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRStmtLLSC, IRStmt, self->wrapped->Ist.LLSC.storedata, storedata, IRExpr)

static PyGetSetDef pyIRStmtLLSC_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtLLSC, endness),
	PYVEX_ACCESSOR_DEF(IRStmtLLSC, result),
	PYVEX_ACCESSOR_DEF(IRStmtLLSC, addr),
	PYVEX_ACCESSOR_DEF(IRStmtLLSC, storedata),
	{NULL}
};

static PyMethodDef pyIRStmtLLSC_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(LLSC, IRStmt);

/////////////////
// MBE IRStmt //
/////////////////

static int
pyIRStmtMBE_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	IRMBusEvent mb; char *mb_str;

	static char *kwlist[] = {"jumpkind", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist, &mb_str)) return -1;
	PYVEX_ENUM_FROMSTR(IRMBusEvent, mb, mb_str, return -1);

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_MBE(mb));
	return 0;
}

PYVEX_ACCESSOR_ENUM(IRStmtMBE, IRStmt, self->wrapped->Ist.MBE.event, event, IRMBusEvent)

static PyGetSetDef pyIRStmtMBE_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtMBE, event),
	{NULL}
};

static PyMethodDef pyIRStmtMBE_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(MBE, IRStmt);

/////////////////
// Exit IRStmt //
/////////////////

static int
pyIRStmtExit_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	pyIRExpr *guard;
	pyIRConst *dst;
	IRJumpKind jk; char *jk_str;
	int offsIP;

	static char *kwlist[] = {"guard", "jumpkind", "dst", "offsIP", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OsOi", kwlist, &guard, &jk_str, &dst, &offsIP)) return -1;
	PYVEX_CHECKTYPE(guard, pyIRExprType, return -1)
	PYVEX_CHECKTYPE(dst, pyIRConstType, return -1)
	PYVEX_ENUM_FROMSTR(IRJumpKind, jk, jk_str, return -1);

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_Exit(guard->wrapped, jk, dst->wrapped, offsIP));
	return 0;
}

PYVEX_ACCESSOR_WRAPPED(IRStmtExit, IRStmt, self->wrapped->Ist.Exit.guard, guard, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRStmtExit, IRStmt, self->wrapped->Ist.Exit.dst, dst, IRConst)
PYVEX_ACCESSOR_ENUM(IRStmtExit, IRStmt, self->wrapped->Ist.Exit.jk, jumpkind, IRJumpKind)
PYVEX_ACCESSOR_BUILDVAL(IRStmtExit, IRStmt, self->wrapped->Ist.Exit.offsIP, offsIP, "i")

static PyGetSetDef pyIRStmtExit_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtExit, guard),
	PYVEX_ACCESSOR_DEF(IRStmtExit, dst),
	PYVEX_ACCESSOR_DEF(IRStmtExit, jumpkind),
	PYVEX_ACCESSOR_DEF(IRStmtExit, offsIP),
	{NULL}
};

static PyMethodDef pyIRStmtExit_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(Exit, IRStmt);

/////////////////
// Dirty IRStmt //
/////////////////

static int
pyIRStmtDirty_init(pyIRStmt *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRStmt);

	IRTemp dest;
	Int regparms;
	const char *name;
	unsigned long long addr;
	PyObject *args_seq;

	static char *kwlist[] = {"regparms", "name", "addr", "args", "tmp", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "isKO|I", kwlist, &regparms, &name, &addr, &args_seq, &dest)) return -1;
	if (!PySequence_Check(args_seq)) { PyErr_SetString(VexException, "need sequence of args for Dirty"); return -1; }

	int seq_size = PySequence_Size(args_seq);
	IRExpr **cargs = (IRExpr **) malloc((seq_size + 1) * sizeof(IRExpr *));
	int i;
	for (i = 0; i < seq_size; i++)
	{
		pyIRExpr *expr = (pyIRExpr *)PySequence_GetItem(args_seq, i);
		PYVEX_CHECKTYPE(expr, pyIRExprType, return -1);
		cargs[i] = expr->wrapped;
	}
        cargs[i] = NULL;

        IRDirty *dirty;
        if (PyDict_GetItemString(kwargs, "tmp")) dirty = PYVEX_COPYOUT(IRDirty, unsafeIRDirty_1_N(dest, regparms, (char*) name, (void *)addr, cargs));
        else dirty = unsafeIRDirty_0_N(regparms, (char*)name, (void *)addr, cargs);

	self->wrapped = PYVEX_COPYOUT(IRStmt, IRStmt_Dirty(dirty));
	return 0;
}

PYVEX_ACCESSOR_WRAPPED(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->cee, cee, IRCallee)
PYVEX_ACCESSOR_WRAPPED(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->guard, guard, IRExpr)
PYVEX_ACCESSOR_BUILDVAL(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->tmp, tmp, "I")
PYVEX_ACCESSOR_ENUM(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->mFx, mFx, IREffect)
PYVEX_ACCESSOR_WRAPPED(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->mAddr, mAddr, IRExpr)
PYVEX_ACCESSOR_BUILDVAL(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->mSize, mSize, "I")
PYVEX_ACCESSOR_BUILDVAL(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->needsBBP, needsBBP, "b")
PYVEX_ACCESSOR_BUILDVAL(IRStmtDirty, IRStmt, self->wrapped->Ist.Dirty.details->nFxState, nFxState, "i")

static PyGetSetDef pyIRStmtDirty_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRStmtDirty, cee),
	PYVEX_ACCESSOR_DEF(IRStmtDirty, guard),
	PYVEX_ACCESSOR_DEF(IRStmtDirty, tmp),
	PYVEX_ACCESSOR_DEF(IRStmtDirty, mFx),
	PYVEX_ACCESSOR_DEF(IRStmtDirty, mAddr),
	PYVEX_ACCESSOR_DEF(IRStmtDirty, mSize),
	PYVEX_ACCESSOR_DEF(IRStmtDirty, needsBBP),
	PYVEX_ACCESSOR_DEF(IRStmtDirty, nFxState),
	{NULL}
};

PyObject *pyIRStmtDirty_args(pyIRStmt* self)
{
	int size; for (size = 0; self->wrapped->Ist.Dirty.details->args[size] != NULL; size++);

	PyObject *result = PyTuple_New(size);
	for (int i = 0; i < size; i++)
	{
		PyObject *wrapped = wrap_IRExpr(self->wrapped->Ist.Dirty.details->args[i]);
		PyTuple_SetItem(result, i, wrapped);
	}
	return result;
}

PyObject *pyIRStmtDirty_fxState(pyIRStmt* self)
{
	int size = self->wrapped->Ist.Dirty.details->nFxState;
	PyObject *result = PyTuple_New(size);
	for (int i = 0; i < size; i++)
	{
		IREffect e = self->wrapped->Ist.Dirty.details->fxState[i].fx;
		const char *e_str;
		PYVEX_ENUM_TOSTR(IREffect, e, e_str, return NULL);

		PyObject *dict = Py_BuildValue("{s:s,s:H,s:H,s:B,s:B}",
					       "fx", e_str,
					       "offset", self->wrapped->Ist.Dirty.details->fxState[i].offset,
					       "size", self->wrapped->Ist.Dirty.details->fxState[i].size,
					       "nRepeats", self->wrapped->Ist.Dirty.details->fxState[i].nRepeats,
					       "repeatLen", self->wrapped->Ist.Dirty.details->fxState[i].repeatLen);

		PyTuple_SetItem(result, i, dict);
	}
	return result;
}

static PyMethodDef pyIRStmtDirty_methods[] =
{
	{"args", (PyCFunction)pyIRStmtDirty_args, METH_NOARGS, "Returns a tuple of the IRExpr arguments to the callee"},
	{"fxState", (PyCFunction)pyIRStmtDirty_fxState, METH_NOARGS, "Returns a tuple of the fxState descriptions for the call"},
	{NULL}
};
PYVEX_SUBTYPEOBJECT(Dirty, IRStmt);
