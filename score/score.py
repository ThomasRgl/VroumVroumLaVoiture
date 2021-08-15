import csv
from pylab import *
import statistics
#import matplotlib.pyplot as plt

# fileNameScore = "score.csv"
# fileNameId = "id.csv"
fileNameFruit = "score.csv"

# with open(fileNameScore, newline = '') as csvfile:
#     spamreader = csv.reader(csvfile, delimiter = ',', quotechar = '|')
#     LaListeScore = []
#     for row in spamreader :
#         LaListeScore.append(row)
#         row.pop(-1)
#         for i in range (len(row)):
#             row[i] = float(row[i])
#             #print(str(row[i])+'\n')
#
# with open(fileNameId, newline = '') as csvfile:
#     spamreader = csv.reader(csvfile, delimiter = ',', quotechar = '|')
#     LaListeId = []
#     for row in spamreader :
#         LaListeId.append(row)
#         row.pop(-1)
#         for i in range (len(row)):
#             row[i] = float(row[i])
#
#
#
# maxi = []
# mini = []
# med = []
# moy = []
# id = []
#
# for i in range(len(LaListeId)) :
#     LaListeId[i] = len(set(LaListeId[i]))
#     #print(row)
#
# for i in range (len(LaListeScore)):
#     maxi.append(max(LaListeScore[i]))
#     mini.append(min(LaListeScore[i]))
#     med.append(statistics.median(LaListeScore[i]))
#     moy.append(statistics.mean(LaListeScore[i]))
#     id.append(LaListeId[i])
#
# x = array(range(len(LaListeScore)))
# yMini = array(mini)
# yMaxi = array(maxi)
# yMoy = array(moy)
# yMed = array(med)
# yId = array(id)
#
# figure(1)
# plot(x, yMini, label = "Minimum", color = "b" )
# plot(x, yMaxi, label = "Maximum", color = "r" )
# plot(x, yMed, label = "Mediane" )
# plot(x, yMoy, label = "Moyenne"  )
# legend()
#

#figure(2)
#plot(x, yId, label = "nb_of_id"  )
#################################################################

with open(fileNameFruit, newline = '') as csvfile:
    spamreader = csv.reader(csvfile, delimiter = ',', quotechar = '|')
    LaListeScore = []
    for row in spamreader :
        LaListeScore.append(row)
        row.pop(-1)
        for i in range (len(row)):
            row[i] = float(row[i])
            #print(str(row[i])+'\n')

maxi = []
mini = []
med = []
moy = []
id = []



for i in range (len(LaListeScore)):
    maxi.append(max(LaListeScore[i]))
    mini.append(min(LaListeScore[i]))
    med.append(statistics.median(LaListeScore[i]))
    moy.append(statistics.mean(LaListeScore[i]))

x = array(range(len(LaListeScore)))
yMini = array(mini)
yMaxi = array(maxi)
yMoy = array(moy)
yMed = array(med)

figure(3)
plot(x, yMini, label = "Minimum", color = "b" )
plot(x, yMaxi, label = "Maximum", color = "r" )
plot(x, yMed, label = "Mediane" )
plot(x, yMoy, label = "Moyenne"  )
legend()
















legend()
show()
