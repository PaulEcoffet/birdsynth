from cython cimport boundscheck, wraparound

cdef extern from "finch.h":
  void finch(double *alpha, double *beta, int size, double *out) nogil

import numpy as np
cimport numpy as np

np.import_array()

DTYPE = np.double
ctypedef np.double_t DTYPE_t

@boundscheck(False)
@wraparound(False)
def synth(np.ndarray[DTYPE_t, ndim=2, mode="c"] alphabeta not None):
  """Synthesize a song with the alpha and beta parameters."""
  assert alphabeta.dtype == DTYPE, "Must be double"
  # Pad with 300 at beginning to remove the puff
  cdef int pad = 300
  cdef double[:] alpha = np.concatenate((alphabeta[0, 0] * np.ones(pad), alphabeta[:, 0])).astype(DTYPE)
  cdef double[:] beta = np.concatenate((alphabeta[0, 1] * np.ones(pad), alphabeta[:, 1])).astype(DTYPE)
  cdef int size = alphabeta.shape[0] + pad

  cdef np.ndarray[DTYPE_t, ndim=1] out = np.zeros(size-2, dtype=DTYPE)
  with nogil:
    finch(&alpha[0], &beta[0], size, &out[0])
  #print(out[:120], out[max(0,pad-10):pad+120])
  return out[pad:]
