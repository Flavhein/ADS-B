import sys
sys.path.insert(0, "../build/")
import ads_b
import numpy as np
import streampu as spu

n_elmts = 112

Fe = 4e6
Ts = 1e-6

def chaine_PM():
    emit=ads_b.EmetteurPM(n_elmts,Fe,Ts)

    src=spu.source_user(112,"../few_sig.txt",auto_reset=False,dtype=spu.float64)
    frame, _ = src.generate()

    yl = emit.process(frame)
    print(f" entre : {frame}")
    print(f" yl : {yl}")

    canal=ads_b.Canal(len(yl),10,int(Fe*Ts))

    sig = canal.process(yl)
    print(f" sig : {sig}")
    print(f"type sig : {type(sig[0][0])}")
    print(f"dir sig : {dir(sig)}")
    print(f" getitem : {sig.__getitem__}")
    print(f"len sig : {len(sig)}")


    output_file = 'few_sig_modul.txt'
    with open(output_file, 'a') as file:
        #line_str = ' '.join(f"{sig[i][0]:.2f}" for i in range(len(sig)))
        #file.write(line_str + '\n')
        print(sig,file=file)

if __name__ == "__main__":
    chaine_PM()