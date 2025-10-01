import sys
sys.path.insert(0, "../build/")
import ads_b
import numpy as np
import streampu as spu

# Parameters

Fse     = 4
seuil   = 0.7
v0      = 0.5

part11_00     = [1.0,1.0,0.0,0.0]
part0_0     = [0.0,0.0,0.0,0.0]
part00_11     = [0.0,0.0,1.0,1.0]
preambule = np.concatenate([part11_00,part11_00,part0_0,part00_11,part00_11,part0_0,part0_0,part0_0])
energie_preamb = np.sqrt(np.sum(preambule))
print(f"energie_preamb : {energie_preamb}")

# Module

src=spu.source_user(480,"./formatee.txt",auto_reset=False,dtype=spu.float64)


porte_F2    = ads_b.Filter_RRR(480,[1.0,1.0])

array_32    = np.ones(32,dtype=np.float64)
porte_32    = ads_b.Filter_RRR(480,array_32)

part1_0     = [1.0,1.0,0.0,0.0]
part0_1     = [0.0,0.0,1.0,1.0]
preamb_array_reel=  np.concatenate([part1_0,part1_0,part0_0,part0_1,part0_1,part0_0,part0_0,part0_0])
porte_preamb= ads_b.Filter_RRR(480,preamb_array_reel)


selector    = ads_b.Select(480, energie_preamb)
extrait     = ads_b.Extract(480,Fse,seuil)

decid       = ads_b.DecisionPM(224,v0)
detect      = ads_b.DetectCRC(112)



# Process

frame, _    = src.generate()
#print(f"frame : {frame}")

denum       = porte_32.process(frame)
#print(f"denum : {denum}")

porte_F2.process.debug = True
use_sig     = porte_F2.process(src.generate.out_data)
#print(f"sig : {use_sig}")

#porte_preamb.process.debug = True
num         = porte_preamb.process(use_sig)
#print(f"num : {num}")



decalage,max,intercorr   = selector.process(num,denum)
#selector.process.debug = True
print(f"decalage : {decalage}")
print(f"intercorr : {intercorr}")

tram        = extrait.process(decalage,max,use_sig)
#print(f"tram : {tram}")

bits = decid.process(tram)
isClear = detect.process(bits)
print(f" IsClear : {isClear}")
print(f"bits : {bits}")


# Affichage

rep = spu.Reporter_probe("Info", "")
prb_dec = spu.probe_value(1, "decalage", None, dtype=spu.int32)
prb_clear = spu.probe_value(1, "IsClear", None, dtype=spu.int32)

rep.register_probes([prb_dec, prb_clear])

prb_clear(isClear)
prb_dec(decalage)

ter = spu.Terminal_dump([rep])
#
seq = spu.Sequence(src.generate)
ter.legend()
seq.exec(ter)
