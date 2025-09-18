import sys
sys.path.insert(0, "../build/")
import ads_b
import numpy as np
import streampu as spu
import pdb

n_elmts = 8

Fe = 20e6
Ts = 1e-6

def chaine_PM():
    emit=ads_b.EmetteurPM(n_elmts,Fe,Ts)

    entre = spu.array([0.0,1.0,0.0,0.0,1.0,1.0,1.0,0.0], dtype=spu.float64)

    pdb.set_trace()

    yl = emit.process(entre)
    print(f" entre : {entre}")
    print(f" yl : {yl}")

    canal=ads_b.Canal(len(yl),10,int(Fe*Ts))

    sig = canal.process(yl)
    print(f" sig : {sig}")

    print(f"len sig : {len(sig)}")

    receiver=ads_b.RecepteurPM(len(sig),Fe,Ts)

    sortie = receiver.process(sig)
    print(f" sortie : {sortie}")

if __name__ == "__main__":
    chaine_PM()