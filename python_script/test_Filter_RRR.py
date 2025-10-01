import sys
sys.path.insert(0, "../build/")
import ads_b
import numpy as np
import streampu as spu


def test_Filter_RRR():
    h = np.array([1.0,0.0,1.0,0.0], dtype=np.float64)
    sig = np.array([1.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0], dtype=np.float64)
    print( f"h : {h}")
    print( f"sig : {sig}")

    porte = ads_b.Filter_RRR(9,h)
    result = porte.process(sig)

    print( f"result : {result}")

if __name__ == "__main__":
    test_Filter_RRR()
