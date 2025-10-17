import sys
sys.path.insert(0, "../build/")

import ads_b
import numpy as np
import matplotlib.pyplot as plt

import streampu as spu

import scipy.io

def affiche_carte(REF_LON, REF_LAT):
    # Styles et couleurs pour les tracés
    STYLES = ['-', '--', ':']
    STYLES_HEAD = ['x', 'o', '<']
    COLORS = plt.get_cmap('tab10').colors
    COLORS = [COLORS[i] for i in range(len(COLORS)) if i != 3]

    plt.figure(1)

    # Bordeaux
    x = np.linspace(-1.3581, 0.7128, 1024)
    y = np.linspace(44.4542, 45.1683, 1024)

    X, Y = np.meshgrid(x, y[::-1])
    im = plt.imread('../fond.png')
    plt.imshow(im, extent=[-1.3581, 0.7128, 44.4542, 45.1683])
    plt.plot(REF_LON, REF_LAT, '.r', markersize=20)
    plt.text(REF_LON + 0.05, REF_LAT, 'Actual pos', color='b')
    plt.gca().set_ylim(plt.gca().get_ylim()[::-1])  # Inverser l'axe y
    plt.xlabel('Longitude en degrés')
    plt.ylabel('Latitude en degrés')
    plt.xlim([-1.3581, 0.7128])
    plt.ylim([44.4542, 45.1683])
    #plt.show()

mat_contents = scipy.io.loadmat('../adsb_msgs.mat')
#print(mat_contents.keys())

data = mat_contents['adsb_msgs']
#print(len(data))


REF_LON = -0.606629  # Longitude de référence
REF_LAT = 44.806884  # Latitude de référence


msg_nb_ligne, msg_nb_colonne = data.shape
print(msg_nb_colonne, msg_nb_ligne)

## Initialisation des coordonnées
coord = np.zeros((2, msg_nb_colonne),np.double)
nom = ''
connaisNom = False

convert = ads_b.Bit2Register(msg_nb_ligne)
detect = ads_b.DetectCRC(msg_nb_ligne)

for i in range(msg_nb_colonne):

    #print(data[:,i])
    list = np.array(data[:,i], dtype=np.float64)
    #print(list)
    input = spu.array(list, dtype=spu.float64)

    isClear = detect.process(input)

    if (isClear[0][0]):
        adresse_sock,format_list,type_sock,nom_sock,altitude_sock,timeFlag_sock,cprFlag_sock,latitude_sock,longitude_sock = convert.process(input)

        adresse_arr=np.array(adresse_sock)
        adresse=""
        for j in range(len(adresse_arr[0])):
            value=chr(adresse_arr[0][j])
            adresse+=value
        print("voici l'adresse ",adresse)

        nom_arr=np.array(nom_sock)
        type_arr=np.array(type_sock)
        latitude_arr=np.array(latitude_sock)
        longitude_arr=np.array(longitude_sock)

        if (9 <= type_arr[0] <= 18) or (20 <= type_arr[0] <= 22):
            coord[0, i] = latitude_arr[0][0]
            coord[1, i] = longitude_arr[0][0]

        if (1 <= type_arr[0] <= 4) and (connaisNom==False):
            for l in range(len(nom_arr[0])):
                value=chr(nom_arr[0][l])
                nom+=value
            print("nom",nom)
            connaisNom = True
        print("===================================")


sanszerolat = coord[0, :] != 0
sanszerolong = coord[1, :] != 0

print("long",coord[1, sanszerolong])
print("lat",coord[0, sanszerolat])

plt.figure()
plt.imshow(plt.imread('fond.png'), extent=[REF_LON-2, REF_LON+2, REF_LAT-2, REF_LAT+2])
affiche_carte(REF_LON=REF_LON,REF_LAT=REF_LAT)
#plt.plot(coord[1, sanszerolat]+8,coord[0, sanszerolong], '--o', markerfacecolor='blue')
#plt.xlabel('Longitude')
#plt.ylabel('Latitude')
#plt.title('Carte des positions ADS-B')
#plt.savefig('../carte_positions.png')
#plt.show()

seq = spu.Sequence(input.task)
seq.export_dot("test.dot")

