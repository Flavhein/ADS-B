import ads_b
import numpy as np
import matplotlib.pyplot as plt

import streampu as spu

import scipy.io

mat_contents = scipy.io.loadmat('../adsb_msgs.mat')
#print(mat_contents.keys())

data = mat_contents['adsb_msgs']
#print(len(data))

msg_nb_ligne, msg_nb_colonne = data.shape
print(msg_nb_colonne, msg_nb_ligne)

convert = ads_b.Bit2Register(msg_nb_ligne)
detect = ads_b.DetectCRC(msg_nb_ligne)

for i in range(msg_nb_colonne):

    #print(data[:,i])
    list = np.array(data[:,i], dtype=np.float64)
    #print(list)
    input = spu.array(list, dtype=spu.float64)

    if (i==0):
        print(input)
        print(len(input))

    isClear = detect.process(input)
    print(isClear)

    #if (isClear):
    adresse_sock,format_list,type_sock,nom_sock,altitude_sock,timeFlag_sock,cprFlag_sock,latitude_sock,longitude_sock = convert.process(input)
    print("adress :",adresse_sock," | nom:",nom_sock," | longitude:",longitude_sock," | latitude:",latitude_sock," | altitude:",altitude_sock)
