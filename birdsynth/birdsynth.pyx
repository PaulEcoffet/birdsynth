from cython cimport boundscheck, wraparound

cdef extern from "finch.h":
  void finch(double *alpha, double *beta, int size, double *out)

import numpy as np
cimport numpy as np

np.import_array()

DTYPE = np.double
ctypedef np.double_t DTYPE_t

@boundscheck(False)
@wraparound(False)
def synth(np.ndarray[DTYPE_t, ndim=2, mode="c"] alphabeta not None):
  assert alphabeta.dtype == DTYPE, "Must be double"
  cdef double[:] alpha = np.asarray(alphabeta[:, 0]).astype(DTYPE)
  cdef double[:] beta = np.asarray(alphabeta[:, 1]).astype(DTYPE)
  cdef int size = alphabeta.shape[0]

  cdef np.ndarray[DTYPE_t, ndim=1] out = np.zeros(size-2, dtype=DTYPE)
  finch(&alpha[0], &beta[0], size, &out[0])
  return out