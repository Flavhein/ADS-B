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

src=spu.source_user(960,"../Source_User.txt",auto_reset=False,dtype=spu.float64)

norme       = ads_b.Norme2(960)
square_g    = ads_b.AbsolueCarre(480,0)
square_d    = ads_b.AbsolueCarre(480,0)

porte_F2    = ads_b.Filter_RRR(480,[1.0,1.0])

array_32    = np.ones(32,dtype=np.float64)
porte_32    = ads_b.Filter_RRR(480,array_32)


porte_preamb= ads_b.Filter_RRR(480,preambule)


selector    = ads_b.Select(480, energie_preamb)
extrait     = ads_b.Extract(480,Fse,seuil)

decid       = ads_b.DecisionPM(224,v0)
detect      = ads_b.DetectCRC(112)

convert     = ads_b.Bit2Register(112)


# Process

frame, _ = src.generate()  # Ça exécute la tâche en plus d'un binding éventuel
sig_reel = norme(frame)

sig_reel_carre = square_g(sig_reel)
print(len(sig_reel_carre))
denum       = porte_32.process(sig_reel_carre)

use_sig     = porte_F2.process(sig_reel)
pre_num     = porte_preamb.process(use_sig)
num         = square_d(pre_num)

decalage,max,_   = selector.process(num,denum)

tram        = extrait.process(decalage,max,use_sig)

bits = decid.process(tram)
isClear = detect.process(bits)

# Affichage

(
    adresse_sock,
    format_list,
    type_sock,
    nom_sock,
    altitude_sock,
    timeFlag_sock,
    cprFlag_sock,
    latitude_sock,
    longitude_sock,
) = convert.process(bits)

repg = spu.Reporter_probe("Aircraft Genral infos", "")
rep = spu.Reporter_probe("Aircraft position", "")

rep_crc = spu.Reporter_probe("Is Clear, yes=1, no = 0","")

prb_lon = spu.probe_value(1, "longitude", None, dtype=spu.float64)
prb_lat = spu.probe_value(1, "latitude", None, dtype=spu.float64)
prb_alt = spu.probe_value(1, "altitude", None, dtype=spu.int32)
rep.register_probes([prb_lon, prb_lat, prb_alt])

prb_addr = spu.probe_value(6, "adress", None, dtype=spu.int8)
prb_nom = spu.probe_value(6, "nom", None, dtype=spu.int8)
prb_addr.str_display = True
prb_nom.str_display = True
repg.register_probes([prb_addr, prb_nom])

prb_crc = spu.probe_value(1, "is clear",None, dtype=spu.int32)
rep_crc.register_probes([prb_crc])

prb_lon(longitude_sock)
prb_lat(latitude_sock)
prb_alt(altitude_sock)
prb_addr(adresse_sock)
prb_nom(nom_sock)
prb_crc(isClear)
ter = spu.Terminal_dump([repg, rep, rep_crc])


# Sequence :
seq = spu.Sequence(src.generate)
seq.export_dot("sequence_read.dot")


ter.legend()

seq.exec(ter)

