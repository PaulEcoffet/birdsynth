import unittest
import subprocess
import os
import numpy as np
from birdsynth import synth as csynth
from io import BytesIO


dir_path = os.path.dirname(os.path.realpath(__file__))



def iosynth(alpha_beta):
    """Return the song signal given the alpha beta parameters.

    alpha_beta - A 2d numpy.array of shape (length, 2)
                 with alpha on the alpha_beta[:, 0] elements
                 and beta on the alpha_beta[:, 1] elements

    Returns - 1D numpy.array with the normalized signal between -1 and 1
    """
    input_bytes = BytesIO()
    input_bytes.write(bytes(str(alpha_beta.shape[0]) + "\n", 'utf-8'))
    np.savetxt(input_bytes, alpha_beta)
    out_raw_call = subprocess.Popen(
        [os.path.join(dir_path, "alphabeta2dat")],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE)
    out_raw = out_raw_call.communicate(input=input_bytes.getvalue())[0]
    input_bytes.close()
    out = np.fromstring(out_raw, dtype=float, sep="\n")
    return out


def _normalize(song):
    t = song - np.nanmean(song)
    t = 2 * (t - np.nanmin(t)) / (np.nanmax(t) - np.nanmin(t)) - 1
    return t


class EquivTest(unittest.TestCase):

    def test_equiv_zeros(self):
        ab = np.zeros((1000, 2))
        np.testing.assert_allclose(_normalize(iosynth(ab)),
                                   _normalize(csynth(ab)), rtol=0.01)

    def test_equiv_ba(self):
        ab = np.loadtxt(os.path.join(dir_path, 'ba_example_ab.dat'))
        iout = _normalize(iosynth(ab))
        cout = _normalize(csynth(ab))
        np.testing.assert_allclose(iout, cout, rtol=0.01)
