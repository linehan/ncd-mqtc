set xtics ("ka265" 12, "ka288" 11, "d70j" 10, "d115j" 9, "c69a" 8, "ka148" 7, "ka74" 6, "cspl" 5, "c69b" 4, "c58i" 3, "ka207" 2, "c568k" 1, "d104j" 0)

set ytics ("ka265" 12, "ka288" 11, "d70j" 10, "d115j" 9, "c69a" 8, "ka148" 7, "ka74" 6, "cspl" 5, "c69b" 4, "c58i" 3, "ka207" 2, "c568k" 1, "d104j" 0)





set palette negative 
set view map
set cbrange [0:1]
splot "ncd.haplo.dat.good" matrix with image 
