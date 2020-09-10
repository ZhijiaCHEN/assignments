set terminal pdf size 3, 3 font "Helvetica,8"
set output 'toy.pdf'

stats 'toy-output.txt' nooutput
last_index = int(STATS_records - 1)
set key bottom right
plot "toy-input.txt" using 1:2 pointtype 0 ps 1 title 'data',\
     "toy-output.txt" using 1:2 with lines lc rgb "blue" lw 3 notitle '', '' using 1:2 every ::last_index with points pointtype 15 lc rgb "red" notitle,\
     "toy-output.txt" using 3:4 with lines lc rgb "blue" lw 3 notitle, '' using 3:4 every ::last_index with points pointtype 15 lc rgb "red" notitle, \
     "toy-output.txt" using 5:6 with lines lc rgb "blue" lw 3 title 'seraching path of centroids', '' using 5:6 every ::last_index with points pointtype 15 lc rgb "red" title 'final centroids'